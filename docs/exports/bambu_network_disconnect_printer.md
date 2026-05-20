# `bambu_network_disconnect_printer`

**Group:** printer connection (LAN)
**RVA:** `0x1b83f0` &nbsp; **VA:** `0x1801b83f0`
**Body size:** 285 bytes &nbsp; **Direct callees:** 5

## Signature

```c
int bambu_network_disconnect_printer(agent_t *agent, std::string dev_id);
```

## Behavior

Close the LAN-mode session to `dev_id`. The local-connect callback fires with a disconnect status when teardown completes.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

