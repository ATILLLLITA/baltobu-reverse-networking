# `bambu_network_get_model_rating_id`

**Group:** Makerworld
**RVA:** `0x1b8ed0` &nbsp; **VA:** `0x1801b8ed0`
**Body size:** 269 bytes &nbsp; **Direct callees:** 5

## Signature

```c
int bambu_network_get_model_rating_id(agent_t *agent);
```

## Behavior

Initiates an asynchronous lookup of a fresh model-rating ID, used as part of the rating workflow via the agent's internal HTTP layer (`FUN_1801a...`). The function returns immediately with the internal helper's status code; the actual value comes back via the registered message callback (typically `set_on_message_fn` or `set_on_user_message_fn`). On internal-helper failure (negative status), an error log line is formatted: `"{}, model_rating_id returned error:{}"` with the function name and the error code.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error (logged). |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Don't be misled by the `_url` suffix, this is a request trigger, not a synchronous URL accessor. The slicer must have registered the matching message callback before calling this.
