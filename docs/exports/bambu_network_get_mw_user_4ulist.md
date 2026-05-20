# `bambu_network_get_mw_user_4ulist`

**Group:** Makerworld
**RVA:** `0x1b8fe0` &nbsp; **VA:** `0x1801b8fe0`
**Body size:** 192 bytes &nbsp; **Direct callees:** 1

## Signature

```c
int bambu_network_get_mw_user_4ulist(agent_t *agent, uint32_t page, uint32_t size, std::string *out);
```

## Behavior

Fetch a page of the Makerworld 'for you' list (recommended models for the logged-in user). `page` is 1-based; `size` is the number of entries per page. The JSON-shaped blob lands in `*out`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Same shape as `get_design_staffpick`. The '4u' in the name is Bambu's shorthand for '4 you'.
