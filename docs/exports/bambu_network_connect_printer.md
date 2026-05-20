# `bambu_network_connect_printer`

**Group:** printer connection (LAN)
**RVA:** `0x1b67a0` &nbsp; **VA:** `0x1801b67a0`
**Body size:** 1087 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_connect_printer(agent_t *agent, std::string dev_id, std::string dev_ip, std::string access_code, std::string version, uint8_t flag);
```

## Behavior

Open a LAN-mode session to a bound printer. The library establishes a TLS-protected MQTT connection directly to the printer's LAN IP, authenticated by the device-side access code. Asynchronous: success/failure reported via `set_on_local_connect_fn`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

`version` is the slicer's reported version (lets the printer log compatible-or-not). `flag` is likely a feature-set bitmask.
