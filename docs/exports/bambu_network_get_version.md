# `bambu_network_get_version`

**Group:** agent lifecycle
**RVA:** `0x1ba5f0` &nbsp; **VA (image base 0x180000000):** `0x1801ba5f0`
**Body size:** 63 bytes &nbsp; **Direct callees:** 1
**MSVC mangling:** none (plain C export)

## Signature

```c
void bambu_network_get_version(std::string *out);
```

The function takes a hidden output-parameter pointing at uninitialized
storage for an MSVC `std::string` (24 bytes on x86-64 MSVC: an inline
SSO buffer + length + capacity). The caller owns the storage and must
destroy the constructed string when done, there is no companion
`bambu_network_free_version` export.

This is the standard MSVC ABI convention for returning small
non-trivially-copyable structs by value.

## Behavior

Constructs an `std::string` into `*out` containing an 11-character
ASCII version literal. For the build we are analysing, the literal is
**`02.06.00.50`** (corresponds to Bambu Studio's Windows networking
DLL build 2.6.0.50, April 2026).

The construction uses MSVC's **small-string optimization (SSO)**:

- The string is stored entirely inside the `std::string` object's
  16-byte inline buffer; no heap allocation occurs.
- The 4th 8-byte field is the capacity (`0xf` = SSO limit of 15
  bytes).
- The 3rd 8-byte field is the length (`0xb` = 11).

The trailing null terminator is written explicitly to the position
that follows the 11 character bytes.

## Side effects

None. The function is a pure constructor: it touches only `*out` and
performs one read of a `.rdata` literal.

## Notes

- The string literal lives at `.rdata` RVA `0x5f2438`. The full
  contents around that RVA are `"02.06.00.50\0\0\0\0\0windows\0"`,
  i.e. version-string then a `"windows"` sibling literal that is
  presumably used by `track_*` or by the `User-Agent` string builder
  elsewhere.
- The single internal callee `FUN_180566489` is the MSVC `std::string`
  body-copy helper (a memcpy-like routine specialised for SSO).
- Because the function takes no agent argument and performs no I/O,
  it is a useful canary to confirm a clean-room replacement loads and
  links correctly before any agent setup is attempted.

## Implementation notes for the clean-room

A correct implementation is roughly:

```c
// pseudo-code; clean-room implementers write this fresh, NOT a copy
// of decompiled body.
void bambu_network_get_version(std::string *out) {
    new (out) std::string{LIBRARY_VERSION};
}
```

with `LIBRARY_VERSION` being whatever version constant the clean-room
library wants to advertise. The exact MSVC SSO layout is determined
by the `std::string` implementation supplied by the linker, not by
this function.
