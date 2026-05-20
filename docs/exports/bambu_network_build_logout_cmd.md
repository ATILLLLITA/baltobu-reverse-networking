# `bambu_network_build_logout_cmd`

**Group:** auth / login
**RVA:** `0x1b66a0` &nbsp; **VA:** `0x1801b66a0`
**Body size:** 64 bytes &nbsp; **Direct callees:** 1

## Signature

```c
void bambu_network_build_logout_cmd(std::string *out, agent_t *agent);
```

## Behavior

Mirror of `build_login_cmd` for logout. Returns the MQTT command payload that initiates a logout. On mismatch, empty string into `*out`.

