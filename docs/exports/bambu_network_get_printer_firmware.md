# `bambu_network_get_printer_firmware`

**Group:** device firmware
**RVA:** `0x1b9680` &nbsp; **VA:** `0x1801b9680`
**Body size:** 434 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_get_printer_firmware(agent_t *agent, std::string dev_id, std::string *out);
```

## Behavior

Look up the firmware version string the printer at `dev_id` is currently running. Returned in `*out`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Reads from the agent's cached device state, does NOT roundtrip to the cloud or the printer. If the agent has never seen the printer, the result is empty.
