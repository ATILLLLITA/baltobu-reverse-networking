# Bambu Networking SDK — Four Small Subsystems (static RE)

Reverse-engineered statically (no network) from:

- **Linux .so (v01.07.01.04)** — `work/Tzeny-CDN/linux_01.07.01.04/libbambu_networking.so` (demangled symtab available). In `.rodata`, **file offset == VA** (delta 0).
- **Windows image (v02.06.00.50)** — `/root/_bambu_new/live/collected/bnet/bnet.bin`. Newer build; carries subsystems/strings the older `.so` lacks (notably `DeviceSubscribeManager` and the full login FSM state set).

Conventions in quoted strings: `{}` = fmtlib placeholder, `%1%`/`%2%`/... = Boost.Format placeholder. All offsets are decimal file offsets. **Inferred** marks behavior deduced from symbols/strings, not from decompiled control flow. Secret VALUES are redacted; protocol facts that are public (OpenBambuAPI) are included.

---

## 1. SFTP / FTP File Transfer — `BBL::Sftp`

### Class / methods (Linux symtab, `Sftp.cpp`)
A self-contained libcurl wrapper. Pimpl idiom (`BBL::Sftp` + nested `BBL::Sftp::priv`).

| Symbol | Role |
|---|---|
| `BBL::Sftp::Sftp(string const&)` @0x1a8e10 | ctor; arg is the target host/base |
| `BBL::Sftp::priv::priv(string const&)` @0x1a8b40 | builds the private state |
| `BBL::Sftp::priv::set_src_path(string)` @0x1a8e60 | local source file to send |
| `BBL::Sftp::ca_file(string const&)` @0x1a8830 | pin a CA bundle |
| `BBL::Sftp::priv::ca_file_supported(void*)` @0x1a8560 | probe curl/TLS backend for CAINFO support |
| `BBL::Sftp::tls_global_init[abi:cxx11]()` @0x1a8970 | one-time TLS init |
| `BBL::Sftp::tls_system_cert_store[abi:cxx11]()` @0x1a8b20 | use OS cert store |
| `BBL::Sftp::priv::set_timeout_connect(long)` @0x1a85d0 | connect timeout |
| `BBL::Sftp::on_progress(function<void(Progress,bool&)>)` @0x1a8890 | progress cb; `bool&` is the **cancel** out-flag |
| `BBL::Sftp::on_complete(function<void(string)>)` @0x1a86b0 | completion cb |
| `BBL::Sftp::on_error(function<void(int,string)>)` @0x1a8770 | error cb (code + message) |
| `BBL::Sftp::priv::file_read_cb(char*,ulong,ulong,void*)` @0x1a81e0 | curl `READFUNCTION` — streams the upload body |
| `BBL::Sftp::priv::xfercb(void*,long,long,long,long)` @0x1a8150 | curl `XFERINFOFUNCTION` (dl/ul totals) → drives `Progress` |
| `BBL::Sftp::priv::sftp_perform()` @0x1a9820 | builds the curl easy handle + runs it |
| `BBL::Sftp::perform()` @0x1a9170 | async: spawns a `std::thread` (`perform()::{lambda()#1}`) |
| `BBL::Sftp::perform_sync()` @0x1a9b90 | blocking variant |
| `BBL::Sftp::upload(string,bool,string,string,string,string,string)` @0x1a9bb0 | the high-level entry (7 args) |
| `BBL::Sftp::cancel()` @0x1a8950 | sets cancel flag |
| `BBL::Sftp::priv::curl_error[abi:cxx11](CURLcode)` @0x1a9380 | maps CURLcode → message |

`upload(...)`'s arg shape (`string, bool, string×5`) is **inferred** to be `(host, useTLS/passive, user, password/access-code, remote-path, local-path, ca)` — consistent with the URL builder below.

### URL scheme & auth (strings from Windows image)
The URL is **built at runtime** via Boost.Format, not stored as a literal — that's why no `sftp://` literal appears in the old `.so`. The format template and scheme tokens are in `bnet.bin`:

```
%1%://%2%:%3%@%4%/%5%      @off 6256648   (scheme://user:pass@host/path)
ftps                       @off 6256636
ftp                        @off 6256644
ftp://%1%                  @off 6277912   (LAN-mode plain form)
bblp                       @off 6278084   (default username)
?port=6000&user=           @off ~6278108
bambu:///local/            @off ~6278128
```

- **Scheme**: `ftps://` (implicit-TLS FTP over libcurl) when TLS is on, else `ftp://`. The class is named `Sftp` but the wire scheme is **FTP(S)**, not SSH SFTP.
- **Auth (public OpenBambuAPI fact, included as requested)**: LAN file transfer authenticates as user **`bblp`** with the printer's **LAN Access Code** as the password, embedded in the userinfo of the URL → `ftps://bblp:<ACCESS_CODE>@<printer-ip>/<path>`. *(Access-code VALUE redacted.)* TLS cert is typically self-signed, so `ca_file()`/`tls_system_cert_store()` and the `ca_file_supported` probe exist to relax/select verification.

### How it differs from the `ft_*` tunnel
- `BBL::Sftp` = **direct LAN FTPS** to the printer (port 6000, `bblp` + access code). It moves the actual print file (3MF/gcode) onto the printer's storage (`bambu:///local/`, `emmc`). No cloud relay.
- The `ft_*` tunnel (documented in `ft-abi.md`) is the **cloud-relayed file-transfer tunnel** used when the device is remote/cloud-only. `Sftp` is local-only and chosen when a LAN channel exists.
- Callers (Linux symtab): `AccountManager::start_send_gcode_to_sdcard(...)` and `start_local_print_with_record(...)` both instantiate `Sftp` and wire its `Progress` callback (`{lambda(BBL::Sftp::Progress,bool&)}`). So `Sftp` is the LAN "send to SD card / local print" path.

---

## 2. Telemetry / Tracking — `bambu_network_track_*` → `TrackingManager`

### C ABI exports (Linux symtab)
```
bambu_network_track_enable          @0x2621a0
bambu_network_track_event           @0x262c90
bambu_network_track_header          @0x262080
bambu_network_track_get_property    @0x265050
bambu_network_track_update_property @0x266030
```
Each thunks to `BambuNetworkAgent::track_*` → `AccountManager::track_*`:
- `AccountManager::track_enable(bool)` / `track_event(string,string)` / `track_header(string)` / `track_get_property(...)` / `track_update_property(string,string,string)`
- `AccountManager::record_track(string, nlohmann::json&)` @0x21da20 — builds the JSON event row.
- `AccountManager::get_user_track_url(string, string&)` @0x20aa00 — resolves the upload endpoint.
- `AccountManager::get_track_key(EncryptKey&, string)` @0x216bd0 — telemetry is **encrypted** before upload.

### `TrackingManager` (the worker; owns the on-disk event cache)
```
TrackingManager::TrackingManager(AccountManager&)    @0x26c600
TrackingManager::start() / stop()                    @0x26ee90 / 0x272df0  (background thread)
TrackingManager::push_event(TrackingEvent&)          @0x26f4d0
TrackingManager::add_track_header(string)            @0x2738e0
TrackingManager::update_property(string,string,string) @0x26c770
TrackingManager::save_file(bool,bool,bool)           @0x270440  (writes .json then .zip)
TrackingManager::parse_tracking_folder()             @0x26dc10
TrackingManager::upload_file()                       @0x26d180  (POSTs the zipped batch)
TrackingManager::get_cli_uuid[abi:cxx11]()           @0x26bec0  (stable client UUID)
TrackingManager::get_new_encrypt_key()/clean_encrypt_key()/get_cache_property*()
```
**Flow (inferred):** events are appended in-memory (`push_event`) and periodically serialized by `save_file` into a timestamped `_<YYYYMMDDHHMMSS>_track_*.json`, zipped, then `upload_file` POSTs the encrypted batch. A worker thread (`start()`) drives the cycle.

### Event payload fields (strings @ ~0x4c2efd / off 4989900, `.so`)
The JSON event keys, in binary order:
```
app_ver   uuid   sv   ts1   agent_ver   cli_id   app_name   evt
```
Plus log/format strings nearby: `Track: push file {}`, `Track: starting...`, `Track: already started`, `Track: tag is empty`, `Track: config dir is empty`, `Track: save_file to {}`, `Track: create zip file failed`, file suffix tokens `.json` / `.zip`, and the filename template `_%Y%m%d%H%M%S_` + `_track_`.
- `evt` = event name (from `track_event`'s first arg), `app_name`/`app_ver` = slicer identity, `agent_ver` = this SDK's version, `cli_id`/`uuid` = client identity, `ts1` = timestamp, `sv` = schema/version. The `tag`/`content` style fields seen elsewhere belong to other JSON producers, not this event row.

### Endpoint (strings @ off 4955148, `.so`)
```
%1%/iot-service/api/user/upload?event_track/slicer=%2%
```
(`%1%` = resolved base/region host from `get_user_track_url`; `%2%` = slicer/app identifier). Sibling endpoints in the same table: `%1%/iot-service/api/user/upload?%2%`, `.../api/user/project/%2%`, `.../api/user/task/%2%`, `.../api/user/profile/%2%?model_id=%3%`. The payload is encrypted (`get_track_key`) before upload.

---

## 3. Device Subscription — `DeviceSubscribeManager` (Windows v02.06.00.50)

The older `.so` has **no** `DeviceSubscribeManager`; subscription there is handled inline by `AccountManager::{add,del,start,stop}_subscribe`. The newer Windows build refactored this into named classes: **`DeviceSubscribeManager`** (per-device local/cloud channel arbitration) layered on a generic **`GenericSubscriptionManager`** (topic plumbing). All strings below are from `bnet.bin`, offsets ~6231560–6237000.

### Per-device subscription lifecycle (log strings, in order)
```
DeviceSubscribeManager: request subscribe cloud device. dev_id={}
DeviceSubscribeManager: request subscribe local device. dev_id={}
DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing
DeviceSubscribeManager: dev_id {} has existing channels, do nothing
DeviceSubscribeManager: existing cloud channel of dev_id {} will be closed cause of force subscribe
DeviceSubscribeManager: existing local channel of dev_id {} will be closed cause of force subscribe
DeviceSubscribeManager: request unSubscribe device. dev_id={}
DeviceSubscribeManager: stop a pending channel. channel={}
DeviceSubscribeManager: unSubscribe a existing channel. channel={}
DeviceSubscribeManager::InternalHandleMessage parse json failed
```

### Local-vs-cloud channel selection (the auto-switch state machine)
```
DeviceSubscribeManager: there is an existing local channel. dev_id={}
DeviceSubscribeManager: start auto create local channel. dev_id={}
DeviceSubscribeManager: auto create local channel succeeded, check cloud channel later. dev_id={}
DeviceSubscribeManager: auto create local channel failed, won't retry. dev_id={}
DeviceSubscribeManager: cloud channel status doesn't support switching to local (uptime too short or ip missing or ...), try later. dev_id={}
DeviceSubscribeManager: local or cloud channel missing, stop auto switch. dev_id={}
DeviceSubscribeManager: status of local channel doesn't support closing cloud channel, retry later. dev_id={}
DeviceSubscribeManager: start auto close cloud channel of dev_id={}
DeviceSubscribeManager: local channel doesn't provide data for a long time, switch to cloud. dev_id={}
```
**Flow (inferred):** on subscribe, prefer **local**: try to auto-create a local channel; if it succeeds, keep cloud open and *later* close cloud once the local channel is healthy (`auto close cloud channel`). If the local channel goes silent ("doesn't provide data for a long time") it **switches back to cloud**. Guards prevent switching when uptime is too short, the device IP is missing, or a channel is absent. `force subscribe` tears down an existing channel and recreates it.

### Device cache it maintains
```
DeviceSubscribeManager update: dev_id={}, dev_ip={}, access_code={}
```
**Inferred:** an in-memory map keyed by `dev_id` holding `{dev_ip, access_code}` (+ current channel handle/state). This is exactly what the local FTPS/MQTT channels need to connect (ip + `bblp` access code → §1). *(access_code VALUE redacted.)*

### Underlying `GenericSubscriptionManager` (topic layer)
```
GenericSubscriptionManager: request subscribe topic {}
GenericSubscriptionManager: request unsubscribe topic {}
GenericSubscriptionManager: request unsubscribe all topics
GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}
GenericSubscriptionManager: can't create channel by topic {}
GenericSubscriptionManager: client closed, channel dead, topic={}
```
A channel implements `ISubscribeChannel`; `DeviceSubscribeManager` picks a local-vs-cloud channel impl, the generic manager owns topic↔channel registration.

### Equivalent in old `.so` (for reference)
`AccountManager::{add_subscribe@0x2031f0, del_subscribe@0x203750, start_subscribe@0x203570, stop_subscribe@0x204530}`. Topics: `device/%1%/report` (subscribe), `device/%1%/request` (publish). Log strings in `.so`: `start_subscribe, machine={}, model={}`, `add_subscribe failed, topic={}, mqtt_cli is disconnect or invalid!`, `add_subscribe exception, topic={}, exception error_str={}, message={}`, `add_subscribe tunnel is ok {}`.

---

## 4. Bind / Login State Machine

Driver symbols (Linux symtab):
```
bambu_network_bind                @0x262760   →  AccountManager::start_bind(...)  @0x230c10 (12,565 bytes)
bambu_network_unbind              @0x262b60
bambu_network_query_bind_status   @0x263680   →  AccountManager::query_bind_status(...)
AccountManager::request_bind_ticket(string& ticket, uint& code, string& body)  @0x20f2b0
AccountManager::build_login_request(string, bool)   @0x1e3550
AccountManager::_parse_login_report(string, string&) @0x230a20
AccountManager::request_bind_list / request_user_unbind / set_on_user_login_fn / is_user_login
```
Binding = (1) HTTP `request_bind_ticket` to get a one-time ticket from cloud, (2) push a `login_bind` MQTT request to the printer with that ticket, (3) wait for the printer's `login_report`, (4) parse it for SUCCESS/reason.

### Full FSM state-string set
**Login FSM (Windows `bnet.bin`, ~off 6275980 — the complete table):**
```
connecting  →  start_bind/start_local  →  send msg  →  login_report
states:  recv_ticket   get_ticket   post_ticket   wait_auth
         recv_err      wait_printer  wait_time
terminal: SUCCESS   FAILURE   finish
```
Also present in this region: `reason`, `err_code`, `, body=`, `finish`, plus connectivity probes `wifi / iot / apix / emqx / base_domain / environment / e-improved / timezone / ping`, and a device-limit guard `reach device limit`.

**Login FSM (Linux `.so`, ~off 4964400 — older subset; `wait_printer/wait_time/wait_info` not yet present):**
```
connecting  →  connect failed  →  ip:  →  start_bind  →  send msg
login_report  →  SUCCESS / wait_auth
recv_ticket   get_ticket   post_ticket   recv_err
reason   dev_ota_ver   err_code   , body=
login_bind: json_str={}
```

### Failure / log strings (with offsets)
Windows (`bnet.bin`):
```
get ticket failed, code=                  @ ~6276148
get ticket timeout                        @ ~6276056
post ticket failed, code=                 @ ~6276216
parse login report failed, reason=        @ ~6276268
receive login report timeout              @ ~6276336
wait_printer failed, job_info=%1%         (print-job FSM)
Login denied / Access denied              @ ~6277460
```
Linux (`.so`):
```
parse login report failed                 @ 4958160
login_bind: bind failed reason={}         @ ~4958112
login_bind: timeout to receive login_report @ ~4958200
publish login request failed              @ ~4964460
get ticket failed, code=                  @ ~4964580
post ticket failed, code=                 @ ~4964610
receive login report timeout              @ ~4964650
connect failed                            @ ~4964440
```

### State flow (inferred)
```
                  ┌────────────┐
   start_bind ──▶ │ connecting │──connect failed──▶ (retry / FAILURE)
                  └─────┬──────┘
                        ▼
                  get_ticket ──get ticket failed,code=──▶ recv_err ──▶ FAILURE
                        │ (HTTP request_bind_ticket)
                        ▼
                  post_ticket ──post ticket failed,code=──▶ recv_err ──▶ FAILURE
                        │ (publish login_bind to printer over MQTT)
                        ▼
                  recv_ticket / wait_auth ──┐
                        │                   │ receive login report timeout ──▶ FAILURE
                        ▼                   │
                  login_report ──parse login report failed,reason=──▶ FAILURE
                        │
                        ├─ wait_printer  (printer ack)
                        ├─ wait_time     (clock/timezone sync)
                        ▼
                     SUCCESS / finish
```
`query_bind_status` / `bambu_network_query_bind_status` polls the cloud to confirm the device is bound to the account after the report. `Login denied` / `Access denied` are the auth-rejection terminals; `reach device limit` is the account-quota terminal.

---

## Cross-subsystem notes
- §1 `Sftp` and §3 `DeviceSubscribeManager` share the LAN credential model: `dev_ip` + `bblp`/`access_code`. The subscribe manager's device cache feeds the FTPS URL builder.
- §2 telemetry's `agent_ver` is this SDK; the bind FSM (§4) connectivity probes (`emqx`, `iot`, `apix`) reuse the same region/host resolution as §2's `get_user_track_url`.
- Version skew: the Linux `.so` (v01.07.01.04) predates the `DeviceSubscribeManager`/`GenericSubscriptionManager` refactor and the `wait_printer/wait_time/wait_info` login states, both of which appear only in the Windows v02.06.00.50 image. Where the two disagree, the Windows image is the newer truth and the `.so` shows the earlier inline design.
