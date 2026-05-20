# Library-internal error code enumeration

Recovered by scanning the 876-function transitive helper closure
for assignments to the return-value register of negative int32
constants. Each row is a return code; the right column lists the
first-level helper functions that use it (and through them, the
exports they back).

These are **library-internal** error codes, what the slicer
sees from `bambu_network_*` / `ft_*` return values. They are
distinct from:

- **HTTP status codes** (standard 4xx/5xx, observable in
  `set_on_http_error_fn` callback)
- **MQTT broker codes** (Paho's `return_code` + `reason_code`,
  logged but not interpreted)
- **Bambu cloud / device application codes** in JSON `err_code`
  fields (e.g. the `0x05022647..0x0502264c` enc_msg codes, see below)

| Code | Frequency | First-level exports using it |
|---:|---:|---|
| `-26` | 6 | (internal helpers only) |
| `-19` | 1 | (internal helpers only) |
| `-18` | 7 | (internal helpers only) |
| `-17` | 2 | (internal helpers only) |
| `-16` | 1 | (internal helpers only) |
| `-14` | 1 | (internal helpers only) |
| `-13` | 1 | (internal helpers only) |
| `-12` | 1 | (internal helpers only) |
| `-11` | 2 | (internal helpers only) |
| `-10` | 1 | (internal helpers only) |
| `-9` | 2 | (internal helpers only) |
| `-8` | 1 | (internal helpers only) |
| `-6` | 2 | (internal helpers only) |
| `-5` | 2 | (internal helpers only) |
| `-4` | 2 | (internal helpers only) |
| `-3` | 2 | (internal helpers only) |
| `-2` | 9 | (internal helpers only) |
| `-1` | 161 | `ft.job_cancel`, `ft.job_create`, `get_model_instance_id`, `get_model_mall_home_url`, `get_model_mall_rating` (+15 more) |

## Bambu application error codes

From the `enc_msg` (encrypted-message handshake) handler:

| Code (decimal) | Code (hex) | Documented meaning |
|---:|---|---|
| 84033543 | `0x05022647` | (unspecified, encrypted-message verification error 1) |
| 84033544 | `0x05022648` | (unspecified, error 2) |
| 84033545 | `0x05022649` | **need reset device pub key** (per log string) |
| 84033546 | `0x0502264A` | (unspecified, error 4) |
| 84033547 | `0x0502264B` | (unspecified, error 5) |
| 84033548 | `0x0502264C` | (unspecified, error 6) |

These are 32-bit integers where the high byte (`0x05`) appears
to be a category and the lower bytes a specific code within it.
The library logs them as `"enc_msg: err_code={}, ..."` and
handles 84033545 specifically (logs `need reset device pub key`
and presumably triggers a cert re-issuance flow); the others
fall through a generic "unknown err_code" path.
