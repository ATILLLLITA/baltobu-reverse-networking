# Architecture

This document describes what `bambu_networking.dll` is, how it is built
internally, and how its public ABI maps onto the protocols it speaks to
the outside world. It is the right starting point for anyone who wants
a complete mental model of the library before diving into specific
exports or wire-protocol details.

## The library in one paragraph

`bambu_networking.dll` is the networking core of Bambu Studio, Bambu
Lab's 3D-print slicer. The slicer itself is published under AGPLv3 and
calls out to this DLL for every interaction with the outside world:
logging in to Bambu's cloud, discovering printers on the local network,
exchanging MQTT messages with printers either directly or via Bambu's
cloud broker, uploading sliced G-code, downloading firmware updates,
publishing to Makerworld, and submitting telemetry. The library ships
as an opaque binary, downloaded by Bambu Studio after installation, 
even though it is dynamically linked into the AGPLv3 slicer.

## System context

```
                  +-------------------------+
                  |    Bambu Studio (UI)    |   the AGPLv3 slicer
                  |    model, slice, GUI    |
                  |    calls bambu_network  |
                  +-----------+-------------+
                              |  C ABI (128 exports)
                              v
                +-----------------------------+
                | bambu_networking.dll        |
                |  BambuNetworkAgent          |
                |  FT (file-transfer) ABI     |
                |  MQTT + HTTP plumbing       |
                +--+----------+----------+----+
                   |          |          |
                   v          v          v
          +--------+--+   +---+----+   +-+-------+
          |  LAN MQTT |   | Cloud  |   |  HTTPS  |
          |  (direct) |   |  MQTT  |   |  REST   |
          +-----+-----+   +---+----+   +---+-----+
                |             |            |
                v             v            v
         +------+------+   +--+-----+   +-----------+
         | Printer(s)  |   | Cloud  |   |  Cloud    |
         | on LAN      |   | broker |   |  REST API |
         | (TLS+MQTT)  |   | mqtts  |   | (cloud)   |
         +-------------+   +--+-----+   +-----+-----+
                              |               |
                              +-------+-------+
                                      v
                       +---------------------------+
                       | Aliyun OSS / AWS S3       |
                       | (region-dependent)        |
                       +---------------------------+
```

The library leaves three transports. LAN MQTT goes directly to printers
found by SSDP-style discovery on the local network; it has the lowest
latency and only works when the slicer and the printer share a subnet.
Cloud MQTT uses a single broker connection at the Bambu cloud
(international or China region) and routes the same MQTT topics the
printers use through Bambu's relay; it works from anywhere. HTTPS REST
covers account and cloud operations that do not fit the MQTT shape
(login, Makerworld, OSS or S3 credential issuance, firmware downloads,
telemetry submission).

A second, independent ABI in the `ft_*` namespace runs over its own
`FTTunnel` transport for large file transfers (G-code uploads,
time-lapse video, firmware payloads). The two namespaces share no
state.

### MQTT broker endpoints

The cloud broker endpoints are hard-coded in `.rdata`:

| Region        | Endpoint                          |
|---------------|-----------------------------------|
| International | `ssl://us.mqtt.bambulab.com:8883` |
| China         | `ssl://cn.mqtt.bambulab.com:8883` |

The Paho MQTT C build embedded in the binary also includes
`WebSocket.c`, so MQTT-over-WebSockets is structurally available, but
the current build has not been observed to use it.

### REST host families

| Region        | API host             | Makerworld           | Event / telemetry    |
|---------------|----------------------|----------------------|----------------------|
| International | `api.bambulab.com`   | `makerworld.com`     | `event.bblmw.com`    |
| China         | `api.bambulab.cn`    | `makerworld.com.cn`  | `event.bblmw.cn`     |

`bambu_network_set_country_code` selects which family is used.

## Internal class layout

The major internal C++ classes can be identified from the MSVC RTTI type
descriptors in the dump's `.rdata`. Class names below are Bambu's own.

```
+----------------------------------------------------------+
|                  BambuNetworkAgent                       |
|         (singleton; lives at .data RVA 0x7a99e0)         |
|                                                          |
|   +---------------------+      +--------------------+    |
|   |   AccountManager    |      |  HTTP / libcurl    |    |
|   |  (login, profile,   |      |  (REST endpoints)  |    |
|   |   refresh token)    |      +----------+---------+    |
|   +---------------------+                 |              |
|                                           v              |
|   +--------------------------+   +-----------------+     |
|   |  DeviceSubscribeManager  |   | Aliyun OSS HMAC |     |
|   |  - auto LAN/cloud switch |   | signer          |     |
|   |  - per-device channels   |   +-----------------+     |
|   +-----+--------------------+                           |
|         |                                                |
|         v                                                |
|   +-------------------------+                            |
|   |   ISubscribeChannel     |  (abstract interface)      |
|   +-----------+-------------+                            |
|        ^                ^                                |
|   +----+-----+   +------+------------+                   |
|   | MqttLocal |  | MqttCloud         |                   |
|   | Subscribe |  | SubscribeChannel  |                   |
|   | Channel   |  |                   |                   |
|   +-----------+   +-------------------+                  |
|        ^                ^                                |
|        +--------+-------+                                |
|                 |                                        |
|       +---------+-----------+                            |
|       | MqttSubscribeChannel|  (common MQTT base)        |
|       +---------+-----------+                            |
|                 |                                        |
|                 v                                        |
|       +-------------------+                              |
|       | MqttConnectionMgr |  (one per broker)            |
|       +---------+---------+                              |
|                 |                                        |
|                 v                                        |
|       +-------------------+                              |
|       | MqttClient        |  (Eclipse Paho MQTT C)       |
|       +-------------------+                              |
|                                                          |
|   +----------------------------+                         |
|   | DirectUploadingTrackingMgr |  (track_* family)       |
|   +----------------------------+                         |
+----------------------------------------------------------+

+-------------------------------------------+
|        ft_*  namespace (independent)      |
|                                           |
|   +-------------------+                   |
|   |   FTTunnel        |  (connection)     |
|   +---------+---------+                   |
|             |                             |
|             v                             |
|   +-------------------+                   |
|   |   FTSession       |                   |
|   +---------+---------+                   |
|             |                             |
|             v                             |
|   +-------------------+                   |
|   |   FTJob           |  (one transfer)   |
|   +-------------------+                   |
+-------------------------------------------+
```

The class names confirmed from RTTI are `BambuNetworkAgent`,
`AccountManager`, `DeviceSubscribeManager`, `GenericSubscriptionManager`,
`MqttClient`, `MqttConnectionManager`, `MqttSubscribeChannel`,
`MqttCloudSubscribeChannel`, `MqttLocalSubscribeChannel`,
`DirectUploadingTrackingManager`, `FTTunnel`, `FTSession`, `FTJob`,
`MqttChannelListener`, `JsonOrJsonBinFramer`, and `SimpleFramer`. The
two framer classes are particularly interesting because they let the
library encode MQTT payloads either as plain JSON or as a binary JSON
variant (commonly CBOR or MessagePack). The slicer presumably
negotiates which framer to use with the printer.

### Embedded libraries

The following libraries are statically linked into the binary,
identified from symbol fragments and string literals:

| Library                | Evidence                                                                |
|------------------------|-------------------------------------------------------------------------|
| Eclipse Paho MQTT C v1.3.9 | `MQTTAsyncV3_Version 1.3.9`, build timestamp `2025-02-08T02:34:36Z` |
| libcurl                | HTTP error message strings                                              |
| OpenSSL                | TLS plus PKCS                                                           |
| BoringSSL fragments    | `br_aes_*` symbols                                                      |
| boost::asio 1.84       | Build path `D:\bambu-studio\release_networking\dep_win_new\usr\local\include\boost-1_84\boost/asio/...` |
| nlohmann/json          | Value-tag layout visible in the `ft_job_create` parsing path            |
| spdlog                 | `{}` format placeholders throughout the log strings                     |

The build path confirms an MSVC plus boost build on a Windows host.

## The agent singleton

`BambuNetworkAgent` is a process-wide singleton. Its pointer is stored
in `.data` at RVA `0x7a99e0` (`DAT_1807a99e0`). The slicer receives a
handle back from `bambu_network_create_agent` and passes it to every
subsequent call, but every export validates the handle against the
global and uses the global from then on. The caller-side `agent_t *` is
a self-check token, not the real handle.

For 104 of the 107 `bambu_network_*` exports the first instructions
are:

```c
if (g_agent != arg) return -1;
```

Three consequences follow. A second `BambuNetworkAgent` cannot be
constructed in the same process; `create_agent` will overwrite the
global. The `agent_t *` argument is interchangeable across exports
provided it matches the global. Any call that arrives before
`create_agent` returns the same `-1`.

The three exports that do not self-check against the global are
`bambu_network_create_agent` (which writes it), `bambu_network_get_version`
(which does not need it), and `bambu_network_check_debug_consistent`
(a seven-byte XOR stub).

### The 16-byte handle wrapper

The `agent_t *` returned by `create_agent` is a 16-byte wrapper:

| Offset | Width | Field                                                                                       |
|-------:|------:|---------------------------------------------------------------------------------------------|
| `0x00` | 8     | Pointer to the 2744-byte real agent state on the heap. Confirmed by `operator delete(state, 0xab8)` in the deep destructor. |
| `0x08` | 8     | A second pointer also freed at teardown (passed size 0 to `operator delete`). Likely a smart-pointer control block or an allocator slot for an `std::function<>`. |

### The 2744-byte real agent state

The deep destructor `FUN_180123880` (called from `destroy_agent`'s
wrapper destructor) visits every owned field in order. Translating its
`param_1[N]` indices to byte offsets gives a near-complete map of the
agent's internal layout:

| Byte offset    | Field                                                                                              |
|---------------:|----------------------------------------------------------------------------------------------------|
| `0x18`         | Pointer to a 304-byte sub-object, almost certainly `AccountManager` (matches the load/save JSON log strings) |
| `0xa0`, `0xb0` | Two collection sub-objects (likely `std::map<...>` shapes for the device-subscription registry and the subscription topic table) |
| `0x4e8`, `0x4f8`, `0x500` | Three smart-pointer sub-objects (HTTP client, MQTT connection manager, OSS/S3 client)   |
| `0x680..0x8c0` | Ten `std::function` callback slots, stride 64 bytes, see below                                     |
| `0x958`        | `std::string config_dir` (target of `set_config_dir`'s helper)                                     |
| `0x9c8`, `0x9d8`, `0x9e8`, `0x9f8` | Four `std::string` fields, almost certainly `country_code`, `cert_file`, `key_file`, `selected_machine` |
| `0xa20`        | Pointer to a 72-byte sub-object                                                                    |
| `0xa38`, `0xa48`| Two `std::shared_ptr<X>` fields                                                                   |
| `0xa50`        | A simple value member (destroyed by `FUN_180565edc`)                                               |
| `0xaa8`        | Pointer to an 88-byte sub-object                                                                   |

The total `0xab8 = 2744` bytes accounts for these fields plus small
POD members (counters, flags) that the destructor does not touch.

### The ten-slot `std::function` callback chain

Each slot at `[0x680..0x8c0]` carries the canonical MSVC `std::function`
destruction pattern, with the small-buffer-optimisation check
identifying whether the held callable lives inline in the 48-byte SBO
buffer or on the heap. The slots are spaced 64 bytes apart, leaving
8 bytes between each 56-byte body for alignment.

The C ABI exposes 13 callback setters; the agent struct stores 10 of
them in this contiguous block. The remaining three are most plausibly
`set_queue_on_main_fn`, `set_get_country_code_fn`, and
`set_server_callback`, which live elsewhere in the struct. They can be
identified by inspecting each setter's helper to learn the offset it
writes to.

## DLL load sequence

When Windows maps the DLL, the protector's stub runs first. After the
anti-debug and unpacking stages complete, control transfers to the
original MSVC entry point inside `.text`. The chain is:

```
OS loader
   |
   v
VMProtect OEP wrapper at RVA 0x1b72917 (inside .1M@)
   |
   v
_DllMainCRTStartup at 0x180565614 (.text)
   |
   +-- on DLL_PROCESS_ATTACH: __security_init_cookie (0x180565d18)
   v
dllmain_dispatch at 0x1805654bc (.text)
   |
   +-- dllmain_raw at 0x1805652d0 (CRT init/teardown reason dispatcher,
   |                                runs _initterm of __xc_a..__xc_z C++
   |                                static constructors on attach,
   |                                __xt_a..__xt_z destructors on detach,
   |                                dynamic TLS init/dtor on thread events)
   |
   +-- user_DllMain at 0x180565dc4 (.text, Bambu Lab code)
         body reduces to `return TRUE;` once the overlapping-instruction
         anti-disassembly trick is read past. VMProtect post-processed
         the function to add a one-shot call into the protector's
         runtime in `.o1}` on DLL_PROCESS_ATTACH.
```

Bambu Lab's source-level `DllMain` is therefore a no-op. Every piece
of library initialisation happens through the explicit C ABI exports
(`create_agent`, `set_*` setters, `init_log`, `start`), not from
DllMain. The full recovery, including the manual instruction decode of
the user-DllMain body and Ghidra's FidDB-resolved MSVC CRT symbols, is
documented in
[`../output/unpacking/dllmain_chain.md`](../output/unpacking/dllmain_chain.md).

A clean-room implementation needs only the trivial DllMain:

```c
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
    return TRUE;
}
```

The call into `.o1}` is part of VMProtect's instrumentation, not Bambu's
design, and must not be replicated.

## Lifecycle

The slicer's startup sequence is:

```
slicer                                  bambu_networking.dll
  |                                              |
  | bambu_network_create_agent()                 |
  |--------------------------------------------->|
  |                                  alloc + write g_agent
  |<------------ agent_t * (== g_agent) ---------|
  |                                              |
  | set_config_dir(agent, path)                  |
  |--------------------------------------------->|
  | set_country_code(agent, "us")                |
  |--------------------------------------------->|
  | set_cert_file(agent, ...)                    |
  |--------------------------------------------->|
  | set_queue_on_main_fn(agent, ...)             |   register this
  |--------------------------------------------->|   before any callback
  |  ...register all callbacks here...           |   can fire
  | set_on_user_login_fn(agent, ...)             |
  | set_on_message_fn(agent, ...)                |
  | ...                                          |
  |                                              |
  | init_log(agent)                              |
  |--------------------------------------------->|
  |                                  open <config_dir>/log
  |                                              |
  | start(agent)                                 |
  |--------------------------------------------->|
  |                                  spawn worker threads:
  |                                    MQTT client thread
  |                                    SSDP discovery thread
  |                                    HTTP request pool
  |                                              |
  | change_user(agent, credentials)              |
  |--------------------------------------------->|
  |                                  publish login cmd; HTTP login
  |                                              |
  |<---- on_user_login_fn(status=0, ...) --------|   (worker thread)
  |                                              |
  | connect_server(agent)                        |
  |--------------------------------------------->|
  |                                  open cloud MQTT broker session
  |<---- on_server_connected_fn() ---------------|
  |                                              |
  | start_discovery(agent)                       |
  |--------------------------------------------->|
  |                                  begin SSDP listener
  |<---- on_ssdp_msg_fn(dev_id, ssdp_blob) ------|   (per discovered printer)
  |                                              |
  |  ... agent live; subscribe, publish, print ...
  |                                              |
  | destroy_agent(agent)                         |
  |--------------------------------------------->|
  |                                  tear down everything, zero g_agent
```

Two non-obvious rules are visible in this sequence. Every callback must
be registered before `start`, because the worker threads spawned in
`start` can fire any callback the moment they begin. Callbacks
registered after `start` may miss early events. And
`set_queue_on_main_fn` deserves special attention because without it
the library has no way to post callbacks onto the slicer's GUI thread.
The slicer's "post this lambda onto main" trampoline is wired in
through that single setter.

## The two-channel pub/sub

Per-printer communication runs over one of two MQTT channels selected
by `DeviceSubscribeManager`:

```
                  +-------------------------------+
                  |   DeviceSubscribeManager      |
                  |   (per-device strategy)       |
                  +-------+-------+---------------+
                          |       |
                          v       v
   +----------------------+     +----------------------+
   | MqttLocalSubscribe   |     | MqttCloudSubscribe   |
   | Channel              |     | Channel              |
   | direct TLS+MQTT      |     | via Bambu broker     |
   | same LAN only        |     | works everywhere     |
   +----------+-----------+     +----------+-----------+
              |                            |
              v                            v
       printer's MQTTS port           api.bambulab.com:8883
```

The recovered log messages document the auto-switching logic verbatim:

> `DeviceSubscribeManager: auto create local channel succeeded, check cloud channel later`

> `DeviceSubscribeManager: local channel doesn't provide data for a long time, switch to cloud`

> `DeviceSubscribeManager: cloud channel status doesn't support switching to local (uptime too short or ip missing or ...)`

The policy prefers LAN once LAN is up, falls back to cloud on LAN
silence, and refuses to switch back too eagerly. A clean-room
implementation should preserve this behaviour: it is what makes the
slicer feel responsive on LAN while still working remotely.

## MQTT topic shape

```
device/<dev_id>/request     slicer publishes here
device/<dev_id>/report      printer publishes here; slicer subscribes
user/u_<user_id>/request    user-scoped commands
```

Payloads are JSON. The exact JSON shapes are documented in
[`05-wire-protocol.md`](05-wire-protocol.md), which covers field names,
types, and optionality recovered from the `.rdata` literals and the
`nlohmann::json::operator[]` callsites.

## REST API surface

The list of URL templates recovered from the dump's `.rdata` is
committed at
[`../output/unpacking/wire_protocol_surface.txt`](../output/unpacking/wire_protocol_surface.txt).
Placeholders in the form `%1%` are `boost::format` substitution slots
filled with host, IDs, and so on.

### Account and authentication

- `/api/sign-in/ticket?to=...`
- `/user-service/user/refreshtoken`
- `/user-service/user/ticket` and `/user-service/user/ticket/<id>`
- `/user-service/user/consent`
- `/user-service/my/profile`
- `/user-service/my/logout`
- `/user-service/my/pincode/<ping_code>` *(the bind/ping code argument of `ping_bind`, carried in the path — not the dev_id; confirmed by behavioral oracle, see [`28`](28-linux-so-recovery.md))*
- `/user-service/my/messages?type=<t>&after=<id>&limit=<n>`
- `/user-service/my/setting`
- `/user-service/my/ticket/<id>`

### IoT and device

- `/iot-service/api/user/bind`
- `/iot-service/api/user/bind_list?dev_ids=...`
- `/iot-service/api/user/device/info`
- `/iot-service/api/user/device/version?dev_id=...`
- `/iot-service/api/user/applications/<app>/cert?aes256=...&ver=1`
- `/iot-service/api/user/notification` and the upload-action variant
- `/iot-service/api/user/print?force=true`
- `/iot-service/api/user/project` and `/project/<id>`
- `/iot-service/api/user/profile/<id>?model_id=...`
- `/iot-service/api/user/task/<id>`
- `/iot-service/api/user/ttcode`
- `/iot-service/api/user/upload?...`
- `/iot-service/api/user/upload?event_track/slicer=...`
- `/v1/iot-service2/api/user/device/file_download?dev_id=...&type=print_fail_snapshot&name=...`

### Slicer resources

- `/iot-service/api/slicer/resource`
- `/iot-service/api/slicer/setting` and `/setting/<id>`

### Tasks and history

- `/user-service/my/task` and `/my/task/<id>`
- `/user-service/my/tasks?deviceId=<dev>&limit=<n>&offset=<n>&status=<n>` *(`limit`/`offset`/`status` always emitted, keys alphabetical; `deviceId` omitted when empty — confirmed live, see [`28`](28-linux-so-recovery.md)/[`29`](29-cloud-print-live.md))*

### Makerworld

- `/design-service/design/staffpick?offset=&limit=`
- `/design-service/model/<id>`
- `/design-service/my/design/recommend?seed=&limit=`
- `/design-user-service/my/preference`
- `/comment-service/rating/<id>` and `/rating/inst/<id>`
- `/my/models/<id>/publish?project_id=&profile_id=`
- `/my/profiles/<id>/publish?project_id=&design_id=`
- `/makerworld/model/<model>/<kind>/ratings/<rating_id>`

### Telemetry and autotest

- `/autotest-report-service/api/user_last_task?user_id=...`

### Object storage credential issuance

- `/user-service/my/ossconfig?useType=<n>` issues short-lived Alibaba
  OSS credentials (China region).
- `/user-service/my/s3config?useType=<n>` issues short-lived AWS S3
  credentials (international region).

The slicer takes the returned credentials and uploads directly to the
object store. The library carries AWS-Sigv4 helpers (visible in the
`%s4-HMAC-SHA256` log line), so the same code path handles both
providers; only the bucket and host change.

## Authentication flow

```
slicer / browser           bambu_networking.dll            cloud
       |                              |                          |
       | (user enters creds in web)   |                          |
       |--------- OAuth flow happens outside the DLL ------------|
       |                              |                          |
       | change_user(credentials)     |                          |
       |----------------------------->|                          |
       |                  POST /api/sign-in/ticket?to=...        |
       |                              |------------------------->|
       |                              |<-------- access token ---|
       |                              |                          |
       |                  build_login_cmd then MQTT publish      |
       |                              |------------------------->|
       |                              |<------- login ack -------|
       |  set_on_user_login_fn(0, json)                          |
       |<-----------------------------|                          |
       |                              |                          |
       | (token used in subsequent HTTP requests)                |
       |                              | GET /iot-service/...     |
       |                              | Authorization: <token>   |
       |                              |------------------------->|
       |                              |                          |
       | (token expires)              |                          |
       |                              | POST /user-service/user/refreshtoken
       |                              |------------------------->|
       |                              |<------- new token -------|
```

Tokens never leave the DLL. The only export that surfaces one is
`get_my_token`, used by the slicer for inter-process passing. The
actual login UI is a webview owned by the slicer; the DLL participates
only once the slicer has obtained credentials.

## File transfer (`ft_*` namespace)

The `ft_*` ABI is an independent surface for transferring large
payloads (G-code, video, firmware). The shape of a full transfer is:

```
slicer                  ft_* ABI                  cloud / printer
  |                        |                            |
  | ft_tunnel_create("...")|                            |
  |----------------------->|                            |
  |                  one-time init of two io_contexts via std::call_once
  |<--- tunnel ------------|                            |
  |                        |                            |
  | ft_tunnel_set_status_cb(tunnel, fn, ctx, ...)       |
  |----------------------->|                            |
  | ft_tunnel_start_connect(tunnel, fn, ctx, ...)       |
  |----------------------->|                            |
  |                        |--- TCP connect ----------->|
  |<--- status cb (connected) -|                        |
  |                        |                            |
  | ft_job_create("{\"cmd_type\":42,...}", &job)        |
  |----------------------->|                            |
  |                  parse JSON, alloc 280-byte job
  |<--- job ---------------|                            |
  |                        |                            |
  | ft_job_set_msg_cb(job, fn, ctx)                     |
  | ft_job_set_result_cb(job, fn, ctx)                  |
  |----------------------->|                            |
  | ft_tunnel_start_job(tunnel, job)                    |
  |----------------------->|                            |
  |                  spawn worker; pull msgs from socket
  |<--- msg cb (progress)  |                            |
  |<--- msg cb (progress)  |                            |
  |<--- result cb (ok)     |                            |
  |                        |                            |
  | ft_job_release(job); ft_tunnel_release(tunnel);     |
```

The two opaque types are sized from their destructors. `ft_tunnel` is
56 bytes (one refcount, one buffer or string, three smart pointers).
`ft_job` is 280 bytes (refcount, parsed JSON spec, internal queue,
future-state, two callback slots; the full inferred layout is in
[`exports/ft_job_release.md`](exports/ft_job_release.md)).

Threading is deliberately different between tunnels and jobs. Tunnel
callbacks (status, connect) are dispatched from worker threads via
refcounted `std::shared_ptr`-style control blocks, which is what
`ft_tunnel_set_status_cb` constructs. Job callbacks (msg, result) are
stored as raw `(fn, ctx)` pairs and delivered synchronously to whatever
thread pulled them out of the job's message queue; the slicer controls
the consumer thread.

## Threading model

```
 +--------------+        +---------------------------+
 |  GUI thread  | <----- |   queue_on_main_fn        |
 | (slicer's    |        |   trampoline registered   |
 |  main loop)  |        |   by set_queue_on_main_fn |
 +------+-------+        +-------------+-------------+
        |                              ^
        |  C ABI calls                 |  callbacks
        v                              |  marshalled here
 +------+------------------------------+-------+
 | bambu_network_*  (these all run on caller's |
 |  thread; most return immediately and        |
 |  enqueue work for the worker pool)          |
 +-+-------------------------+-----------------+
   |                         |
   v                         v
+--+------------+      +-----+---------------+
| MQTT worker   |      | HTTP / libcurl pool |
| (Paho async)  |      | (boost::asio        |
|   reads loop  |      |  io_context)        |
|   publish     |      |   request queue     |
+---+-----------+      +-----+---------------+
    |                        |
    v                        v
   network                 network
```

The library is single-agent and multi-worker. The slicer's GUI thread
issues C ABI calls; worker threads do the actual I/O; results come back
via callbacks that the library posts onto the GUI thread through
`set_queue_on_main_fn`. The slicer can rely on every callback running
on its main thread, which is essential for direct UI updates without
extra synchronisation on the slicer side.

## Where to go from here

The complete reference layout is:

- [`04-public-api.md`](04-public-api.md) catalogs the 128 exports by
  functional family.
- [`exports/`](exports/) contains one file per export.
- [`05-wire-protocol.md`](05-wire-protocol.md) gives the message
  schemas with field names, types, optionality, and error codes.
- [`06-internals.md`](06-internals.md) decodes the FT `cmd_type` enum,
  the RTTI hierarchy, the helper map, the subsystem log catalog, and
  the agent struct layout details.
- [`07-internals-flow.md`](07-internals-flow.md) covers the runtime
  behaviour: the UML class diagram, the call hierarchy from C ABI
  through helpers to subsystems, sequence diagrams for the main
  flows, the four state machines, the JSON build and parse pipeline,
  and the worker-to-GUI callback marshalling.
- [`01-binary.md`](01-binary.md) and [`02-unpacking.md`](02-unpacking.md)
  cover the binary itself and the unpacking flow that made the rest of
  this documentation possible.
