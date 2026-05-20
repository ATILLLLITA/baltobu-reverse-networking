# `bambu_network_set_extra_http_header`

**Group:** configuration (HTTP)
**RVA:** `0x1bbd30` &nbsp; **VA:** `0x1801bbd30`
**Body size:** 173 bytes &nbsp; **Direct callees:** 5

## Signature

```c
int bambu_network_set_extra_http_header(agent_t *agent,
                                     std::map<std::string,std::string> *headers);
```

## Behavior

The headers argument is an MSVC `std::map<std::string,std::string>` (96-byte object, red-black tree header with self-referencing sentinel). The library copies the map's contents into its own per-request header set so every outbound HTTP request includes them. Self-check + map cleanup on both paths.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Typical use: add `User-Agent`, `X-BBL-Slicer-Version`, custom tracking IDs. A subsequent call replaces the entire set; partial updates are not supported.
