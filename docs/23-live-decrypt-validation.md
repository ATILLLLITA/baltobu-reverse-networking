# Live on-wire validation (decrypted local MQTT) + OpenSSL key extraction

A live capture of `bambu-studio.exe` talking to a freshly cloud-bound printer, decrypted to
confirm the static findings against ground truth. No account tokens / secrets are reproduced.

## Channels observed (one bound printer, restarted slicer)

| Stream | Peer | TLS | Cipher | Role |
|--------|------|-----|--------|------|
| local | `192.168.88.96:8883` (printer) | **1.2** | `0xc030` ECDHE_RSA_AES256_GCM_SHA384 | printer's own MQTT broker (LAN) |
| cloud | `*.mqtt.bambulab.com:8883` (EMQX/AWS) | **1.3** | `0x1301` AES_128_GCM_SHA256 | Bambu cloud broker |

So Studio keeps **both** channels up: direct LAN MQTT to the printer (TLS 1.2) and the cloud
broker (TLS 1.3) — matching the `DeviceSubscribeManager` local/cloud arbiter (`small-subsystems`).

## OpenSSL TLS 1.2 key extraction (technique)

Bambu Studio's statically-linked OpenSSL does **not** honour `SSLKEYLOGFILE`. The TLS 1.2
master secret was instead recovered from a `MiniDumpWriteDump` of the live process:

1. From the pcap: `client_random`, `server_random`, the negotiated cipher, and the **ServerHello
   `session_id`** of the target stream; plus the client `Finished` record (seq 0).
2. In the minidump (`Memory64List`), find the 32-byte `session_id` — it's a member of OpenSSL's
   `SSL_SESSION`, adjacent to `master_key[48]`.
3. Scan ±16 KB around the `session_id` for a 48-byte window whose `master_key` **GCM-verifies**
   the client `Finished` (TLS1.2 PRF-SHA384, key-block 72). One hit.
4. Emit `CLIENT_RANDOM <cr> <ms>` and decrypt.

(This is the OpenSSL analogue of the Go-`ekm` technique used for the farm server; the anchor is
`session_id` instead of a Go slice header.)

## Result — local channel is plain JSON (no binary framing)

29 `device/<sn>/report` PUBLISH messages decrypted, **0 non-JSON** payloads → the local channel
carries **plain UTF-8 JSON**, confirming `16-mqtt-channels-framing`. The `JsonOrJsonBinFramer`
binary path is **not** used for the device report stream (it is the cloud RPC/job tunnel).

### Real on-wire `push_status` (full snapshot, validates `20-pushstatus-schema`)

```json
{"print":{
  "ipcam":{"ipcam_dev":"1","ipcam_record":"disable","timelapse":"disable","resolution":"","tutk_server":"enable","mode_bits":3},
  "upload":{"status":"idle","progress":0,"message":""},
  "net":{"conf":0,"info":[{"ip":<int-encoded>,"mask":16777215}]},
  "nozzle_temper":27.78,"nozzle_target_temper":0,"bed_temper":25.53,"bed_target_temper":0,"chamber_temper":5,
  "mc_print_stage":"1","heatbreak_fan_speed":"0","cooling_fan_speed":"0","big_fan1_speed":"0","big_fan2_speed":"0",
  "mc_percent":0,"mc_remaining_time":0,"ams_status":0,"ams_rfid_status":6,"hw_switch_state":1,
  "spd_mag":100,"spd_lvl":2,"print_error":0,"lifecycle":"product", …}}
```

Every key matches the open-source schema in `20-pushstatus-schema.md`. Differential updates carry
only the changed keys with `"msg":1` (e.g. just `wifi_signal`+`sequence_id`); the full snapshot
omits `msg` (or `msg:0`). `net.info[].ip` is a little-endian int-encoded IPv4. `tutk_server:enable`
corroborates the camera/TUTK stack (`09-camera-tutk-brtc`).

## Cloud channel (TLS 1.3) — and why it's idle

The cloud broker negotiated **TLS 1.3** (`0x1301`). Two independent obstacles to reading the
`JsonOrJsonBinFramer` wire format from a normal capture:

1. **The cloud channel carries no application data while the printer is LAN-reachable.**
   `DeviceSubscribeManager` prefers the local channel; with the printer on the LAN, *all*
   `push_status` flows over local TLS 1.2 (decrypted above) and the cloud MQTT session holds only
   keepalives — **0 app-data records** captured. So even decrypting TLS 1.3 yields nothing here.
2. **Downgrade is blocked.** `OPENSSL_CONF` with `MaxProtocol=TLSv1.2` is **ignored** by the
   slicer's statically-linked OpenSSL (it still offered TLS 1.3); a MITM downgrade is impossible
   because the client pins the Bambu cert chain.

To exercise the cloud framer one must **force cloud-only** (block the printer's LAN IP so the
local channel fails over to cloud) **and trigger a cloud job/RPC** (e.g. a cloud print or file
transfer — the binary framer is the cloud job tunnel, not the plain-JSON report relay), then
extract the TLS 1.3 traffic secrets from a fresh process dump. High effort for the
CBOR-vs-MessagePack discriminator byte; pursued separately.

> Captured/decrypted on the operator's own machine and printer for interoperability. No account
> tokens, keys, serial-bound secrets, or the master secret value are published here.
