# `ft_tunnel_start_job`

**Group:** `ft_*` job dispatch (tunnels)
**RVA:** `0x24a580` &nbsp; **VA (image base 0x180000000):** `0x18024a580`
**Body size:** 1514 bytes &nbsp; **Direct callees:** ~10
**MSVC mangling:** none (plain C export)

## Signature

```c
int ft_tunnel_start_job(ft_tunnel *tunnel, ft_job *job);
```

Bind a `job` (created via [`ft_job_create`](ft_job_create.md)) to a
connected `tunnel` and start running it. The job moves from
"inert" to "in flight" as a result.

## Behavior

This is the largest function in the `ft_*` family, it wires up
all of the job's runtime state to the tunnel's worker. The flow:

1. If either argument is NULL, returns `-1`.
2. Call into the tunnel session's "create future" helper
   (`FUN_18026f0c0`), passing the **`cmd_type`** at `job+0x14` and
   the JSON spec object stored at `job+0x28`/`job+0x30`. The
   helper returns a fresh "job future" with:
   - state pointers at offsets `+0..+0x10` (moved into
     `job+0x40..+0x48`, the **future state**)
   - cancellation pointers at offsets `+0x10..+0x20` (moved into
     `job+0x50..+0x58`, the **cancel channel**)
   - the worker-side queue at offset `+0x20..+0x40` (moved into
     `job+0x60..+0x80`, the **internal msg queue**)
   - a worker identifier at offset `+0x40` (moved into
     `job+0x80`)

   These are the same fields documented in
   [`ft_job_cancel`](ft_job_cancel.md) (the cancel channel) and
   [`ft_job_get_msg`](ft_job_get_msg.md) (the message queue at
   `+0x50`, wait, the recovered body of `ft_job_get_msg` reads
   from `job+0x50` and this function moves into `job+0x60`. The
   `+0x50` vs `+0x60` discrepancy is the next thing to nail down;
   one of them is misindexed in our reading. Worth verifying against
   the dispatcher source.)

3. Construct an internal "sleep guard" lock at `job + 0x88`
   (`FUN_180565ee2`, `EnterCriticalSection`-like). The function
   short-circuits to a no-return error if it fails.

4. **If `job->msg_cb` is set** (field at `job+0xd8` non-NULL):
   register a worker that pulls messages from the internal queue,
   passes each to the user-side `msg_cb_fn` with its `msg_cb_ctx`,
   and uses the tunnel session's dispatcher
   (`FUN_18026c680(tunnel->session, ...)`) to schedule the
   callback on the worker thread that originated the message.
   The wrapper is a refcounted callback control block at
   `&DAT_1805fe700`.

5. **If `job->result_cb` is set** (field at `job+0xe8` non-NULL):
   register a similar worker that fires the result callback when
   the job reaches a terminal state. Uses a 0x50-byte (80-byte)
   wrapper at `&DAT_1805fe728`.

6. Release the sleep guard. Return `0`.

## Side effects

- Heap allocations: the job future, two refcounted callback
  wrappers (only allocated if the corresponding callback is set).
- Mutates the job's internal state, after this call,
  `ft_job_cancel`, `ft_job_get_msg`, `ft_job_get_result` and the
  msg/result callbacks (if set) all become live and can fire from
  worker threads.
- Connects to the tunnel session's I/O loop so progress can
  proceed asynchronously.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | The job is in flight. |
| `-1` | Either argument was NULL. |

## Notes

- The msg-callback path uses
  [`FUN_18026c680`](#) (a "post-to-this-thread" dispatcher on the
  tunnel session) to ensure each msg callback fires from the
  thread the message arrived on. This guarantees ordering relative
  to other work on that thread.
- The function does **not** retain the job's refcount. The
  contract is: the tunnel takes a non-owning observer for as long
  as the job runs; the caller must keep an outstanding reference
  until they no longer want the callbacks to fire. Premature
  `ft_job_release` while the job is in flight is undefined.
- The two callback registrations are independent, registering
  only `msg_cb` is valid, only `result_cb` is valid, both are
  valid, neither is valid (though in that case the caller must
  poll via `ft_job_get_msg` / `ft_job_get_result`).
- The function reads several `std::shared_ptr<T>` fields out of
  the future-helper's return, `FUN_18026f0c0` constructs them
  and `start_job` swaps them into the job, releasing whatever
  was there before. So calling `ft_tunnel_start_job` on a job
  that has already been started will:
  - cancel the old run (via the released cancel channel's
    destructor invoking the cancellation method).
  - start a fresh run on the same tunnel.

  This is not a documented re-entry contract; relying on it is
  brittle.

## Implementation notes for the clean-room

```c
int ft_tunnel_start_job(ft_tunnel *tunnel, ft_job *job) {
    if (!tunnel || !job) return -1;
    /* synchronously construct future-state + cancel-chan + msg-queue
       on the tunnel's worker, swap them into the job */
    ft_job_runtime rt = ft_session_create_future(tunnel->session,
                                                 job->cmd_type,
                                                 &job->spec_json);
    /* move-in: rt -> job->future_state / cancel_chan / msg_queue */
    if (job->msg_cb_fn)    ft_attach_msg_worker(tunnel, job);
    if (job->result_cb_fn) ft_attach_result_worker(tunnel, job);
    return 0;
}
```
