# `bambu_network_install_device_cert`

**Group:** configuration (device cert)
**RVA:** `0x1ba660` &nbsp; **VA:** `0x1801ba660`
**Body size:** 137 bytes &nbsp; **Direct callees:** 3

## Signature

```c
void bambu_network_install_device_cert(agent_t *agent,
                                     std::string cert_pem,
                                     bool flag);
```

## Behavior

Installs a device-side TLS client certificate. The first arg is the agent (used as the global self-check elsewhere; not directly checked here in the recovered body), the second is a PEM-encoded certificate as an MSVC `std::string` by value, the third is a boolean flag (likely 'is for development' or 'force overwrite'). Internally calls `FUN_1801a85a0(g_agent, cert_pem, flag)`. The caller-side string is destroyed before return.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

The recovered body does **not** include a self-check against the global agent, it always invokes the internal helper. That's a small inconsistency with the rest of the agent-taking exports; clean-room callers should still treat this as an agent-scoped operation. The flag's exact meaning is one of TBD set: force-overwrite, dev-build, root-vs-leaf.
