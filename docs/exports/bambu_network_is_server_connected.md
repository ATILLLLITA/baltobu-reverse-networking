# `bambu_network_is_server_connected`

**Group:** printer connection (cloud)
**RVA:** `0x1ba6f0` &nbsp; **VA:** `0x1801ba6f0`
**Body size:** 23 bytes &nbsp; **Direct callees:** 1

## Signature

```c
bool bambu_network_is_server_connected(agent_t *agent);
```

## Behavior

Returns `true` (non-zero) when the cloud MQTT broker connection is up. Like `is_user_login`, this is a cheap synchronous accessor.

