# `bambu_network_start`

**Group:** agent lifecycle
**RVA:** `0x1bc750` &nbsp; **VA (image base 0x180000000):** `0x1801bc750`
**Body size:** 41 bytes &nbsp; **Direct callees:** 1
**MSVC mangling:** none (plain C export)

## Signature

```c
int bambu_network_start(agent_t *agent);
```

## Behavior

Spins up the agent's runtime, worker threads, the MQTT loop, the
SSDP discovery listener, the cloud connection (if any),
HTTP-keepalive timers, etc.

1. Validate `agent == g_agent`; return `-1` if not.
2. Call the agent's `start` method (`FUN_1801a8670`) on the global
   agent pointer.
3. Return `0`.

## Side effects

- Spawns one or more worker threads (the MQTT pub/sub loop almost
  certainly runs on its own thread; SSDP discovery probably has a
  separate one).
- Opens sockets for any keepalive connections (cloud MQTT, HTTPS
  long-pollers).
- Subsequent callbacks (`set_on_message_fn`, `set_on_ssdp_msg_fn`,
  etc.) become live and may fire from worker threads.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | Workers started. |
| `-1` | The supplied handle did not match the current `g_agent`. |

## Notes

- Required calling order:
  ```text
  set_config_dir (+ other prerequisite setters) →
    create_agent →
    set_on_*_fn family (register every callback BEFORE start) →
    init_log →
    start →
    [the agent is now live; login/connect/MQTT may fire callbacks]
  ```
- Callback setters work in any order *relative to each other* but
  must all be done before `start`, because the worker threads can
  fire any registered callback as soon as they begin running.
- There is no observable "stop" or "pause" export in the catalog ,
  the only way to tear down the workers is
  [`bambu_network_destroy_agent`](bambu_network_destroy_agent.md).
- Same self-check token pattern as the rest of the agent-taking
  exports. See
  [`bambu_network_destroy_agent`](bambu_network_destroy_agent.md)
  for the broader explanation.

## Implementation notes for the clean-room

```c
int bambu_network_start(agent_t *agent) {
    if (g_agent != agent) return -1;
    agent_start_workers(g_agent);  // spawn threads, open MQTT, etc.
    return 0;
}
```
