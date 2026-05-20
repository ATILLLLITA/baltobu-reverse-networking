# `bambu_network_get_camera_url_for_golive`

**Group:** camera streaming
**RVA:** `0x1b8650` &nbsp; **VA:** `0x1801b8650`
**Body size:** 255 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_get_camera_url_for_golive(agent_t *agent, std::string dev_id, std::string golive_id, std::string *out);
```

## Behavior

The 'GoLive' variant: returns the URL for Bambu's livestream feature (`golive_id` is the active livestream session). Otherwise structurally identical to `get_camera_url`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

GoLive sessions are time-bounded; the returned URL may expire.
