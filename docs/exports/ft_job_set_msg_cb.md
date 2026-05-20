# `ft_job_set_msg_cb`

**Group:** `ft_*` callback registration (jobs)
**RVA:** `0x249cc0` &nbsp; **VA (image base 0x180000000):** `0x180249cc0`
**Body size:** 28 bytes &nbsp; **Direct callees:** 0
**MSVC mangling:** none (plain C export)

## Signature

```c
typedef void (*ft_job_msg_cb_t)(void *ctx, ft_job_msg *msg);

int ft_job_set_msg_cb(ft_job *job, ft_job_msg_cb_t fn, void *ctx);
```

The signature shape (`ctx, msg`) is the most plausible reading of the
callback's two recovered parameter slots. We do not yet have a
captured callsite to confirm the argument order; that gets pinned
down when we document the worker that fires the callback.

## Behavior

- If `job == NULL`, returns `-1` (`0xFFFFFFFF`).
- Otherwise stores `fn` at offset `+0xe8` of the job struct and
  `ctx` at offset `+0xf0`. Returns `0`.

There is no atomic protection on the write; callers must not race
`ft_job_set_msg_cb` against the worker that fires the callback. The
documented usage is "register once at job creation time, before the
job starts running."

## Side effects

- Two 8-byte writes into the job struct. No I/O, no allocation.

## Notes

- The job's msg callback is stored as a **raw** `fn + ctx` pair, not
  wrapped in a `std::shared_ptr`. Compare this with the tunnel-side
  [`ft_tunnel_set_status_cb`](ft_tunnel_set_status_cb.md), which
  uses a fully refcounted control block.
- The reason is presumably that **job message callbacks are
  delivered synchronously from the caller's thread** (likely via
  [`ft_job_get_msg`](ft_job_get_msg.md) /
  [`ft_job_try_get_msg`](ft_job_try_get_msg.md)), so there is no
  worker thread that could outlive the registration. The tunnel
  status callback, by contrast, can fire from a worker.
- The set-after-start unsafety is consistent with the rest of the
  `ft_*` ABI, primitives, not policy.

## Implementation notes for the clean-room

```c
int ft_job_set_msg_cb(ft_job *job, ft_job_msg_cb_t fn, void *ctx) {
    if (!job) return -1;
    job->msg_cb_fn  = fn;
    job->msg_cb_ctx = ctx;
    return 0;
}
```

The struct field offsets do not need to match the original
implementation, only the ABI contract does. The clean-room can
keep its own struct layout.
