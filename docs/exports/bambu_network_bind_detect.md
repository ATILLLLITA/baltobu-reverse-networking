# `bambu_network_bind_detect`

**Group:** device binding
**RVA:** `0x1b6430` &nbsp; **VA:** `0x1801b6430`
**Body size:** 481 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_bind_detect(agent_t *agent, std::string dev_id, std::string dev_ip, std::string access_code, std::string *out);
```

## Behavior

Poll the in-flight bind to learn whether the printer has acknowledged the handshake. Same first arguments as `bind`. Result blob in `*out`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

