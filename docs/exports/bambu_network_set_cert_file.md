# `bambu_network_set_cert_file`

**Group:** configuration (TLS)
**RVA:** `0x1bbbf0` &nbsp; **VA:** `0x1801bbbf0`
**Body size:** 149 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_set_cert_file(agent_t *agent,
                            std::string cert_path,
                            std::string key_path);
```

## Behavior

Two `std::string` args by value. The library uses them as the on-disk paths for a TLS client cert and key, used by the device binding handshake and any MQTT TLS session. Self-checks the global and destroys both caller-side strings even on the error path.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

The paths are interpreted by the runtime, relative paths are resolved against the working directory. Files are read lazily; if missing, TLS handshakes will fail later.
