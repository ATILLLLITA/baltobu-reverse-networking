# `ft_free`

**Group:** `ft_*` memory management
**RVA:** `0x2492c0` &nbsp; **VA (image base 0x180000000):** `0x1802492c0`
**Body size:** 7 bytes &nbsp; **Direct callees:** 1 (CRT `free`)
**MSVC mangling:** none (plain C export)

## Signature

```c
void ft_free(void *p);
```

The caller-side rule is: any pointer that an `ft_*` function returned
to you and explicitly documented as "must be released by `ft_free`"
must be passed to this function exactly once when you are done with
it. Passing `NULL` is safe; passing a pointer that did not originate
from an `ft_*` function is undefined behaviour.

## Behavior

A direct tail-call into the C runtime's `free()` (UCRT
`ucrtbase!free`, resolved through VMProtect's import resolver). The
function does not retain or examine `p` beyond passing it through.

## Side effects

- Frees the heap block at `p` via the same allocator the library
  used internally (UCRT's heap on Windows).
- After the call, `p` must not be dereferenced.

## Notes

- The disassembled body is effectively `jmp [iat_free]`. In the
  unpacked dump the IAT thunk has been replaced by VMProtect's import
  protection, so the disassembly shows an absolute call into the
  ASLR-relocated UCRT image, this is the runtime address, not the
  static one. Recovering the real symbol name requires either Frida
  trace data or reversing the VMProtect API-id resolver at
  `FUN_18163ec4b` (described in [`../01-binary.md`](../01-binary.md)).
- Why a separate `ft_free` instead of asking callers to use plain
  `free()`: on Windows, MSVC builds can use multiple CRTs in the
  same process. Passing a pointer allocated by `bambu_networking.dll`'s
  UCRT to a different CRT's `free()` crashes. Exposing a libary-local
  `free` keeps the allocator paired.
- The same rationale explains why the `ft_*` API also has
  `ft_job_msg_destroy` and `ft_job_result_destroy`, those are
  *typed* destructors that run destructors on the C++ payloads
  before releasing the storage; `ft_free` is the plain-bytes
  variant.

## Implementation notes for the clean-room

```c
// clean-room replacement is trivial; the only constraint is that
// the implementation share an allocator with whatever allocated the
// pointer it is freeing.
void ft_free(void *p) {
    free(p);
}
```
