# `bambu_network_start_send_gcode_to_sdcard`

**Group:** print job submission
**RVA:** `0x1bd380` &nbsp; **VA:** `0x1801bd380`
**Body size:** 651 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_start_send_gcode_to_sdcard(agent_t *agent, std::string dev_id, std::string gcode_path, std::string remote_name);
```

## Behavior

Upload G-code to the printer's SD card *without* starting a print. Useful for queueing files to print later via `start_sdcard_print`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

