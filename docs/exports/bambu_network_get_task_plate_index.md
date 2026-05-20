# `bambu_network_get_task_plate_index`

**Group:** print job submission
**RVA:** `0x1ba0a0` &nbsp; **VA:** `0x1801ba0a0`
**Body size:** 98 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_get_task_plate_index(agent_t *agent, std::string task_id, std::string *out);
```

## Behavior

Return the current plate index being printed in the given task. Used by the slicer to track multi-plate print progress.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

