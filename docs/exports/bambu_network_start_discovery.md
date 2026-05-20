# `bambu_network_start_discovery`

**Group:** discovery
**RVA:** `0x1bc780` &nbsp; **VA:** `0x1801bc780`
**Body size:** 23 bytes &nbsp; **Direct callees:** 1

## Signature

```c
int bambu_network_start_discovery(agent_t *agent);
```

## Behavior

Start the SSDP-style LAN discovery listener. Each discovered printer is reported via `set_on_ssdp_msg_fn`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

There is no companion `stop_discovery`, the listener runs for the agent's lifetime once started.
