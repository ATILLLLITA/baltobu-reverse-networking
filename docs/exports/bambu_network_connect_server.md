# `bambu_network_connect_server`

**Group:** printer connection (cloud)
**RVA:** `0x1b6be0` &nbsp; **VA:** `0x1801b6be0`
**Body size:** 280 bytes &nbsp; **Direct callees:** 5

## Signature

```c
int bambu_network_connect_server(agent_t *agent);
```

## Behavior

Open the cloud MQTT broker connection. Required before any cloud-mode pub/sub. The `set_on_server_connected_fn` callback fires when the connection is up.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Distinct from `connect_printer`: that's the LAN per-printer session; this is the single cloud MQTT broker session that carries all cloud-routed traffic.
