# `bambu_network_start_subscribe`

**Group:** pub/sub
**RVA:** `0x1bd610` &nbsp; **VA:** `0x1801bd610`
**Body size:** 78 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_start_subscribe(agent_t *agent, std::string topic);
```

## Behavior

Start (or restart) the subscription loop, with `topic` as the initial subscription (alternative entry to `add_subscribe`).

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

The single-argument shape suggests this is shorthand for 'subscribe to this one topic and run the loop'. `add_subscribe` is the multi-topic variant.
