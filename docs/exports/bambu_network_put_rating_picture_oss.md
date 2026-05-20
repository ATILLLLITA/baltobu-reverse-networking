# `bambu_network_put_rating_picture_oss`

**Group:** cloud media
**RVA:** `0x1badc0` &nbsp; **VA:** `0x1801badc0`
**Body size:** 493 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_put_rating_picture_oss(agent_t *agent, std::string local_path, std::string *out_url);
```

## Behavior

Upload a model-rating photo to Alibaba OSS. The library:

1. Asks the cloud for short-lived OSS credentials (same flow as `get_oss_config` with scene = 'rating-picture').
2. Uploads the file at `local_path` directly to OSS.
3. Returns the resulting public URL in `*out_url`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

