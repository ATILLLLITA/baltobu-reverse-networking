# `bambu_network_query_bind_status`

**Group:** device binding
**RVA:** `0x1bb250` &nbsp; **VA:** `0x1801bb250`
**Body size:** 322 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_query_bind_status(agent_t *agent, std::string dev_id, std::string *out);
```

## Behavior

Read back the current bind state of `dev_id` from the cloud: bound / unbound / pending. JSON-shaped reply in `*out`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

