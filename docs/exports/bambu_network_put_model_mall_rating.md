# `bambu_network_put_model_mall_rating`

**Group:** Makerworld
**RVA:** `0x1bab60` &nbsp; **VA:** `0x1801bab60`
**Body size:** 603 bytes &nbsp; **Direct callees:** 9

## Signature

```c
int bambu_network_put_model_mall_rating(agent_t *agent, std::string model_id, std::string rating_json);
```

## Behavior

Submit a user's rating + review for the model `model_id`. `rating_json` carries the star rating and any comment text.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

