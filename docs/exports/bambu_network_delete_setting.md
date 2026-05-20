# `bambu_network_delete_setting`

**Group:** user settings
**RVA:** `0x1b81f0` &nbsp; **VA:** `0x1801b81f0`
**Body size:** 417 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_delete_setting(agent_t *agent, std::string setting_id);
```

## Behavior

Remove a single user setting by ID.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

