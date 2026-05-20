# `bambu_network_get_subtask_info`

**Group:** print job submission
**RVA:** `0x1ba020` &nbsp; **VA:** `0x1801ba020`
**Body size:** 123 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_get_subtask_info(agent_t *agent, std::string subtask_id, std::string *out);
```

## Behavior

Like `get_subtask` but indexes by the subtask ID directly (rather than walking down from the parent task). Use when you have the subtask ID in hand.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

