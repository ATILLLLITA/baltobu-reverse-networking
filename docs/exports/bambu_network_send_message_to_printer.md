# `bambu_network_send_message_to_printer`

**Group:** messaging (LAN)
**RVA:** `0x1bb950` &nbsp; **VA:** `0x1801bb950`
**Body size:** 657 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_send_message_to_printer(agent_t *agent, std::string dev_id, std::string payload, uint32_t qos, uint32_t retain);
```

## Behavior

LAN-direct counterpart to `send_message`. Requires `connect_printer` to have established the LAN session.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Recovered body has an `Instruction overlap` warning (Ghidra artefact in the cleanup path; the function decompiles cleanly otherwise).
