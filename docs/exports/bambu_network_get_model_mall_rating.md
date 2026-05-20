# `bambu_network_get_model_mall_rating`

**Group:** Makerworld
**RVA:** `0x1b8cc0` &nbsp; **VA:** `0x1801b8cc0`
**Body size:** 267 bytes &nbsp; **Direct callees:** 5

## Signature

```c
int bambu_network_get_model_mall_rating(agent_t *agent, std::string model_id, std::string *out);
```

## Behavior

Fetch the aggregate rating data for a Makerworld model into `*out`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

