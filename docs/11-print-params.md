# BBL::PrintParams / BBL::PublishParams — struct layout & MQTT `project_file` serialization

Static reverse-engineering of `libbambu_networking.so` (clean Linux build, **v01.07.01.04**,
with symbols). Cross-checked against the unpacked Windows memory image **v02.06.00.50**
(`bnet.bin`) for newer JSON keys. No network, cloud, or printer interaction was performed —
this is pure binary/string analysis. No secrets are present; all recovered tokens are JSON
field names and format strings.

In this `.so`, `.text` and `.rodata` have **VA == file offset** (delta 0), so rip-relative
`lea` targets read directly out of the file. Offsets below come from disassembling the
`PrintParams` copy-ctor and the serializer functions; items that could not be pinned to a
specific struct offset by the disassembler are marked **(inferred)**.

## Source functions

| Symbol | VA | What it does |
|--------|----|--------------|
| `BBL::PrintParams::PrintParams(const&)` | `0x248120` (1626 B) | copy-ctor — reveals full field layout |
| `BBL::PrintParams::~PrintParams()` | `0x248780` | dtor |
| `BBL::AccountManager::start_local_print(PrintParams,…)` | `0x23dc50` (9302 B) | **builds the `project_file` MQTT command** (LAN print) |
| `BBL::AccountManager::start_local_print_with_record(PrintParams,…)` | `0x23bbd0` | FTP upload + `create_task_record`, then calls the above |
| `BBL::AccountManager::start_print(PrintParams,…)` | `0x233d30` (20333 B) | cloud print: POST to `user-service/my/task` |
| `BBL::AccountManager::create_cloud_task_record(PrintParams,…)` | `0x238ca0` | cloud task record (camelCase body) |
| `BBL::AccountManager::start_send_gcode_to_sdcard(PrintParams,…)` | `0x21de60` | gcode-to-SD via SFTP |
| `BBL::AccountManager::start_publish(PublishParams,…)` | `0x213e30` (6108 B) | MakerWorld-style project publish (cloud) |
| `BBL::BambuNetworkAgent::start_*` | `0x25c730…0x25d350` | thin dispatchers to the AccountManager methods |

Only the copy-ctor/dtor of `PrintParams` are exported as members; there is no
`PrintParams::to_json`. Serialization is **open-coded** inside `start_local_print` /
`start_print` using nlohmann::json (`operator[]` with key string literals).

---

## 1. `BBL::PrintParams` field table

Struct is a flat aggregate of `std::string` members (32 B each: SSO ptr at +0, inline buf at
+0x10) plus a trailing scalar/bool block. Total size ≈ **0x2c0**. The copy-ctor copies the
following members in order (offsets are exact, from `0x248120`):

| Offset | Type | JSON key (`project_file`) | Meaning / notes |
|--------|------|---------------------------|-----------------|
| `0x20` | std::string | — | dev_id / target serial (used to build `device/<sn>/request` topic) (inferred) |
| `0x40` | std::string | — | dev_ip (LAN address) (inferred) |
| `0x60` | std::string | — | path / project name region (inferred) |
| `0x80` | std::string | — | task name / 3mf name (inferred) |
| `0xa0` | std::string | — | (inferred) |
| `0xc0` | int (8 B slot) | — | **plate index** — feeds `Metadata/plate_%1%.gcode` for `param` (exact: `lea r13,[rbx+0xc0]` at `0x23ec29`) |
| `0xc8` | std::string | `param` | `"Metadata/plate_<N>.gcode"` (built from plate idx) |
| `0xe8` | std::string | `url` | `file:///…`, `ftp://…` (LAN) — see §3 |
| `0x108`| std::string | `md5` | file MD5 |
| `0x128`| std::string | `subtask_id` / `task_id` | id strings (exact ref `lea …,[rbx+0x128]` at `0x23ea43`) |
| `0x148`| std::string | `subtask_name` | (inferred) |
| `0x168`| std::string | `project_id` | `"0"` when LAN (inferred) |
| `0x188`| std::string | `profile_id` | `"0"` when LAN (inferred) |
| `0x1a8`| int (8 B slot)| — | scalar (inferred — enum/flag) |
| `0x1b0`| std::string | `ams_mapping` | AMS slot mapping JSON/string |
| `0x1d0`| std::string | `bed_type` | e.g. `"auto"` |
| `0x1f0`| std::string | — | (inferred) |
| `0x210`| std::string | — | (inferred) |
| `0x230`| bool | — | flag (movzx at `0x24843b`) |
| `0x238`| std::string | — | (inferred) |
| `0x258`| std::string | — | (inferred) |
| `0x278`| **bool** | `bed_leveling` | one-L key (matches farm capture); movzx feeds key at `0x23f2ff` |
| `0x279`| **bool** | `flow_cali` | flow calibration enable |
| `0x27a`| **bool** | `vibration_cali` | vibration calibration enable |
| `0x27b`| **bool** | `layer_inspect` | first-layer inspection enable |
| `0x27c`| **bool** | `timelapse` | timelapse enable |
| `0x27d`| **bool** | `use_ams` | use AMS |
| `0x280`| std::string | — | (inferred) |
| `0x2a0`| std::string | — | last member (inferred) |

**Bool block is the high-confidence result** — the 6 booleans at `0x278..0x27d` were matched
to JSON keys by aligning each `movzx [rbx+off]` to the immediately-following key `lea` in the
serializer:

```
+0x278 -> "bed_leveling"
+0x279 -> "flow_cali"
+0x27a -> "vibration_cali"
+0x27b -> "layer_inspect"
+0x27c -> "timelapse"
+0x27d -> "use_ams"
```

The string-member → key assignment for `param`/`url`/`md5`/`subtask_id`/`bed_type`/
`ams_mapping` is anchored on exact disassembly hits (`0xc0`, `0xc8`, `0x128`, `0x1b0`,
`0x1d0`); the rest (`project_id`, `profile_id`, `subtask_name`, dev_id/ip/path) are assigned
by serialization order and semantics and marked (inferred).

---

## 2. `BBL::PublishParams` fields

`PublishParams` (passed by value to `start_publish` / `bambu_network_start_publish`) is a
much smaller aggregate. In `AccountManager::start_publish` (`0x213e30`) only `std::string`
members at **+0x20, +0x40, +0x60, +0x80** are touched (32-B stride, same SSO layout):

| Offset | Type | Meaning (inferred from publish flow) |
|--------|------|--------------------------------------|
| `0x20` | std::string | project name / title |
| `0x40` | std::string | 3MF file path on disk (`{} is not eixsts` check, FTP/OSS upload source) |
| `0x60` | std::string | comment / design metadata |
| `0x80` | std::string | dest / id (project or design id region) |

The publish path is **cloud-only** (no MQTT `project_file`): it requests
`%1%/iot-service/api/user/project`, uploads the 3MF (`upload_3mf` → OSS/S3 via
`my/ossconfig` / `my/s3config`), then publishes via
`/my/models/<id>/publish?project_id=…&profile_id=…` and
`/my/profiles/<id>/publish?project_id=…&design_id=…`. The exact public field names of the C
ABI `PublishParams` struct are not present as literals in this build (the struct is filled by
the `bambu_network_start_publish` C wrapper); the four members above are the only ones the
networking layer reads.

---

## 3. How `start_print` serializes into the `project_file` MQTT command

`AccountManager::start_local_print` open-codes the MQTT payload. Recovered key emission order
(rip-relative `lea` references into `.rodata`, in code order from `0x23eaf1`):

```json
{
  "print": {
    "command":      "project_file",
    "sequence_id":  "<n>",                       // AccountManager::m_sequence_id, "%d"
    "param":        "Metadata/plate_<N>.gcode",  // N = plate index, struct +0xc0
    "project_id":   "0",                          // "0" in LAN mode
    "profile_id":   "0",
    "task_id":      "0",
    "subtask_id":   "<id>",
    "subtask_name": "<name>",
    "file":         "",
    "url":          "file:///…",                  // see URL selection below
    "md5":          "<md5>",
    "timelapse":      <bool>,                      // +0x27c
    "bed_type":       "auto"|…,                    // +0x1d0
    "bed_leveling":   <bool>,                      // +0x278  (ONE L)
    "flow_cali":      <bool>,                      // +0x279
    "vibration_cali": <bool>,                      // +0x27a
    "layer_inspect":  <bool>,                      // +0x27b
    "ams_mapping":    "<map>",                     // +0x1b0
    "use_ams":        <bool>                        // +0x27d
  }
}
```

The payload is then published to topic `device/<dev_id>/request` (format `device/%1%/request`
at `0x4bb813`), logged as `print_job: json = {}`.

**URL selection** (the `url` value) branches on the source / storage:

| Source flag | URL form | rodata |
|-------------|----------|--------|
| `from_normal` (local file) | `file:///<path>` or `ftp://<ip>` | `0x4bc3f4`, `0x4bc385` |
| `from_sd_card` | `file:///mnt/sdcard/<name>` | `0x4bc3e0` |
| `from_sdcard_view` | `file:///<name>` | `0x4bc3fd` |

`subtask_id`, `task_id`, `project_id`, `profile_id` are emitted as `"0"` for plain LAN prints
and only become real ids when a cloud task record was created first
(`create_task_record` / `start_local_print_with_record`).

The **cloud** path (`start_print` → `create_cloud_task_record`) uses an entirely different,
**camelCase** body POSTed to `%1%/user-service/my/task`:

```
modelId, title, deviceId, filamentSettingIds, profileId, plateIndex,
timelapse, bedType, bedLeveling, flowCali, vibrationCali, layerInspect,
amsMapping, amsDetailMapping, useAms, oriProfileId, oriModelId
```

(rodata cluster `0x4bbf12…0x4bbfce`). So the same `PrintParams` is serialized **twice**, with
two different naming conventions: camelCase for the cloud REST task, snake_case for the MQTT
`project_file` command.

---

## 4. Field-name deltas vs the farm capture

Our farm capture of a printer-side `project_file` command had:
`param`, `url` (`hpart://…`), `task_id`, `subtask_id`, `ams_mapping2`, `bed_leveling`
(one L), `use_ams`, `size`, `md5`, `plate_md5`, `bed_temp`.

Comparison against this **v01.07.01.04** Linux SDK and the **v02.06.00.50** Windows image:

| Capture key | In Linux 01.07.01.04 | In Windows 02.06.00.50 | Note |
|-------------|----------------------|------------------------|------|
| `param` | yes | yes | identical |
| `url` (`hpart://`) | emits `file:///`/`ftp://` | adds `brtc://emmc/`, `bambu:///local/`, `?port=6000&user=` | `hpart://` is a newer firmware-side scheme; SDK side uses file/ftp/brtc/bambu URIs |
| `task_id` / `subtask_id` | yes | yes | identical |
| `ams_mapping` | **`ams_mapping` only** | **`ams_mapping` + `ams_mapping2`** | `ams_mapping2` **added after 01.07** (present at `0x5fcd08` in Windows image, absent in Linux build) |
| `bed_leveling` (one L) | yes (`0x4bc165`) | yes (`0x5fc550`) | matches capture — note single L |
| `use_ams` | yes | yes | identical |
| `bed_type` | yes | yes | not in capture key list but always emitted |
| `md5` | yes | yes | identical |
| `plate_md5` | **absent** | **absent** | added in firmware/SDK newer than both images analyzed |
| `bed_temp` | **absent** | **absent** | added later (not in either binary) |
| `size` | **absent** | **absent** | added later |

New keys present in the **Windows 02.06.00.50** `project_file` cluster that are **not** in the
Linux 01.07 build: `nozzle_mapping`, `ams_mapping2`, `extrude_cali_manual_mode`,
`extrude_cali_flag`, `nozzle_offset_cali`, `auto_bed_leveling`. These reflect the AMS-2 /
multi-nozzle and calibration features added between the two firmware lines.

**Bottom line:** the captured `project_file` schema is a forward evolution of the SDK
serializer documented here. The stable core (`param`, `url`, `task_id`, `subtask_id`,
`bed_leveling` (one L), `use_ams`, `md5`, `bed_type`, `timelapse`, `flow_cali`,
`vibration_cali`, `layer_inspect`, `ams_mapping`) is identical; `ams_mapping2`, `plate_md5`,
`bed_temp`, and `size` are later additions (only `ams_mapping2` is yet present, in the Windows
02.06 image).
