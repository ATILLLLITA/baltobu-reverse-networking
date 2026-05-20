# `bambu_network_update_cert`

**Group:** configuration (TLS root certs)
**RVA:** `0x1be420` &nbsp; **VA:** `0x1801be420`
**Body size:** 288 bytes &nbsp; **Direct callees:** 5

## Signature

```c
int bambu_network_update_cert(agent_t *agent);
```

## Behavior

Triggers the agent's internal cert-refresh routine (`FUN_1801ab030`). Likely re-downloads the bundled CA list from Bambu's CDN or re-reads the on-disk cert file. Logs a formatted error message if the internal helper returns non-zero (`"{}, bambu_network_update_cert returned error:{}"`).

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Caller should not run this in a tight loop; cert refresh is a heavy operation that may involve HTTPS. Typical use is at startup or on a long-period timer.
