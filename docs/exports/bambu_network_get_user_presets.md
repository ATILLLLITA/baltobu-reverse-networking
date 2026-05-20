# `bambu_network_get_user_presets`

**Group:** user data
**RVA:** `0x1ba310` &nbsp; **VA:** `0x1801ba310`
**Body size:** 46 bytes &nbsp; **Direct callees:** 1

## Signature

```c
int bambu_network_get_user_presets(agent_t *agent, std::function<void(...)> *cb);
```

## Behavior

Initiates an async fetch of the user's print-preset library (filaments / processes / printers configured in Bambu Studio's preset manager). The result delivered through the provided callback `cb` (an MSVC `std::function`); the function copies the callback by value via its vtable.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Distinct from the agent's preset-cache: this triggers a server round-trip and then invokes the callback with the parsed result.
