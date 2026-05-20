# `bambu_network_track_header`

**Group:** telemetry
**RVA:** `0x1bdc80` &nbsp; **VA:** `0x1801bdc80`
**Body size:** 432 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_track_header(agent_t *agent, std::string headers_json);
```

## Behavior

Set the agent-wide telemetry header, a JSON object of key/value pairs that get attached to every subsequent `track_event` call. Typical contents: client ID, slicer version, OS version, country code.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Headers are sticky across the agent lifetime. A subsequent call replaces the full set.
