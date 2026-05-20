# `bambu_network_get_model_mall_detail_url`

**Group:** Makerworld
**RVA:** `0x1b89f0` &nbsp; **VA:** `0x1801b89f0`
**Body size:** 464 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_get_model_mall_detail_url(agent_t *agent, std::string model_id, std::string *out);
```

## Behavior

Compose the Makerworld detail-page URL for `model_id` and store it in `*out`. Internally formats `https://makerworld.com/<lang>/models/<id>` against the agent's configured country code / language.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

`model_id` is the Makerworld model identifier (string of digits). The caller owns the destruction of `*out` and of `model_id`.
