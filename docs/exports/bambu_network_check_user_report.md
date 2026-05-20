# `bambu_network_check_user_report`

**Group:** telemetry consent
**RVA:** `0x1b6740` &nbsp; **VA:** `0x1801b6740`
**Body size:** 41 bytes &nbsp; **Direct callees:** 1

## Signature

```c
int bambu_network_check_user_report(agent_t *agent);
```

## Behavior

Trigger a fetch of the user's current consent flags (whether they have agreed to share anonymised usage data with Bambu's analytics). Result delivered via `set_on_user_message_fn`. Distinct from the `track_*` family (which is the under-the-hood event pipeline), `check_user_report` is the user-facing consent switch.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

