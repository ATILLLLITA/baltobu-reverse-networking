# Bambu Networking SDK — push_status / device-report Field Catalog

Static reverse engineering (no network). Sources:

- Linux `libbambu_networking.so` v01.07.01.04 (Tzeny-CDN) + demangled symtab
- Windows `bnet.bin` v02.06.00.50 (memory image of bambu_networking.dll)

Method: located parser/dispatch symbols in the symtab, then harvested contiguous
JSON-key clusters in `.rodata` via `grep -abo` + `od -An -c`. In ELF the dumped
`.rodata` VA equals the file offset, so byte offsets map directly to keys.

---

## Headline finding (important)

**The networking SDK is a transport/dispatch layer. It does NOT parse the printer
`push_status` telemetry body.** The classic device-report fields
(`nozzle_temper`, `bed_temper`, `mc_print_stage`, `mc_percent`,
`mc_remaining_time`, `gcode_state`, `print_type`, `wifi_signal`,
`ams_rfid_status`, `stg`, `stg_cur`, `print_error`, …) are **absent from both
binaries**.

Evidence:

- `push_status` literal: **not present** in Linux 01.07 at all; present in Windows
  02.06 only **once**, sitting next to `msg` (offset 6299024) — i.e. used as the
  message-type / topic discriminator string, not as a field container.
- `nozzle_temper`, `bed_temper`, `mc_percent`, `gcode_state`, `mc_print_stage`,
  `wifi_signal`, `print_type`, `stg_cur`, `task_id`, `mc_remaining_time`,
  `ams_rfid_status`: **count = 0** in both binaries.
- The MQTT receive path is `mqtt::async_client::on_message_arrived` →
  `AccountManager::set_on_message_fn` / `BambuNetworkAgent::set_on_message_fn`,
  whose callback signature is `void(std::string topic, std::string payload)`.
  The raw JSON payload is forwarded **opaquely** to the host app (Bambu Studio /
  OrcaSlicer), which owns the `push_status` schema. The SDK never destructures it.
- The only `*print_error*` hit in Linux (offset 6958885) is a vendored-C source
  symbol (`free_string`, `ui_lib.c`, `txt_db.c` neighbours), not a JSON key.

So: the 35+ telemetry keys from baltobu's wire-protocol notes and the farm capture
are **real on the wire**, but they are read by the **slicer**, not by this SDK.
This catalog therefore documents what the SDK *itself* reads and writes.

---

## What the SDK actually touches

Three distinct JSON-key clusters were recovered, each from a contiguous
`.rodata` block:

1. **Device cache / connection record** — keys the SDK reads to track each LAN/cloud device.
2. **Print/upload command construction** — outgoing MQTT/HTTP command payload keys the SDK builds.
3. **Config / account / login-report** — keys the SDK reads from cloud auth and saves to `BambuNetworkEngine.conf`.

---

### 1. Per-device cache fields (device record)

Contiguous cluster. Linux @ offset 4935716; Windows @ offset 6259168.
These are the canonical device-record keys (the "dev cache").

| Key               | Linux 01.07 | Windows 02.06 | Notes |
|-------------------|:-----------:|:-------------:|-------|
| `dev_name`        | ✅ | ✅ | device display name |
| `dev_type`        | ✅ | ✅ | model code |
| `dev_signal`      | ✅ | ✅ | LAN/cloud signal strength |
| `dev_ip`          | ✅ | ✅ | LAN IP |
| `dev_id`          | ✅ | ✅ | serial / device id (also appears many other places) |
| `connect_type`    | ✅ | ✅ | lan / cloud |
| `sec_link`        | ✅ | ✅ | secure-link flag |
| `bind_state`      | ✅ | ✅ | bound / free |
| `dev_version`     | ❌ | ✅ | firmware version (added in 02.06 cluster) |
| `connection_name` | ❌ | ✅ | connection label (added in 02.06 cluster) |

Linux cluster order: `dev_name, dev_type, dev_signal, dev_ip, dev_id,
connect_type, sec_link, bind_state` (8 keys).
Windows cluster order: `dev_name, dev_type, dev_signal, dev_ip, dev_id,
connect_type, sec_link, connection_name, dev_version, bind_state` (10 keys).
`dev_version` also exists standalone elsewhere in Linux; the *new* contribution in
02.06 is its inclusion in the device-record cluster plus `connection_name`.

> Matches baltobu's documented cache set; **extends it with `connection_name`**.

---

### 2. Print / upload command-construction keys (SDK → printer, outgoing)

These are keys the SDK assembles when building print, upload, calibration and
file-management commands. They are written, not read from push_status.

Linux cluster @ ~4964700; Windows cluster @ ~6278000.

| Key                        | Linux 01.07 | Windows 02.06 | Category |
|----------------------------|:-----------:|:-------------:|----------|
| `command`                  | ✅ | ✅ | command envelope discriminator |
| `sequence_id`              | ✅ | ✅ | request correlation id (config-context cluster in Linux) |
| `subtask_id`               | ✅ | ✅ | job identity |
| `subtask_name`             | ✅ | ✅ | job identity |
| `task_id` (cloud HTTP form)| ⚠️ inferred | ⚠️ inferred | not a literal `.rodata` key in either build; constructed via `%1%` templates |
| `job_id`                   | ✅ | ⚠️ inferred | print-job tracking |
| `project_file`             | ✅ | ✅ | 3mf project reference |
| `profile_print_3mf`        | ✅ | — | profile print payload |
| `cloud_file`               | ✅ | — | cloud source file |
| `lan_file`                 | ✅ | ✅ | LAN source file |
| `file_path`                | — | ✅ | local file path (storage cmds) |
| `dest_storage`             | — | ✅ | sdcard / emmc target |
| `dest_name`                | — | ✅ | upload destination name |
| `use_ams`                  | ✅ | ✅ | AMS enable flag |
| `ams_mapping`              | ✅ | ✅ | filament-slot mapping |
| `ams_mapping2`             | ❌ | ✅ | second-AMS / dual-nozzle mapping (new) |
| `nozzle_mapping`           | ❌ | ✅ | nozzle→slot mapping (new in 02.06) |
| `bed_type`                 | ✅ | ✅ | build-plate type |
| `bed_leveling`             | ✅ | ⚠️ inferred | pre-print cal flag |
| `flow_cali`                | ✅ | ⚠️ inferred | flow calibration flag |
| `vibration_cali`           | ✅ | ✅ | vibration calibration flag |
| `layer_inspect`            | ✅ | ✅ | first-layer inspection flag |
| `extrude_cali_manual_mode` | ❌ | ✅ | manual extrusion-cal flag (new) |
| `from_normal`              | ✅ | ✅ | print source origin |
| `from_sd_card` / `from_sdcard_view` | ✅ | ✅ | print source origin |
| `request_project_profile_id` | ✅ | — | cloud project/profile request |
| `project_id`               | ⚠️ template | ✅ | publish/cloud project id (`%2%` template in Linux) |
| `profile_id`               | ✅ | ✅ | cloud profile id |
| `param`                    | ✅ (cluster) | ✅ | generic command param blob |

Notes:
- `url`, `file`, `cfg`, `dev_ip`, `error_msg` from baltobu's list are
  SDK-internal command/identity fields, present as templates/strings but not
  in the device-report body.
- 02.06 adds multi-material/multi-nozzle print params: `ams_mapping2`,
  `nozzle_mapping`, `extrude_cali_manual_mode`, plus the storage-management trio
  `file_path` / `dest_storage` / `dest_name`.

---

### 3. Config / account / login-report keys (cloud auth, SDK reads)

Linux cluster @ ~4960900. Read from the login/refresh report and persisted to
`BambuNetworkEngine.conf`.

| Key                  | Linux 01.07 | Windows 02.06 | Category |
|----------------------|:-----------:|:-------------:|----------|
| `account`            | ✅ | ⚠️ inferred | identity |
| `user_id`            | ✅ | ⚠️ inferred | identity |
| `setting_id`         | ✅ | ⚠️ inferred | identity |
| `refresh_token`      | ✅ | ⚠️ inferred | auth (secret — value not extracted) |
| `refresh_expires_in` | ✅ | ⚠️ inferred | auth |
| `login_status`       | ✅ | ⚠️ inferred | auth |
| `autotest_token`     | ✅ | ⚠️ inferred | auth/test |
| `avatar`             | ✅ | ⚠️ inferred | profile |
| `model_id`           | ✅ | ⚠️ inferred | cloud model ref |
| `isModelSave`        | ✅ | ⚠️ inferred | cloud model flag |
| `sequence_id`        | ✅ | ✅ | request correlation |
| `wifi`               | ✅ | ⚠️ inferred | bootstrap endpoint group |
| `tutk`               | ✅ | ⚠️ inferred | P2P (TUTK) endpoint group |
| `iot`                | ✅ | ⚠️ inferred | IoT endpoint group |
| `apix`               | ✅ | ⚠️ inferred | API endpoint group |
| `emqx`               | ✅ | ⚠️ inferred | MQTT broker endpoint group |
| `base_domain`        | ✅ | ⚠️ inferred | region domain |
| `environment`        | ✅ | ⚠️ inferred | env (pre/qa/dev/prod) |
| `e-improved`         | ✅ | ⚠️ inferred | telemetry opt-in |
| `timezone`           | ✅ | ⚠️ inferred | locale |

(Windows column "inferred" = present elsewhere in the image but the exact
contiguous cluster was not re-dumped; the SDK is the same project so the read
set is expected identical plus additions.)

---

## Build delta summary (01.07 → 02.06)

New in Windows 02.06 networking SDK that was NOT in Linux 01.07:

- Device record: `connection_name`, `dev_version` (added to the dev-record cluster).
- Print command: `ams_mapping2`, `nozzle_mapping`, `extrude_cali_manual_mode`
  (multi-AMS / dual-nozzle / manual extrusion-cal).
- Storage management: `file_path`, `dest_storage`, `dest_name`.

Removed/absent in 02.06 cluster vs 01.07: `profile_print_3mf`, `cloud_file`,
`request_project_profile_id` were not seen in the 02.06 command cluster (may have
moved or been refactored).

---

## Reconciliation with prior notes

- **baltobu wire-protocol 35+ push_status keys** — these are the printer→host
  telemetry schema. **Confirmed real on the wire, but owned by the slicer, not by
  this SDK.** The SDK only stamps the `push_status` / `msg` envelope and forwards
  the payload via the `on_message` callback.
- **Farm-capture push_status keys** (`nozzle_temper`, `bed_temper`,
  `mc_print_stage`, `mc_percent`, `mc_remaining_time`, `gcode_state`,
  `print_type`, `wifi_signal`, `ams_rfid_status`, `stg`, `stg_cur`, `command`,
  `msg`, `sequence_id`, `print_error`) — only `command`, `msg`, `sequence_id`
  appear in the SDK (envelope/correlation). The telemetry fields are not parsed
  here; consume them in the host app off the `on_message` callback payload.
- **baltobu device cache** — confirmed and **extended** with `connection_name`.

No secrets are reproduced in this document (token *values* were never extracted;
only key *names* are listed).

⚠️ = inferred / not directly confirmed as a contiguous `.rodata` literal in that build.
