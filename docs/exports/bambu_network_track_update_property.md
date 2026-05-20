# `bambu_network_track_update_property`

**Group:** telemetry
**RVA:** `0x1bdf20` &nbsp; **VA:** `0x1801bdf20`
**Body size:** 846 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_track_update_property(agent_t *agent, std::string key, std::string value, /* opaque */ void *flags);
```

## Behavior

Set the value of a single global telemetry property (`key`). Similar to `track_header` but per-key instead of replacing the entire header set. The fourth argument is unidentified, likely a flag set (e.g. 'persist to disk', 'send immediately').

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Persistent properties: surface client identity, AB-test buckets, user-segment information.
