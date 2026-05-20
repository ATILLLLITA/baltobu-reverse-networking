# Internal flow and runtime behaviour

The earlier documents describe what the library is (the binary, the
unpacking method, the architecture, the ABI, the wire protocol, the
internal data structures). This document describes what the library
does at runtime: which classes own which behaviour, which function
calls which other function, what happens on each public ABI call from
top to bottom, and which conditions move the various state machines.

It is the document a clean-room implementer should read after the
architecture overview and the public ABI catalog. It links each
recovered fact to its evidence in the dump so that any disagreement
can be checked at the source.

## The library in layers

The runtime behaviour of `bambu_networking.dll` factors into seven
layers. Calls always travel from the top of the stack toward the
bottom, replies (events, callbacks) always travel from the bottom
back up through the GUI-thread trampoline.

```
+--------------------------------------------------------------+ <- caller side
|  Bambu Studio (AGPLv3 slicer)                                |
|  GUI thread + worker threads owned by the slicer             |
+--------------------------------------------------------------+
                              |   C ABI (128 exports)
                              v
+--------------------------------------------------------------+ <- ABI boundary
|  Layer 1: C-ABI shims                                        |
|  - per-export self-check (`if (g_agent != arg) return -1;`)  |
|  - argument unmarshalling (MSVC `std::string`, `std::function`)|
|  - call into one of 117 internal agent helpers                |
+--------------------------------------------------------------+
                              |
                              v
+--------------------------------------------------------------+
|  Layer 2: Agent helpers (vtable members of BambuNetworkAgent)|
|  117 helpers reachable from the C ABI, grouped by feature:   |
|  identity, callbacks, MQTT pub/sub, REST verbs, FT, telemetry|
+--------------------------------------------------------------+
                              |
                              v
+--------------------------------------------------------------+
|  Layer 3: Subsystems                                         |
|  AccountManager, DeviceSubscribeManager,                     |
|  GenericSubscriptionManager, MqttConnectionManager,          |
|  MqttCloudSubscribeChannel, MqttLocalSubscribeChannel,       |
|  FTTunnel/FTSession/FTJob, DirectUploadingTrackingManager    |
+--------------------------------------------------------------+
                              |
                              v
+--------------------------------------------------------------+
|  Layer 4: Codecs and framers                                 |
|  nlohmann/json + value_t tag,                                |
|  JsonOrJsonBinFramer, SimpleFramer,                          |
|  boost::format URL templates                                 |
+--------------------------------------------------------------+
                              |
                              v
+--------------------------------------------------------------+
|  Layer 5: Embedded libraries                                 |
|  Eclipse Paho MQTT C 1.3.9, libcurl, OpenSSL plus BoringSSL, |
|  boost::asio 1.84, spdlog                                    |
+--------------------------------------------------------------+
                              |
                              v
+--------------------------------------------------------------+
|  Layer 6: VMProtect runtime                                  |
|  IAT resolver in .o1}, syscall stubs in private ntdll copy   |
+--------------------------------------------------------------+
                              |
                              v
+--------------------------------------------------------------+
|  Layer 7: OS                                                 |
|  WS2_32, IPHLPAPI, bcrypt, CRYPT32, WINTRUST, UCRT           |
+--------------------------------------------------------------+
```

Three boundaries are load-bearing for clean-room work. The ABI
boundary at the top of layer 1 is the only contract the slicer
depends on. The codec boundary between layers 3 and 4 is the wire
contract that printers and the cloud depend on. Everything in between
is implementation detail that a clean-room library is free to
restructure.

The remaining layers explain why some details look strange. Layer 6
is the reason every IAT call in the unpacked dump goes through a
VMProtect thunk before reaching the system DLL. Layer 5 is why the
binary carries log strings in spdlog format, JSON keys in
nlohmann/json conventions, and TLS in OpenSSL plus BoringSSL.

## Class diagram

The class graph is reconstructed from MSVC RTTI type descriptors in
`.rdata`, helper-method adjacency, log-string subsystem prefixes, and
the deep destructor at `FUN_180123880`. The notation is informal but
consistent: `<|---` marks inheritance, `*---` marks composition
(owned member, destroyed with the owner), `o---` marks aggregation
(referenced but not owned), and `--->` marks an association without
ownership claim.

```
                        +---------------------------+
                        |     BambuNetworkAgent     |
                        |   (singleton; .data       |
                        |    RVA 0x7a99e0)          |
                        +---+------+------+------+--+
                            *      *      *      *
                            |      |      |      |
                            |      |      |      |
                            v      v      v      v
              +-------------+  +---+--+   |      |
              |Account-     |  |Cloud |   |      |
              |Manager      |  |HTTP  |   |      |
              |(304 B sub-  |  |client|   |      |
              | object at   |  |(libcurl)|  |
              | +0x18)      |  +---+--+   |      |
              +------+------+      |      |      |
                     o             v      v      v
                     |       +-----+--+ +-+---+ +-+-------+
                     v       |Aliyun  | |Direct| |Mqtt    |
                +----+----+  |OSS/AWS | |Upload| |Connect |
                |std::map | |S3 signer| |Track |  |ion    |
                |<dev_id, | +--------+ |Mgr   |  |Manager |
                |sub_info>|             +------+ +---+----+
                |(+0xa0,  |                          *
                | +0xb0)  |                          |
                +---------+                          |
                                                     v
                                            +--------+--------+
                                            |  MqttClient     |
                                            |  (Paho)         |
                                            +--------+--------+
                                                     |
                                                     v
                                                 socket
                                                 TLS

       +-------------------------------------+
       | DeviceSubscribeManager              |
       |  - per-device strategy              |
       |  - LAN-vs-cloud auto-switch         |
       +---+-----------+---------------------+
           o           o
           |           |
           v           v
   +-------+----+   +--+-------------+
   |MqttLocal-  |   |MqttCloud-      |
   |Subscribe-  |   |Subscribe-      |
   |Channel     |   |Channel         |
   +-----+------+   +--------+-------+
         <|---------+--------+
                    |
            +-------+---------+
            |MqttSubscribe-   |
            |Channel          |
            +-------+---------+
                    <|----+
                          |
                +---------+--------+
                |ISubscribeChannel | (abstract)
                +------------------+

   +------------------------------+
   |GenericSubscriptionManager    |  topic-level subscription book
   +------------------------------+

   +------------------------------+
   |MqttChannelListener           |  callback interface delivered
   +------------------------------+  by MqttConnectionManager

   +----------------------+         +---------------------+
   |JsonOrJsonBinFramer   |         |SimpleFramer         |
   +----------------------+         +---------------------+
            ^                              ^
            |                              |
            +-----------+------------------+
                        |  (negotiated framer; both implementations
                        |   carry encode/decode entry points)
                        v
                  MQTT payload bytes

   ft_* namespace (independent, no shared state with the agent)

   +--------------------+
   |     FTTunnel       |   (56 B; refcounted via shared_ptr)
   |  - boost::asio     |   one tunnel per peer
   |  - status_cb slot  |
   +---------+----------+
             *
             |
             v
   +---------+----------+
   |     FTSession      |   per-connection state and framer
   +---------+----------+
             *
             |
             v
   +---------+----------+
   |       FTJob        |   (280 B; one job per logical transfer)
   |   - msg_cb slot    |
   |   - result_cb slot |
   +---------+----------+
             <|-------+--------+----------+-----------+-----------+
                     |        |          |           |           |
              +------+--+  +--+----+   +-+-----+  +--+----+  +---+----------+
              |FTRequest-  |FTSub  |  |FTDelete|  |FTDown-|  |FTUploadFile  |
              |ListInfoJob |Files  |  |Files   |  |loadFiles|  |             |
              | cmd_type=1 |cmd=2  |  |cmd=3   |  |cmd=4    |  | cmd=5       |
              +------------+-------+  +--------+  +---------+  +-------------+
                                       <|
                                        +-- FTRequestMediaAbility  cmd=7
```

The composition relationships (`*---`) are evidence-based: every
field marked composition is destroyed by the deep destructor and
therefore owned by `BambuNetworkAgent`. The aggregation relationships
(`o---`) are evidence-weighted: the agent holds maps and per-device
state that point at owned objects elsewhere.

The two framer classes (`JsonOrJsonBinFramer`, `SimpleFramer`) are
not subclasses of a common base in the recovered RTTI. They both
carry independent encode and decode entry points. The library
selects between them at MQTT-publish time, probably using a printer
capability flag carried in the device subscription report.

## Function call hierarchy from the C ABI

Every public export goes through the same three-stage shape:

```
1. agent self-check
       if (g_agent != arg) return -1;
2. argument unmarshalling
       MSVC std::string  -> internal copy + caller destroyed
       MSVC std::function -> cloned via vtable into agent slot
       int / pointer     -> passed through
3. tail call into the helper
       jmp / call FUN_180XXXX (one of 117 reachable helpers)
       helper does the real work
       wrapper destroys any owned argument and returns
```

The 117-helper map is committed at
[`../output/unpacking/agent_internal_helpers.txt`](../output/unpacking/agent_internal_helpers.txt)
with inferred role labels at
[`../output/unpacking/agent_method_labels.md`](../output/unpacking/agent_method_labels.md).
The callable-export-to-helper relationship is one-to-one for 104 of
the 107 `bambu_network_*` exports and follows the same pattern for
every `ft_*` export.

For 42 exports the helper itself dispatches into a deeper shared
helper, `FUN_1800b6470`, which is the common "validate args plus
enqueue work on the worker pool" body. The same pattern applies to
`FUN_1800b89c0` (37 callers). These two functions are the central
funnel through which any operation that must run on the worker pool
passes.

### Call hierarchy by subsystem

The following call trees are the recovered call chains from each
major export family down to the subsystem that does the work. The
arrow notation means "calls" (synchronous transfer). A `[worker]`
annotation means the receiving function runs on a worker thread,
typically because the caller has enqueued work there.

```
bambu_network_create_agent
   |                          (entry obscured by anti-disassembly stub;
   |                           inferred from observed effects)
   |--> allocate 2744 B heap state, allocate 16 B wrapper, write g_agent
   |--> construct AccountManager (304 B sub-object at +0x18)
   |--> install MQTT, HTTP, OSS smart-pointer slots at +0x4e8/+0x4f8/+0x500
   |--> zero the 10 std::function callback slots at +0x680..+0x8c0
   `--> return wrapper pointer

bambu_network_set_<thing>          (every setter)
   `--> self-check g_agent
        `--> helper at FUN_1801a91e0/.../FUN_1801a9af0
             `--> write to agent state at the helper's known offset
             `--> destroy caller's std::string/std::function

bambu_network_set_on_<callback>_fn  (12 of 13 callbacks)
   `--> self-check g_agent
        `--> helper at FUN_1801a94c0..FUN_1801a9970
             `--> clone caller's std::function via its vtable
                  into agent slot at +0x680 + slot * 0x40
             `--> destroy caller's std::function

bambu_network_set_queue_on_main_fn
   `--> self-check g_agent
        `--> helper at FUN_1801a99f0
             `--> store the trampoline outside the 10-slot block
                  (this is the GUI marshaller; see "callback marshalling")

bambu_network_init_log
   `--> self-check g_agent
        `--> helper at FUN_1801a8590
             `--> open <config_dir>/log; install spdlog sinks

bambu_network_start
   `--> self-check g_agent
        `--> helper at FUN_1801a8670 (start_workers)
             |--> spawn Paho async worker thread (MqttConnectionManager)
             |--> spawn SSDP discovery thread
             `--> initialise boost::asio io_context, spawn HTTP pool

bambu_network_change_user                   (user login)
   `--> self-check g_agent
        `--> helper at FUN_1801a70b0
             |--> AccountManager::set_credentials([worker])
             |--> POST /api/sign-in/ticket?to=... via libcurl
             |--> parse token response at FUN_180189070
             |--> MqttConnectionManager::Publish(studio_userlogin)
             |--> on broker ack, fire set_on_user_login_fn(0, json)
                  routed through queue_on_main_fn

bambu_network_connect_server                (cloud broker open)
   `--> self-check g_agent
        `--> helper at FUN_1801a73e0
             `--> MqttConnectionManager::Connect(ssl://...mqtt...:8883)
                  [worker] async; on success
                       fire set_on_server_connected_fn()
                       routed through queue_on_main_fn

bambu_network_start_discovery
   `--> self-check g_agent
        `--> helper at FUN_1801a9be0
             `--> SSDPListener::start
                  [worker] per discovery
                       fire set_on_ssdp_msg_fn(dev_id, ssdp_blob)

bambu_network_add_subscribe                 (per-device subscribe)
   `--> self-check g_agent
        `--> helper at FUN_1801a6cc0
             `--> DeviceSubscribeManager::request_subscribe(dev_id)
                  |--> if has local: log "auto create local channel ..."
                  |--> spawn MqttLocalSubscribeChannel + MqttCloudSubscribeChannel
                  `--> register both channels with MqttConnectionManager(s)

bambu_network_send_message                  (cloud publish)
   `--> self-check g_agent
        `--> helper at FUN_1801a92c0
             `--> MqttCloudSubscribeChannel::Publish(topic, payload)
                  `--> MqttConnectionManager::Publish
                       `--> Paho MQTTAsync_send

bambu_network_send_message_to_printer       (LAN publish)
   `--> self-check g_agent
        `--> helper at FUN_1801a92e0
             `--> MqttLocalSubscribeChannel::Publish

ft_tunnel_create
   |--> std::call_once initialises two io_contexts on first call
   `--> alloc 56 B FTTunnel, set status_cb slot, return tunnel

ft_tunnel_start_connect
   `--> FTTunnel::start_connect [worker]
        `--> boost::asio TCP connect
             |--> on success: fire status_cb(connected)
             `--> on failure: fire status_cb(error)

ft_job_create("{...}", &out)
   |--> nlohmann::json::parse(spec)
   |--> read cmd_type integer key
   |--> dispatch to BBL::FTRequestListInfoJob / FTSubFiles / FTDeleteFiles
   |    / FTDownloadFiles / FTUploadFile / FTRequestMediaAbility
   `--> alloc 280 B FTJob, store cmd_type, sequence, req body

ft_tunnel_start_job(tunnel, job)
   `--> FTTunnel::start_job [worker]
        `--> frame request, write to socket
             `--> on incoming msg: fire job.msg_cb (sync to consumer)
             `--> on final: fire job.result_cb

track_event(key, json)
   `--> self-check g_agent
        `--> helper at FUN_1801a9d80
             `--> DirectUploadingTrackingManager::enqueue(event)
                  `--> batch worker [worker]
                       `--> POST /iot-service/api/user/upload?event_track/slicer=...
```

The pattern is consistent. Layer 1 in the call shape (the self-check
plus the wrapper) is identical across exports; the divergence begins
in layer 2 (the helper), which is what makes the helper table the
right level of granularity for clean-room re-implementation.

## Worker threads and callback dispatch

`bambu_network_start` brings three worker pools online. The pools
are independent: an outage in one does not block the others, and
their callbacks marshal back to the GUI thread through the same
`queue_on_main_fn` trampoline.

```
                                  +-----------------------+
                                  |  Slicer GUI thread    |
                                  |  - calls C ABI        |
                                  |  - hosts queue_on_main|
                                  +-----+-----------+-----+
                                        ^           |
                                        |           |
                                        |  posts    |  C ABI
                                        |  lambdas  |  calls
                                        |           v
       +-------------+   +-----------------+   +---------------------+
       | SSDP thread |   | MQTT worker     |   | HTTP/libcurl pool   |
       | (1 thread)  |   | (Paho async,    |   | (boost::asio,       |
       |             |   |  1 reader,      |   |  io_context with N  |
       |             |   |  N writers)     |   |  worker threads)    |
       +------+------+   +-------+---------+   +-----+---------------+
              |                  |                   |
              v                  v                   v
        UDP/SSDP            MQTT broker         HTTPS to api host
        on LAN              (us or cn cloud,
                             or printer LAN)
```

The MQTT worker is structurally split inside Paho into a reader and
several writers. The reader runs in `MQTTAsync_receiveThread_main`
(visible from spdlog format strings) and parses incoming MQTT
control packets. Writers are short-lived: they wake up to send a
single PUBLISH frame, then exit. Reconnect logic lives in
`MqttClientKeeper` and its worker (`MqttClientKeeper: thread
exiting` is the shutdown log).

The HTTP pool runs on boost::asio's `io_context`. The library uses
exactly one io_context for all REST calls; libcurl is configured to
share the io_context's reactor. Two io_contexts exist in the FT
subsystem (initialised lazily via `std::call_once` on the first
`ft_tunnel_create`), keeping FT traffic isolated from the agent's
HTTP pool.

### Callback marshalling through `set_queue_on_main_fn`

Every callback registered on the agent is invoked on a worker
thread. The library routes the call through the trampoline the
slicer registered:

```
worker thread                      queue_on_main_fn               GUI thread
 |                                                                     |
 |  some event happens                                                  |
 |  e.g. push_status arrives on the MQTT reader                         |
 |                                                                     |
 |  build std::function<void()> wrapping the user callback              |
 |  capture the parsed JSON payload by value                            |
 |  capture the slot index so the right set_on_*_fn is fired            |
 |                                                                     |
 |  invoke queue_on_main_fn(fn)                                         |
 |  -------------------------------------------------------------->     |
 |                                                              fn()    |
 |                                                              user    |
 |                                                              code    |
 |                                                              runs    |
 |                                                              here    |
```

The trampoline that the slicer registers is responsible for posting
the closure onto whichever runqueue the slicer uses (wxWidgets event
loop, Qt event loop, custom). The library does not care which.

Three callback families do not use this trampoline:

The `set_get_country_code_fn` callback is synchronous. It is invoked
inline on whichever thread is making a REST request that needs to
know the current country code (so the host family can be selected).
The slicer must therefore make this callback thread-safe.

The `ft_tunnel_set_status_cb` callback is dispatched from FT worker
threads via a refcounted control block; if it captures slicer state,
the slicer must marshal it onto the right thread itself.

The `ft_job_set_msg_cb` and `ft_job_set_result_cb` callbacks are
delivered synchronously to whichever thread is calling
`ft_job_get_msg`, `ft_job_try_get_msg`, or `ft_job_get_result`. The
slicer controls that thread, which is why these two callback slots
are stored as raw `(fn, ctx)` pairs rather than as cloned
`std::function`s.

## Sequence diagrams

The following sequences cover the seven flows that account for almost
all observable runtime behaviour. Each sequence is recovered from a
combination of helper-to-helper call chains, log-string emission
order, and JSON-key access order in the relevant parser. Direct
references to specific functions are included where they exist.

### Startup

```
slicer GUI                  bambu_networking.dll       worker pool
   |                              |                          |
   | create_agent()               |                          |
   |----------------------------->|                          |
   |             alloc state, write g_agent                  |
   |             construct AccountManager                    |
   |             zero callback slots                         |
   |<--- agent_t * ---------------|                          |
   |                              |                          |
   | set_config_dir(agent, path)  |                          |
   |----------------------------->| stores at +0x958         |
   | set_country_code(agent,"us") |                          |
   |----------------------------->| selects api.bambulab.com |
   | set_cert_file(agent, ...)    |                          |
   |----------------------------->|                          |
   | set_queue_on_main_fn(...)    |                          |
   |----------------------------->| stores GUI trampoline    |
   |                              |                          |
   | ...register all set_on_*_fn callbacks BEFORE start()... |
   | set_on_user_login_fn         |                          |
   | set_on_server_connected_fn   |                          |
   | set_on_message_fn            |                          |
   | set_on_ssdp_msg_fn           |                          |
   | set_on_subscribe_failure_fn  |                          |
   | set_on_local_connect_fn      |                          |
   | set_on_local_message_fn      |                          |
   | set_on_http_error_fn         |                          |
   | set_on_user_message_fn       |                          |
   | set_on_printer_connected_fn  |                          |
   | set_get_country_code_fn      |                          |
   | set_server_callback          |                          |
   |----------------------------->|                          |
   |                              |                          |
   | init_log(agent)              |                          |
   |----------------------------->| open log files           |
   | start(agent)                 |                          |
   |----------------------------->| spawn -----------------> | MQTT reader
   |                              |        ----------------> | SSDP discoverer
   |                              |        ----------------> | HTTP pool
   |                              |                          |
   | change_user(agent, creds)    |                          |
   |----------------------------->|--- POST sign-in/ticket ->| HTTP pool
   |                              |       <-- token ---------|
   |                              |                          |
   |                              |--- build_login_cmd ----->| MQTT writer
   |                              |       <-- ack -----------|
   |                              |                          |
   |<--- on_user_login_fn(0, ...) | (posted via queue_on_main)|
   |                              |                          |
   | connect_server(agent)        |                          |
   |----------------------------->|--- MQTT connect -------->| MQTT reader
   |<--- on_server_connected_fn   |                          |
   |                              |                          |
   | start_discovery(agent)       |                          |
   |----------------------------->|--- listen -------------->| SSDP
   |<--- on_ssdp_msg_fn(dev_id,...) (per discovered host)    |
```

The order in which setters are called matters. Setters that store
into the agent struct (the 13 callbacks, `set_config_dir`,
`set_country_code`, `set_cert_file`, `set_extra_http_header`,
`set_user_selected_machine`) can be called in any order, but they
must complete before `start`, because `start` spawns workers that
immediately consume those fields.

### User login

```
slicer                           DLL                          cloud
  |                              |                              |
  | change_user(agent, creds)    |                              |
  |----------------------------->|                              |
  |                              | self-check g_agent           |
  |                              | helper FUN_1801a70b0         |
  |                              | AccountManager::set_credentials
  |                              |                              |
  |                              |--- POST /api/sign-in/ticket->|
  |                              |    body = {credentials}      |
  |                              |<-- 200 OK + token bundle ----|
  |                              | parse at FUN_180189070       |
  |                              | extract token, refresh_token,|
  |                              |   user_id, expires_in        |
  |                              | cache in AccountManager      |
  |                              |                              |
  |                              | build_login_cmd helper       |
  |                              |   constructs JSON:           |
  |                              |   { "command":               |
  |                              |       "studio_userlogin",    |
  |                              |     "sequence_id": "10001",  |
  |                              |     "data": {...} }          |
  |                              |                              |
  |                              |--- MQTT publish to ---------->| broker
  |                              |    user/u_<id>/request       |
  |                              |<-- printer ack (login) ------|
  |                              |                              |
  |                              | on broker ack:               |
  |                              |   queue_on_main_fn posts     |
  |                              |   on_user_login_fn(0, json)  |
  |                              |                              |
  |<--- on_user_login_fn(0, json)|                              |
  |     status==0 means success  |                              |
```

`status` is 0 for success and an integer error code otherwise. The
error path emits log line
`request_refreshtoken failed, error={}, body={}, status={}` when the
refresh fails; the original login path emits an analogous error log
that has not been pulled into the catalog yet.

### Cloud MQTT publish from slicer to printer

```
slicer                              DLL                            broker
  |                                  |                               |
  | send_message(agent, dev_id,      |                               |
  |              "{...payload...}",  |                               |
  |              callback_ctx)       |                               |
  |--------------------------------->|                               |
  |                                  | self-check g_agent            |
  |                                  | helper FUN_1801a92c0          |
  |                                  | lookup MqttCloudSubscribe-    |
  |                                  |   Channel for dev_id          |
  |                                  | MqttCloudSubscribeChannel::   |
  |                                  |   Publish(topic, payload)     |
  |                                  | MqttConnectionManager::       |
  |                                  |   Publish(self, payload)      |
  |                                  | log: "MqttConnectionManager:: |
  |                                  |   Publish, self = {}"         |
  |                                  | Paho MQTTAsync_send (writer)  |
  |                                  |---------- PUBLISH ----------->|
  |                                  |                               |
  |                                  |<------ PUBACK (QoS 1) --------|
  |                                  |                               |
  |                                  | log: "OnMessageDeliveried!"   |
  |<-- (no callback unless the user callback is registered           |
  |     and queued through queue_on_main_fn)
```

The slicer-side publish is fire-and-forget at the ABI level. There
is no explicit ack callback. The MQTT layer logs delivery via
`MqttConnectionManager::OnMessageDeliveried!, self = {}` but the
agent does not propagate this to the slicer.

### Push-status reception (printer to slicer)

```
broker                          DLL                          slicer GUI
  |                              |                              |
  |---- PUBLISH push_status ---->|                              |
  |     topic = device/<id>/report                              |
  |     payload = {"command":"push_status", ...}                |
  |                              |                              |
  |             Paho reader thread invokes MqttClient callback  |
  |             MqttCloudSubscribeChannel::OnMessage receives   |
  |             MqttSubscribeChannel::HandleMessage parses JSON |
  |               log: (on failure) "HandleMessage parse json   |
  |                                  failed"                    |
  |                                                             |
  |             DeviceSubscribeManager::InternalHandleMessage   |
  |             routes by dev_id                                 |
  |                                                             |
  |             agent code:                                     |
  |               wrap parsed JSON in std::function<void()>     |
  |               capture: callback slot + payload              |
  |               post via queue_on_main_fn(closure)            |
  |                              |---------------------------- >|
  |                              |                       slicer |
  |                              |                       posts  |
  |                              |                       on its |
  |                              |                       runloop|
  |                              |<------------ closure runs -----
  |                              |  invokes on_message_fn(      |
  |                              |    dev_id, json_string)      |
  |                              |  set by set_on_message_fn    |
```

The reader-thread-to-GUI-thread hop is required because the slicer's
UI cannot be touched from arbitrary worker threads. The MQTT reader
never blocks on the GUI; the closure post is non-blocking, returning
immediately so the reader can pick up the next PUBLISH.

### LAN-to-cloud auto-switching

`DeviceSubscribeManager` runs an arbiter that decides, per device,
whether the active subscription is LAN-direct or cloud-routed. The
arbiter is what `add_subscribe` triggers and what fires every time a
new device is bound.

```
[arbiter for dev_id X]
   |
   +-- on first subscribe:
   |     log: "request subscribe local device. dev_id = X"
   |     try MqttLocalSubscribeChannel.connect(printer_ip)
   |       log: "MqttLocalSubscribeChannel local mqtt client
   |             start connecting. topic = ..., ip = ..."
   |       success: log: "auto create local channel succeeded,
   |                       check cloud channel later. dev_id = X"
   |                set state = LOCAL_ONLY_TRY_CLOUD
   |       failure: log: "auto create local channel failed,
   |                       won't retry. dev_id = X"
   |                set state = NO_LOCAL
   |
   +-- on cloud channel connected:
   |     state == LOCAL_ONLY_TRY_CLOUD ->
   |       state = BOTH_PREFER_LOCAL
   |     state == NO_LOCAL ->
   |       state = CLOUD_ONLY
   |
   +-- on local channel silent for too long:
   |     state == BOTH_PREFER_LOCAL ->
   |       log: "local channel doesn't provide data for a long time,
   |             switch to cloud. dev_id = X"
   |       state = BOTH_PREFER_CLOUD
   |
   +-- on cloud channel data fresh and local up again:
   |     state == BOTH_PREFER_CLOUD ->
   |       (try local: but uptime check)
   |       log: "cloud channel status doesn't support switching to
   |             local(uptime too short or ip missing or ...), try
   |             later. dev_id = X"
   |       state stays BOTH_PREFER_CLOUD until cloud uptime
   |       threshold met
   |
   +-- on remove:
         log: "request unSubscribe device. dev_id = X"
         destroy both channels
```

The arbiter's preference is local-first because LAN latency is
better. The reluctance to switch back is intentional: it prevents
"flap" where a transiently-up LAN channel keeps stealing the
subscription from a stable cloud channel. Clean-room implementations
should reproduce this hysteresis.

### FT upload (file_upload, cmd_type 5)

```
slicer                       ft_* ABI                    printer
   |                            |                            |
   | ft_tunnel_create("...peer..", &tunnel)                  |
   |--------------------------->|                            |
   |                            | first call: std::call_once |
   |                            |   inits two io_contexts    |
   |                            | alloc 56 B FTTunnel        |
   |<-- tunnel -----------------|                            |
   |                            |                            |
   | ft_tunnel_set_status_cb(tunnel, fn, ctx)                |
   |--------------------------->| store cb in tunnel slot    |
   |                            |                            |
   | ft_tunnel_start_connect(tunnel, fn, ctx, n, m)          |
   |--------------------------->| FTTunnel::start_connect    |
   |                            |   [worker]                 |
   |                            |   boost::asio TCP connect  |
   |                            |--- TCP SYN --------------->|
   |                            |<-- TCP ACK ----------------|
   |                            | tunnel state = CONNECTED   |
   |<-- status_cb(connected) ---|                            |
   |                            |                            |
   | ft_job_create("{                                        |
   |    \"cmd_type\": 5,                                     |
   |    \"sequence\": 12345,                                 |
   |    \"req\": {                                           |
   |       \"type\": \"...\",                                |
   |       \"path\": \"...\",                                |
   |       \"dest_name\": \"a.gcode\",                       |
   |       \"dest_storage\": \"sdcard\",                     |
   |       \"override\": true,                               |
   |       \"total\": 12345678                               |
   |    }                                                    |
   | }", &job)                  |                            |
   |--------------------------->| nlohmann::json::parse      |
   |                            | dispatch cmd_type==5       |
   |                            | alloc 280 B BBL::FTUpload- |
   |                            |   File                     |
   |<-- job --------------------|                            |
   |                            |                            |
   | ft_job_set_msg_cb(job, fn, ctx)                         |
   |--------------------------->| store cb pair raw          |
   | ft_job_set_result_cb(job, fn, ctx)                      |
   |--------------------------->|                            |
   |                            |                            |
   | ft_tunnel_start_job(tunnel, job)                        |
   |--------------------------->| FTTunnel::start_job        |
   |                            |   [worker]                 |
   |                            | frame request via          |
   |                            |   JsonOrJsonBinFramer      |
   |                            |--- write request --------->|
   |                            |<-- progress msg -----------|
   |                            | enqueue msg in job queue   |
   |<-- msg_cb(progress) -------| (sync to consumer thread)  |
   |                            |<-- progress msg -----------|
   |<-- msg_cb(progress) -------|                            |
   |                            |<-- final ack --------------|
   |<-- result_cb(ok) ----------|                            |
   |                            |                            |
   | ft_job_release(job)        |                            |
   |--------------------------->| dec refcount; on zero,     |
   |                            |   destruct, free 280 B     |
   | ft_tunnel_release(tunnel)  |                            |
   |--------------------------->| dec refcount; on zero,     |
   |                            |   close socket, free 56 B  |
```

The arrows annotated `[worker]` show where the FT subsystem leaves
the slicer's thread. The `ft_tunnel_set_status_cb` slot is dispatched
from the FT worker; everything else is delivered synchronously to
the consumer thread.

### Token refresh loop

The refresh is owned by the AccountManager and runs as a periodic
task on the HTTP pool. The exact period is not statically visible
but its decision tree is:

```
[every period]
  read access_token expiry from AccountManager state
  read refresh_token expiry from AccountManager state
  if access_token has < N seconds left:
     log: "need new access token, left time = N"
     POST /user-service/user/refreshtoken
       body  = { "refresh_token": "..." }
     on 200:
       parse new bundle (same shape as login response;
         keys: token, refresh_token, expires_in, refresh_expires_in)
       AccountManager::update_tokens(new_bundle)
       fire (asynchronously) any consumer with the new bearer header
     on non-200:
       log: "request_refreshtoken failed, error={}, body={}, status={}"
       (presumed) raise event for the slicer to re-prompt login
  else:
     log: "do not need new access token, left time = N"
```

The library does not expose the refresh-token rotation event to the
slicer through any callback; the slicer must call `get_my_token` if
it wants to read the current token after a refresh.

### Telemetry event submission

```
slicer                         DLL                          cloud
  |                            |                            |
  | track_event(agent,         |                            |
  |             "name",        |                            |
  |             "{json}")      |                            |
  |--------------------------->|                            |
  |                            | self-check g_agent         |
  |                            | helper FUN_1801a9d80       |
  |                            | DirectUploadingTracking-   |
  |                            |   Manager::enqueue(event)  |
  |                            | event blob constructed at  |
  |                            |   FUN_1801c5600 ->         |
  |                            |   { evt, tag, content, ts1,|
  |                            |     uuid, cli_id, user_id, |
  |                            |     device_id, app_name,   |
  |                            |     app_ver, agent_ver,    |
  |                            |     os, platform, sv, ver, |
  |                            |     name, data_id }        |
  |                            | enqueue in batch buffer    |
  |                            |                            |
  |                            | [batch worker, periodic]   |
  |                            | POST                       |
  |                            |   /iot-service/api/user/   |
  |                            |   upload?event_track/      |
  |                            |   slicer=...               |
  |                            |--------------------------->|
  |                            |<-- 200 -------------------|
```

`track_enable` toggles the per-process enablement flag.
`track_header` registers a per-request header carrier; the slicer
sets these to add custom request metadata. `track_update_property`
and `track_get_property` update or read sticky properties that
accompany every event. `track_remove_files` clears pending batch
files on disk.

## State machines

The library hosts four independently-visible state machines. Each is
documented from log strings, parser keys, and observable behaviour.

### Bind and login

The bind-and-login state machine is the most complex. Its states are
emitted into MQTT messages as a `status` string and read by
`bambu_network_query_bind_status`. The complete vocabulary:

```
                +-----------------+
                |  recv_ticket    |   client received a bind ticket
                |                 |   from the cloud (start state for
                |                 |   the printer side)
                +--------+--------+
                         |
                         v
                +--------+--------+
                |   get_ticket    |   slicer requests a ticket
                +--------+--------+
                         |
        get ticket OK    |    get ticket failed/timeout
            +------------+------------+
            v                         v
   +--------+--------+         +------+-----------------+
   |  post_ticket    |         |  FAILURE              |
   |  (submit to     |         |  log: "get ticket     |
   |   printer)      |         |  failed, code={}" or  |
   +--------+--------+         |  "get ticket timeout" |
            |                  +-----------------------+
            v
   +--------+--------+
   |  wait_auth      |   waiting for printer to
   |                 |   authenticate the ticket
   +--------+--------+
            |
   auth OK  |    auth fail
    +-------+-----------+
    v                   v
+---+---------+     +---+--------+
| wait_info   |     |  recv_err  |
|             |     |            |
| waiting for |     +------------+
| printer info|
+----+--------+
     |
     v
+----+----------+
| wait_printer  |   waiting for printer's
|               |   final ack
+----+----------+
     |
     v
+----+----------+
| wait_time     |   waiting for time-sync
|               |   exchange (sec_link)
+----+----------+
     |
     v
+----+----------+
|  bound (terminal positive)
+---------------+

         +-----------------+
         |   FAILURE       |   terminal negative
         |                 |   reachable from any
         |                 |   state on protocol error
         +-----------------+
```

The failure log fragments tied to specific states are:

```
get ticket failed, code={}
get ticket timeout
post ticket failed, code={}
parse login report failed, reason={}
receive login report timeout
wait_printer failed, job_info={}
```

The `recv_err` state is reached when the printer publishes an error
in its bind reply. The `reson_err_code` field (Bambu's typo for
"reason") carries the printer-side error code.

### MQTT connection

`MqttConnectionManager` runs the connection state machine for each
broker (LAN and cloud). The log catalog is the source:

```
       +-------------+
       |  Initial    |
       +------+------+
              |
              v
       +------+-------+
       |  Connecting  |  log: "MqttConnectionManager create!"
       +------+-------+
              |
   success     |        failure (retry allowed)
       +------+----------+
       v                 v
+------+------+    +-----+------+       +------+---------+
| Connected   |    | Connecting |       |  Won't retry   |
| log:        |    | (retry)    |       |  log: "won't   |
| "OnConnected!"|  |            |       |   retry any    |
+--+--------+-+    +-----+------+       |   more"        |
   |        ^           |              +----------------+
   |        |     retry succeeds
   |        +-----------+
   |
   |  network lost / disconnect requested
   v
+--+------------+
| Reconnecting  |   log: "OnConnectionLost!, cause = {}",
| (auto)        |        "MqttClientKeeper: reconnect a manager"
+------+--------+
       |
       v
+------+--------+
|  Connected    |   log: "OnAutoReconnection!",
|  (after reconnect)|  then per-channel
|                  |  "start to resubscribe, channel = {}"
+---------------+

       +-------------+
       |  Destroyed  |   log: "MqttConnectionManager destroy!
       +-------------+         alive channels count = {}"
```

The `alive channels count` in the destroy log is a leak detector: if
the manager is destroyed while channels are still registered, the
count is non-zero and the agent has misbehaved.

### LAN-vs-cloud arbiter (DeviceSubscribeManager)

Five-state arbiter, per device:

```
              +-------------------+
              |  NO_CHANNELS      |
              +--------+----------+
                       |  add_subscribe
                       v
              +--------+----------+
              |  LOCAL_TRYING     |   "request subscribe local device"
              +-+------+----------+
       success  |      |  failure
                v      v
+---------------+----+----+-------+
| LOCAL_ONLY_TRY_CLOUD   |  NO_LOCAL  |
|                        |            |   "auto create local channel
| "auto create local     |            |    failed, won't retry"
|  channel succeeded,    |            |
|  check cloud channel   |            |
|  later"                |            |
+-----------+------------+------+-----+
            | cloud connects   |
            v                  v
+-----------+----------+  +----+-------+
| BOTH_PREFER_LOCAL    |  | CLOUD_ONLY |
|                      |  +------+-----+
| "request subscribe   |
|  cloud device"       |
+-----+----------+-----+
      |          ^
      | local    | cloud-supports-local switch
      | silent   |   (uptime threshold met)
      v          |
+-----+----------+----+
| BOTH_PREFER_CLOUD   |
|                     |
| "local channel      |
|  doesn't provide    |
|  data for a long    |
|  time, switch to    |
|  cloud"             |
+---------------------+
```

The other DeviceSubscribeManager log lines fit into the transitions:

```
"existing local channel of dev_id {} will be closed cause of force subscribe"
"existing cloud channel of dev_id {} will be closed cause of force subscribe"
"start auto close cloud channel of dev_id"
"start auto create local channel"
"status of local channel doesn't support closing cloud channel, retry later"
"local or cloud channel missing, stop auto switch"
"there is an existing local channel"
"device {} is subscribing by channel {}, do nothing"
"dev_id {} has existing channels, do nothing"
"stop a pending channel"
"unSubscribe a existing channel"
```

The implementation is therefore not a clean three-state arbiter; it
carries history and idempotence guards. The "do nothing" log lines
are the guard hits. The "retry later" lines are the hysteresis
points.

### FT job lifecycle

```
              +------------+
              |  Created   |  ft_job_create returns
              |            |  state = CREATED (post-parse)
              +-----+------+
                    |
                    | ft_tunnel_start_job
                    v
              +-----+------+
              |  Running   |  framer issuing frames
              +-----+------+
                    |
       msg ........ | ........ msg
       (multiple)
                    |
                    v
              +-----+------+
              | Completed  |  result_cb fired with ok
              +-----+------+
                    |
                    | ft_job_release
                    v
              +-----+------+
              | Destroyed  |  280 B freed
              +------------+

  ft_job_cancel ........ valid in any pre-Completed state
       transitions Running -> Cancelled
       result_cb fires with non-zero (cancellation code)

  Error path: at any time, frame error or network error transitions
       to Failed; result_cb fires; log "FTJob failed, error = {}"
```

`ft_job_get_msg` blocks the caller until a message is available; its
non-blocking sibling `ft_job_try_get_msg` returns immediately with
the `-2` "no message" code if the queue is empty. `ft_job_get_result`
blocks until the result is decided.

## Data path: JSON build and parse pipeline

Every JSON-shaped message on the wire goes through the
nlohmann/json library. The build path and the parse path are
symmetric.

```
build path (slicer -> wire)

  helper code constructs a fresh nlohmann::json object
       ^
       |  for each field:
       |    j[<key_string>] = <value>      // operator= on json
       |    operator= sets value_t tag then copies the value
       |
  framer wraps the json:
       JsonOrJsonBinFramer::encode  ->  raw bytes
         either j.dump(indent=-1) for plain JSON
         or     j.to_cbor / to_msgpack for the binary variant
  bytes go to MQTT publish or HTTP body


parse path (wire -> slicer)

  raw bytes from MQTT receiver or HTTP response
       |
  framer detects shape:
       SimpleFramer::decode  ->  json
       or JsonOrJsonBinFramer::decode  ->  json
       |
  parser code reads:
       v = j[<key_string>]               // operator[] returns reference
       (FUN_1800e6ca0 is the operator[] symbol)
       v.get<T>() reads the typed value
       |
  decoded fields placed into a struct or fired
       into a callback as a raw JSON string for the slicer to parse
```

The recovery method behind the wire-protocol document leans on this
symmetry. For each function in `.text` we counted calls to
`FUN_1800e6ca0` and harvested the string literals; the key set
recovered from the parser is the upper bound on field names that
ever appear on the wire.

A given message is encoded by exactly one function (the matching
build helper) and decoded by exactly one function (the matching
parser). The key vocabularies of those two functions agree; that
agreement is what locks the wire shape down with high confidence.

## Internal helper VA ranges by group

The 117 internal helpers cluster into contiguous VA ranges that
correspond to subsystems and to vtables. Empirically:

```
0x1800b6470   shared "validate and enqueue" body (42 callers)
0x1800b89c0   shared "post on worker pool" body (37 callers)
0x1800b6550   ft.job_create entry
0x1800ba580   ft.tunnel_start_job entry
0x1800bf3f0   ft.job_get_result entry
0x1800bfba0   ft.job_create + ft.tunnel_create shared entry
0x1800ce1c0   set_extra_http_header entry
0x1800e6ca0   nlohmann::json::operator[](const std::string&)
0x180123880   ~BambuNetworkAgent deep destructor
0x18017a8f0   token-response parser (twin of 0x180189070)
0x180189070   token-response parser (sign-in/ticket)
0x18018a4f0   alternative token-response shape
0x18018c4d0   bind status payload reader
0x18015c620   build_login_cmd helper
0x18015c970   cloud-environment payload reader
0x180177ee0   Makerworld design publish payload builder
0x180181f60   OSS or S3 credential bundle reader
0x180192b90   push_status payload parser
0x18019f910   print job request payload reader
0x1801a6ae0   ~BambuNetworkAgent outer wrapper
0x1801a7090   build_logout_cmd helper
0x1801a70b0   change_user helper
0x1801a8590   init_log helper
0x1801a8670   start_workers helper
0x1801a90b0   set_cert_file helper
0x1801a91e0   set_config_dir helper
0x1801a92c0   send_message (cloud publish) helper
0x1801a92e0   send_message_to_printer (LAN publish) helper
0x1801a9300   set_country_code helper
0x1801a93b0   set_extra_http_header helper
0x1801a9440   set_get_country_code_fn helper
0x1801a94c0   set_on_http_error_fn helper
0x1801a9540   set_on_local_connect_fn helper
0x1801a95c0   set_on_local_message_fn helper
0x1801a9640   set_on_message_fn helper
0x1801a96c0   set_on_printer_connected_fn helper
0x1801a9740   set_on_server_connected_fn helper
0x1801a97c0   set_on_ssdp_msg_fn helper
0x1801a9870   set_on_subscribe_failure_fn helper
0x1801a98f0   set_on_user_login_fn helper
0x1801a9970   set_on_user_message_fn helper
0x1801a99f0   set_queue_on_main_fn helper
0x1801a9a70   set_server_callback helper
0x1801a9af0   set_user_selected_machine helper
0x1801a9be0   start_discovery helper
0x1801a9d80   track_event helper
0x1801c5600   telemetry event blob builder
0x18023cda0   ft cmd_type enum-to-string helper
0x18024c510   sub_file (cmd_type 2) reader
0x18024d970   file_upload (cmd_type 5) reader
0x180256770   file_download (cmd_type 4) reader
```

The contiguity of the 13 callback setters at
`0x1801a9440..0x1801a9970` (intervals of 0x80, every helper is the
same shape) is the clearest evidence that they are members of a
single vtable on `BambuNetworkAgent`. Almost every adjacent
agent helper from `0x1801a6XXX` to `0x1801a9XXX` is one of the
public exports' single-call body, ordered by export name.

The complete inferred-name mapping is at
[`../output/unpacking/agent_method_labels.md`](../output/unpacking/agent_method_labels.md).

## Where data is shared across threads

Mutable state shared between worker threads and the GUI thread is
the source of every threading-related contract in the library. The
shared fields are:

```
+ Agent global pointer (g_agent at .data 0x7a99e0)
   Read on every ABI call. Written only inside create_agent and
   destroy_agent. The slicer must call neither concurrently with
   anything else; in practice slicers call both during single-
   threaded startup/shutdown so this is not a constraint.

+ Agent struct fields touched by setters
   set_config_dir, set_country_code, set_cert_file, set_extra_http_header,
   set_user_selected_machine each write to a single std::string field
   in the agent struct. These writes are not synchronised; the
   contract is "set before start, do not change while workers run".

+ 10 std::function callback slots at +0x680..+0x8c0
   Written by set_on_*_fn before start. Read by workers after start.
   The library does not lock the slots; the slicer must not call
   set_on_*_fn concurrently with worker activity.

+ queue_on_main_fn slot (separate from the 10 above)
   Read by every worker thread that fires a callback. Written by
   set_queue_on_main_fn before start. Same contract.

+ AccountManager state
   Read and written by the HTTP pool (token refresh) and by the
   MQTT writer (login publish). Locked internally by AccountManager.

+ DeviceSubscribeManager per-device state
   Read and written by MQTT readers (channel events) and by ABI
   calls (add_subscribe, del_subscribe). Locked internally.

+ MqttConnectionManager broker state
   Locked internally by the manager.
```

The arrangement is conservative: subsystems lock their own state and
the high-level fields are write-once before start. A clean-room
implementation can keep this discipline and avoid the more subtle
ordering bugs that finer-grained locking would introduce.

## Per-callback dispatch by subsystem

The 13 callback setters are not symmetric in who fires them. The
mapping below is recovered from the subsystems that emit the
relevant log lines and from the call sites that route through
`queue_on_main_fn`.

| Callback                          | Fired by                                  | Trigger                                        |
|-----------------------------------|-------------------------------------------|------------------------------------------------|
| `set_on_user_login_fn`            | AccountManager                            | login ack received over MQTT                   |
| `set_on_server_connected_fn`      | MqttConnectionManager (cloud broker)      | broker session established                     |
| `set_on_printer_connected_fn`     | MqttConnectionManager (LAN broker)        | per-printer LAN MQTT connected                 |
| `set_on_local_connect_fn`         | MqttLocalSubscribeChannel                 | per-printer LAN channel up                     |
| `set_on_message_fn`               | MqttCloudSubscribeChannel + Cloud dispatch | cloud push_status or generic message arrived  |
| `set_on_local_message_fn`         | MqttLocalSubscribeChannel                 | LAN-direct push arrived                        |
| `set_on_user_message_fn`          | GenericSubscriptionManager                | message on `user/u_<id>/...` topic             |
| `set_on_ssdp_msg_fn`              | SSDP discovery worker                     | new printer discovered                         |
| `set_on_subscribe_failure_fn`     | DeviceSubscribeManager + GenericSubscriptionManager | subscribe request denied or failed   |
| `set_on_http_error_fn`            | libcurl wrapper (HTTP pool)               | any HTTP request fails                         |
| `set_get_country_code_fn`         | libcurl wrapper                           | called inline before a REST request            |
| `set_queue_on_main_fn`            | every callback above                      | trampoline through which the closure is posted |
| `set_server_callback`             | generic server event router               | events that do not fit another callback        |

A clean-room implementation must preserve this dispatch pattern.
Replacing `set_server_callback` with an inline event handler is
fine; replacing the per-event setters with a single dispatcher is
not, because the slicer code registers handlers per-event and the
library is observed to call only the relevant one.

## Implications for the clean-room implementation

The patterns documented here translate into specific obligations for
a clean-room library.

The agent singleton must store the pointer at a fixed offset (or use
the slicer's wrapper consistently); 104 of 107 exports validate
against the global, and changing the contract would require also
changing the slicer.

Setters and start must respect the "set before start" rule. A
clean-room library is free to lock everything finely, but the slicer
relies on the simpler discipline and does not synchronise its
setter calls.

The queue_on_main_fn trampoline is the only way to deliver callbacks
on the slicer's GUI thread. Every callback fired from a worker must
route through it.

The LAN-vs-cloud arbiter's hysteresis must be preserved. The five
recovered states and the "uptime too short, retry later" guard are
the exact preferences a Bambu-compatible slicer expects.

The FT subsystem must keep job and tunnel callbacks on different
delivery semantics. Status callbacks come from workers; message and
result callbacks come from the consumer thread.

The wire-protocol shape (field names, types, optionality) cannot
change without breaking compatibility with both printers and the
cloud. The wire layer of the clean-room library is the only place
where the shapes documented in `05-wire-protocol.md` are binding.

## See also

- [`03-architecture.md`](03-architecture.md) for the system context,
  the agent singleton's wrapper and struct, the lifecycle from the
  slicer's perspective, and the embedded library list.
- [`04-public-api.md`](04-public-api.md) for the 128-export catalog
  and the cross-cutting export-shape patterns.
- [`05-wire-protocol.md`](05-wire-protocol.md) for the JSON shapes,
  REST templates, MQTT topics, and the error codes.
- [`06-internals.md`](06-internals.md) for the FT `cmd_type` enum,
  the RTTI hierarchy, and the subsystem log catalog.
- [`../output/unpacking/agent_method_labels.md`](../output/unpacking/agent_method_labels.md)
  for the full helper-to-export mapping with inferred role labels.
- [`../output/unpacking/subsystem_method_map.md`](../output/unpacking/subsystem_method_map.md)
  for the per-method log-string attribution.
- [`../output/unpacking/dllmain_chain.md`](../output/unpacking/dllmain_chain.md)
  for the CRT entry chain and the user DllMain decode.
