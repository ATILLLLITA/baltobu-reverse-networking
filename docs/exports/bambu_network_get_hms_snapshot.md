# `bambu_network_get_hms_snapshot`

**Group:** Health Management System
**RVA:** `0x1b8810` &nbsp; **VA:** `0x1801b8810`
**Body size:** 194 bytes &nbsp; **Direct callees:** 1

## Signature

```c
int bambu_network_get_hms_snapshot(agent_t *agent, std::string dev_id, std::string region, std::string *out);
```

## Behavior

Fetch the printer Health Management System (HMS) snapshot, a JSON blob containing the device's recent errors / warnings.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

`region` selects the localised text variant of the HMS messages (e.g. `"en"`, `"zh-cn"`). The output blob is JSON; the slicer parses it to render error UI.
