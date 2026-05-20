# `bambu_network_get_camera_url`

**Group:** camera streaming
**RVA:** `0x1b8570` &nbsp; **VA:** `0x1801b8570`
**Body size:** 209 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_get_camera_url(agent_t *agent, std::string dev_id, std::string *out);
```

## Behavior

Look up the RTSP / HTTP camera-stream URL for the given printer in `dev_id`. Used by the slicer to render the live camera feed.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `<0` | Internal helper returned an error. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Returns immediately with an internal status code; if non-zero the URL was not produced. URL is the LAN-mode address when LAN is active, otherwise a cloud-relay URL.
