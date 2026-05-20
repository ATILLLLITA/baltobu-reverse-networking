# `bambu_network_stop_subscribe`

**Group:** pub/sub
**RVA:** `0x1bd660` &nbsp; **VA:** `0x1801bd660`
**Body size:** 78 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_stop_subscribe(agent_t *agent);
```

## Behavior

Stop the subscribe loop. Existing subscriptions are not torn down client-side; they're simply not delivered until `start_subscribe` runs again.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

