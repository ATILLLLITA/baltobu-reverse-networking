# `ft_tunnel_retain`

**Group:** `ft_*` reference counting (tunnels)
**RVA:** `0x24a230` &nbsp; **VA (image base 0x180000000):** `0x18024a230`
**Body size:** 15 bytes &nbsp; **Direct callees:** 0 (atomic instruction only)
**MSVC mangling:** none (plain C export)

## Signature

```c
void ft_tunnel_retain(ft_tunnel *tunnel);
```

## Behavior

Mirror of [`ft_job_retain`](ft_job_retain.md): an atomic
`LOCK INC dword ptr [tunnel]` on the 32-bit reference count at offset
0 of the opaque `ft_tunnel` struct. Null-safe (no-op when `tunnel` is
`NULL`).

## Side effects

- Increments the refcount of `*tunnel` by exactly 1.
- Nothing else.

## Notes

- Implementation is byte-identical to `ft_job_retain` apart from the
  exported symbol name and the function entry RVA. Same struct
  invariant (refcount at offset 0). Same atomic operation.
- See [`ft_job_retain`](ft_job_retain.md) for the broader explanation
  of the retain/release contract and the choice of `LOCK INC`.
- The matching destructor is [`ft_tunnel_release`](ft_tunnel_release.md).
