# `bambu_network_enable_multi_machine`

**Group:** configuration (feature flag)
**RVA:** `0x1b8510` &nbsp; **VA:** `0x1801b8510`
**Body size:** 21 bytes &nbsp; **Direct callees:** 1

## Signature

```c
void bambu_network_enable_multi_machine(agent_t *agent);
```

## Behavior

Toggle the agent's multi-printer mode. The function has no return value, the global self-check is the only validation, and if it fails the function silently returns without doing anything.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Multi-machine mode allows the agent to maintain MQTT subscriptions and connection state for several printers simultaneously. Disabled by default in older builds; this export flips the flag on. There is no companion disable function, only `destroy_agent` to reset state.
