# `bambu_network_request_setting_id`

**Group:** user settings
**RVA:** `0x1bb640` &nbsp; **VA:** `0x1801bb640`
**Body size:** 130 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_request_setting_id(agent_t *agent, std::string region);
```

## Behavior

Initiate a fetch of the user's settings IDs for the given region. Results presumably delivered via `set_on_user_message_fn`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Used by the slicer's settings UI to enumerate the settings the user has stored.
