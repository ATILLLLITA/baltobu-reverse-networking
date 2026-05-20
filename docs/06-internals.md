# Internals

This document covers the parts of the library that are not visible
from the ABI surface but matter to a clean-room implementation. It
includes the FT `cmd_type` enum, the RTTI-derived class hierarchy, the
internal helper map, the subsystem log catalog, and the agent's
internal struct layout.

All findings are derived from static analysis of the unpacked dump
[`../unpacking/dumps/bambu_networking_dump.dll`](../unpacking/dumps/bambu_networking_dump.dll).
The committed raw extractions live under
[`../output/unpacking/`](../output/unpacking/).

## FT `cmd_type` enum

`ft_job_create` parses a JSON spec, reads the `cmd_type` integer key,
and dispatches to one of nine concrete `BBL::FT*` job classes. The
enum-to-string helper at VA `0x18023cda0` (`FUN_18023cda0`) decompiles
into a clean switch table:

| `cmd_type`     | String name              | Concrete class                  | Purpose                                                |
|---------------:|--------------------------|---------------------------------|--------------------------------------------------------|
| `1`            | `list_info`              | `BBL::FTRequestListInfoJob`     | Directory listing or file metadata fetch               |
| `2`            | `sub_file`               | `BBL::FTSubFiles`               | Subscribe to live changes on a file or directory       |
| `3`            | `file_delete`            | `BBL::FTDeleteFiles`            | Delete one or more files on the printer                |
| `4`            | `file_download`          | `BBL::FTDownloadFiles`          | Pull a file from the printer (firmware, timelapse, snapshot) |
| `5`            | `file_upload`            | `BBL::FTUploadFile`             | Push a file to the printer (G-code, 3MF)               |
| `6`            | reserved (gap)           |                                 |                                                        |
| `7`            | `get_media_capability`   | `BBL::FTRequestMediaAbility`    | Query the printer's media subsystem features           |
| `0x100` (256)  | `list_change_notify`     |                                 | Server push: directory contents changed                |
| `0x101` (257)  | `list_resync_notify`     |                                 | Server push: client should re-sync from scratch        |
| `0x1000` (4096)| `task_cancel`            |                                 | Control op: cancel an in-flight FT job                 |

The `ft_*` namespace therefore carries six client-to-printer commands
(`list_info`, `sub_file`, `file_delete`, `file_download`, `file_upload`,
`get_media_capability`), two server-to-client push notifications
(`list_change_notify`, `list_resync_notify`), and one control op
(`task_cancel`). The integer ranges (1 to 7 vs 0x100 plus vs 0x1000
plus) are intentional: clients only emit values in the low range; the
high ranges are reserved for push events and control ops.

## RTTI class hierarchy

MSVC RTTI emits one `.?AV<Name>@<Namespace>@@` mangled string per
class. Sixteen classes were extracted in the `BBL` (BambuLab)
namespace:

```
BBL::FTJob (base class for every FT job)
  BBL::FTRequestListInfoJob       cmd_type = 1
  BBL::FTSubFiles                 cmd_type = 2
  BBL::FTDeleteFiles              cmd_type = 3
  BBL::FTDownloadFiles            cmd_type = 4
  BBL::FTUploadFile               cmd_type = 5
  BBL::FTRequestMediaAbility      cmd_type = 7

BBL::ISubscribeChannel (interface)
  BBL::MqttSubscribeChannel       (common MQTT impl)
    BBL::MqttCloudSubscribeChannel
    BBL::MqttLocalSubscribeChannel

BBL::MqttChannelListener          (callback interface)
BBL::MqttConnectionManager
  nested: MqttClientCallback
  nested: MqttClientListener

BBL::JsonOrJsonBinFramer          (codec: JSON or binary JSON framing)
BBL::SimpleFramer                 (simpler frame codec)
```

The inheritance edges above are inferred from the names and the
adjacency of methods inside the dump, not from RTTI's base-class
tables (MSVC stores those in vtable-adjacent `_RTTI_BaseClass_Array`
structures that have not been walked yet). The naming convention is
consistent enough that the inferences are sound, but the indented
hierarchy should be treated as a hypothesis until confirmed by
walking the base-class arrays.

The two framer classes (`JsonOrJsonBinFramer`, `SimpleFramer`) suggest
the library can encode MQTT payloads either as plain JSON or as a
binary JSON variant (probably CBOR or MessagePack, both common
nlohmann/json output formats). The slicer presumably negotiates which
framer to use with the printer.

## Embedded libraries

Cross-validated from string fragments and symbol shapes:

| Library                | Evidence                                                              |
|------------------------|-----------------------------------------------------------------------|
| Eclipse Paho MQTT C v1.3.9 | `MQTTAsyncV3_Version 1.3.9`, build timestamp `2025-02-08T02:34:36Z` |
| libcurl                | HTTP error message strings                                            |
| OpenSSL                | TLS and PKCS code                                                     |
| BoringSSL fragments    | `br_aes_*` symbols                                                    |
| boost::asio 1.84       | Build path `D:\bambu-studio\release_networking\dep_win_new\usr\local\include\boost-1_84\boost/asio/...` |
| nlohmann/json          | Tag-byte value layout visible in the `cmd_type` parsing path          |
| spdlog                 | `{}` format placeholder convention throughout log strings             |

## Internal log catalog

81 subsystem-tagged log format strings were recovered across nine
subsystems. The full list is at
[`../output/unpacking/internal_log_catalog.txt`](../output/unpacking/internal_log_catalog.txt).
The subsystem counts are:

| Subsystem                       | Count | Note                                                  |
|---------------------------------|------:|-------------------------------------------------------|
| `AccountManager`                | 2     | Login persistence                                     |
| `DeviceSubscribeManager`        | 21    | The LAN-vs-cloud failover decisions                   |
| `FTJob` (incl. subclasses)      | 1     | One shared "FTJob failed" emitter                     |
| `GenericSubscriptionManager`    | 6     | Topic-level subscription bookkeeping                  |
| `MqttClient` (`MqttClientKeeper`) | 4   | Reconnect logic                                       |
| `MqttConnectionManager`         | 25    | Broker connect/disconnect/reconnect cycle             |
| `MqttCloudSubscribeChannel`     | ~4    | Cloud-channel internals                               |
| `MqttLocalSubscribeChannel`     | ~3    | Local-channel internals                               |
| `bambu_network_*` and `ft_*` export-side | ~15 | Argument validators and high-level error loggers |

A representative selection from `DeviceSubscribeManager` documents the
failover policy verbatim:

> `auto create local channel succeeded, check cloud channel later. dev_id = {}`

> `local channel doesn't provide data for a long time, switch to cloud. dev_id = {}`

> `cloud channel status doesn't support switching to local (uptime too short or ip missing or ...), try later. dev_id = {}`

> `existing local channel of dev_id {} will be closed cause of force subscribe`

`MqttConnectionManager` documents the broker lifecycle:

> `Connection(mqtt) OK! self = {}, cli id = {}`

> `Connection(mqtt) failed! self = {}, return code = {}, reason code = {}, retry= {}`

> `OnConnectionLost!, self = {}, cause = {}`

> `OnAutoReconnection!, self = {}`

> `start to resubscribe, channel = {}`

`AccountManager` confirms the agent's identity state is persisted as
JSON and reloaded on startup:

> `AccountManager::load_from_json() failed! exception={}`

> `AccountManager::request_project_id, body={}`

The catalog is a Rosetta stone for live log reading. Any log line that
begins with one of these subsystem prefixes can be matched against the
catalog to learn which subsystem produced it and what fields appear in
the `{}` placeholders.

## Subsystem activity

The total number of cross-references to each subsystem's log strings
gives a rough measure of how much code volume each subsystem occupies:

| Subsystem                     | Log strings | Total xrefs | Implication                                       |
|-------------------------------|------------:|------------:|---------------------------------------------------|
| `MqttConnectionManager`       | ~25         | 486         | Heaviest subsystem                                |
| `DeviceSubscribeManager`      | 21          | 421         | Second-heaviest (LAN-vs-cloud arbiter)            |
| `MqttSubscribeChannel` (incl. impls) | ~10  | ~190 + 125  | Channel implementations are non-trivial           |
| `GenericSubscriptionManager`  | 6           | 126         | Topic-level subscription bookkeeping              |
| `MqttClientKeeper`            | 4           | 84          | Reconnect path                                    |
| `AccountManager`              | 2           | 43          | Small log footprint                               |
| `FTJob` and subclasses        | 1           | 21          | One shared error emitter, referenced from 21 callsites |

The per-method attribution (which exact function emits each xref) is
noisy because Ghidra's function-boundary detection is unreliable on
this dump (the `.pdata` section is garbled, see
[`02-unpacking.md`](02-unpacking.md)). The totals above are robust;
the per-method table at
[`../output/unpacking/subsystem_method_map.md`](../output/unpacking/subsystem_method_map.md)
should be read with that limitation in mind.

## Internal helper map

Each `bambu_network_*` export, after its self-check against `g_agent`,
calls one or two internal helper functions on the agent. These
helpers carry the real implementation. 117 distinct internal helpers
have been identified reachable from the C ABI. Two tables are
committed:

- [`../output/unpacking/agent_internal_helpers.txt`](../output/unpacking/agent_internal_helpers.txt) is the concise mapping of 117 helpers to their calling exports.
- [`../output/unpacking/agent_method_labels.md`](../output/unpacking/agent_method_labels.md) is the larger map of 149 helpers (including shared C++ runtime helpers like `std::string` copy) with inferred names based on caller semantics.

A representative selection of the inferred labels:

| Internal helper VA | Called by export                      | Inferred role                          |
|--------------------|---------------------------------------|----------------------------------------|
| `FUN_1801a6ae0`    | `destroy_agent`                       | `~BambuNetworkAgent()`                 |
| `FUN_1801a8590`    | `init_log`                            | `init_log()`                           |
| `FUN_1801a8670`    | `start`                               | `start_workers()`                      |
| `FUN_1801a91e0`    | `set_config_dir`                      | `set_config_dir()`                     |
| `FUN_1801a9300`    | `set_country_code`                    | `set_country_code()`                   |
| `FUN_1801a90b0`    | `set_cert_file`                       | `set_cert_file()`                      |
| `FUN_1801a7520`    | `enable_multi_machine`                | `enable_multi_machine()`               |
| `FUN_1801a7530`    | `get_bambulab_host`                   | `bambulab_host() const`                |
| `FUN_1801a80e0`    | `get_studio_info_url`                 | `studio_info_url() const`              |
| `FUN_1801a93b0`    | `set_extra_http_header`               | `set_extra_http_header()`              |
| `FUN_1801a99f0`    | `set_queue_on_main_fn`                | `set_queue_on_main()`                  |
| `FUN_1801a9a70`    | `set_server_callback`                 | `set_server_callback()` (returns int)  |
| `FUN_1801a9440`    | `set_get_country_code_fn`             | `set_get_country_code_fn()`            |
| `FUN_1801a94c0`    | `set_on_http_error_fn`                | `set_on_http_error_fn()`               |
| `FUN_1801a9540`    | `set_on_local_connect_fn`             | `set_on_local_connect_fn()`            |
| `FUN_1801a95c0`    | `set_on_local_message_fn`             | `set_on_local_message_fn()`            |
| `FUN_1801a9640`    | `set_on_message_fn`                   | `set_on_message_fn()`                  |
| `FUN_1801a96c0`    | `set_on_printer_connected_fn`         | `set_on_printer_connected_fn()`        |
| `FUN_1801a9740`    | `set_on_server_connected_fn`          | `set_on_server_connected_fn()`         |
| `FUN_1801a97c0`    | `set_on_ssdp_msg_fn`                  | `set_on_ssdp_msg_fn()`                 |
| `FUN_1801a9870`    | `set_on_subscribe_failure_fn`         | `set_on_subscribe_failure_fn()`        |
| `FUN_1801a98f0`    | `set_on_user_login_fn`                | `set_on_user_login_fn()`               |
| `FUN_1801a9970`    | `set_on_user_message_fn`              | `set_on_user_message_fn()`             |
| `FUN_1801a9af0`    | `set_user_selected_machine`           | `set_user_selected_machine()`          |

The 13 callback setters all live in a contiguous VA range
(`0x1801a9440..0x1801a9970`), only about 1.4 KiB of code for all 13
vtable slots. They are most likely members of the same vtable on
`BambuNetworkAgent`, emitted in declaration order. Internal addresses
adjacent to these helpers are very likely the corresponding getter
methods and other agent members. Walking that area with Ghidra's
cross-reference graph would let a future reader recover the full
agent class layout.

## Agent struct internal layout

The deep destructor `FUN_180123880` (called from `destroy_agent`'s
wrapper destructor) visits every owned field in order. Decompiling it
gives a near-complete tour of the agent's internal layout.

### The 16-byte wrapper

The `agent_t *` returned by `create_agent` is a 16-byte wrapper:

| Offset | Width | Field                                                                                      |
|-------:|------:|--------------------------------------------------------------------------------------------|
| `0x00` | 8     | Pointer to the 2744-byte real agent state on the heap. Confirmed by `operator delete(state, 0xab8)` in the destructor. |
| `0x08` | 8     | A second pointer also freed at teardown (passed size 0 to `operator delete`); likely a smart-pointer control block or an allocator slot. |

### The 2744-byte real agent state

`param_1[N]` in the destructor becomes byte offsets `N * 8`. The
high-signal fields:

| Byte offset                  | What it is                                                                                              |
|-----------------------------:|---------------------------------------------------------------------------------------------------------|
| `0x18`                       | Pointer to a 304-byte sub-object. Almost certainly `AccountManager` (matches the load/save JSON log strings). |
| `0xa0`, `0xb0`               | Two collection sub-objects (probably `std::map<...>` shapes for the device-subscription registry and the subscription topic table). |
| `0x4e8`, `0x4f8`, `0x500`    | Three smart-pointer sub-objects: HTTP client, MQTT connection manager, and the OSS/S3 client.            |
| `0x680..0x8c0`               | Chain of 10 `std::function` callback slots, stride 64 bytes each. See below.                             |
| `0x958`                      | `std::string config_dir`. `set_config_dir`'s helper writes here.                                         |
| `0x9c8`, `0x9d8`, `0x9e8`, `0x9f8` | Four `std::string` fields back-to-back. Almost certainly `country_code`, `cert_file`, `key_file`, `selected_machine`. |
| `0xa20`                      | Pointer to a 72-byte sub-object.                                                                         |
| `0xa38`, `0xa48`             | Two `std::shared_ptr<X>` fields.                                                                         |
| `0xa50`                      | A simple value member.                                                                                   |
| `0xaa8`                      | Pointer to an 88-byte sub-object.                                                                        |

The total `0xab8 = 2744` bytes accounts for these fields plus small
POD members (counters, flags) that the destructor does not touch.

### The 10-slot `std::function` callback chain

Each slot at `[0x680..0x8c0]` carries the canonical MSVC
`std::function` destruction pattern. The slot layout is:

```
0x648 +-------------------+
      |  std::function 1  |  56 bytes:
      |   vtable          |   +0x00: vtable pointer
      |   SBO buffer      |   +0x08: 48-byte SBO
      |   callable ptr    |   +0x38: pointer to the callable
0x680 +-------------------+
      |  std::function 2  |
0x6C0 +-------------------+
        ... 10 slots total at stride 64 bytes (8 bytes padding per slot) ...
0x8C0 +-------------------+
```

The 64-byte stride leaves 8 bytes between each 56-byte body and the
next, likely for cache-line alignment.

The C ABI exposes 13 callback setters; the agent struct stores 10 of
them in this contiguous block. The remaining three most plausibly are
`set_queue_on_main_fn`, `set_get_country_code_fn`, and
`set_server_callback`, which live elsewhere in the struct. They can
be identified by inspecting their helpers to learn the offset each
one writes to.

### Caveats on the layout

This map is derived from the deep destructor, so it enumerates every
field with a destruction step. Small POD fields (ints, raw pointers)
that do not need cleanup are invisible. Several offsets are pointer
fields whose pointee layouts are separate questions (the
`AccountManager`'s 304-byte layout, the `std::map` shapes, the
sub-objects at `0xa20` and `0xaa8`).

The mined offset table in
[`../output/unpacking/agent_struct_field_map.md`](../output/unpacking/agent_struct_field_map.md)
complements this view. Those offsets come from setter and getter
helpers rather than the destructor, so the table also catches some
POD fields the destructor would skip.

## What this unlocks

Combined with the per-export reference and the architecture document,
the material in this file gives a clean-room implementer enough to:

Define the right `ft_*` job command type enum with stable integer
values matching Bambu's.

Pick class boundaries that mirror Bambu's so log output looks similar
(`DeviceSubscribeManager`, `MqttConnectionManager`, and so on).

Use the right JSON field names so the slicer can round-trip messages
without changes.

Mirror the LAN-vs-cloud failover policy exactly, including its
reluctance to switch back to local on freshly-up cloud connections.

## Helper body corpus

All 184 internal helpers reachable from the C ABI have been
decompiled into `output/ghidra_decompile/helpers/` (gitignored under
the clean-room policy). A concatenated dump for grep-based analysis
is at `output/ghidra_decompile/all_helpers.c`. The transitive corpus
at depth three from any export, used for error-code mining and
control-flow analysis of JSON callsites, is at
`output/ghidra_decompile/transitive/` and
`output/ghidra_decompile/all_transitive.c` (876 functions).

These corpora are gitignored. They are the raw material from which
the documentation in this directory is built; the documentation is
the sanctioned artefact for clean-room consumption.

## See also

[`07-internals-flow.md`](07-internals-flow.md) takes the material in
this file and the per-export reference and turns them into a runtime
view: the UML class diagram, the call hierarchy from the C ABI down
to each subsystem, the sequence diagrams for the main flows
(startup, login, MQTT publish and receive, the LAN-cloud arbiter,
FT upload, token refresh, telemetry), the four state machines
(bind, MQTT connection, LAN-cloud, FT job), and the JSON build and
parse pipeline.
