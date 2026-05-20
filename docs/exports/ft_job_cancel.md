# `ft_job_cancel`

**Group:** `ft_*` lifetime (jobs)
**RVA:** `0x2492d0` &nbsp; **VA (image base 0x180000000):** `0x1802492d0`
**Body size:** 274 bytes &nbsp; **Direct callees:** ~3
**MSVC mangling:** none (plain C export)

## Signature

```c
int ft_job_cancel(ft_job *job);
```

## Behavior

Requests cancellation of a running job, asynchronously.

1. If `job == NULL`, returns `-1`.
2. Format the log message `"FT: job cancel, name = {}, id = {}"`
   using the `name` (job + 0x60) and `id` (job + 0x80) fields, both
   stored as MSVC `std::string` (SSO-aware). Heap-allocate a buffer
   for the formatted string via `operator new(0x30)` plus possible
   reallocations for variable length.
3. (The log message is built but the body does not call any
   explicit logger sink in the recovered body, the formatting goes
   into a local buffer that is freed before return. Likely the log
   message is dispatched via a callback or trace channel that
   Ghidra did not resolve. We have not pinned this down.)
4. Free the log buffer with the library's UCRT allocator.
5. Reach into the future state at `*(job + 0x40)` and call the
   method at vtable slot `+0x10` of the field at
   `(future_state) + 0x150`, that's the future / promise's
   cancellation entry point.
6. Returns `0` on success.

The job is **not** released by this call. Cancellation is a request
on the underlying future; the caller still owns its reference and
must `ft_job_release` when done.

## Side effects

- Heap allocation + free of the log message buffer.
- Possibly invokes a logger sink with the formatted message
  (unconfirmed).
- Calls into the future/promise to flip its state to cancelled ,
  this may unblock any thread waiting on the future and will cause
  subsequent [`ft_job_get_result`](ft_job_get_result.md) calls to
  see a cancelled / error status.
- May trigger the registered result callback (see
  [`ft_job_set_result_cb`](ft_job_set_result_cb.md)) with an
  error-coded `ft_job_result`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | Cancel request submitted. |
| `-1` | `job` was NULL. |

## Notes

- The log-format substitution reveals two fields of `ft_job` that
  are inferable structure-wise: `name` at offset `+0x60` and `id`
  at offset `+0x80`. Both are MSVC `std::string` (24-byte SSO
  layout, the size constants `0x22 = 34` and `0x2F = 47` visible
  in the body are the *string length* values used to format the
  log, not the struct field sizes).
- This is a "fire-and-forget" cancellation primitive. Callers do
  not get a synchronous confirmation; they observe the cancel by
  reading the eventual result.
- Concurrency: calling `ft_job_cancel` while the worker is mid-job
  is the expected use case. The worker observes the cancelled
  future state and exits early.

## Implementation notes for the clean-room

```c
int ft_job_cancel(ft_job *job) {
    if (!job) return -1;
    ft_log("FT: job cancel, name = %s, id = %s", job->name, job->id);
    ft_future_cancel(job->state->cancel_chan);
    return 0;
}
```
