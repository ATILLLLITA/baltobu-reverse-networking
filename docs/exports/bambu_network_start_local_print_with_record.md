# `bambu_network_start_local_print_with_record`

**Group:** print job submission
**RVA:** `0x1bca00` &nbsp; **VA:** `0x1801bca00`
**Body size:** 720 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_start_local_print_with_record(agent_t *agent, std::string dev_id, std::string gcode_path, std::string job_name, std::string opts);
```

## Behavior

Variant of `start_local_print` that also creates a cloud-side print record (so the print appears in the user's task history even though the data flow was LAN-only).

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

`opts` carries the recording configuration (whether to capture time-lapse video, snapshot intervals, etc.).
