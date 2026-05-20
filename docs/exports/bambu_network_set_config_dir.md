# `bambu_network_set_config_dir`

**Group:** configuration (filesystem)
**RVA:** `0x1bbc90` &nbsp; **VA:** `0x1801bbc90`
**Body size:** 71 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_set_config_dir(agent_t *agent, std::string config_dir);
```

The `config_dir` is passed by value following the MSVC `std::string`
ABI (24-byte SSO-aware object on the stack; the callee owns the
destruction).

## Behavior

1. Self-check `if (g_agent != agent)`; on mismatch, still destroy
   the caller's `config_dir` string before returning `-1`.
2. Copy `config_dir` into a local stack buffer.
3. Call the agent's internal setter (`FUN_1801a91e0`) with the
   copied path.
4. Destroy the caller's `config_dir` string. Return `0`.

The path is used as the root directory for any on-disk caches the
library writes, the log file (`init_log`), HTTP response caches,
TLS cert caches, the device-binding ticket store, etc.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | Path stored. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

- Must be called **after** `create_agent` (the self-check requires
  the global to be set) but **before** `init_log` (so the log file
  has a target directory).
- The library does NOT validate that the path exists, is writable
  or contains expected files. Errors surface later as I/O failures
  when the relevant subsystem tries to use it.
- Even on the error path the caller's string is destroyed. This
  is MSVC's "pass by value, callee destroys" convention; the
  cleanup is mandatory whether the function does its primary work
  or not.
