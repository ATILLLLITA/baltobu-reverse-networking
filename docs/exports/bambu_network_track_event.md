# `bambu_network_track_event`

**Group:** telemetry
**RVA:** `0x1bd7a0` &nbsp; **VA:** `0x1801bd7a0`
**Body size:** 608 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_track_event(agent_t *agent, std::string event_name, std::string properties_json);
```

## Behavior

Record one telemetry event with the given `event_name` and a JSON-string `properties_json` of extra key/value attributes. The event is queued internally; the backend flush happens asynchronously. Both `std::string` arguments are pass-by-value and destroyed by the callee.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Expects `track_enable` to have been called first; otherwise the event is dropped silently. The properties string must be valid JSON object syntax (e.g. `"{\"key\":\"value\"}"`).
