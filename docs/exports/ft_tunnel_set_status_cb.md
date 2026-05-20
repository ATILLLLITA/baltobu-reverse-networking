# `ft_tunnel_set_status_cb`

**Group:** `ft_*` callback registration (tunnels)
**RVA:** `0x24a240` &nbsp; **VA (image base 0x180000000):** `0x18024a240`
**Body size:** 424 bytes &nbsp; **Direct callees:** ~3 (`operator new`, an
internal registrar, plus implicit shared_ptr destructors)
**MSVC mangling:** none (plain C export)

## Signature

```c
typedef void (*ft_tunnel_status_cb_t)(void *ctx, /* status */ ...);

int ft_tunnel_set_status_cb(ft_tunnel *tunnel,
                            ft_tunnel_status_cb_t fn,
                            void *ctx,
                            void *unknown);
```

The fourth parameter (`unknown`) was visible in the recovered body ,
it is forwarded as the 4th argument of an internal registrar
(`FUN_180244890`). The slicer's actual call would clarify what it
is; plausible options include an "initial status to deliver" or a
flag controlling whether the callback fires for the current state on
registration. For now this is **not confirmed**.

## Behavior

Distinct from the job-side setters: this one wraps the (fn, ctx) in
a **heap-allocated refcounted callback object** and atomically swaps
it into the tunnel, releasing any previously-registered callback.
The flow is:

1. Null-check `tunnel`; return `-1` if NULL.
2. `operator new(0x28)`, allocate a 40-byte callback control block
   with the layout:

   | Offset | Width | Field |
   |-------:|------:|-------|
   | `0x00` | 8 | vtable pointer (set to `&DAT_1805fe6d8`) |
   | `0x08` | 4 | strong refcount (initialised to 1) |
   | `0x0c` | 4 | weak refcount (initialised to 1) |
   | `0x10` | 8 | spare / placeholder (zeroed) |
   | `0x18` | 8 | `fn` (the callback function pointer) |
   | `0x20` | 8 | `ctx` (the callback context pointer) |

3. Increment the strong refcount (now 2), one reference will be
   held by the tunnel, one by the internal registrar.
4. Write `&cb_block->fn` (i.e. `cb_block + 0x18`) into the tunnel
   at offset `+0x28`, and the cb_block itself into `+0x30`. So the
   tunnel struct has the shape:

   | Offset | Width | Field |
   |-------:|------:|-------|
   | `0x28` | 8 | pointer-to-callable inside the cb block (cached) |
   | `0x30` | 8 | full cb block (for refcount ownership) |

5. **Release the previously-registered cb block** if any (atomic
   decrement, destroy via vtable if it hit zero).
6. Hand the new cb block to the tunnel's owner / dispatcher via
   `FUN_180244890(tunnel->dispatcher + 0x250, ...)`, taking another
   strong reference so the dispatcher can fire the callback even if
   the tunnel itself is released.
7. Drop the temporary local references created during the swap.
8. Return `0`.

## Side effects

- One 40-byte heap allocation (the callback control block).
- Potential cleanup of a previously-registered callback (its
  destructor fires; can in turn invoke a user-side teardown).
- Registers the new callback with the tunnel's internal
  dispatcher / I/O loop so it can fire from a worker thread.

## Why a refcounted wrapper here

Unlike the job-side setters
([`ft_job_set_msg_cb`](ft_job_set_msg_cb.md) /
[`ft_job_set_result_cb`](ft_job_set_result_cb.md)) which store the
(fn, ctx) as a raw pair, the tunnel status callback can be fired
from a **worker thread** (the I/O loop). The refcount wrapper
guarantees that even if `ft_tunnel_release` runs while the worker is
mid-callback, the cb block, and therefore the captured `ctx`, is
kept alive long enough for the callback to return.

This is essentially the C-ABI manifestation of:

```cpp
std::shared_ptr<callback_t> cb;  // tunnel.cb_block
```

## Notes

- The control-block layout (`vtable @ +0`, `strong @ +8`,
  `weak @ +12`) is MSVC's `std::_Ref_count_base` layout, same as
  the smart-pointer fields in
  [`ft_tunnel_release`](ft_tunnel_release.md) /
  [`ft_job_release`](ft_job_release.md). The `vtable` at
  `&DAT_1805fe6d8` will hold the typed destructor and deallocator
  pairs.
- `operator new` returning NULL would lead to a NULL-deref on the
  immediately-following stores; in real MSVC builds with the default
  `operator new`, that branch is unreachable (would throw). The
  recovered body has a defensive `if (plVar7 != NULL)` anyway.
- The four-argument shape (`tunnel`, `fn`, `ctx`, `unknown`) was
  recovered from the calling convention's first four argument
  registers; the 4th register was used by the body. The job-side
  setters have only the three-argument shape.

## Implementation notes for the clean-room

```c
struct ft_tunnel_cb_block {
    /* internals - clean-room is free to pick a layout */
    atomic_uint32_t strong;
    atomic_uint32_t weak;
    ft_tunnel_status_cb_t fn;
    void *ctx;
};

int ft_tunnel_set_status_cb(ft_tunnel *tunnel,
                            ft_tunnel_status_cb_t fn,
                            void *ctx,
                            void *opaque4) {
    if (!tunnel) return -1;
    auto *block = ft_tunnel_cb_block_new(fn, ctx);
    auto *prev  = atomic_exchange(&tunnel->status_cb, block);
    if (prev) ft_tunnel_cb_block_release(prev);
    ft_dispatcher_register_cb(tunnel->dispatcher, block, opaque4);
    return 0;
}
```

The clean-room is free to use whatever refcount/atomic primitives
it likes, as long as the worker thread sees a consistent
`(fn, ctx)` pair for the duration of any callback invocation.
