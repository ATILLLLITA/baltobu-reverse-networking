# Public API

`bambu_networking.dll` exposes a C ABI consisting of 128 exported
functions in two namespaces. Every export is a plain C function with
no C++ name mangling, so the public surface is a clean C ABI even
though the implementation is C++. This document indexes the exports by
functional family and describes the cross-cutting patterns that
recur across them. The per-export reference lives in
[`exports/`](exports/); each file there gives the recovered signature,
the behaviour at the ABI level, the return codes, and any
implementation notes that affect a clean-room implementation.

## Namespace summary

| Namespace        | Count | Purpose                                       |
|------------------|------:|-----------------------------------------------|
| `bambu_network_` |   107 | The broad networking API (login, MQTT, REST). |
| `ft_`            |    21 | The refcounted file-transfer subsystem.        |

The `bambu_network_*` family is the historical, agent-centric API used
for every interaction with the cloud and with printers. The `ft_*`
family is a separate file-transfer ABI added later; it has its own
lifecycle, its own transport, and shares no state with the agent.

## Cross-cutting patterns

### Agent singleton with self-check token

The `BambuNetworkAgent` is a process-wide singleton stored at `.data`
RVA `0x7a99e0` (`DAT_1807a99e0`). 104 of the 107 `bambu_network_*`
exports begin with the same guard:

```c
if (g_agent != arg) return -1;
```

The handle the slicer passes to every export is therefore a self-check
token, not a real reference. The library uses the global from that
point on. A clean-room implementation can pick any internal pointer
discipline it likes as long as it preserves this contract at the ABI
boundary.

Three exports do not self-check against the global. `create_agent`
writes the global. `get_version` does not need any state.
`check_debug_consistent` is a seven-byte XOR stub with no agent
dependency.

### `std::function` callback-setter pattern

All 13 callback-registration exports
(`set_on_*_fn`, `set_queue_on_main_fn`, `set_get_country_code_fn`,
`set_server_callback`) take an MSVC `std::function<...>` by pointer
and clone it via its own vtable into agent-owned storage. The caller's
`std::function` is destroyed before return regardless of success or
failure. The canonical walkthrough is at
[`exports/bambu_network_set_on_http_error_fn.md`](exports/bambu_network_set_on_http_error_fn.md).

Ten of the 13 callbacks are stored in a contiguous block at agent
offset `0x680..0x8c0` (10 slots, stride 64 bytes). The remaining three
live elsewhere in the struct; see
[`03-architecture.md`](03-architecture.md) for the offset map.

### Pass-by-value `std::string`

Most setters take their string arguments by MSVC `std::string` value.
The callee destroys the caller's `std::string` before returning,
regardless of the success or error path. Clean-room implementers can
pick a different ABI internally but must preserve this contract at the
export boundary. Examples include `set_config_dir`, `set_cert_file`,
`set_country_code`, `set_user_selected_machine`, `set_extra_http_header`,
and all the `build_*` builders.

### Refcount discipline in `ft_*`

The `ft_*` family uses Cocoa-style explicit reference counting. Each
opaque type has matched `_retain` and `_release` exports:

```
ft_job_retain    ft_job_release
ft_tunnel_retain ft_tunnel_release
```

Internally each refcount is a `std::shared_ptr<T>` control block, so
the C ABI is just a thin wrapper. Returned message and result objects
have their own destruction entry points (`ft_job_msg_destroy`,
`ft_job_result_destroy`); there is also a generic `ft_free` for
buffers the library allocates that the caller has to release. The
ownership rule is consistent: anything the library allocates and
returns through a pointer must be released by the caller via the
matching destroyer.

The two opaque types are sized:

| Type         | Size  | Source                                                   |
|--------------|------:|----------------------------------------------------------|
| `ft_tunnel`  | 56 B  | `operator delete` size in `ft_tunnel_release`            |
| `ft_job`     | 280 B | `operator delete` size in `ft_job_release`               |

### Two threading conventions

The library uses two distinct delivery conventions depending on the
callback site. Tunnel callbacks (status, connect) are dispatched from
worker threads via refcounted `std::shared_ptr`-style control blocks,
so they can be invoked concurrently with caller code. Job callbacks
(msg, result) are stored as raw `(fn, ctx)` pairs and delivered
synchronously to whatever thread pulled the event out of the job's
queue; the slicer controls that thread.

For the `bambu_network_*` family, every callback runs on whichever
thread the slicer registered via `set_queue_on_main_fn`. That trampoline
is the only way the library has to post callbacks onto the slicer's
GUI thread, and it must be registered before `start` because the worker
threads spawned in `start` can fire callbacks immediately.

## `bambu_network_*` (107 exports)

### Agent lifecycle (5)

| Export                          | Note                                                                                           |
|---------------------------------|------------------------------------------------------------------------------------------------|
| `bambu_network_create_agent`    | Constructs the singleton and writes the new pointer into the global at `.data` RVA `0x7a99e0`. Body recovery is blocked by an anti-disassembly stub at the entry; the ABI shape is inferred from how the rest of the library uses the returned handle. |
| `bambu_network_destroy_agent`   | Tears down the agent and zeroes the global.                                                    |
| `bambu_network_start`           | Spawns worker threads (MQTT, SSDP discovery, HTTP pool). Every callback must be registered before this call. |
| `bambu_network_init_log`        | Initialises logging. Calling order is `set_config_dir` then `create_agent` then `init_log`; the body requires the global agent to already exist. |
| `bambu_network_get_version`     | Returns the library version string. Confirmed value `02.06.00.50` for this build.              |

### Configuration (8)

`bambu_network_set_config_dir`, `bambu_network_set_country_code`,
`bambu_network_set_cert_file`, `bambu_network_set_extra_http_header`,
`bambu_network_set_user_selected_machine`,
`bambu_network_enable_multi_machine`, `bambu_network_update_cert`,
`bambu_network_install_device_cert`.

`set_config_dir` and `set_cert_file` write straight into agent storage.
`set_country_code` selects the REST host family (international vs
China). `install_device_cert` does not self-check against the global
agent, which is inconsistent with the rest of the family; a clean-room
replacement should treat it the same as the others to keep callers
honest.

### Callback registration (13)

`set_get_country_code_fn`, `set_on_http_error_fn`,
`set_on_local_connect_fn`, `set_on_local_message_fn`,
`set_on_message_fn`, `set_on_printer_connected_fn`,
`set_on_server_connected_fn`, `set_on_ssdp_msg_fn`,
`set_on_subscribe_failure_fn`, `set_on_user_login_fn`,
`set_on_user_message_fn`, `set_queue_on_main_fn`, `set_server_callback`.

All 13 share the `std::function`-clone pattern documented above.
`set_queue_on_main_fn` is the GUI-thread trampoline. The remaining 12
are paired with worker-thread events from the relevant subsystem.

### Authentication and login (6)

`build_login_cmd`, `build_login_info`, `build_logout_cmd`,
`change_user`, `is_user_login`, `user_logout`.

The login flow is split between builders, which produce MQTT command
payloads, and the verbs (`change_user`, `user_logout`), which trigger
the actual flow. The OAuth login UI is outside the DLL; the library
participates once the slicer has credentials.

### Identity and profile (8)

`get_user_id`, `get_user_name`, `get_user_nickanme` (Bambu's typo),
`get_user_avatar`, `get_user_info`, `get_my_profile`, `get_my_token`,
`get_user_selected_machine`.

`get_my_token` is the only export that surfaces the access token; it
exists so the slicer can pass the token to a child process or to a
webview without having to re-login.

### User data (6)

`get_user_presets`, `get_user_print_info`, `get_user_tasks`,
`get_my_message`, `get_mw_user_4ulist`, `get_mw_user_preference`.

The `mw_` prefix is Bambu's abbreviation for Makerworld. `4ulist` is a
Makerworld "For You" recommendation feed.

### Telemetry consent (3)

`check_user_report`, `check_user_task_report`, `report_consent`.

This family handles the explicit "share usage data" consent toggle in
the slicer; it is distinct from the per-event `track_*` family.

### Settings (5)

`get_setting_list`, `get_setting_list2`, `put_setting`,
`delete_setting`, `request_setting_id`.

The two `get_setting_list` versions are an API-evolution artefact. The
`2` variant is used by newer slicer builds.

### Device binding (7)

`request_bind_ticket`, `bind`, `bind_detect`, `ping_bind`,
`query_bind_status`, `unbind`, `modify_printer_name`.

Binding associates a physical printer with an account. The flow is
`request_bind_ticket` to get a one-time ticket from the cloud, `bind`
to start the bind, `bind_detect` to wait for the printer's
acknowledgement, `ping_bind` to keep it alive, `query_bind_status` to
inspect state, and `unbind` to release. The state machine and its
strings are documented in
[`05-wire-protocol.md`](05-wire-protocol.md).

### Printer connection (5)

`connect_printer`, `disconnect_printer`, `connect_server`,
`is_server_connected`, `refresh_connection`.

`connect_printer` and `disconnect_printer` are LAN-mode verbs;
`connect_server` opens the cloud MQTT broker session.

### Pub/Sub (5)

`add_subscribe`, `del_subscribe`, `start_subscribe`, `stop_subscribe`,
`start_publish`.

These wrap the cloud-MQTT and LAN-MQTT subscribe-and-publish surface.
Subscriptions are per-device.

### Messaging (2)

`send_message` (cloud), `send_message_to_printer` (LAN).

### Print job submission (9)

`start_print`, `start_local_print`, `start_local_print_with_record`,
`start_sdcard_print`, `start_send_gcode_to_sdcard`, `get_slice_info`,
`get_subtask`, `get_subtask_info`, `get_task_plate_index`.

`start_sdcard_print` prints from a file already present on the
printer's SD card. `start_send_gcode_to_sdcard` uploads a G-code
without immediately printing it.

### Cloud media (5)

`get_oss_config`, `put_rating_picture_oss`, `del_rating_picture_oss`,
`put_model_mall_rating`, `get_model_mall_rating`.

`get_oss_config` issues short-lived Alibaba OSS or AWS S3 credentials
(the call routes through different endpoints depending on country
code). The rating-picture verbs upload and remove model-rating photos.

### Service URLs and Makerworld endpoints (8)

`get_bambulab_host`, `get_studio_info_url`, `get_model_mall_home_url`,
`get_model_mall_detail_url`, `get_model_publish_url`,
`get_model_rating_id`, `get_model_instance_id`, `get_design_staffpick`.

These return URL templates the slicer uses to open browser views or to
construct REST requests. They are the ABI's region-aware URL accessor
layer.

### Camera streaming (2)

`get_camera_url`, `get_camera_url_for_golive`.

`get_camera_url` returns the printer's RTSP or HTTP camera URL;
`get_camera_url_for_golive` returns the cloud-relayed GoLive variant.

### Device firmware and HMS (2)

`get_printer_firmware`, `get_hms_snapshot`.

HMS stands for Health Management System; this is the diagnostic
snapshot the printer publishes for cloud-side health monitoring.

### Discovery (1)

`start_discovery` begins the SSDP listener; the matching
`set_on_ssdp_msg_fn` callback delivers each discovered host.

### Telemetry (`track_*`, 6)

`track_enable`, `track_event`, `track_header`, `track_update_property`,
`track_get_property`, `track_remove_files`.

The `track_*` family is independent of the `report_consent` family.
It is the per-event telemetry pipeline, batched and uploaded via
`/iot-service/api/user/upload?event_track/slicer=...`.

### Debug (1)

`check_debug_consistent` is a seven-byte stub that returns a constant.
It is almost certainly a debug-build assertion guard.

### Misc (1)

`bambu_network_set_server_callback` is separate from the `set_on_*_fn`
family. It registers a generic server-side event router that the
library uses for protocol events that do not fit the dedicated
callbacks.

## `ft_*` (21 exports)

The `ft_*` ABI is a refcounted file-transfer surface with two opaque
types, `ft_tunnel` and `ft_job`. A job runs over a tunnel. Both polling
(`ft_job_get_msg`, `ft_job_try_get_msg`, `ft_job_get_result`) and
callback delivery (`ft_job_set_msg_cb`, `ft_job_set_result_cb`) are
supported. Connect and transfer have async and sync variants
(`ft_tunnel_start_connect` vs `ft_tunnel_sync_connect`).

The 21 functions, by family:

| Family             | Functions                                                                                |
|--------------------|------------------------------------------------------------------------------------------|
| Lifecycle metadata | `ft_abi_version`, `ft_free`                                                              |
| Tunnel             | `ft_tunnel_create`, `ft_tunnel_retain`, `ft_tunnel_release`, `ft_tunnel_shutdown`, `ft_tunnel_start_connect`, `ft_tunnel_sync_connect`, `ft_tunnel_set_status_cb`, `ft_tunnel_start_job` |
| Job                | `ft_job_create`, `ft_job_retain`, `ft_job_release`, `ft_job_cancel`, `ft_job_get_msg`, `ft_job_try_get_msg`, `ft_job_get_result`, `ft_job_set_msg_cb`, `ft_job_set_result_cb`, `ft_job_msg_destroy`, `ft_job_result_destroy` |

A note on `ft_tunnel_shutdown`: it is a stub that returns `-1`
unconditionally in this build. It exists in the export table but is
not implemented. A clean-room library should make `ft_tunnel_release`
the canonical teardown verb.

## Per-export reference

Every export has its own document under [`exports/`](exports/). The
filename matches the export name. The structure of each document is:

- The functional group and the relative virtual address inside the
  unpacked dump.
- The recovered C signature with named, typed parameters.
- The behaviour at the ABI level.
- A table of return values where applicable.
- Notes that affect a clean-room implementation but do not belong in
  the formal contract.

The index by family is at [`exports/README.md`](exports/README.md).

## Recovery confidence

Of the 128 exports, 31 are hand-written documents based on direct
reading of the recovered C: the pilot batch, the entire `ft_*`
namespace, and the agent lifecycle. The remaining 97 are
template-generated and signature-confident; each family was sampled
sufficiently to lock down the shape, then a per-family generator
produced the markdown. Where a parameter's semantic meaning is not
visible directly in the body, the doc explicitly marks it as
"inferred", "opaque", or "TBD" rather than overstating.

The runtime behaviour behind these exports (which subsystem runs
each one, what the sequence diagrams look like, where callbacks
come from) is in [`07-internals-flow.md`](07-internals-flow.md).

The follow-up items worth flagging for any future contributor are:

- Several functions carry a trailing pair of opaque 64-bit arguments
  that appear unused in the recovered bodies (`get_my_profile`,
  `get_my_token`, `track_get_property`, `track_update_property`,
  `put_setting`, `get_hms_snapshot`, `get_camera_url_for_golive`).
  They are most likely future-reserved or used only on a guarded code
  path. Confirmation needs callsite analysis or a live hook.
- The `bind` family parameter order is partly inferred. Confirmation
  from a live session will firm the shape up.
- The `create_agent` body recovery is blocked by an anti-disassembly
  stub. A manual disassembly override or a Frida hook can unblock it.
