# `bambu_network_set_country_code`

**Group:** configuration (locale)
**RVA:** `0x1bbce0` &nbsp; **VA:** `0x1801bbce0`
**Body size:** 78 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_set_country_code(agent_t *agent, std::string country_code);
```

## Behavior

Pass-by-value `std::string` (typically a 2-letter ISO 3166-1 code, matching what `set_get_country_code_fn`'s callback returns). The library uses the code for URL building, regional Makerworld endpoints and TLS cert pinning. Self-checks the global agent and destroys the caller's string in both success and error paths (MSVC pass-by-value convention).

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Set after `create_agent` but before any HTTP / MQTT activity. A subsequent call replaces the previous value.
