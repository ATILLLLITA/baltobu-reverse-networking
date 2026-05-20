# `ft_job_create`

**Group:** `ft_*` lifecycle (jobs)
**RVA:** `0x2493f0` &nbsp; **VA (image base 0x180000000):** `0x1802493f0`
**Body size:** 1080 bytes &nbsp; **Direct callees:** ~10
**MSVC mangling:** none (plain C export)

## Signature

```c
int ft_job_create(const char *spec_json, ft_job **out_job);
```

`spec_json` is a NUL-terminated JSON string describing the job to
create. `out_job` is a caller-owned pointer that receives the new
job handle on success.

## Behavior

1. If either argument is NULL, returns `-1`.
2. Parse `spec_json` as JSON into a `nlohmann::json`-like internal
   structure (the call to `FUN_1800fa1f0` is the JSON parser; the
   library appears to use nlohmann/json's tagged-union value
   representation, the type bytes `0x04` (bool), `0x05/0x06`
   (int), `0x07` (double) visible in the recovered body match
   nlohmann's value types).
3. Look up the `"cmd_type"` key in the parsed object and extract
   it as a 32-bit unsigned integer. If `cmd_type` is missing or of
   the wrong JSON type, the function calls a no-return error
   helper (`FUN_180566477` with a "type must be number" message).
4. `operator new` a 0x118-byte (280-byte) `ft_job` struct (matching
   the size released by [`ft_job_release`](ft_job_release.md)).
5. Initialise the struct:
   - `refcount = 1`
   - the parsed JSON object handle is moved into a slot
   - several std::string and queue members are
     default-constructed
   - the extracted `cmd_type` is stored at offset `+0x14`
   - a fresh internal-state member is allocated at `+0x88`
     (`FUN_180565ed6(..., 2)`, likely a two-counter atomic init)
6. Build the job's `name` field at offset `+0x60` as
   `"C API " + cmd_type_string` (the formatter call to
   `FUN_18023cda0` returns the human-readable cmd_type string for
   the given enum value).
7. Write the new job pointer to `*out_job`. Free the temporary
   strings used during construction. Return `0`.

## Side effects

- One `operator new(0x118)` plus auxiliary heap allocations for the
  parsed JSON tree, the `name` string, etc.
- May call into an MSVC `__cxa_throw`-like no-return error helper
  if the spec is malformed.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | `*out_job` was set; the caller now owns the reference. |
| `-1` | Either argument was NULL. |

## Notes

- The caller is responsible for `ft_job_release`ing the returned
  job exactly once.
- The job is **inert** after creation, to actually run it, attach
  to a tunnel via [`ft_tunnel_start_job`](ft_tunnel_start_job.md).
- The `cmd_type` enum is internal to the library; the slicer
  presumably stores symbolic names (`"upload"`, `"download"`,
  `"slice"`, …) for the JSON spec and lets the library map them
  via `FUN_18023cda0` (which is the enum-to-string helper). We do
  not yet have the enum decoded; that lives in `.rdata`.
- The `Instruction overlap` warning at `0x1802496a3` is a Ghidra
  artefact, not protector anti-disassembly. Decompilation correct.
- Malformed JSON (missing/wrong `cmd_type`) crashes the process
  via a no-return error path. The library does not surface a
  recoverable error code for spec validation.

## Implementation notes for the clean-room

```c
int ft_job_create(const char *spec_json, ft_job **out) {
    if (!spec_json || !out) return -1;
    json spec = json::parse(spec_json);
    auto cmd = spec.at("cmd_type").get<uint32_t>();  // throws on type mismatch
    auto *job = new ft_job{cmd, /* defaults */};
    job->name = std::string("C API ") + ft_cmd_type_name(cmd);
    *out = job;
    return 0;
}
```
