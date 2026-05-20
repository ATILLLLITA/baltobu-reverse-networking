# `bambu_network_get_user_tasks`

**Group:** user data
**RVA:** `0x1ba4a0` &nbsp; **VA:** `0x1801ba4a0`
**Body size:** 322 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_get_user_tasks(agent_t *agent, std::string after_id, std::string *out);
```

## Behavior

Fetch a page of the user's task list (cloud-side print jobs) starting after `after_id` for pagination. Result blob (JSON) is stored in `*out`. `after_id` empty means "start from the most recent".

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Result includes task IDs, statuses, model thumbnails and timestamps. Used by the slicer's history UI.
