# `bambu_network_get_slice_info`

**Group:** print job submission
**RVA:** `0x1b9e90` &nbsp; **VA:** `0x1801b9e90`
**Body size:** 160 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_get_slice_info(agent_t *agent, std::string slice_id, std::string *out);
```

## Behavior

Query metadata about a sliced model (estimated print time, filament usage, plate count). Result blob in `*out`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

