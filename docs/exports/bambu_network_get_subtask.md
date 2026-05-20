# `bambu_network_get_subtask`

**Group:** print job submission
**RVA:** `0x1b9f70` &nbsp; **VA:** `0x1801b9f70`
**Body size:** 173 bytes &nbsp; **Direct callees:** 1

## Signature

```c
int bambu_network_get_subtask(agent_t *agent, std::string task_id, std::string *out);
```

## Behavior

Look up the subtask (single-plate print attempt) details for the given parent task ID. Result JSON in `*out`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

