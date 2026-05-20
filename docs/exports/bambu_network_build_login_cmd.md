# `bambu_network_build_login_cmd`

**Group:** auth / login
**RVA:** `0x1b6620` &nbsp; **VA:** `0x1801b6620`
**Body size:** 64 bytes &nbsp; **Direct callees:** 1

## Signature

```c
void bambu_network_build_login_cmd(std::string *out, agent_t *agent);
```

## Behavior

Returns the MQTT command-topic payload that the slicer should publish to initiate a login on the cloud broker, via the hidden first-param `std::string`. On global-mismatch, populates `*out` with an empty string and returns. The returned string is a fully-formed JSON command body, opaque to the slicer; it just publishes it.

## Notes

Paired with `set_on_user_login_fn` for the result. The slicer's flow is: build_login_cmd → publish to the broker → await callback.
