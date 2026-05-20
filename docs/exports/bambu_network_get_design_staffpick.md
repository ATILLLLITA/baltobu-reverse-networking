# `bambu_network_get_design_staffpick`

**Group:** Makerworld
**RVA:** `0x1b8750` &nbsp; **VA:** `0x1801b8750`
**Body size:** 192 bytes &nbsp; **Direct callees:** 1

## Signature

```c
int bambu_network_get_design_staffpick(agent_t *agent, uint32_t page, uint32_t size, std::string *out);
```

## Behavior

Fetch a page of Makerworld's editorial 'staff-pick' list. `page` is 1-based; `size` is the number of entries per page. Result is a JSON blob stored in `*out`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Typical usage is in the slicer's gallery / Browse view. The JSON shape matches Makerworld's `/api/staffpick` REST endpoint.
