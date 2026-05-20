# `ft_job_get_result`

**Group:** `ft_*` polling (jobs)
**RVA:** `0x249930` &nbsp; **VA (image base 0x180000000):** `0x180249930`
**Body size:** 573 bytes &nbsp; **Direct callees:** ~7
**MSVC mangling:** none (plain C export)

## Signature

```c
int ft_job_get_result(ft_job *job,
                      uint64_t reserved,
                      ft_job_result *out_result);
```

The second parameter (`reserved`) is taken but not used by the
recovered body in a meaningful way, it is likely an enum / mode
flag (e.g. "result-kind to request"). The slicer's callsite would
clarify; until then treat as opaque.

## Behavior

Reads the job's **terminal result** out of the underlying
"shared future" object the job holds at `*(job + 0x40)`.

1. If `job == NULL`, returns `-1`.
2. Take a strong reference on the future's shared-pointer at
   `(job->state)->+0xb8`.
3. If the future has not been initialised, the function calls a
   no-return error helper (`FUN_180566250`), this is an
   `std::future_error` analog; effectively a contract violation.
4. Call the future's `get` / `peek` vtable method (vtable slot
   `+0x10`). The returned object has a 1-byte status discriminator
   at offset `+0x28`:
   - `-1` (`0xff`): not ready yet, leaves `out_result` zeroed.
   - `0`: success, copies the result's buffer (a
     `std::vector<uint8_t>`-shaped triple at offsets `+16..+24`)
     into a heap-allocated buffer attached to a transient
     `ft_job_result` value.
   - any other byte: error, copies an error code (32-bit at `+0`)
     and an error-tag (32-bit at `+4`) into the transient value,
     plus an error-detail container at `+8` (via
     `FUN_1800b6470`).
5. Drop the strong reference held in step 2. If that reference drop
   triggered final destruction of the future, the vtable destructor
   chain runs.
6. Copy 32 bytes of the transient `ft_job_result` into `*out_result`
   (the caller-supplied output). Returns `0`.

In other words: the function flattens an internal future-state into
a 32-byte value-type that the caller can interpret, and it does so
*idempotently*, calling it again either re-reads the same result
or signals "not ready yet" (returning success but a status byte that
the result's structure can use to discriminate).

## Side effects

- Heap allocation: when the result is a *success* with a non-empty
  body, an `operator new` allocates the result payload (`+0x10`
  in `ft_job_result`). The caller-side
  [`ft_job_result_destroy`](ft_job_result_destroy.md) frees this
  later.
- Strong-ref / weak-ref atomic decrements on the future's control
  block. Possibly invokes destructors / dtors if the count hit zero.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | `out_result` populated (which can itself encode success-with-data, success-with-error, or not-ready-yet). |
| `-1` | `job` was NULL. |

## Notes

- The function does NOT have a separate `try`-variant in the export
  table, the "not ready yet" case is encoded inside the
  `ft_job_result` value (status byte at offset `+0x18` likely; see
  [`ft_job_result_destroy`](ft_job_result_destroy.md)).
- The 32-byte result struct (8 dwords) carries: a status / kind
  word, a 4-byte secondary tag, two heap pointers, and one final
  scalar, see the inferred layout in `ft_job_result_destroy`.
- The recovered body has an `Instruction overlap` warning at
  `0x180249a3c..0x180249a3d`. This is a Ghidra disassembly
  artefact, not protector anti-disassembly; it occurs in the
  arithmetic-overflow check on `uVar17 + 0x27`. Decompilation is
  correct.
- Calling `ft_job_get_result` against a never-started or
  uninitialised job will invoke an `std::future_error` and crash
  the process (`FUN_180566250` is no-return). The clean-room
  replacement should preserve that fail-loud behavior, silently
  succeeding here would hide real bugs.

## Implementation notes for the clean-room

```c
int ft_job_get_result(ft_job *job, uint64_t reserved, ft_job_result *out) {
    (void)reserved;
    if (!job) return -1;
    ft_future_state *fs = ft_job_share_future(job);  // strong-ref bump
    if (!fs) abort();  // matches Bambu's std::future_error path
    /* peek the future; populate out; release fs */
    /* caller frees out->payloads via ft_job_result_destroy */
    return 0;
}
```
