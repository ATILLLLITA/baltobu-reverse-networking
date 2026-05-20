# `bambu_network_add_subscribe`

**Group:** pub/sub
**RVA:** `0x1b5f20` &nbsp; **VA:** `0x1801b5f20`
**Body size:** 78 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_add_subscribe(agent_t *agent, std::string topic);
```

## Behavior

Register interest in an MQTT topic. The library will subscribe on the next `start_subscribe` (or immediately if subscribe-loop is running). Subscription failures arrive via `set_on_subscribe_failure_fn`; subscribed messages arrive via `set_on_message_fn`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

