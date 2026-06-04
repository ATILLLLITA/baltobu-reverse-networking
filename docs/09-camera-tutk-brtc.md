# Bambu Camera / Liveview Stack — TUTK (Kalay P2P) + BRTC (Agora RTC)

Static analysis only. No network/cloud/printer calls were made. Secrets (license keys, app IDs,
tokens, access codes) are redacted — the binaries reference them by name/format but I did not extract
literal credential values.

## Sources analyzed

- Linux (symbols): `work/Tzeny-CDN/linux_01.07.01.04/libbambu_networking.so` + `.symtab.txt`
- `analysis-work/tzeny-cdn/libBambuSource.so.symtab.txt` (the BRTC/source library, statically linked into the SDK)
- macOS universal dylibs: `incoming/brtc-macos-2026-05-16-fex/extracted/libBambuSource.dylib`,
  `libbambu_networking.dylib`, and `AgoraRtcKit.framework` (the BRTC backend)
- Windows v02.06.00.50 image strings: `/root/_bambu_new/live/collected/bnet/bnet.bin`

Note: `objdump` disassembly was blocked by the sandbox for binaries under `work/`. Findings below come
from symtabs, `.rodata`/strings, demangled symbols, and the macOS BambuSource/Agora binaries (which carry
rich log strings). Where I infer beyond a literal string, it is marked **[inferred]**.

---

## 1. High-level architecture

There are **two cooperating libraries**:

| Library | Role |
|---|---|
| `libbambu_networking` | Cloud account/HTTP/MQTT control plane. Owns `bambu_network_get_camera_url`, `AccountManager`, fetches the **ttcode** (TUTK credential bundle) from cloud. |
| `libBambuSource` (`Bambu_*` C-API, class `BambuTunnel`) | The media/transport plane. Opens a tunnel to the printer camera and demuxes the H.264/JPEG stream. Bundles **TUTK/Kalay (ThroughTek)** statically and links **Agora RTC** as the "BRTC" backend. Also provides a LAN tunnel. |

`libbambu_networking` does **not** itself open the camera socket. It hands a **camera URL** to the UI/app
layer, which then calls `libBambuSource` (`Bambu_Open(url)`), and `BambuSource` selects a transport based
on the URL scheme.

```
App  --bambu_network_get_camera_url-->  libbambu_networking (cloud HTTP)
       returns a "bambu:///{tutk|agora|local}?..." URL
App  --Bambu_Open(url)-->  libBambuSource
       url scheme picks transport:  TUTK P2P  |  Agora BRTC  |  LAN
       BambuSource yields demuxed video frames (camera_h264) to the app
```

---

## 2. Camera-URL flow (`libbambu_networking`)

### Entry chain (Linux symtab, VAs)

```
bambu_network_get_camera_url                         @ 0x261380   (C export, thin wrapper)
  -> BBL::BambuNetworkAgent::get_camera_url(string, function<void(string)>)   @ 0x25d1d0
       -> BBL::AccountManager::get_camera_url(const string& dev_id,
                                              function<void(string)> cb)       @ 0x20bfd0  (1639 bytes; core)
```

Supporting:
- `BBL::AccountManager::get_tutk_region[abi:cxx11]()` @ 0x1dcf40 — returns the user's TUTK region string.
- `BBL::AccountManager::get_user_track_url(...)` @ 0x20aa00 — separate analytics/track URL (not camera).

### What `get_camera_url` actually does

It performs a cloud HTTP GET to obtain a **ttcode** (TUTK code / camera credential bundle), keyed by
`dev_id`. Evidence (`.rodata` / strings):

- Endpoint template: `%1%/iot-service/api/user/ttcode`
  (`%1%` = the region base URL; same `%1%/iot-service/api/user/...` family as the rest of the cloud API.)
- Region wiring: `region update tutk = {}`, `get_tutk_region`, `&region=`, `region missing in parameters or hostname`.
- Log strings (Windows image, confirms semantics):
  - `get_camera_ttcode success device {}`
  - `get_camera_ttcode info failed! device={} body={} error={} status={}`
  - response fields: `ttcode`, `ttcode_enc` (encrypted ttcode variant)
- The callback signature is `function<void(string)>` — the single string passed back is the **camera URL**.

### Camera-URL scheme (the string handed to BambuSource)

From `Bambu_Open` / `Bambu_Create` URL dispatch strings (Windows image + macOS `libBambuSource`):

```
bambu:///tutk?uid=<DEVICE_UID>            -> TUTK/Kalay P2P transport
bambu:///agora?app=<APP_ID>&region=<...>  -> Agora "BRTC" transport  (also seen: bambu:///agora?app=)
bambu:///local/?port=6000&user=bblp&...   -> LAN transport (printer on local net)
brtc://emmc/%1%                           -> on-printer recording playback (eMMC), not live
bambu:///logo/camera_h264                 -> built-in placeholder/logo clip (camera_h264 / camera_h264_len blob)
```

So the cloud ttcode response selects **which transport** the client uses:
- Cloud P2P via Kalay → `bambu:///tutk?uid=...`
- Cloud relay via Agora SD-RTN → `bambu:///agora?app=...&region=...`
- LAN → `bambu:///local/...`

> **Live-captured TUTK URL (confirmed, redacted).** The full `bambu:///tutk?…` query the client
> builds from the `ttcode` response is:
> ```
> bambu:///tutk?uid=<ttcode>&authkey=<8hex>&passwd=<6hex>&region=us&device=<serial>&net_ver=<plugin_ver>&dev_ver=<fw_ver>&refresh_url=1&…
> ```
> Note `uid` carries the **`ttcode` value** (not the device serial — the serial is in `device=`);
> `authkey`/`passwd`/`region` come straight from the `ttcode` response (`type:"tutk"`), and
> `net_ver`/`dev_ver` are the plugin and printer-firmware versions. On the test account `agora` was
> disabled (`agora_service:"disable"`, `tutk_server:"enable"` in `push_status`) so only the TUTK
> form was emitted. See [`29`](29-cloud-print-live.md).

**[inferred]** The cloud decides P2P-vs-Agora based on NAT/relay reachability; both are "cloud" liveview
from the user's perspective. The encrypted `ttcode_enc` field is used when the device's key info says the
payload must be encrypted (`enc_msg: get_camera_url_for_golive, dev_id = {} has no encrypt flag info`).

### golive variant

Windows image also exports `bambu_network_get_camera_url_for_golive` (P1/A1-series "Go Live" feature). It
adds an extra header (`enc_msg: get_camera_url_for_golive add extra header`) and returns a `cdnUrl` —
i.e. a CDN-backed stream rather than a direct P2P tunnel. The Linux 01.07.01.04 build is the CDN variant
(`cdnUrl` present in `.rodata`).

---

## 3. TUTK / IOTC API surface (inside `libBambuSource`)

This is **ThroughTek Kalay (IOTC + AVAPI)**, statically linked. Region/license set via
`TUTK_SDK_Set_Region` and `TUTK_SDK_Set_License_Key` (key value redacted). Two layers:

### IOTC (session / NAT traversal) layer

```
IOTC_Initialize / IOTC_Initialize2 / IOTC_DeInitialize
IOTC_Connect_ByUID / IOTC_Connect_ByUIDEx   <- client connects to printer by device UID
IOTC_Connect_Stop_BySID
IOTC_Listen / IOTC_Device_Login / IOTC_Device_LoginEx   <- device side
IOTC_Get_Nat_Type                            <- NAT type probe (full-cone/symmetric...)
IOTC_Session_Check / IOTC_Session_Check_Ex / IOTC_Check_Session_Status
IOTC_Check_Device_OnlineEx
IOTC_Session_Channel_ON / _OFF / _Get_Free_Channel   <- multiplexed sub-channels (AV, RDT, ...)
IOTC_Session_Read / _Write / _Write_DTLS              <- DTLS-secured session payloads
IOTC_Set_Master_Region / IOTC_Set_Connection_Option / IOTC_TCPRelayOnly_TurnOn
IOTC_Lan_Search / _Lan_Search2 / IOTC_Search_Device_*  <- LAN/UDP discovery of printers
IOTC_WakeUp_WakeDevice / IOTC_WakeUp_*                  <- wake sleeping device
IOTC_Set_Partial_Encryption / IOTC_Device_Update_Authkey
IOTC_sCHL_* (sCHL_connect/accept/read/write, PSK/DTLS cert ctx)  <- secure channel w/ PSK + DTLS
```

NAT-traversal flow (from macOS `libBambuSource` log strings): the SDK runs P2P tasks
`AddUDPP2PConnectTask`, `AddUDPQueryDeviceInfoTask`, `AddUDPRelayConnectTask`,
`AddUDPRelaySessionInfoRequestTask` ("Add P2P task!", "Add udp relay task") — i.e. it attempts direct UDP
P2P, then falls back to a UDP relay; TCP-relay-only mode is available
(`IOTC_TCPRelayOnly_TurnOn`). `IOTC_Set_Master_Region` is set from `get_tutk_region`.

### AVAPI (media/AV streaming over an IOTC channel) layer

```
avInitialize / avDeInitialize
avClientStart / avClientStart2 / avClientStartEx / avClientStart_inner   <- client opens AV channel
avServStart2 / avServStart3 / avServStartEx / avServStart2_inner / avServStop / avServExit   <- device side
avClientRequestTokenWithIdentity / avClientRequestDeleteTokenWithIdentity   <- per-session auth token
AvTokenRequestMediate / AvTokenDeleteMediate                                 <- token mediation
avClientRequestServerAbility / avClientFreeServerAbility
avSendFrameData / avRecvFrameData2          <- video frame transport
avSendAudioData / avRecvAudioData
avSendIOCtrl / avSendIOCtrlFrame / avRecvIOCtrl_new / avSendJSONCtrlRequest   <- in-band control (ctrl camera, PTZ-style)
avClientSetMaxBufSize / avClientCleanVideoBuf / avClientCleanLocalVideoBuf / avClientCleanAudioBuf
avEnableVSaaS / getVSaaSInterface / NewVSaaSInterface / DeleteVSaaSInterface  <- ThroughTek "Video Surveillance as a Service" cloud-record
avServNotifyCloudRecordStream / JsonVsaasPullStreamParser                     <- cloud recording / VSaaS pull
```

Block/packet plumbing: `tutk_block_Fifo*`, `tutk_packet_Fifo*`, `avPutDataToAVFrameFifo`,
`tutk_platform_*` (threads, snprintf, rand), `TUTK3rdSHA256`, and an embedded `tutkCurlHttps*` HTTPS
client (`tutkCurlHttpsCreateHandle/AddHeader/Perform/...`) used for VSaaS/token REST calls. The huge set
of `tutk_third_*` symbols in the symtab is a **privately-namespaced static OpenSSL + libcurl + json-c**
copy bundled with the Kalay SDK (DTLS, X509, PSK, etc.) — not part of the camera protocol itself.

---

## 4. BRTC components (Agora RTC)

"BRTC" = **Bambu RTC**, implemented on top of **Agora's SD-RTN / RTC SDK**. Confirmed by the macOS
artifact set: `AgoraRtcKit.framework`, `AgoraCore.framework`, `Agoraffmpeg.framework`,
`AgoraSoundTouch.framework`, and the backend host string `api.sd-rtn.com` (Agora Software-Defined
Real-time Network).

### Classes / glue inside `libBambuSource`

- `BambuTunnel` — abstract tunnel base (C-API `Bambu_*` dispatches to it).
- `BambuTunnelTutk` — TUTK/Kalay implementation.
- `BambuTunnelAgora` — Agora/BRTC implementation.
- `StaticAgoraLib` / `AgoraLib` — wrapper that loads Agora and implements
  `agora::rtc::IRtcEngineEventHandler`. `Fake_createAgoraRtcEngine` exists as a stub
  (**[inferred]** used when the Agora framework isn't present, e.g. the CDN-only Linux build).
- `scrambleWithKey` / `descrambleWithKey` — payload obfuscation over the RTC data channel.

### Agora event/handler surface (demangled)

```
BambuTunnelAgora::joinChannel()  (also "joinChannel enableEncryption")
BambuTunnelAgora::leaveChannel()
BambuTunnelAgora::onJoinChannelSuccess(channel, uid, elapsed)
BambuTunnelAgora::onLeaveChannel(RtcStats)
BambuTunnelAgora::onUserOffline(uid, USER_OFFLINE_REASON_TYPE)
BambuTunnelAgora::onRemoteVideoStateChanged(uid, REMOTE_VIDEO_STATE, reason, elapsed)
BambuTunnelAgora::onEncodedVideoFrameReceived(uid, payload, len, agora::rtc::EncodedVideoFrameInfo&)
BambuTunnelAgora::onTokenPrivilegeWillExpire(token) -> refreshToken / updateToken
BambuTunnelAgora::sendRdtMessage(...)   <- RDT = reliable data channel (in-band control)
BambuTunnelAgora::RegisterObserver(...) / getConnectionState()
StaticAgoraLib::refreshToken(BambuTunnelAgora&)
```

So BRTC liveview = the client `joinChannel`s an Agora channel (channel id + `app` id + region/areaCode
from the `bambu:///agora?app=...&region=...` URL, plus an Agora **token**), enables encryption, and
receives the camera stream via `onEncodedVideoFrameReceived` (encoded H.264 frames, fed into the same
`Bambu_GetStreamInfo`/`Bambu_ReadSample` pipeline). Token lifecycle is handled via
`onTokenPrivilegeWillExpire` -> `refreshToken`/`updateToken`. The **app id and token are redacted**
(they are fetched at runtime from the ttcode response / cloud, not hard-coded literals I extracted).

### Handshake summary (BRTC)
1. Cloud `get_camera_url` returns `bambu:///agora?app=<APP_ID>&region=<...>` (+ token in the ttcode bundle).
2. `Bambu_Open` -> `BambuTunnelAgora::initAgora` (create `IRtcEngine` via Agora SDK).
3. `joinChannel(token, channel, uid)` with `enableEncryption`.
4. `onJoinChannelSuccess` -> printer is the remote publisher; `onEncodedVideoFrameReceived` delivers frames.
5. Control via `sendRdtMessage` (RDT reliable data transport); teardown via `leaveChannel`.

---

## 5. BambuSource public media C-API (`Bambu_*`, class `BambuTunnel`)

```
Bambu_Init / Bambu_Deinit
Bambu_Create / Bambu_Destroy
Bambu_Open(url) / Bambu_Close            <- url = bambu:///{tutk|agora|local}?...
Bambu_StartStream(bool) / Bambu_StartStreamEx
Bambu_GetStreamCount / Bambu_GetStreamInfo(i, __Bambu_StreamInfo*)
Bambu_GetDuration / Bambu_Seek           <- for recordings (brtc://emmc/...)
Bambu_ReadSample                         <- pull demuxed sample (H.264 / JPEG)
Bambu_SendMessage / Bambu_RecvMessage    <- in-band control messages to printer
Bambu_GetLastErrorMsg / Bambu_SetLogger / Bambu_FreeLogMsg
```

`__Bambu_StreamInfo` (Obj-C type encoding recovered from macOS): `type`, `sub_type`, `format` union of
`video{width,height,frame_rate}` / `audio{sample_rate,channel_count,sample_size}`, `format_type`,
`format_size`, `max_frame_size`, `format_buffer`. Video decode helpers: `just::avcodec::video_info_from_jpeg`,
`video_info_from_sps`, H.264 SPS/VUI/HRD parsers — i.e. the printer chamber camera streams **H.264** (and
JPEG for some frames/models).

### LAN tunnel (`LocalTunnel_*` / `LocalServer_*`)

```
LocalTunnel_Create / Open / Read / Write / Close / Destroy / GetLastError(Msg)
LocalServer_Create / Accept / Destroy
```

This backs `bambu:///local/?port=6000&user=bblp&...`. The LAN camera socket is **TCP port 6000** on the
printer, authenticated with the printer **access code** as `user=bblp` (access code redacted). Quality
levels `liveview` / `liveviewL` / `liveviewH` (low/high) appear as in-band requests. Printer discovery is
**SSDP on UDP 1990** (`SsdpDiscovery::start_discover(), create sock 1990 thread`).

---

## 6. Liveview: cloud vs LAN/farm

| Aspect | Cloud (TUTK P2P / Agora BRTC) | LAN (LocalTunnel) | Farm liveview (already documented) |
|---|---|---|---|
| URL source | `bambu_network_get_camera_url` -> cloud ttcode | same call, returns `bambu:///local/...` when on LAN | server-issued; not from this SDK call |
| Transport | Kalay UDP P2P / relay, **or** Agora SD-RTN channel | direct **TCP 6000** to printer (`user=bblp`+access code) | printer HTTP PUT JPEG -> farm server -> client HTTP GET |
| Discovery | cloud (device UID, region) | **SSDP UDP 1990** | server inventory |
| Codec | H.264 (encoded frames) | H.264 / JPEG | **JPEG snapshots** (MJPEG-style) |
| Auth | ttcode + Agora token (`avClientRequestTokenWithIdentity` / Agora token) | printer access code | server session |
| Server hop | Agora SD-RTN (`api.sd-rtn.com`) or Kalay relay | none (peer-to-peer on LAN) | farm server is the relay |

**How the farm liveview differs:** The farm/OpenBambuFarm liveview path we documented (printer PUTs JPEG
frames to the server, client GETs them) is **independent of this TUTK/BRTC stack**. It does **not** use
`libBambuSource`, Kalay, or Agora. It is a server-mediated MJPEG-style pull, whereas the official SDK
liveview is either (a) P2P/relayed via ThroughTek Kalay, (b) routed through Agora's RTC network (BRTC),
or (c) a direct LAN TCP socket on port 6000. The farm server effectively **replaces** the cloud
P2P/Agora relay with a simple JPEG store-and-forward, sidestepping TUTK and Agora entirely.
**[inferred]** This is why the farm approach needs no Kalay license key or Agora app id — it never opens a
`bambu:///` tunnel.

---

## 7. Ports / endpoints (recovered)

| Endpoint / port | Purpose |
|---|---|
| `%1%/iot-service/api/user/ttcode` (HTTPS, region base URL) | fetch camera ttcode / TUTK+Agora creds |
| `api.sd-rtn.com` | Agora SD-RTN (BRTC) signaling/media backend |
| TUTK/Kalay relay+masterserver (region-selected; addr not literal) | P2P NAT traversal / UDP relay |
| TCP **6000** (printer) | LAN camera liveview (`bambu:///local/?port=6000&user=bblp`) |
| UDP **1990** (LAN) | SSDP printer discovery |
| `ssl://{cn,us}.mqtt.bambulab.com:8883` | cloud MQTT control (not camera, but adjacent) |
| `brtc://emmc/%1%` | on-printer eMMC recording playback (VOD, not live) |

---

## 8. Secrets / redactions

- TUTK/Kalay **license key** (`TUTK_SDK_Set_License_Key`) — referenced by symbol; value not extracted.
- Agora **App ID** and channel **token** — supplied at runtime in the ttcode response / `bambu:///agora?app=`; not hard-coded literals I pulled.
- Printer **access code** (`user=bblp` LAN auth) — per-device, not in the binary.
- `ttcode_enc` payloads are encrypted; encryption-flag logic noted but keys not recovered.

---

## 9. Open items / inferences to verify

- **[inferred]** Exact cloud decision logic for returning `tutk` vs `agora` vs `cdnUrl` (likely NAT type +
  model + golive flag) — would need disassembly of `AccountManager::get_camera_url` (0x20bfd0), which the
  sandbox blocked here.
- **[inferred]** `Fake_createAgoraRtcEngine` is the no-Agora fallback path; confirm it disables BRTC on the
  CDN-only Linux build.
- Whether `liveviewL`/`liveviewH` map to distinct Agora streams or to in-band resolution requests on the
  same channel — string evidence only.
