# `bambu_network_get_user_info`

**Group:** identity
**RVA:** `0x1ba190` &nbsp; **VA:** `0x1801ba190`
**Body size:** 243 bytes &nbsp; **Direct callees:** 5

## Signature

```c
int bambu_network_get_user_info(agent_t *agent);
```

## Behavior

Initiate an asynchronous fetch of the logged-in user's full profile (the JSON blob behind the simple accessors `get_user_id`, `get_user_name`, `get_user_avatar` etc.). Result is delivered via the `set_on_user_message_fn` callback. Errors are logged internally; the return value is the internal helper's status.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Used at startup to refresh the cached profile. Some sub-fields (avatar, nickname) may already be populated from the prior session's persisted state.
