# `bambu_network_get_bambulab_host`

**Group:** service URLs
**RVA:** `0x1b8530` &nbsp; **VA:** `0x1801b8530`
**Body size:** 64 bytes &nbsp; **Direct callees:** 1

## Signature

```c
void bambu_network_get_bambulab_host(std::string *out, agent_t *agent);
```

## Behavior

Returns the base Bambu cloud host URL (e.g. `bambulab.cn` / `bambulab.com` depending on the agent's country code) via the hidden first parameter, MSVC `std::string`-by-value convention. On global-mismatch the function still constructs an **empty** `std::string` (length=0, capacity=15, SSO buffer null-terminated) into `*out` and returns; the caller-side error path is just "empty string returned".

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

The URL is computed inside the agent from the configured country code and the active environment. The base cloud host is read-only, there is no companion setter.
