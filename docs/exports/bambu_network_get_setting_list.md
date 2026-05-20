# `bambu_network_get_setting_list`

**Group:** user settings
**RVA:** `0x1b9840` &nbsp; **VA:** `0x1801b9840`
**Body size:** 756 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_get_setting_list(agent_t *agent, std::string filter, std::string region, std::string *out);
```

## Behavior

Fetch the list of cloud-side user settings. `filter` selects which subset (e.g. 'all', 'preferences', 'devices'); `region` is the locale for translated descriptions. Result JSON in `*out`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

