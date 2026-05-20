# `bambu_network_start_local_print`

**Group:** print job submission
**RVA:** `0x1bc7a0` &nbsp; **VA:** `0x1801bc7a0`
**Body size:** 594 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_start_local_print(agent_t *agent, std::string dev_id, std::string gcode_path, std::string job_name);
```

## Behavior

LAN-mode print submission. The G-code is uploaded directly to the printer's local storage and printing starts. Requires `connect_printer` first.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

