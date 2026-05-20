# `ft_tunnel_shutdown`

**Group:** `ft_*` lifetime (tunnels)
**RVA:** `0x24a3f0` &nbsp; **VA (image base 0x180000000):** `0x18024a3f0`
**Body size:** 6 bytes &nbsp; **Direct callees:** 0
**MSVC mangling:** none (plain C export)

## Status: **stub in this build**

The recovered body is:

```
mov eax, 0xFFFFFFFF
ret
```

i.e. the function unconditionally returns the constant `-1` (or
`0xFFFFFFFF` interpreted as unsigned). It does **not** touch the
tunnel, perform any I/O, or affect any state.

In the build under analysis (Bambu Studio's networking DLL
`02.06.00.50`) the export is therefore a **no-op stub**. We have not
seen any wiring that would route to a real implementation.

## Signature (declared)

```c
// declared shape, the body in this build is a stub.
int ft_tunnel_shutdown(ft_tunnel *tunnel);
```

The declared signature is inferred from the family naming pattern,
not from the recovered body (which ignores its argument anyway).
Confirmation will require an older or newer build that actually
implements it.

## Possible explanations

- **Vestigial export.** The function was part of an earlier API
  shape and is now redundant, the cleanup performed by
  [`ft_tunnel_release`](ft_tunnel_release.md) is sufficient and the
  shutdown function was emptied out rather than removed (which would
  break the ABI).
- **Feature flag turned off at compile time.** A different build of
  `bambu_networking` may have a real body here.
- **Optimised out.** Less likely, MSVC would emit at least a small
  prologue/epilogue.

## Implementation notes for the clean-room

A correct clean-room replacement can mirror the stub:

```c
int ft_tunnel_shutdown(ft_tunnel *tunnel) {
    (void)tunnel;
    return -1;  // matches Bambu's 02.06.00.50 behavior
}
```

If a future build reveals a real implementation, the clean-room
should match its semantics. As a defensive measure, the
implementation should not assume `tunnel` is non-NULL.

## Notes

- The stub returning `-1` means callers that check the return value
  for success will always see "failed to shutdown". The slicer
  likely never relies on this return; the documented contract is
  probably "best-effort, release is what actually frees you".
- If a Linux `.so` or macOS `.dylib` variant with a non-stub body
  becomes available, that would resolve the ambiguity.
