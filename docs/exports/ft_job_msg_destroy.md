# `ft_job_msg_destroy`

**Group:** `ft_*` typed destructors
**RVA:** `0x249b80` &nbsp; **VA (image base 0x180000000):** `0x180249b80`
**Body size:** 37 bytes &nbsp; **Direct callees:** 1 (UCRT `free`)
**MSVC mangling:** none (plain C export)

## Signature

```c
void ft_job_msg_destroy(ft_job_msg *msg);
```

`ft_job_msg` is the value-type returned by
[`ft_job_get_msg`](ft_job_get_msg.md) /
[`ft_job_try_get_msg`](ft_job_try_get_msg.md), see those for the
struct contract from the caller's side.

## Behavior

Null-safe. When `msg` is non-NULL:

1. Calls UCRT `free()` on the heap pointer stored at offset `+0x08`
   of `*msg`.
2. Writes `NULL` to that same field.

The function does **not** free the outer `msg` struct itself. The
outer struct is owned by the caller, typically it is allocated on
the caller's stack as the output parameter to `ft_job_get_msg`. The
heap allocation that this destructor frees is the *payload buffer*
the library allocated to carry the message's variable-length body.

## Side effects

- Returns the payload buffer to the heap.
- Zeroes the payload pointer to make the function idempotent, a
  second call on the same struct is a no-op.

## Inferred layout of `ft_job_msg`

| Offset | Width | Field |
|-------:|------:|-------|
| `0x00` |  ? | (fixed-size header, not touched by destroy) |
| `0x08` |  8 | `payload`: heap pointer, freed here |

The header at `+0x00` is not touched; it is likely a small status /
type discriminator that the caller reads alongside the payload.
[`ft_job_get_msg`](ft_job_get_msg.md) populates the whole struct,
including the header.

## Notes

- The destructor frees through the library's own UCRT allocator (the
  absolute call into `ucrtbase!free` at runtime address
  `0x7fff32585de0` in the dump). Callers must NOT free the payload
  themselves, see [`ft_free`](ft_free.md) for the rationale.
- The destructor frees only `+0x08`; if the struct grows further
  pointer fields in a future ABI revision, this destructor will need
  updating in lockstep. (One reason the typed destructors exist
  rather than expecting callers to `ft_free` each field.)

## Implementation notes for the clean-room

```c
void ft_job_msg_destroy(ft_job_msg *msg) {
    if (msg) {
        free(msg->payload);
        msg->payload = NULL;
    }
}
```
