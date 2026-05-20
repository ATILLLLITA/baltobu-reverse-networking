# `ft_job_get_msg`

**Group:** `ft_*` polling (jobs)
**RVA:** `0x249860` &nbsp; **VA (image base 0x180000000):** `0x180249860`
**Body size:** 196 bytes &nbsp; **Direct callees:** ~4
**MSVC mangling:** none (plain C export)

## Signature

```c
int ft_job_get_msg(ft_job *job,
                   uint32_t timeout_ms,
                   ft_job_msg *out_msg);
```

The third parameter is an output `ft_job_msg` *value*, caller-owned
storage that the function fills in. Caller must release the heap
payload by calling [`ft_job_msg_destroy`](ft_job_msg_destroy.md) on
`out_msg` when done.

## Behavior

Blocking pop from the job's internal message queue.

1. If `job == NULL`, returns `-1`.
2. Pops a message from the queue object at `job + 0x50` with the
   given `timeout_ms`. Internally this is a producer/consumer queue ,
   the producer side is the worker thread doing the actual transfer.
3. If the pop **times out** (no message in the window), the function
   tears down its local scratch and returns `-2`.
4. If a message was popped, copies 16 bytes (the `ft_job_msg` value)
   from an internal staging buffer into `*out_msg`. Returns `0`.

The 16 bytes copied are the full `ft_job_msg` payload, see the
inferred layout in [`ft_job_msg_destroy`](ft_job_msg_destroy.md).

## Side effects

- May block the calling thread for up to `timeout_ms` milliseconds
  while waiting on the queue.
- On success, transfers ownership of the message's heap payload to
  the caller.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | A message was popped and copied into `*out_msg`. |
| `-1` (`0xFFFFFFFF`) | `job` was NULL. |
| `-2` (`0xFFFFFFFE`) | Timeout, no message was available within `timeout_ms`. |

## Notes

- The non-blocking sibling is
  [`ft_job_try_get_msg`](ft_job_try_get_msg.md).
- Calling `ft_job_get_msg` with `timeout_ms == 0` likely behaves the
  same as `ft_job_try_get_msg` (queue-pop with a zero deadline
  returns immediately on empty), but the function signatures and
  the slicer's callsites would need to be checked to confirm.
- Callers can also receive messages **push-style** by registering a
  callback via [`ft_job_set_msg_cb`](ft_job_set_msg_cb.md). The two
  delivery paths probably do not coexist on the same job, pick one.

## Implementation notes for the clean-room

```c
int ft_job_get_msg(ft_job *job, uint32_t timeout_ms, ft_job_msg *out) {
    if (!job) return -1;
    if (ft_msgq_pop(&job->msg_queue, timeout_ms, out)) return 0;
    return -2;
}
```

The on-the-wire format of an `ft_job_msg` is a clean-room concern;
this function is just the C ABI of the polling primitive.
