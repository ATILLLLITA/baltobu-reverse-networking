# `bambu_network_get_setting_list2`

**Group:** user settings
**RVA:** `0x1b9b40` &nbsp; **VA:** `0x1801b9b40`
**Body size:** 847 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_get_setting_list2(agent_t *agent, std::string filter, std::string region, std::string *out);
```

## Behavior

Newer/revised version of `get_setting_list` with the same outer ABI. The result JSON likely uses a v2 schema (more fields, fewer nested wrappers). The slicer can probe both endpoints, newer Bambu builds use `_list2`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

The presence of two versions is a strong API-evolution marker. Clean-room implementers should support both for slicer compatibility.
