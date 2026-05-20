# `bambu_network_put_setting`

**Group:** user settings
**RVA:** `0x1bafb0` &nbsp; **VA:** `0x1801bafb0`
**Body size:** 668 bytes &nbsp; **Direct callees:** 8

## Signature

```c
int bambu_network_put_setting(agent_t *agent, std::string setting_id, std::string value_json, /* opaque */, /* opaque */);
```

## Behavior

Set the value of a single cloud-side user setting. `setting_id` names the setting, `value_json` is its new value. Both strings destroyed by the callee.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Server-side persisted setting (e.g. 'preferred filament', 'default print quality'). The two trailing opaque parameters are presumably option flags.
