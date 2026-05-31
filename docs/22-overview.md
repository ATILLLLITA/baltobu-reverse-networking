# Linux-Rosetta + live-DLL companion series — overview (08–19)

A cross-source reverse of Bambu's networking SDK (`libbambu_networking` / `bambu_networking.dll`),
complementing the main baltobu effort. Two independent inputs the DLL-only path doesn't use:

1. **Linux `01.07.01.04`** — a symbol-bearing, *unprotected* build: real `.text` bodies, named,
   addresses/sizes; `.rodata` VA == file offset. The "Rosetta stone" for the VMProtect'd Windows DLL.
2. **Windows `02.06.00.50`** — a `ReadProcessMemory` dump of the live `bambu-studio.exe` module,
   where VMProtect is already decrypted in memory (the newer build with `ft_*`, `enc_msg`, etc.).

All findings are facts (symbols, signatures, struct sizes, JSON keys, hostnames). No vendor code,
no secret key material. Cross-verified against baltobu `05`/`06` and our own live farm capture.

## The map

| Doc | Subsystem | Key result |
|-----|-----------|------------|
| `08-linux-rosetta` | core ABI | `create_agent` body recovered; **79 exact C++ ABI signatures**; cloud host matrix |
| `09-camera-tutk-brtc` | camera | `libBambuSource`=media plane; **Kalay(TUTK)+Agora(=BRTC)**; ttcode flow; URL-scheme transport selector; H.264 |
| `10-ft-abi` | file tunnel | **21 `ft_*` C signatures**; intrusive-refcount handle model; blocking vs try; callback types |
| `11-print-params` | print | `PrintParams`/`PublishParams` layout; dual snake/camel serialization; version deltas |
| `12-login-json` | login | `studio_userlogin {command,seq,data:{name,avatar}}`; printer-login `{login,command,wifi,tutk}`; sign-in ticket/token |
| `13-error-codes` | errors | lib negative codes + `enc_msg` app codes `0x05022647–4C`; HTTP/Paho/X509 pass-through |
| `14-device-report-fields` | telemetry | **SDK does NOT parse push_status** (opaque forward); documents SDK-owned cache/command/config keys |
| `15-oss-upload-sigv4` | upload | OSS/S3 STS schema; full **AWS-SigV4 + Aliyun-OSS-v1** signing; `my/task`/publish payloads |
| `16-mqtt-channels-framing` | mqtt | cloud vs local channels (TLS:8883, client-id/user/cert); report = plain JSON; framer = cloud tunnel (wire fmt open) |
| `17-ssdp-discovery` | discovery | `SsdpDiscovery` on lssdp; `urn:…device:3dprinter:1`; full `Dev*.bambu.com` header set |
| `18-device-cert-security` | auth | `app_cert_install`/`enc_msg` device-cert handshake; custom pinned X509; `aes256` cert fetch; `sec_link` |
| `19-http-client` | http | `BBL::Http` libcurl wrapper; `X-BBL-*` headers; refresh-on-401 retry; pinned TLS + `CURLOPT_RESOLVE` |

## Architecture in one paragraph

`bambu_network_create_agent` builds a 16-byte handle → a ~2.7 KB `BBL::BambuNetworkAgent`
singleton (a **facade**). Every C export validates the handle == singleton and delegates to a
C++ method. The agent owns `AccountManager` (auth/login/tickets/region/MQTT-connect),
`DeviceSubscribeManager` (the cloud/local MQTT channel arbiter), a `BBL::Http` libcurl client,
an OSS/S3 signer, and `SsdpDiscovery`. Control rides MQTT (`device/<sn>/{request,report}`,
both TLS:8883); the report body is **plain JSON forwarded opaquely to the slicer**, which owns
the `push_status` schema. The `ft_*` tunnel + `libBambuSource` (Kalay/Agora) are independent
planes for file transfer and camera.

## Build delta (01.07 → 02.06), recurring theme

02.06 adds: `ft_*` tunnel API, `enc_msg`/`app_cert_install` device-cert security, `ams_mapping2`
+ multi-nozzle keys, the full `X-BBL-*` header suite, `DevVersion`/`DevInf` SSDP headers, and
`connection_name`/`dev_version` in the device cache. 01.07 is the older `TunnelMqtt`-based arch
with no Framer classes.

## Open items / resolutions

- **Full `push_status` schema** — ✅ **RESOLVED** via the open-source slicer
  (`BambuStudio`/`OrcaSlicer` `DeviceManager.cpp`, AGPL). See `20-pushstatus-schema`.
- **Framer wire format** (CBOR vs MessagePack, discriminator byte) — **deferred.** A live capture
  of the cloud broker (`us.mqtt.bambulab.com` → EMQX Cloud Pro on AWS, `54.185.138.159`) confirmed
  it speaks **TLS 1.3** (19-byte keepalive records: 2-byte MQTT PING + content-type + 16-byte tag),
  and an idle session emits only keepalive PINGs — no PUBLISH carries the framer payload. Recovering
  the CBOR/MsgPack discriminator would need a *triggered* cloud PUBLISH plus TLS 1.3 traffic-secret
  extraction from the slicer's (OpenSSL) memory — high effort, low value. Left open.
