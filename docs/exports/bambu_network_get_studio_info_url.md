# `bambu_network_get_studio_info_url`

**Group:** service URLs
**RVA:** `0x1b9f30` &nbsp; **VA:** `0x1801b9f30`
**Body size:** 64 bytes &nbsp; **Direct callees:** 1

## Signature

```c
void bambu_network_get_studio_info_url(std::string *out, agent_t *agent);
```

## Behavior

Returns the URL Bambu Studio uses for its 'what's new' / version-info page via the hidden first parameter, MSVC `std::string`-by-value convention. On global-mismatch the function still constructs an **empty** `std::string` (length=0, capacity=15, SSO buffer null-terminated) into `*out` and returns; the caller-side error path is just "empty string returned".

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

The URL is computed inside the agent from the configured country code and the active environment. The 'what's new' / version info URL is read-only, there is no companion setter.
