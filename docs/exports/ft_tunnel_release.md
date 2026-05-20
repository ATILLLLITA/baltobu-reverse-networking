# `ft_tunnel_release`

**Group:** `ft_*` reference counting (tunnels)
**RVA:** `0x24a140` &nbsp; **VA (image base 0x180000000):** `0x18024a140`
**Body size:** 233 bytes &nbsp; **Direct callees:** ~5
**MSVC mangling:** none (plain C export)

## Signature

```c
void ft_tunnel_release(ft_tunnel *tunnel);
```

Matching destructor to [`ft_tunnel_retain`](ft_tunnel_retain.md). The
caller must call this exactly once for each held reference, including
the implicit reference returned by `ft_tunnel_create`.

## Behavior

Atomically decrements the 32-bit refcount at `*(int *)tunnel` with
`LOCK DEC`. If the old value was 1, the function frees the tunnel and
all of its owned resources:

1. Destroy the embedded member at offset `+0x08` via `FUN_18026fb80`
   (looks like an `std::string` or buffer destructor, argument is
   loaded as a single `uint64`).
2. Three embedded `std::shared_ptr<T>` smart-pointer fields at offsets
   `+0x10`, `+0x20` and `+0x30`. For each: decrement strong-ref,
   destroy + decrement weak-ref + dealloc if the count hit zero. Same
   MSVC control-block pattern as in
   [`ft_job_release`](ft_job_release.md).
3. Deallocate the outer 0x38-byte (56 byte) struct via
   `operator delete[]`.

## Side effects

- On every call: one atomic decrement on `*tunnel`.
- On the final release: up to 4 heap frees (one buffer + three smart
  pointers) plus any user-side callbacks fired by the smart-pointer
  destructors' captured callables (status callback, see
  [`ft_tunnel_set_status_cb`](ft_tunnel_set_status_cb.md)).

## Inferred layout of `ft_tunnel`

| Offset | Width | Field |
|-------:|------:|-------|
| `0x00` |  4 | `refcount` |
| `0x08` |  8 | embedded buffer/string (destroyed via `FUN_18026fb80`) |
| `0x10` |  8 | `shared_ptr<a>` |
| `0x20` |  8 | `shared_ptr<b>` |
| `0x30` |  8 | `shared_ptr<c>` |
| `0x38` | total | (struct size, used by `operator delete[]`) |

Three smart-pointer fields plausibly correspond to: the **socket /
transport handle**, the **status-callback holder** (matching
`ft_tunnel_set_status_cb`), and one more (likely an **owner agent
pointer** or a job queue). Determining which is which requires
looking at `ft_tunnel_create` and `ft_tunnel_set_status_cb`.

## Notes

- The size constant `0x38 = 56` is hard-coded into the
  `operator delete[]` call. `ft_tunnel` is therefore 56 bytes, much
  smaller than `ft_job` (280 bytes).
- The release function does **not** call any "shutdown" or "disconnect"
  helper before deallocating. The socket close happens
  transparently when the shared_ptr at offset `+0x08` or one of
  `+0x10..+0x30` destructs (whichever owns the OS handle). So
  callers can safely `ft_tunnel_release` an active tunnel without
  first calling [`ft_tunnel_shutdown`](ft_tunnel_shutdown.md).

## Implementation notes for the clean-room

```c
void ft_tunnel_release(ft_tunnel *tunnel) {
    if (!tunnel) return;
    if (atomic_fetch_sub(&tunnel->refcount, 1) == 1) {
        ft_tunnel_destroy(tunnel);  // close socket, drop cb, etc.
        free(tunnel);
    }
}
```
