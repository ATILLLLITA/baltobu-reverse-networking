# `ft_job_retain`

**Group:** `ft_*` reference counting (jobs)
**RVA:** `0x249cb0` &nbsp; **VA (image base 0x180000000):** `0x180249cb0`
**Body size:** 15 bytes &nbsp; **Direct callees:** 0 (atomic instruction only)
**MSVC mangling:** none (plain C export)

## Signature

```c
void ft_job_retain(ft_job *job);
```

`ft_job` is an opaque pointer obtained from `ft_job_create`. The
first 32 bits of the object pointed to by `ft_job *` are the
reference count, every retain/release pair manipulates this field.

## Behavior

Atomically increments the reference count stored at `*(int *)job`.
Passing `NULL` is a no-op; the function checks `if (job != NULL)`
before touching the refcount.

The atomic operation is `LOCK INC dword ptr [job]` (the disassembly
exposes the `lock`-prefix on the increment), which provides full
sequential consistency on x86-64.

## Side effects

- Increments the refcount of `*job` by exactly 1.
- No other state is touched. No callbacks. No allocations.

## Notes

- This is one half of a CoreFoundation- / GObject- / `shared_ptr`-style
  refcount pair. The matching `ft_job_release` (RVA TBD; documented
  in a sibling file once that batch is written) atomically decrements
  and frees when the count reaches zero.
- The 32-bit refcount width is enough for any practical use of the
  job handle, even an unrealistic 1M ref/sec retain rate would take
  ~71 minutes to overflow. The library does not appear to defend
  against overflow, however; consumers that hold thousands of
  retains would be a bug anyway.
- The retain/release pattern means the consumer is expected to call
  `ft_job_release` exactly once for every reference they hold,
  including the implicit one returned by `ft_job_create`. The
  library may hold additional references internally (e.g. a tunnel
  may retain its in-flight jobs).
- The same atomic-increment pattern is used by `ft_tunnel_retain`,
  and the same release pattern presumably by `ft_tunnel_release`.

## Implementation notes for the clean-room

```c
// pseudo-code; clean-room implementers write this fresh.
void ft_job_retain(ft_job *job) {
    if (job != NULL) {
        // any atomic increment with at least relaxed ordering;
        // the library uses LOCK INC which is full seq_cst, match
        // it for safety unless you can prove the load relaxes.
        __atomic_fetch_add((_Atomic uint32_t *)job, 1, __ATOMIC_SEQ_CST);
    }
}
```

The clean-room implementation must use the **same memory layout** as
the original, i.e. the reference count must sit at offset 0 of the
`ft_job` opaque struct. Callers do not interact with this layout
directly, but it constrains how the rest of the `ft_job` struct can
be laid out internally.
