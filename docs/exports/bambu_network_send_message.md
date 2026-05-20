# `bambu_network_send_message`

**Group:** messaging (cloud)
**RVA:** `0x1bb6d0` &nbsp; **VA:** `0x1801bb6d0`
**Body size:** 634 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_send_message(agent_t *agent, std::string dev_id, std::string payload, uint32_t qos, uint32_t retain);
```

## Behavior

Send an MQTT message to the printer `dev_id` via the cloud broker. `payload` is the body; `qos` is the MQTT QoS level (0/1/2); `retain` is the MQTT retain flag.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Distinct from `send_message_to_printer` (LAN-direct). This one routes via the cloud broker, works even if the printer is off-network from the slicer.
