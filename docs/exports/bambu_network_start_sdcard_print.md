# `bambu_network_start_sdcard_print`

**Group:** print job submission
**RVA:** `0x1bd130` &nbsp; **VA:** `0x1801bd130`
**Body size:** 577 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_start_sdcard_print(agent_t *agent, std::string dev_id, std::string sd_path, std::string job_name);
```

## Behavior

Print a file that is already on the printer's SD card. No upload step, just sends a command instructing the printer to begin printing the named file.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

