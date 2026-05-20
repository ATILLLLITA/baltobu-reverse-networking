# `bambu_network_track_enable`

**Group:** telemetry
**RVA:** `0x1bd6b0` &nbsp; **VA:** `0x1801bd6b0`
**Body size:** 230 bytes &nbsp; **Direct callees:** 5

## Signature

```c
int bambu_network_track_enable(agent_t *agent);
```

## Behavior

Enable the agent's internal telemetry / analytics subsystem. After this call, subsequent `track_event` calls are forwarded to the configured backend; without it, they are silently dropped. There is no companion `track_disable`, telemetry runs until `destroy_agent`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Independent of `bambu_network_report_consent` / `check_user_report`. Those are about user-facing consent flags; `track_*` is the under-the-hood event pipeline.
