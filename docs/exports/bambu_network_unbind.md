# `bambu_network_unbind`

**Group:** device binding
**RVA:** `0x1be270` &nbsp; **VA:** `0x1801be270`
**Body size:** 419 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_unbind(agent_t *agent, std::string dev_id);
```

## Behavior

Release the binding between the user's account and the printer `dev_id`. Server-side and local cache are updated.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

