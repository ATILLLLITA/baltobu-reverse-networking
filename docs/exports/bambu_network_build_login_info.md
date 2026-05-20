# `bambu_network_build_login_info`

**Group:** auth / login
**RVA:** `0x1b6660` &nbsp; **VA:** `0x1801b6660`
**Body size:** 64 bytes &nbsp; **Direct callees:** 1

## Signature

```c
void bambu_network_build_login_info(std::string *out, agent_t *agent);
```

## Behavior

Same pattern as `build_login_cmd`. Returns the metadata payload (client info, slicer version, OS, country) that accompanies the login command, separate from the command itself, presumably transmitted as a second MQTT message or used as the HTTP request body in the HTTP-based login path.

