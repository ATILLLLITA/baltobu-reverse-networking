# `bambu_network_check_user_task_report`

**Group:** telemetry consent
**RVA:** `0x1b6770` &nbsp; **VA:** `0x1801b6770`
**Body size:** 41 bytes &nbsp; **Direct callees:** 1

## Signature

```c
int bambu_network_check_user_task_report(agent_t *agent);
```

## Behavior

Same shape as `check_user_report` but for task-report consent, the separate flag that controls whether print-job metadata can be uploaded back to Bambu for the 'community prints' / quality dashboards.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

