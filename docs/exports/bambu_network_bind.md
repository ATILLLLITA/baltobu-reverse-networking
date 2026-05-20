# `bambu_network_bind`

**Group:** device binding
**RVA:** `0x1b5f70` &nbsp; **VA:** `0x1801b5f70`
**Body size:** 1213 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_bind(agent_t *agent, std::string dev_id, std::string dev_ip, std::string access_code, std::string token, uint8_t flag, std::string *out);
```

## Behavior

Initiate the full bind handshake: the slicer hands over the printer's LAN IP, its on-device access code, and the bind ticket (`token`). The library uses these to perform a TLS handshake with the printer and register the binding cloud-side. Result reported via `*out` (a status/JSON blob) and/or via callback.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Body has an `Instruction overlap` warning (Ghidra artefact, not anti-disassembly). 7 parameters total. The `flag` byte is likely a 'force' or 'is-LAN-only' option.
