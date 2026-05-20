# `bambu_network_init_log`

**Group:** agent lifecycle
**RVA:** `0x1ba630` &nbsp; **VA (image base 0x180000000):** `0x1801ba630`
**Body size:** 41 bytes &nbsp; **Direct callees:** 1
**MSVC mangling:** none (plain C export)

## Signature

```c
int bambu_network_init_log(agent_t *agent);
```

## Behavior

Initialises the agent's logging subsystem.

1. Validate `agent == g_agent`; return `-1` if not.
2. Call the agent's `init_log` method (`FUN_1801a8590`) on the
   global agent pointer. Likely opens the on-disk log file under
   the config directory (see `bambu_network_set_config_dir`), sets
   the active log level, and attaches the runtime logger.
3. Return `0`.

## Side effects

- Allocates and opens the log file (filesystem I/O).
- Attaches a process-wide logger so subsequent agent calls emit
  log lines.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | Logging initialised. |
| `-1` | The supplied handle did not match the current `g_agent`. |

## Notes

- The required calling order is: **`create_agent` first** (so the
  global exists), then any `set_*` configuration calls in any
  order (`set_config_dir`, `set_country_code`, `set_cert_file`,
  the seventeen `set_on_*_fn` callback setters, …), then
  `init_log` (so the log file picks up the configured directory),
  then `start`. Confirmed by spot-checking
  [`bambu_network_set_config_dir`](#), it also rejects with `-1`
  when `g_agent != passed_arg`, so it cannot precede
  `create_agent` either, contrary to the original catalog
  hypothesis.
- The function takes the agent pointer but actually operates on
  the global, the parameter is a *self-check token*, the same
  pattern used by every other agent-taking export. See
  [`bambu_network_destroy_agent`](bambu_network_destroy_agent.md)
  for the broader explanation of this pattern.

## Implementation notes for the clean-room

```c
int bambu_network_init_log(agent_t *agent) {
    if (g_agent != agent) return -1;
    agent_init_log(g_agent);
    return 0;
}
```
