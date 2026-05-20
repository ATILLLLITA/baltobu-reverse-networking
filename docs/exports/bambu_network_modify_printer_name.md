# `bambu_network_modify_printer_name`

**Group:** device binding
**RVA:** `0x1ba730` &nbsp; **VA:** `0x1801ba730`
**Body size:** 628 bytes &nbsp; **Direct callees:** 8

## Signature

```c
int bambu_network_modify_printer_name(agent_t *agent, std::string dev_id, std::string new_name);
```

## Behavior

Rename a bound printer (cloud-side display name). Persists server-side.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

