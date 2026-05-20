# `bambu_network_get_my_token`

**Group:** identity
**RVA:** `0x1b9300` &nbsp; **VA:** `0x1801b9300`
**Body size:** 421 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_get_my_token(agent_t *agent, std::string *out, /* opaque */, /* opaque */);
```

## Behavior

Returns the user's current access token (likely a JWT) into `*out`. The trailing opaque parameters mirror the pattern in `get_my_profile` and appear unused.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

**Security-sensitive.** The token authenticates HTTP calls; anything that holds it can act as the user. Bambu Studio uses this internally for its custom HTTP requests against the cloud API; clean-room implementations should follow the same pattern.
