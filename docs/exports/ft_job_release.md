# `ft_job_release`

**Group:** `ft_*` reference counting (jobs)
**RVA:** `0x249bb0` &nbsp; **VA (image base 0x180000000):** `0x180249bb0`
**Body size:** 187 bytes &nbsp; **Direct callees:** ~5
**MSVC mangling:** none (plain C export)

## Signature

```c
void ft_job_release(ft_job *job);
```

The matching destructor to [`ft_job_retain`](ft_job_retain.md). Each
caller that holds an `ft_job` reference must call `ft_job_release`
exactly once when done, including the implicit reference returned by
`ft_job_create`.

## Behavior

Atomically decrements the 32-bit refcount at `*(int *)job` with
`LOCK DEC`. If the old value was 1 (so the new count is 0), the
function then frees the `ft_job` and all of its owned resources:

1. **Two embedded `std::shared_ptr<T>` control blocks** at offsets
   `+0x100` and `+0x110` of the struct (the `msg_cb` and `result_cb`
   callback holders, see [`ft_job_set_msg_cb`](ft_job_set_msg_cb.md)
   and [`ft_job_set_result_cb`](ft_job_set_result_cb.md)). For each:
   - decrement the strong-ref count at offset 4 of the control block;
   - if it hit 0, invoke `vtable[0]` (destructor) and then decrement
     the weak-ref count at offset 12 of the control block;
   - if that also hit 0, invoke `vtable[1]` (deallocator).
2. **Three embedded data members** at offsets `+0x8`, `+0x40` and
   `+0x88` are destroyed via local helpers (`FUN_1801250f0`,
   `FUN_180124fa0` and `FUN_180565edc`, these are MSVC-emitted
   destructors for `std::string`, `std::vector`-like containers, or
   internal queue/buffer types).
3. **Deallocate the outer 0x118-byte (280 byte) struct** via
   `operator delete[]`.

## Side effects

- On every call: one atomic decrement on `*job`.
- On the final release: ~5 heap frees (two callback shared_ptrs,
  three contained members, the outer struct) and any user-side
  callbacks the held smart pointers' destructors invoke.

## Inferred layout of `ft_job`

This destructor exposes the struct layout. Field offsets in bytes
(width in parens, _our_ informal names):

| Offset | Width | Field |
|-------:|------:|-------|
| `0x00` |  4 | `refcount` |
| `0x08` |  ? | embedded member 1 (destroyed by `FUN_1801250f0`) |
| `0x40` |  ? | embedded member 2 (destroyed by `FUN_180124fa0`) |
| `0x88` |  ? | embedded member 3 (destroyed by `FUN_180565edc`) |
| `0x100`|  8 | `result_cb` `std::shared_ptr<callable>` |
| `0x110`|  8 | `msg_cb`    `std::shared_ptr<callable>` |
| `0x118`| total | (struct size, used by `operator delete`) |

## Notes

- The two callback fields are stored as `std::shared_ptr` so they can
  be safely shared between the job and any in-flight worker thread ,
  the worker that fires the callback can hold a strong reference for
  the duration of the invocation without keeping the job alive.
- The size constant `0x118 = 280` is hard-coded into the call to
  `operator delete[]` and is the strongest available evidence for the
  outer struct's size.
- The `shared_ptr` control-block layout used (`strong @ +4`, `vtable
  @ +0`, `weak @ +12`) is MSVC's `std::_Ref_count_base` layout. This
  is informational; the clean-room replacement does not need to use
  `std::shared_ptr` so long as the C ABI behavior (atomic decrement
  + cleanup-when-zero) is preserved.

## Implementation notes for the clean-room

```c
// pseudo-code; do NOT copy decompiled body.
void ft_job_release(ft_job *job) {
    if (!job) return;
    if (atomic_fetch_sub(&job->refcount, 1) == 1) {
        // destroy members + callbacks, then free the outer struct.
        ft_job_destroy(job);
        free(job);  // or operator delete[] if using C++ allocator.
    }
}
```
