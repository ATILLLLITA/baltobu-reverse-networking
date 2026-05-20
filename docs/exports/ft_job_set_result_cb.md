# `ft_job_set_result_cb`

**Group:** `ft_*` callback registration (jobs)
**RVA:** `0x249cf0` &nbsp; **VA (image base 0x180000000):** `0x180249cf0`
**Body size:** 28 bytes &nbsp; **Direct callees:** 0
**MSVC mangling:** none (plain C export)

## Signature

```c
typedef void (*ft_job_result_cb_t)(void *ctx, ft_job_result *result);

int ft_job_set_result_cb(ft_job *job, ft_job_result_cb_t fn, void *ctx);
```

## Behavior

- If `job == NULL`, returns `-1`.
- Otherwise stores `fn` at offset `+0xd8` of the job struct and
  `ctx` at offset `+0xe0`. Returns `0`.

Mirrors [`ft_job_set_msg_cb`](ft_job_set_msg_cb.md) field-for-field
apart from the offsets (`+0xd8/+0xe0` instead of `+0xe8/+0xf0`). The
job thus has **two** independent (fn, ctx) callback slots, one for
incremental progress messages, one for the terminal result.

## Side effects

- Two 8-byte writes into the job struct.

## Notes

- Same race caveat as `ft_job_set_msg_cb`: register before
  [`ft_tunnel_start_job`](ft_tunnel_start_job.md) is invoked. The
  library does not guard the field against concurrent updates.
- The result callback is presumably fired exactly once per job, when
  the job reaches a terminal state (success or failure). The exact
  trigger flow will be visible once
  [`ft_tunnel_start_job`](ft_tunnel_start_job.md) and the worker
  routines are documented.

## Implementation notes for the clean-room

```c
int ft_job_set_result_cb(ft_job *job, ft_job_result_cb_t fn, void *ctx) {
    if (!job) return -1;
    job->result_cb_fn  = fn;
    job->result_cb_ctx = ctx;
    return 0;
}
```
