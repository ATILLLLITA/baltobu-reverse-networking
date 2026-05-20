# `ft_job_try_get_msg`

**Group:** `ft_*` polling (jobs)
**RVA:** `0x249d20` &nbsp; **VA (image base 0x180000000):** `0x180249d20`
**Body size:** 154 bytes &nbsp; **Direct callees:** ~3
**MSVC mangling:** none (plain C export)

## Signature

```c
int ft_job_try_get_msg(ft_job *job, ft_job_msg *out_msg);
```

Non-blocking sibling of [`ft_job_get_msg`](ft_job_get_msg.md). No
timeout, returns immediately whether or not a message is available.

## Behavior

1. If `job == NULL`, returns `-1`.
2. Try-pops a message from the queue at `job + 0x50` via the
   non-blocking variant of the queue's pop primitive.
3. If the queue was empty, returns `-2` (no allocation, no
   modification to `*out_msg`).
4. Otherwise copies 16 bytes (the `ft_job_msg` value) into
   `*out_msg`. Returns `0`.

## Side effects

- Never blocks.
- On success, transfers ownership of the message's heap payload to
  the caller; the caller must release it with
  [`ft_job_msg_destroy`](ft_job_msg_destroy.md).

## Return values

| Return | Meaning |
|------:|---------|
| `0` | A message was popped and copied into `*out_msg`. |
| `-1` | `job` was NULL. |
| `-2` | The queue was empty at the moment of the call. |

## Notes

- Use this in a UI/main thread that cannot afford to block on the
  job's worker. Pair with a periodic timer or a callback-driven
  notification scheme.
- The implementation in this build differs from `ft_job_get_msg`
  only in calling the queue's non-blocking try-pop
  (`FUN_1802702e0`) instead of the timed pop
  (`FUN_18026e5f0`). Same staging buffer, same out-struct layout,
  same return-value convention.
- The library's queue primitives are likely standard
  condition-variable + mutex protected `std::deque<ft_job_msg>`
  containers; the "try" variant locks, peeks, returns `false` if
  empty.

## Implementation notes for the clean-room

```c
int ft_job_try_get_msg(ft_job *job, ft_job_msg *out) {
    if (!job) return -1;
    if (ft_msgq_try_pop(&job->msg_queue, out)) return 0;
    return -2;
}
```
