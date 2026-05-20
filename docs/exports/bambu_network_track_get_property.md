# `bambu_network_track_get_property`

**Group:** telemetry
**RVA:** `0x1bda10` &nbsp; **VA:** `0x1801bda10`
**Body size:** 612 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_track_get_property(agent_t *agent, std::string *out, std::string key, /* opaque */ void *flags);
```

## Behavior

Read back the value of the telemetry property named `key` into `*out`. Returns the internal helper's status code. The fourth argument is the same opaque flag set as in `track_update_property`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Used to confirm what is currently being attached to outgoing events.
