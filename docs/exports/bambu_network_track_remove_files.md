# `bambu_network_track_remove_files`

**Group:** telemetry
**RVA:** `0x1bde30` &nbsp; **VA:** `0x1801bde30`
**Body size:** 235 bytes &nbsp; **Direct callees:** 5

## Signature

```c
int bambu_network_track_remove_files(agent_t *agent);
```

## Behavior

Delete the agent's on-disk telemetry buffers from the config directory. Used at user-data-clear time or when telemetry is disabled by user choice.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Idempotent. Does not stop the in-memory event queue from accepting future events, only purges what is already on disk.
