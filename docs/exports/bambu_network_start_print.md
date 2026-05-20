# `bambu_network_start_print`

**Group:** print job submission
**RVA:** `0x1bccd0` &nbsp; **VA:** `0x1801bccd0`
**Body size:** 698 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_start_print(agent_t *agent, void *opts, std::string dev_id, std::string slice_url, std::string job_name);
```

## Behavior

Cloud-mode print submission. Pass a sliced job (referenced by its OSS-uploaded `slice_url`) to printer `dev_id` to begin printing. `opts` is an options-blob pointer (likely flags / cooldown / AMS settings); `job_name` is the human-readable name that surfaces in the slicer's job history.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Async, observe progress via `set_on_message_fn` (printer status events). The slicer is expected to have already uploaded the slice to OSS before calling this.
