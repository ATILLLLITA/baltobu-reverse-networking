# `bambu_network_refresh_connection`

**Group:** printer connection
**RVA:** `0x1bb3a0` &nbsp; **VA:** `0x1801bb3a0`
**Body size:** 41 bytes &nbsp; **Direct callees:** 1

## Signature

```c
int bambu_network_refresh_connection(agent_t *agent);
```

## Behavior

Force a reconnect of any drowsy / disconnected sessions. Useful after the system wakes from sleep, switches network, or the user explicitly hits a 'reconnect' button.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

