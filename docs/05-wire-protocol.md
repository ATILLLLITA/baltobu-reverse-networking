# Wire protocol

This document describes the JSON schemas the library builds or parses
on the wire, the authentication headers it attaches to HTTP requests,
the MQTT command vocabulary, the FT command envelopes, and the error
codes the library either returns to the caller or interprets from
server responses.

The schemas are recovered statically from the unpacked dump. The
recovery method is documented at the end of this file.

## Authentication

### Bearer token

Every authenticated HTTP request adds:

```
Authorization: Bearer %s
```

The token is an opaque access-token string. The library treats it as a
string and does not parse it; in practice it is a JWT issued by
Bambu's cloud.

### AWS-Sigv4

The library implements AWS-Sigv4 signing for direct uploads to S3 or
OSS:

```
Authorization: %s4-HMAC-SHA256 Credential=%s/%s, SignedHeaders=%s, Signature=%s
```

The same code path serves both Aliyun OSS and AWS S3; only the host,
bucket, and credential bundle differ.

### Custom request headers

The library decorates every cloud request with these:

```
X-BBL-Agent-OS-Type
X-BBL-Agent-Version
X-BBL-Client-ID
X-BBL-Device-ID
X-BBL-Executable-info
X-BBL-Language
```

## OAuth and cloud bootstrap

### Token response

The response to `POST /api/sign-in/ticket?to=...` is parsed at
`0x180189070`. Each `j["key"] = ...` assignment is preceded by an
`nlohmann::json::value_t` tag store, which yields the type for each
key.

```jsonc
{
  "account":            string,    // email or username
  "user": {                        // nested object
    /* values: string, int, string, int, string, int, string, string, string */
  },
  "user_id":            string,
  "name":               string,
  "avatar":             string,    // URL
  "token":              string,    // access token; attaches as Authorization: Bearer
  "refresh_token":      string,
  "expires_in":         int,       // access_token lifetime in seconds
  "refresh_expires_in": int,
  "login_status":       int,
  "autotest_token":     string
}
```

`0x18017a8f0` references the same twelve keys. `0x18018a4f0` references
all of them plus `data` and `uid`; that is an alternative response
shape used in a different code path, and the slicer must treat both.

### Token refresh

The refresh logic is observable from `.rdata` log messages. The
control loop is:

```
loop:
    if access_token expires soon  -> log "need new access token, left time = N"
                                       POST /user-service/user/refreshtoken
                                       receive new {token, refresh_token, expires_in, refresh_expires_in}
    else if refresh_token expires soon -> (presumably re-login)
    else                          -> log "do not need new access token, left time = N"
```

The failure path emits
`"request_refreshtoken failed, error={}, body={}, status={}"`.

### Cloud-environment payload

After login, the cloud returns a configuration object telling the
client which region's endpoints to use. Function `0x18015c970` reads
it:

```jsonc
{
  "apix":         string,       // REST API host
  "iot":          string,       // IoT MQTT endpoint
  "emqx":         string,       // EMQ X MQTT broker
  "login":        string,       // login portal
  "tutk":         string,       // peer-to-peer NAT traversal endpoint
  "e-improved":   string,
  "base_domain":  string,       // "bambulab.com" or "bambulab.cn"
  "environment":  string,       // "production" or similar
  "timezone":     string,
  "command":      "studio_userlogin",
  "sequence_id":  string,
  "wifi":         { /* possibly per-region settings */ }
}
```

This explains how the same DLL serves both international and China
regions. The cloud tells the client which endpoints to use rather than
the client hard-coding them. The hard-coded URLs in
[`03-architecture.md`](03-architecture.md) are defaults and fallbacks.

## MQTT command vocabulary

### Login (`studio_userlogin`)

Sent by the slicer to initiate a cloud login session. Recovered from
the body of `bambu_network_build_login_cmd`'s helper at
`FUN_18015c620`:

```jsonc
{
  "command":     "studio_userlogin",
  "sequence_id": "10001",
  "data": {
    "name":   string,    // from AccountManager's cached user record
    "avatar": string
    // additional nested fields exist; the exact shape depends on build
  }
}
```

The `sequence_id` is a string, not a number. The literal `"10001"` is
hard-coded for the login command.

### Logout (`studio_useroffline`)

```jsonc
{
  "command":     "studio_useroffline",
  "sequence_id": string
}
```

### Push status (`push_status`)

The printer-state push, emitted by the printer over MQTT and consumed
by the slicer via `set_on_message_fn`. Function `0x180192b90` is the
parser; it touches 35 distinct keys. Not every push carries every key;
the union represents the full vocabulary.

```jsonc
{
  "command":      "push_status",
  "sequence_id":  string,
  "stage":        int,           // printing stage code (mapped server-side / firmware-side)
  "subtask_id":   string,
  "subtask_name": string,
  "task_id":      string,
  "project_id":   string,
  "profile_id":   string,
  "url":          string,        // current G-code URL
  "file":         string,
  "file_path":    string,
  "md5":          string,
  "use_ams":      bool,
  "ams_mapping":  array,         // AMS slot mappings (filament colour assignments)
  "ams_mapping2": array,         // newer schema variant
  "nozzle_mapping":     array,   // multi-nozzle (H2D) only
  "nozzle_offset_cali": *,
  "bed_type":           string,
  "bed_leveling":       bool,
  "auto_bed_leveling":  *,
  "extrude_cali_flag":  *,
  "extrude_cali_manual_mode": *,
  "flow_cali":          *,
  "vibration_cali":     *,
  "layer_inspect":      *,
  "timelapse":          *,
  "dev_id":             string,
  "dev_ip":             string,
  "result":             string,
  "error_msg":          string,
  "cfg":                object,
  "param":              object,
  "dest_name":          string,
  "dest_storage":       string
}
```

### Print job request

Function `0x18019f910` carries the slicer-to-printer print command,
with overlapping snake_case and camelCase variants for backward
compatibility with older firmware. The vocabulary is a superset of
`push_status` plus `autoBedLeveling`, `extrudeCaliFlag`, and
`nozzleOffsetCali` (camelCase).

### Device subscription report

The per-device entry the agent caches. Function `0x1800f1390` reads
these keys:

```jsonc
{
  "dev_id":          string,
  "dev_ip":          string,
  "dev_name":        string,
  "dev_type":        string,    // printer model code
  "dev_version":     string,    // firmware version
  "dev_signal":      int,       // Wi-Fi signal strength
  "bind_state":      string,    // "bound" | "unbound" | "pending"
  "connect_type":    string,    // "lan" | "cloud"
  "connection_name": string,
  "sec_link":        string     // secure (TLS) link state
}
```

### Bind and login status report

Function `0x18018c4d0` reads the bind status payload:

```jsonc
{
  "command":      string,
  "sequence_id":  string,
  "login":        string,
  "ticket":       string,
  "ssdp_version": string,
  "status":       string,      // see the state machine below
  "stage":        int,
  "dev_id":       string,
  "dev_ip":       string,
  "result":       string,
  "error":        string,
  "error_msg":    string,
  "err_code":     int,
  "recv_err":     int,
  "reason":       string,
  "sec_link":     string
}
```

The bind and login flow has its own server-side error code fields named
`reson_err_code` (Bambu's typo for "reason") and `reson_error`, in
addition to the standard `err_code`.

## REST messages

### Makerworld design publish

Function `0x180177ee0` builds the Makerworld publish payload. The
casing is camelCase (REST follows Makerworld's external conventions),
unlike the MQTT messages which are snake_case. A few keys appear in
both casings inside the same object.

```jsonc
{
  "modelId":             string,
  "designId":            string,
  "profileId":           string,
  "oriModelId":          string,
  "oriProfileId":        string,
  "title":               string,
  "cover":               string,    // URL
  "filamentSettingIds":  array,
  "amsId":               string,
  "amsMapping":          array,
  "ams_id":              string,    // both casings coexist here
  "amsMapping2":         array,
  "amsDetailMapping":    array,
  "useAms":              bool,
  "nozzleInfos":         array,
  "nozzleMapping":       array,
  "nozzleOffsetCali":    *,
  "bedType":             string,
  "bedLeveling":         bool,
  "autoBedLeveling":     *,
  "extrudeCaliFlag":     *,
  "extrudeCaliManualMode": *,
  "flowCali":            *,
  "vibrationCali":       *,
  "layerInspect":        *,
  "plateIndex":          int,
  "slotId":              string,
  "slot_id":             string,
  "cfg":                 object,
  "mode":                string,
  "timelapse":           *,
  "sequence_id":         string,
  "deviceId":            string
}
```

### OSS or S3 credential bundle

Function `0x180181f60` reads the response to
`/my/ossconfig?useType=N` or `/my/s3config?useType=N`:

```jsonc
{
  "accessKeyId":     string,    // STS access key
  "accessKeySecret": string,    // STS secret
  "securityToken":   string,    // STS session token
  "bucketName":      string,
  "endpoint":        string,    // "oss-cn-shanghai.aliyuncs.com" or "s3.amazonaws.com"
  "cdnUrl":          string,    // CDN-fronted URL prefix
  "expiration":      string     // ISO 8601 timestamp
}
```

Aliyun OSS and AWS S3 share this shape; it is the standard STS response.

## FT subsystem messages

The `ft_*` namespace uses its own protocol over the `FTTunnel`
transport, not MQTT. The schemas are built by functions in the
`0x18024X` range. Common envelope:

```jsonc
{
  "cmdtype":     int,        // see the cmd_type enum in 06-internals.md
  "sequence":    int,        // monotonic sequence id
  "req":         object,     // request body (per cmd_type)
  "api_version": string,     // negotiated framer/protocol version
  "result":      string,     // response side
  "peer":        string,     // peer address
  "peer_t":      string      // peer type
}
```

### `file_upload` (cmd_type = 5)

Function `0x18024d970` reads the request body:

```jsonc
"req": {
  "type":         string,
  "path":         string,    // path on the printer
  "dest_name":    string,    // file name
  "dest_storage": string,    // "sdcard" or "internal"
  "storage":      string,
  "override":     bool,
  "total":        int        // total size in bytes
}
```

### `file_download` (cmd_type = 4)

Function `0x180256770` reads the chunk request body:

```jsonc
"req": {
  "frag_id":  int,           // chunk index
  "offset":   int,           // byte offset
  "size":     int,           // chunk size
  "file_md5": string
}
```

The presence of `frag_id`, `offset`, `size`, and `file_md5` per chunk
strongly indicates chunked downloads with MD5 verification per chunk:
the receiver checks each chunk's MD5 to catch corruption rather than
computing one large hash at the end.

### `sub_file` (cmd_type = 2)

Function `0x18024c510`:

```jsonc
{
  "cmdtype":           2,
  "command":           "sub_file",
  "sequence":          int,
  "sequence_id":       string,
  "is_mem_file":       bool,
  "mem_dl_param_size": int,
  "offset":            int,
  "path":              string,
  "target_path":       string,
  "file_rel_path":     string,
  "peer_host":         string,
  "version":           string,
  "req":               object
}
```

## Telemetry events

Function `0x1801c5600` reads telemetry event blobs (matches the
`track_event` export's serialised payload). The endpoint is
`/iot-service/api/user/upload?event_track/slicer=...`.

```jsonc
{
  "evt":        string,         // event name
  "tag":        string,
  "content":    object,         // event-specific payload
  "ts1":        string,         // ISO 8601 timestamp
  "data_id":    string,
  "uuid":       string,         // random per event
  "cli_id":     string,         // client id
  "user_id":    string,
  "device_id":  string,
  "app_name":   "BambuStudio",
  "app_ver":    string,
  "agent_ver":  "02.06.00.50",  // library version
  "os":         "Windows",
  "platform":   string,
  "sv":         string,         // schema version
  "ver":        string,
  "name":       string
}
```

## Field optionality

Each `j["key"]` access via `nlohmann::json::operator[]` was analysed in
control-flow context. Calls at brace depth 1 (outer function body) are
always executed and indicate required fields. Calls inside an `if`,
`while`, or `for` block (depth two or more) indicate optional fields.

Across the 103 keys analysed:

| Class       | Count | Meaning                                                |
|-------------|------:|--------------------------------------------------------|
| Required    | 48    | Every site is at outer depth; present in every message |
| Optional    | 35    | Every site is inside a control-flow block; only present when conditions are met |
| Mixed       | 20    | Some functions treat it as required, others as optional|

The full per-key table is at
[`../output/unpacking/json_keys_optionality.tsv`](../output/unpacking/json_keys_optionality.tsv).

### Required-key examples

These are present in every message that names them:

`apix`, `base_domain`, `country_code`, `cmd_type`, `command`,
`sequence_id`, `accessKeyId`, `accessKeySecret`, `bucketName`,
`cdnUrl`, `endpoint`, `expiration`, `securityToken`, `ams_mapping`,
`ams_mapping2`, `bed_leveling`, `bed_type`, `cfg`, `autoBedLeveling`,
`auto_bed_leveling`, `dev_id`, `dev_ip`, `token`, `refresh_token`,
`user_id`, `expires_in`, `refresh_expires_in`.

### Optional-key examples and reasons

| Key                           | Reason it is optional                                              |
|-------------------------------|--------------------------------------------------------------------|
| `nozzle_mapping`              | Multi-nozzle printers only; single-nozzle prints omit it.          |
| `images`                      | Only present when an image attachment exists.                      |
| `score`                       | Only on rating responses.                                          |
| `ticket`                      | Only present after `request_bind_ticket` issues one.               |
| `recv_err`                    | Only on error responses.                                           |
| `reason`                      | Error-path field.                                                  |
| `bind`, `connect`             | Only in flows where the binding is being modified.                 |
| `dest_name`, `dest_storage`   | Only on file operations with an explicit destination.              |
| `err_code`                    | Only on error responses.                                           |
| `extrude_cali_manual_mode`    | Only when manual calibration is active.                            |
| `force`                       | Only when overriding a prior state.                                |
| `refreshtoken`                | Only on the response to `/user-service/user/refreshtoken`.         |
| `status`, `id`, `type`        | Context-dependent across many subsystems.                          |
| `different_settings_to_system`| Only present when there is a settings delta to report.             |
| `model`, `version`            | Only on device-info responses.                                     |

## Error codes

### Library-internal return codes

Every `bambu_network_*` and `ft_*` export returns an `int`. Non-zero
values are documented here. Recovery method: scan every function in
the transitive helper closure (876 functions at depth three from any
export) for `uVar_ret = 0xffffffXX;` assignments.

| Code  | Hex          | Frequency           | Meaning                                                            |
|-----:|--------------|---------------------|--------------------------------------------------------------------|
| `0`  | `0x00000000` | universal            | success                                                            |
| `-1` | `0xffffffff` | 98 sites / 84 fns    | generic failure (global agent mismatch, NULL arg, helper failure)  |
| `-2` | `0xfffffffe` | 8 sites / 8 fns      | typically "timeout / no message" (e.g. `ft_job_get_msg` queue pop) |
| `-3` | `0xfffffffd` | 1 / 1                | rare, specific failure                                             |
| `-4` | `0xfffffffc` | 2 / 2                | session not connected (e.g. `ft_tunnel_sync_connect`, session != 3)|
| `-5` | `0xfffffffb` | 2 / 2                | rare                                                               |
| `-6` | `0xfffffffa` | 1 / 1                | internal setter rejected (e.g. `set_server_callback` helper failure)|
| `-8` | `0xfffffff8` | 1 / 1                | specific                                                           |
| `-9` | `0xfffffff7` | 2 / 2                | specific                                                           |
| `-10` to `-14` | `0xfffffff6..0xfffffff2` | 1 each | specific failures in specific call paths                |
| `-17`| `0xffffffef` | 2 / 2                | preconditions not met                                              |
| `-18`| `0xffffffee` | 8 sites / 6 fns      | invalid argument (used in several validators)                      |
| `-19`| `0xffffffed` | 3 / 1                | related to -18                                                     |
| `-26`| `0xffffffe6` | 5 / 5                | dev_id or device validation failed (e.g. `bambu_network_bind`)     |

The full attribution table (which export emits which code, at which
callsite) is at
[`../output/unpacking/error_codes.md`](../output/unpacking/error_codes.md).

### Bambu application error codes (`enc_msg`)

The encrypted-message handshake (used for device cert exchange) has
these codes from `.rdata` log literals:

| Code      | Hex          | Documented meaning      |
|----------:|--------------|-------------------------|
| 84033543  | `0x05022647` | verification error 1    |
| 84033544  | `0x05022648` | verification error 2    |
| 84033545  | `0x05022649` | need reset device pub key |
| 84033546  | `0x0502264A` | verification error 4    |
| 84033547  | `0x0502264B` | verification error 5    |
| 84033548  | `0x0502264C` | verification error 6    |

Other codes flow through an `"enc_msg: unknown err_code, payload = {}"`
fallback path. The high byte `0x05` appears to be a category
identifier; we have not seen other categories appear in this build,
but the parsing code is generic.

### Login and bind state machine

The bind and login flow uses string state values in the MQTT
message's `status` field. The values appear adjacent in `.rdata`, which
is the layout MSVC emits for enum-to-string tables:

| State or verb   | Meaning                                            |
|-----------------|----------------------------------------------------|
| `recv_ticket`   | client received a bind ticket from the cloud      |
| `get_ticket`    | request a ticket (verb)                            |
| `post_ticket`   | submit the ticket to the printer (verb)            |
| `wait_auth`     | waiting for printer to authenticate the ticket     |
| `wait_info`     | waiting for the printer to send its info           |
| `wait_printer`  | waiting for the printer's final ack                |
| `wait_time`     | waiting for time-sync exchange                     |
| `recv_err`      | received an error from the printer                 |
| `FAILURE`       | terminal failure state                             |

The failure-mode log fragments tied to these states are:

```
get ticket failed, code={}        // {} is HTTP status of the cloud's response
get ticket timeout
post ticket failed, code={}
parse login report failed, reason={}
receive login report timeout
wait_printer failed, job_info={}
```

## What lives outside this library

The schemas above cover everything the library either constructs or
parses. Three classes of information are deliberately not in the DLL
and need to be sourced elsewhere if a clean-room implementation wants
them:

The mapping of int-valued state codes to human-readable strings (for
example `stage: 5` -> "filament loading") lives in Bambu Studio's
slicer code (the AGPLv3 part), not in this library. The library
transports the integer through transparently.

Server-side application error codes beyond the `enc_msg` set live on
the cloud. When the cloud sends an arbitrary `err_code` field the
library does not branch on, it stuffs it into the callback payload
and forwards it. The codes the library does branch on are documented
above; everything else is opaque.

Printer-firmware semantics for command effects (what changing
`extrude_cali_flag` actually does to the print) live in the printer.
The library carries the field; it does not interpret it.

## Recovery method

For each function in `.text`, the decompiler output was searched for
calls of the form `FUN_1800e6ca0(_, "<KEY>")`. The function
`FUN_1800e6ca0` is `nlohmann::json::operator[](const std::string &)`,
which indexes into a JSON object by key. The literal-string arguments
to those calls are the keys the function touches. The exercise was run
over every function that has a call edge into `FUN_1800e6ca0` (100
functions in this build). The raw output is at
[`../output/unpacking/json_keys_per_function.tsv`](../output/unpacking/json_keys_per_function.tsv).

For each key access the surrounding `j[k] = v` assignment was examined
to recover the value type. MSVC emits the `nlohmann::json::value_t` tag
store as `local_X._0_1_ = N` immediately before the assignment. The
tag values are `3` for string, `5` for int, `6` for unsigned, `7` for
float, `4` for bool, `1` for object, and `2` for array. By correlating
tag-store and operator[] calls 239 confirmed key types were recovered.
Raw at
[`../output/unpacking/json_keys_typed.tsv`](../output/unpacking/json_keys_typed.tsv).

For optionality, the assigned brace-depth of each operator[] callsite
inside its enclosing function was tracked. Depth 1 means outside any
control-flow block (always executed). Depth 2 or more means inside an
`if`, `while`, or `for` block (conditional). The result is the
classification at the top of the optionality section.

For library-internal error codes, every function in the 876-function
transitive-helper corpus was scanned for `uVar_ret = 0xffffffXX;`
assignments. The corpus is at
`output/ghidra_decompile/transitive/` (gitignored).

For Bambu application error codes, the `.rdata` log strings were
filtered for numeric ranges visible inside `enc_msg` log emitters.
The six codes documented above are the ones with explicit log text.

## See also

- [`03-architecture.md`](03-architecture.md) for the high-level system
  map and the REST endpoint inventory.
- [`06-internals.md`](06-internals.md) for the FT `cmd_type` enum, the
  RTTI hierarchy, and the helper map.
- [`../output/unpacking/wire_protocol_surface.txt`](../output/unpacking/wire_protocol_surface.txt) for
  raw REST URL templates and MQTT topic templates.
- [`../output/unpacking/json_keys_per_function.tsv`](../output/unpacking/json_keys_per_function.tsv) for
  per-function key vocabularies.
