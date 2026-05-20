# `bambu_network_get_user_print_info`

**Group:** user data
**RVA:** `0x1ba340` &nbsp; **VA:** `0x1801ba340`
**Body size:** 283 bytes &nbsp; **Direct callees:** 5

## Signature

```c
int bambu_network_get_user_print_info(agent_t *agent);
```

## Behavior

Async fetch of the user's print-history summary (job count, totals, recent prints). Result delivered via `set_on_user_message_fn`. Returns the internal helper's status.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Read-only. Caching strategy: the library may serve from a local cache and refresh in background.
