# `bambu_network_get_mw_user_preference`

**Group:** Makerworld
**RVA:** `0x1b90a0` &nbsp; **VA:** `0x1801b90a0`
**Body size:** 153 bytes &nbsp; **Direct callees:** 1

## Signature

```c
int bambu_network_get_mw_user_preference(agent_t *agent, std::function<void(std::string)> *cb);
```

## Behavior

Async fetch of the user's Makerworld preferences (preferred filaments / hidden categories / language / ...). Delivered via the provided `std::function` callback, which the function copies and stores until the result arrives.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Used by Makerworld's gallery to tailor results to the user.
