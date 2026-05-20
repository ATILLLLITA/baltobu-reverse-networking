# `bambu_network_get_oss_config`

**Group:** cloud storage
**RVA:** `0x1b94b0` &nbsp; **VA:** `0x1801b94b0`
**Body size:** 462 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_get_oss_config(agent_t *agent, std::string scene, std::string *out, /* opaque */, /* opaque */);
```

## Behavior

Request a short-lived **Alibaba Cloud OSS (Object Storage Service)** credential bundle for the slicer to upload to. `scene` selects which OSS bucket / prefix is requested (e.g. 'avatar', 'rating-picture', 'time-lapse'). The returned JSON `*out` contains an access key, secret, security token, endpoint, bucket name and expiry.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

OSS = Alibaba's S3. Bambu's cloud is hosted on Aliyun. The credentials are scoped to the requesting user and the named scene; they expire within minutes-to-hours. The slicer is expected to upload to OSS directly using these creds, not through Bambu's API.
