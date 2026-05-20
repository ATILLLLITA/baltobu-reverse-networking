# `bambu_network_get_my_profile`

**Group:** identity
**RVA:** `0x1b9160` &nbsp; **VA:** `0x1801b9160`
**Body size:** 409 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_get_my_profile(agent_t *agent, std::string *out, /* opaque */, /* opaque */);
```

## Behavior

Synchronous-style fetch (returns a JSON profile blob in `*out`) of the logged-in user's profile. Two trailing opaque parameters (both 64-bit) appear unused in the recovered body, likely future-reserved or used only by a guarded code path.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Distinct from `get_user_info` (async, callback-delivered): this one is the immediate-return form.
