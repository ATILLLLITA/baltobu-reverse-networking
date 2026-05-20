# `bambu_network_get_my_message`

**Group:** user data
**RVA:** `0x1b9140` &nbsp; **VA:** `0x1801b9140`
**Body size:** 26 bytes &nbsp; **Direct callees:** 1

## Signature

```c
int bambu_network_get_my_message(agent_t *agent, std::string after_id);
```

## Behavior

Async fetch of the user's notification / inbox messages, starting after `after_id` for pagination. Results delivered via `set_on_user_message_fn`. The `after_id` argument is destroyed by the callee.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Distinct from `set_on_user_message_fn` (which delivers pushed messages as they arrive), this is the explicit-pull variant for history / catch-up.
