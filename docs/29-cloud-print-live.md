# Live-validated print flow: task-create body and `project_file` schemes

[`05-wire-protocol`](05-wire-protocol.md) and [`11-print-params`](11-print-params.md) describe the
print path from the static binary. This document records the **live-captured** validation of that
path on a real account/printer (`02.07` plugin), which pins two things the static read leaves
ambiguous: the exact **`POST /my/task` request body**, and the **`project_file` `url` scheme per
delivery mode**.

## `POST /v1/user-service/my/task` — task-create request body

Captured byte-exact from the slicer heap during a real cloud print. Keys are emitted in
**lexicographic order** (it is an `nlohmann::json` ordered-map dump). All identifying values below
are redacted:

```json
{"amsDetailMapping":[{"ams":255,"filamentId":"<fil>","filamentType":"PLA","nozzleId":1,
   "sourceColor":"#RRGGBBAA","targetColor":"#RRGGBBAA"}],
 "amsMapping":[-1],
 "amsMapping2":[{"amsId":255,"slotId":0}],
 "autoBedLeveling":1,"bedLeveling":true,"bedType":"textured_plate","cfg":"0","cover":"",
 "deviceId":"<serial>","extrudeCaliFlag":2,"extrudeCaliManualMode":0,
 "filamentSettingIds":[],"flowCali":false,"layerInspect":true,"mode":"cloud_file",
 "modelId":"<model_id>","nozzleInfos":[],"nozzleOffsetCali":2,"oriModelId":"",
 "oriProfileId":0,"plateIndex":1,"profileId":<profile_id>,"sequence_id":"20000",
 "timelapse":true,"title":"<name>","useAms":false,"vibrationCali":false}
```

Notes:

- `profileId` is a **bare number** (not quoted); `oriProfileId` is an int; `sequence_id` is the
  string `"20000"`.
- `mode` is `"cloud_file"` for cloud, `"lan_file"` for LAN.
- `amsDetailMapping[]` entry shape is `{ams, filamentId, filamentType, nozzleId, sourceColor,
  targetColor}` with `#`-prefixed RRGGBBAA colors. `amsMapping:[-1]` + `amsMapping2:[{amsId:255,
  slotId:0}]` is the "external spool / no AMS" mapping.
- `nozzleMapping` is omitted when empty. The calibration ints (`autoBedLeveling`, `extrudeCaliFlag`,
  `extrudeCaliManualMode`, `nozzleOffsetCali`) and `cfg` (a string) round out the body.

## `project_file` MQTT command — `url` scheme by delivery mode

The print-start command on `device/<serial>/request` is `print.project_file` in every mode; only
the `url` (and a few cloud-only fields) change with **where the gcode lives**:

| Mode | `url` | Notes |
|---|---|---|
| **Cloud** | `https://…s3…amazonaws.com/…/<model>_<profile>_<n>.3mf?X-Amz-Algorithm=AWS4-HMAC-SHA256&…` | direct S3 presigned (SigV4); see [`15-oss-upload-sigv4`](15-oss-upload-sigv4.md). Cloud-only fields observed: `design_id`, `job_id` (int), `cfg`, `extrude_cali_flag`, `extrude_cali_manual_mode`, `nozzle_offset_cali`, `auto_bed_leveling`, plus a trailing `"reason":"success","result":"success"` echo. `job_type:1`, `md5` UPPERCASE hex. |
| **LAN / offline, generic** | `ftp:///<filename>` | file pre-pushed to printer storage; command carries `file`, `subtask_name`, `md5:""`, `bed_type:"auto"`, the four cali booleans, `use_ams`, `ams_mapping:[]`, `profile_id/project_id/task_id/subtask_id:"0"`, `sequence_id`. |
| **eMMC** | (dedicated builder) | the 3MF is first uploaded over the printer's brtc/file channel (below), then started from internal eMMC. |
| **sdcard (legacy X1C)** | `file:///mnt/sdcard/…` | the only path present in the old symbolized `v01.07`. |

`sequence_id` for printer-bound commands is a numeric string cycling **20000 → 90000** then
wrapping back to `20000`.

## eMMC store transport (brtc file channel)

eMMC delivery is a **store-then-print** split. The store is *not* an MQTT `project_file`; it is a
framed file-management exchange on the printer's brtc/file channel (the upload worker connects with
the printer **IP + LAN access code + serial**):

```
# storage capability query (studio -> printer), then reply
{"cmdtype":7,"req":{"api_version":3,"peer":"studio","peer_t":3}}
{"cmdtype":7,"mtype":12289,"reply":{"api_version":3,"storage":["emmc","udisk"],"upload_storage":["emmc","udisk"]},"result":0,"sequence":N}
# upload init + chunked fragments (sum of sizes == total)
{"cmdtype":5,"req":{"path":"<name>.gcode.3mf","storage":"emmc","total":<bytes>,"type":"model"}}
{"cmdtype":5,"frag_id":0,"req":{"offset":0,"size":<n>}}
{"cmdtype":5,"frag_id":1,"req":{"file_md5":"<md5 of whole file>","offset":<n>,"size":<m>}}
{"cmdtype":5,"mtype":12289,"reply":{"chunk_size":<n>,"offset":0}}   # acks; result:0 = OK
```

- `cmdtype 7` = storage caps, `cmdtype 5` = file upload; `mtype:12289` is the reply frame type;
  `type:"model"` for a 3MF; the last/early fragment carries `file_md5` (md5 of the whole file).
- The slicer-side entry is `bambu_network_start_send_gcode_to_sdcard`. Once stored, the print is
  started with a `project_file` whose `url` selects the on-printer copy. See
  [`16-mqtt-channels-framing`](16-mqtt-channels-framing.md) for the surrounding framing.

## Command-security observed live

When a print starts on a secured printer the client publishes an **`app_cert_install`** command on
`device/<serial>/request` (plus `set_airduct` on H2-series, and `stop` for cancel) — the first live
confirmation that the print path exercises the command-security layer documented in
[`27-mqtt-sec-app-cert`](27-mqtt-sec-app-cert.md). The on-the-wire HTTP carries the full
`X-BBL-*` proof header set (`x-bbl-app-certification-id: CN=<cert_id>.bambulab.com:<hex>`,
`x-bbl-device-security-sign: <base64 RSA-2048>`, two distinct version sources for
`X-BBL-Client-Version` vs `X-BBL-Agent-Version`, etc.; see [`19-http-client`](19-http-client.md)).

## Cross-references

- [`28-linux-so-recovery`](28-linux-so-recovery.md) — how these were captured without a debugger.
- [`15-oss-upload-sigv4`](15-oss-upload-sigv4.md), [`20-pushstatus-schema`](20-pushstatus-schema.md),
  [`27-mqtt-sec-app-cert`](27-mqtt-sec-app-cert.md).
