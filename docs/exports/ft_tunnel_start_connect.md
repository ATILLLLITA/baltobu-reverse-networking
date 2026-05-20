# `ft_tunnel_start_connect`

**Group:** `ft_*` connect (tunnels)
**RVA:** `0x24a400` &nbsp; **VA (image base 0x180000000):** `0x18024a400`
**Body size:** 373 bytes &nbsp; **Direct callees:** ~3
**MSVC mangling:** none (plain C export)

## Signature

```c
typedef void (*ft_tunnel_connect_cb_t)(void *ctx, /* status */ ...);

int ft_tunnel_start_connect(ft_tunnel *tunnel,
                            ft_tunnel_connect_cb_t fn,
                            void *ctx,
                            void *opaque4);
```

## Behavior

Asynchronously connects the tunnel. Returns immediately; fires
`fn(ctx, ...)` when the connect attempt completes (success or
failure).

The implementation is structurally **identical** to
[`ft_tunnel_set_status_cb`](ft_tunnel_set_status_cb.md): it wraps
the `(fn, ctx)` pair in a refcounted callback control block, swaps
it into the tunnel struct (this time at offsets `+0x18`/`+0x20`,
not `+0x28`/`+0x30`), and registers it with the underlying I/O
session.

1. If `tunnel == NULL`, returns `-1`.
2. `operator new(0x28)` a callback control block with the same
   layout as `set_status_cb` (vtable `&DAT_1805fe6b0`, strong/weak
   ref both 1, `(fn, ctx)` at `+0x18`/`+0x20`).
3. Bump the strong ref (now 2), one for the tunnel, one for the
   I/O session.
4. Write the new cb block into the tunnel at offset `+0x18`
   (pointer-to-callable, cached) and `+0x20` (full block, for
   ownership). Release any previously-registered connect callback.
5. Hand the cb block to the underlying session's connect entry
   (`FUN_18026ee30(tunnel->session, ...)`).
6. Return `0`.

## Side effects

- One 40-byte heap allocation for the cb block.
- Replaces any previously-registered connect callback (its
  destructor fires).
- Schedules an asynchronous connect on the I/O session; the
  callback will be invoked from a worker thread when the connect
  resolves.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | Connect was scheduled. |
| `-1` | `tunnel` was NULL. |

## Notes

- The slots used here (`+0x18`/`+0x20`) are distinct from the ones
  used by [`ft_tunnel_set_status_cb`](ft_tunnel_set_status_cb.md)
  (`+0x28`/`+0x30`). The tunnel struct therefore has **two
  independent callback slots**: one for the connect outcome, one
  for ongoing status. Combined with the data-bus pointer at
  `+0x10`, this maxes out the 56-byte tunnel struct identified in
  [`ft_tunnel_release`](ft_tunnel_release.md).
- The 4th parameter (`opaque4`) is forwarded to the session's
  connect entry. Its semantics are not yet known, same situation
  as in `ft_tunnel_set_status_cb`.
- Pair with [`ft_tunnel_sync_connect`](ft_tunnel_sync_connect.md)
  if you want a blocking variant.

## Implementation notes for the clean-room

```c
int ft_tunnel_start_connect(ft_tunnel *tunnel, ft_tunnel_connect_cb_t fn,
                            void *ctx, void *opaque4) {
    if (!tunnel) return -1;
    auto *block = ft_cb_block_new(fn, ctx);
    auto *prev  = atomic_exchange(&tunnel->connect_cb, block);
    if (prev) ft_cb_block_release(prev);
    ft_session_start_connect(tunnel->session, block, opaque4);
    return 0;
}
```
