# `bambu_network_del_rating_picture_oss`

**Group:** cloud media
**RVA:** `0x1b8090` &nbsp; **VA:** `0x1801b8090`
**Body size:** 270 bytes &nbsp; **Direct callees:** 6

## Signature

```c
int bambu_network_del_rating_picture_oss(agent_t *agent, std::string oss_url);
```

## Behavior

Delete a previously-uploaded rating picture from OSS by URL.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

