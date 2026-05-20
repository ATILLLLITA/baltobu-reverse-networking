# `bambu_network_ping_bind`

**Group:** device binding
**RVA:** `0x1ba9b0` &nbsp; **VA:** `0x1801ba9b0`
**Body size:** 427 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_ping_bind(agent_t *agent, std::string dev_id);
```

## Behavior

Keep a previously-established binding alive, the cloud expires bindings that go silent. Lightweight; the slicer can call this on a long-period timer.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Used to prevent the cloud from forgetting a printer that is currently offline but expected to come back.
