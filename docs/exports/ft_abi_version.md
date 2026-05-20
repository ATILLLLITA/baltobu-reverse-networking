# `ft_abi_version`

**Group:** `ft_*` ABI versioning
**RVA:** `0x2492b0` &nbsp; **VA (image base 0x180000000):** `0x1802492b0`
**Body size:** 6 bytes &nbsp; **Direct callees:** 0
**MSVC mangling:** none (plain C export)

## Signature

```c
int ft_abi_version(void);
```

## Behavior

Returns the constant integer **`1`**. No parameters, no I/O, no
allocation.

This export exists so that a slicer can call it at startup and
verify that the loaded `bambu_networking` library exposes a known
ABI generation before attempting to use the rest of the `ft_*`
surface. If a future build of the library bumps the `ft_*` ABI
(adds incompatible parameters, removes exports, changes the
`ft_tunnel` / `ft_job` opaque struct sizes), this integer is the
single sanctioned way for callers to discover that.

## Side effects

None.

## Notes

- Implementation in the build we analysed is literally
  `mov eax, 1; ret`. The compiler emitted no prologue/epilogue and
  no stack adjustment because the function takes no arguments.
- This is the **only** `ft_*` export that returns a non-pointer,
  non-`void` value other than the `ft_*_get_*` family. It is also
  the only one safe to call before any handle has been created.
- Clean-room note: the replacement library should pick its own
  starting ABI version. If we are bug-compatible with Bambu's
  build (`02.06.00.50`), returning `1` matches what the slicer
  expects.
