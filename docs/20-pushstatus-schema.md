# Bambu `push_status` / MQTT Report JSON Schema

Recovered from the **open-source slicer** (the networking SDK forwards `push_status` opaquely; the parsing logic that defines the schema lives in the slicer GUI device layer).

**Source (AGPL-3.0):**
- `bambulab/BambuStudio` — `src/slic3r/GUI/DeviceManager.cpp` (`MachineObject::parse_json`) and `src/slic3r/GUI/DeviceCore/*` (`DevStatus`, `DevHMS`, `DevFilaSystem`, `DevDefs.h`).
- `SoftFever/OrcaSlicer` (fork) — `src/slic3r/GUI/DeviceManager.{cpp,hpp}` (same parser, historically the canonical reference).

Note: recent BambuStudio `master` has refactored the parser into `src/slic3r/GUI/DeviceCore/` (`DevStatus.cpp`, `DevHMS.cpp`, `DevFilaSystem.cpp`, `DevPrintTaskInfo.cpp`) and introduced an integer `job_state` field; the long-standing **string `gcode_state`** field documented below is what virtually all deployed firmware still emits and is the canonical field to parse.

No secrets, tokens, or credentials are involved — this is public AGPL source.

---

## Message envelope

The printer publishes to MQTT topic `device/<DEV_SN>/report`. The body is a JSON object with a top-level category key; the printer status lives under `"print"`:

```json
{ "print": { "command": "push_status", ... } }
```

Other top-level categories use the same envelope shape: `"info"` (module/version list, `command: "get_version"`), `"system"`, `"camera"`, `"upgrade"`, `"xcam"`.

### Common envelope keys (inside the category object)

| Key | Type | Meaning |
|-----|------|---------|
| `command` | string | Message type, e.g. `push_status`, `push_info`, `get_version`, `gcode_line`, `project_file`, `pause`, `resume`, `stop`, `ams_filament_setting`, `print_speed`, `extrusion_cali` … |
| `sequence_id` | string (int) | Monotonic request/response correlation id |
| `msg` | int | `0` = full snapshot (push_all), `1` = differential/partial update. **Critical: in differential messages only changed keys are present — clients must merge, not replace.** |
| `result` | string | `success` / failure on command ACKs |
| `reason` | string | Failure explanation on command ACKs |
| `param` | string | Command parameter payload (e.g. gcode line) |

---

## Temperatures (`print`)

| Key | Type | Meaning |
|-----|------|---------|
| `nozzle_temper` | float | Current nozzle temp (°C) |
| `target_nozzle` / `nozzle_target_temper` | float | Nozzle target temp |
| `nozzle_temp_range` | int[] | Nozzle temp limits |
| `bed_temper` | float | Current heatbed temp |
| `bed_target_temper` / `target_bed` | float | Heatbed target temp |
| `bed_temp_range` | int[] | Bed temp limits |
| `chamber_temper` | float | Current chamber temp |
| `target_chamber` | float | Chamber target temp (heated-chamber models) |

> Multi-nozzle (H2D etc.) report temps inside a per-extruder `device.extruder` / `device.nozzle` substructure in newer firmware; classic single-nozzle uses the flat keys above.

---

## Progress / Stage (`print`)

| Key | Type | Meaning |
|-----|------|---------|
| `gcode_state` | string | **Print state enum (strings below).** |
| `mc_print_stage` | int (string) | Machine-control stage class: `"1"` while running a printable stage, `"2"`/etc for auxiliary. Combined with `stg_cur` for the human-readable stage. |
| `mc_print_sub_stage` | int | Sub-stage id |
| `mc_percent` / `mc_print_percent` | int | Print progress % (0–100) |
| `mc_remaining_time` / `mc_left_time` | int | Estimated time remaining (**minutes**) |
| `mc_print_line_number` | int (string) | Current gcode line |
| `layer_num` | int | Current layer |
| `total_layer_num` | int | Total layers in the job |
| `gcode_file` | string | Filename of the running gcode |
| `gcode_file_prepare_percent` | int | File-transfer/prepare progress % |
| `subtask_name` | string | Task/print name |
| `subtask_id` | string | Task id |
| `print_type` | string | `local` / `cloud` / `system` / `idle` |
| `print_error` | int | Active print-error code (0 = none); 32-bit, formatted as `%08X` for the HMS wiki lookup |
| `fail_reason` | int/string | Failure reason code |
| `stg` | int[] | Full ordered list of stages for the job |
| `stg_cur` | int | Current stage id (index into the printer's stage table — heatbed preheat, nozzle preheat, scan bed surface, change filament, M400 pause, calibrate extrusion, auto bed leveling, etc.) |
| `lifecycle` | string | `product` / `engineer` |
| `home_flag` | int | Bitfield of homing/axis/AMS/sensor status flags |
| `hw_switch_state` | int | Filament-presence micro-switch state |
| `spd_lvl` | int | Print-speed level (see `DevPrintingSpeedLevel`) |
| `spd_mag` | int | Speed magnitude / feedrate override % |
| `s_obj` | int[] | Skipped-object ids (object-skipping feature) |
| `job_id` / `task_id` / `queue_number` | string/int | Cloud job / queue identifiers |
| `profile_id` / `project_id` / `design_id` / `model_id` | string | Cloud asset ids for MakerWorld linkage |

### `gcode_state` enum (string values)

The string the printer sends in `gcode_state`:

| String | Meaning |
|--------|---------|
| `IDLE` | Idle / ready |
| `INIT` / `PREPARE` | Booting / preparing the job (heating, loading) |
| `SLICING` | Slicing in progress (cloud-slice flow) |
| `RUNNING` | Actively printing |
| `PAUSE` | Paused (user or error) |
| `FINISH` | Job finished successfully |
| `FAILED` | Job aborted / failed |
| `PREPARING` / `PAUSING` / `RESUMING` / `STOPPING` / `FINISHING` | Transient transition states (newer firmware) |
| `UNKNOWN` | Unrecognized |

Newer BambuStudio `master` additionally exposes an integer `job_state` (`DevJobState` in `DevStatus.h`) which mirrors these:

```
JobStateIdle=0, JobStateSlicing=1, JobStatePrepare=2, JobStateStarting=3,
JobStateRunning=4, JobStatePause=5, JobStatePausing=6, JobStateResuming=7,
JobStateFinish=8, JobStateFailed=9, JobStateFinishing=10, JobStateStoppping=11
```

---

## Fans (`print`)

| Key | Type | Meaning |
|-----|------|---------|
| `cooling_fan_speed` | int (string) | Part-cooling fan, 0–15 gear scale (×~6.67 ≈ %) |
| `big_fan1_speed` | int (string) | Aux/big fan 1 (chamber/aux part cooling) |
| `big_fan2_speed` | int (string) | Aux/big fan 2 (chamber exhaust) |
| `heatbreak_fan_speed` | int (string) | Hotend heatbreak fan |
| `fan_gear` | int | Packed bitfield encoding all fan gear values |

---

## AMS / Filament (`print`)

### Top-level AMS keys

| Key | Type | Meaning |
|-----|------|---------|
| `ams` | object | AMS container; holds `ams[]` array plus state fields below |
| `ams.ams` | array | List of AMS units (each AMS holds 4 trays) |
| `ams.ams_exist_bits` | hex string | Bitmask of which AMS units physically exist |
| `ams.tray_exist_bits` | hex string | Bitmask of which trays have filament present |
| `ams.tray_read_done_bits` | hex string | Per-tray RFID read-complete flags |
| `ams.tray_reading_bits` | hex string | Trays currently being RFID-read |
| `ams.tray_is_bbl_bits` | hex string | Trays holding genuine Bambu (RFID) filament |
| `ams.tray_now` | string | Currently loaded/active tray id (`255`/`254` = none/virtual) |
| `ams.tray_pre` | string | Previously active tray |
| `ams.tray_tar` | string | Target tray for a pending change |
| `ams.ams_rfid_status` | int | RFID subsystem state |
| `ams.insert_flag` | bool | Filament inserted into buffer |
| `ams.power_on_flag` | bool | AMS powered |
| `ams.version` | int | AMS firmware/protocol version |

### Per-AMS-unit object (`ams.ams[]`)

| Key | Type | Meaning |
|-----|------|---------|
| `id` | string | AMS unit index (0-based) |
| `humidity` | string | Humidity level 1–5 (1 = driest) |
| `humidity_raw` | string | Raw humidity % (newer AMS) |
| `temp` | string | AMS internal temperature |
| `tray` | array | The 4 tray/slot objects |

### Per-tray object (`ams.ams[].tray[]`) and `vt_tray`/`vir_slot`

| Key | Type | Meaning |
|-----|------|---------|
| `id` | string | Tray index within the AMS |
| `tray_info_idx` | string | Bambu filament-database preset id (e.g. `GFA00` = Bambu PLA Basic) |
| `tray_type` | string | Material type (`PLA`, `PETG`, `ABS`, `TPU`, `PA`, `PC`, `Support`, …) |
| `tray_sub_brands` | string | Sub-brand / variant name |
| `tray_color` | string | Color as 8-char `RRGGBBAA` hex |
| `tray_weight` | string | Spool weight (g) |
| `tray_diameter` | string | Filament diameter (mm) |
| `nozzle_temp_min` / `nozzle_temp_max` | string | Recommended nozzle temp range |
| `bed_temp` | string | Recommended bed temp |
| `bed_temp_type` | string | Bed-temp category |
| `remain` | int | Estimated remaining filament % (RFID spools, −1 if unknown) |
| `k` | float | Pressure-advance / flow K factor (per-filament cali) |
| `n` | float | Flow N factor |
| `cali_idx` | int | Active PA calibration index |
| `tag_uid` | string | RFID tag UID (`0` if none) |
| `tray_uuid` | string | Unique tray/spool id |
| `ctype` | int | Color type (multi-color/gradient flag) |
| `cols` | string[] | Multi-color list (gradient/silk) |
| `drying_temp` / `drying_time` | string | Recommended drying params |

`vt_tray` (a.k.a. `vir_slot` / `vt_slot` in newer firmware) is the **external/virtual spool** (filament fed from outside the AMS); it carries the same per-tray fields and uses id `254`/`255`.

### AMS status & mapping

| Key | Type | Meaning |
|-----|------|---------|
| `ams_status` | int | Packed status: `main = (ams_status >> 8) & 0xFF`, `sub = ams_status & 0xFF`. Main values (`AmsStatusMain` from `DevDefs.h`): `IDLE=0x00`, `FILAMENT_CHANGE=0x01`, `RFID_IDENTIFYING=0x02`, `ASSIST=0x03`, `CALIBRATION=0x04`, `COLD_PULL=0x07`, `SELF_CHECK=0x10`, `DEBUG=0x20`, `UNKNOWN=0xFF` |
| `ams_mapping` | int[] | Slicer→tray mapping result (set by the slicer / echoed back) |
| `ams_calibrate_remain_flag` | bool | Whether remaining-filament estimation is enabled |
| `filam_bak` | int[] | Filament-backup (auto-switch when a spool runs out) list |

---

## HMS errors (`print.hms`)

`hms` is an array of error objects; each is two 32-bit integers:

```json
"hms": [ { "attr": 50397266, "code": 65538 }, ... ]
```

### Bit layout (from `DevHMS.cpp` `parse_hms_info(attr, code)`)

```
module_id  = (attr >> 24) & 0xFF      // ModuleID
module_num = (attr >> 16) & 0xFF
part_id    = (attr >>  8) & 0xFF
reserved   = (attr >>  0) & 0xFF

msg_level  =  code >> 16              // HMSMessageLevel
msg_code   =  code & 0xFFFF
```

The long error code (for the wiki lookup) is reassembled as:

```c
sprintf(buf, "%02X%02X%02X%02X00%02X%04X",
        module_id, module_num, part_id, reserved, msg_level, msg_code);
```

→ wiki URL: `https://wiki.bambulab.com/en/x1/troubleshooting/hmscode/<long_code>` (locale-substituted).

### `HMSMessageLevel`

| Value | Name | Meaning |
|-------|------|---------|
| 0 | `HMS_UNKNOWN` | Unknown |
| 1 | `HMS_FATAL` | Fatal — printing stopped |
| 2 | `HMS_SERIOUS` | Serious |
| 3 | `HMS_COMMON` | Common warning |
| 4 | `HMS_INFO` | Informational |

### `ModuleID` (subset, from `DevHMS.h`)

`MODULE_UKNOWN=0x00 … MODULE_MAINBOARD=0x05, MODULE_AMS=0x07, MODULE_XCAM=0x0C … MODULE_15=0x0F`.

Related: top-level `print.print_error` (int, 32-bit, `%08X`) is the single active blocking print error, separate from the rolling `hms[]` warning list. Cleared via the `clean_print_error` command.

---

## Lights (`print.lights_report[]`)

Array of `{ "node": <string>, "mode": <string> }`:

| Field | Values |
|-------|--------|
| `node` | `chamber_light`, `work_light` (which lamp) |
| `mode` | `on`, `off`, `flashing` |

---

## Calibration (`print`)

| Key | Type | Meaning |
|-----|------|---------|
| `ams_calibrate_remain_flag` | bool | Remaining-filament cali enabled |
| `cali_version` | int | Calibration data schema version |
| Per-tray `k`, `n`, `cali_idx` | — | PA/flow calibration (see AMS) |

Calibration is also *commanded* (not just reported) via `command: "calibration"` with boolean params: `bed_leveling`, `vibration`, `motor_noise`, `xcam_cali`; and PA flow via `extrusion_cali` / `flowrate_cali` commands.

---

## Network (`print`)

| Key | Type | Meaning |
|-----|------|---------|
| `wifi_signal` | string | RSSI, e.g. `"-50dBm"` |
| `net` | object | Network/conf substructure (newer firmware) |
| `online.ahb` | bool | Aux/AMS bus online |
| `online.rfid` | bool | RFID reader online |
| `online.version` | int | Online-state schema version |

---

## Upgrade (`print.upgrade_state` + top-level `upgrade`)

| Key | Type | Meaning |
|-----|------|---------|
| `upgrade_state.sequence_id` | string | Correlation id |
| `upgrade_state.progress` | string | Upgrade % |
| `upgrade_state.status` | string | `IDLE` / `UPGRADING` / `UPGRADE_SUCCESS` / `UPGRADE_FAIL` … |
| `upgrade_state.message` | string | Human status text |
| `upgrade_state.dis_state` | int | Display/availability state (0 none, 1 available, 2 force) |
| `upgrade_state.err_code` | int | Upgrade error code |
| `upgrade_state.ahb_new_version_number` | string | New version for AHB module |
| `upgrade_state.ota_new_version_number` | string | New OTA firmware version |
| `upgrade_state.new_version_state` | int | Whether a newer version exists |
| `upgrade_state.module` | string | Module currently upgrading |
| `force_upgrade` | bool | Force-upgrade required |

(The `info`/`get_version` message separately lists every module: `name`, `sw_ver`, `hw_ver`, `sn`, `flag`.)

---

## Camera / IPCam (`print.ipcam`)

| Key | Type | Meaning |
|-----|------|---------|
| `ipcam_dev` | string | Camera present (`"1"`/`"0"`) |
| `ipcam_record` | string | Recording enabled (`enable`/`disable`) |
| `timelapse` | string | Timelapse enabled |
| `resolution` | string | `720p` / `1080p` |
| `mode_bits` | int | Capability/feature bitfield |
| `tutk_server` | string | TUTK (P2P relay) server reachability |
| `rtsp_url` | string | Local RTSP stream URL (LAN cameras) |
| `agora_service` / `bitrate_store` | string | Streaming backend details |

---

## XCam — AI / vision (`print.xcam`)

| Key | Type | Meaning |
|-----|------|---------|
| `allow_skip_parts` | bool | Auto object-skip on failure allowed |
| `buildplate_marker_detector` | bool | Build-plate marker detection on |
| `printing_monitor` | bool | AI print monitoring on |
| `spaghetti_detector` | bool | Spaghetti-failure detection on |
| `first_layer_inspector` | bool | First-layer inspection on |
| `halt_print_sensitivity` | string | Sensitivity `low`/`medium`/`high` |
| `print_halt` | bool | Auto-halt on detected failure |

---

## Misc device / capability (`print`)

| Key | Type | Meaning |
|-----|------|---------|
| `nozzle_diameter` | string | e.g. `0.4` |
| `nozzle_type` | string | `hardened_steel` / `stainless_steel` |
| `sdcard` | bool/int | SD card present/state |
| `online` | object | Module online flags (see Network) |
| `support_*` | bool | Numerous feature-capability flags the slicer reads to gate UI: `support_flow_calibration`, `support_bed_leveling`, `support_ams_humidity`, `support_timelapse`, `support_motor_noise_cali`, `support_refresh_nozzle`, `support_chamber_temp`, `support_print_without_sd`, etc. |
| `cfg` | int | Packed device-configuration bitfield |
| `device` | object | Per-component substructure (extruders/nozzles, fans, AMS, chamber) in multi-tool firmware |

---

### Citation

All keys, enums and bit-layouts above are derived from the **open-source (AGPL-3.0) BambuStudio** slicer — `src/slic3r/GUI/DeviceManager.cpp` (`MachineObject::parse_json`) and `src/slic3r/GUI/DeviceCore/` (`DevStatus.{h,cpp}`, `DevHMS.{h,cpp}`, `DevDefs.h`, `DevFilaSystem.cpp`) — and the **OrcaSlicer** fork (`SoftFever/OrcaSlicer`, `src/slic3r/GUI/DeviceManager.{cpp,hpp}`), which is the historically canonical parser for the same `device/<sn>/report` `push_status` payload. No proprietary networking-SDK code, secrets, or credentials are involved.
