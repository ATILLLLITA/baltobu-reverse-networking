# `bambu_network_del_subscribe`

**Group:** pub/sub
**RVA:** `0x1b81a0` &nbsp; **VA:** `0x1801b81a0`
**Body size:** 78 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_del_subscribe(agent_t *agent, std::string topic);
```

## Behavior

Mirror of `add_subscribe`: remove a topic from the subscription list. Unsubscribe is sent to the broker on the next loop pass.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

