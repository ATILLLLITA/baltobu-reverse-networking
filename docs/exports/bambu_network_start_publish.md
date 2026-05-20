# `bambu_network_start_publish`

**Group:** pub/sub
**RVA:** `0x1bcf90` &nbsp; **VA:** `0x1801bcf90`
**Body size:** 406 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_start_publish(agent_t *agent, std::string topic, std::string payload);
```

## Behavior

Publish a single message to the cloud broker on `topic` with the given `payload`. Synchronous-style return; delivery happens asynchronously after return.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

