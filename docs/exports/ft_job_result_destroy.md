# `ft_job_result_destroy`

**Group:** `ft_*` typed destructors
**RVA:** `0x249c70` &nbsp; **VA (image base 0x180000000):** `0x180249c70`
**Body size:** 54 bytes &nbsp; **Direct callees:** 1 (UCRT `free`, called twice)
**MSVC mangling:** none (plain C export)

## Signature

```c
void ft_job_result_destroy(ft_job_result *result);
```

`ft_job_result` is the value-type returned by
[`ft_job_get_result`](ft_job_get_result.md).

## Behavior

Null-safe. When `result` is non-NULL:

1. UCRT `free()` on the heap pointer at offset `+0x08`.
2. UCRT `free()` on the heap pointer at offset `+0x10`.
3. Zero both pointers to make the function idempotent.
4. Zero the field at offset `+0x18` (32-bit).
5. Zero the field at offset `+0x00` (32-bit).

The outer struct itself is not freed, it is caller-owned, the same
way [`ft_job_msg_destroy`](ft_job_msg_destroy.md) treats `ft_job_msg`.

## Side effects

- Two heap frees through the library's UCRT.
- Five fields zeroed.

## Inferred layout of `ft_job_result`

| Offset | Width | Field (informal name) |
|-------:|------:|-----------------------|
| `0x00` |  4 | `status` / `kind` (32-bit) |
| `0x08` |  8 | `data_a`: heap pointer, freed here |
| `0x10` |  8 | `data_b`: heap pointer, freed here |
| `0x18` |  4 | `length` or secondary flag (32-bit) |

The struct holds **two** variable-length heap buffers, plus two
small scalar fields. The "two heap buffers" shape is consistent with
a result that carries both a body and a header / typed-key payload ,
e.g. `(blob, mime_type)` or `(uploaded_url, etag)`.

The exact semantics of `data_a` vs `data_b` and of `+0x00` /
`+0x18` will come out when we document
[`ft_job_get_result`](ft_job_get_result.md), which populates them.

## Notes

- Compared to `ft_job_msg_destroy`, this destructor frees *two*
  pointers and zeros *more* scalar fields. That's the only structural
  difference; the contract from the caller's side (null-safe,
  idempotent, outer struct caller-owned) is the same.
- Both destructors use the runtime-resolved UCRT `free` (absolute
  call into `ucrtbase!free`). The clean-room replacement should use
  whatever `free()` the library was built against to keep the
  allocator paired with the corresponding `ft_job_get_result`.

## Implementation notes for the clean-room

```c
void ft_job_result_destroy(ft_job_result *result) {
    if (result) {
        free(result->data_a);
        free(result->data_b);
        result->data_a = NULL;
        result->data_b = NULL;
        result->status_or_kind = 0;
        result->length_or_flag = 0;
    }
}
```
