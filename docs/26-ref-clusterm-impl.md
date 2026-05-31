# ClusterM / open-bamboo-networking — Reference Notes

Source: <https://github.com/ClusterM/open-bamboo-networking> (license AGPL-3.0).
Fetched 2026-05-31 via WebFetch of repo root, README, NETWORK_PLUGIN.md, STATUS.md and
the raw source files cited inline. Everything below is already public in that repo; no
secrets beyond what the repo itself publishes.

---

## 1. What it implements

A **working, clean-room C++ reimplementation of Bambu Studio's `libbambu_networking`
plugin** (the `.so`/`.dylib`/`.dll` Studio dlopen's). It is **LAN-first**: it
re-implements the C++ ABI Studio calls and routes everything over the LAN when possible,
contacting the cloud only for account login and preset sync.

- **Language:** C++ (~81%), plus CMake/Python/shell glue. Deps via `vcpkg.json`.
- **Two symbol sets in one binary:** the `bambu_network_*` ABI (connect, print, user)
  and the `ft_*` ABI (file transfer over port 6000). Plus a companion
  `libBambuSource.so` for camera + file-browser (the `stubs/BambuSource.cpp` build).
- **Implemented & working (per STATUS.md):**
  - LAN discovery — SSDP multicast `239.255.255.250:1990`, NOTIFY on UDP 2021.
  - LAN MQTT telemetry + commands (`mqtts://<ip>:8883`, user `bblp` / pass `<access_code>`).
  - Cloud account login (browser OAuth ticket → token), preset/filament sync.
  - LAN printing (FTPS and/or `:6000` emmc upload, then MQTT `project_file`).
  - **Camera: LOCAL only** — MJPEG/TLS port 6000 (A1/A1mini) and RTSPS/H.264 port 322
    (P1S/X1/P2S/H/X2D). Tested on P2S.
  - `ft_*` LAN native file transfer over port 6000 (BambuTunnelLocal).
- **NOT implemented:** cloud/remote camera (TUTK/Agora p2p), MQTT command RSA
  signing (so non-Developer-Mode printing fails with `84033543`), MakerWorld,
  cloud `ft_*`/TUTK URLs (return `FT_EIO`).

ABI surface is documented in `NETWORK_PLUGIN.md` (print entry points
`bambu_network_start_print`, `start_local_print[_with_record]`, `start_sdcard_print`,
`start_send_gcode_to_sdcard`; callback set `OnMsgArrivedFn`, `OnUserLoginFn`,
`OnMessageFn`, `OnLocalConnectedFn`, `QueueOnMainFn`; init sequence
`create_agent` → `set_config_dir` → `init_log` → `set_cert_file("…/cert",
"slicer_base64.cer")` → register callbacks → `set_country_code` → `start` →
`start_discovery`). The C++ ABI uses `std::string`/`std::vector`/`std::function`, so
libstdc++ ABI compatibility is mandatory.

---

## 2. CAMERA / liveview — **THE most valuable part**

### Does it solve the camera?
- **LOCAL camera: YES** (both transports). This is the strongest contribution.
- **REMOTE / cloud camera (TUTK Kalay / Agora BRTC): NO** — explicitly out of scope.
  `src/abi_camera.cpp` is a stub returning empty (`callback(std::string{})`) with the
  comment that *"Cloud-signed TUTK/Agora liveview is intentionally not implemented —
  this plugin is a LAN-first replacement."* No Agora App ID, no Kalay license, no
  cloud-issued p2p token is extracted, hardcoded, or reproduced. **It gives us nothing
  for the remote camera.**

### LOCAL camera lives in `stubs/BambuSource.cpp` (the `libBambuSource` lib)

URL scheme dispatch (how Studio asks BambuSource to open a stream):
```
bambu:///local/<ip>?port=6000          → local MJPEG over TLS on port 6000
bambu:///rtsps___<user>:<passwd>@<ip>  → RTSPS on port 322
bambu:///rtsp___<user>:<passwd>@<ip>   → RTSP  on port 554
```
Entry points: `Bambu_Create()` (parse URL, alloc tunnel), `Bambu_Open()`
(`obn::tls::dial_tls()` 5 s timeout), `Bambu_StartStream()` (sends auth via
`ssl_write_all()`), `Bambu_ReadSample()` (read header, validate, return frame).

#### Local-6000 handshake (MJPEG over TLS) — concrete wire format
TLS first: `dial_tls(ip, 6000)`, chain validated against `printer.cer` with `CN=serial`
unless `OBN_SKIP_TLS_VERIFY` is set.

**80-byte auth packet** sent immediately after TLS (`build_auth_packet()`):
```
[0..3]    uint32_le = 0x40     (payload size = 64)
[4..7]    uint32_le = 0x3000   (type: auth)
[8..15]   uint32_le = 0        (flags/pad, 8 bytes)
[16..47]  32 bytes  username  ASCII, NUL-padded   (default "bblp")
[48..79]  32 bytes  password  ASCII, NUL-padded   (the access code)
```
(16-byte fixed header + 32-byte user + 32-byte pass = 80 bytes total.)

**Per-frame 16-byte header**, then a raw JPEG of `payload_size` bytes:
```
[0..3]    uint32_le payload_size   (JPEG byte count)
[4..7]    uint32_le itrack
[8..11]   uint32_le flags
[12..15]  uint32_le pad
```
JPEG validated `FF D8 … FF D9`; max frame `1u << 20` (1 MB).

This is exactly the local-6000 camera auth + framing we needed to nail down. It
matches the same port-6000 BambuTunnelLocal endpoint used for `ft_*` (different
`type`/`cmd_type` selectors on the same TLS socket).

#### RTSPS port 322 path — `stubs/rtsp_client.cpp`
- `dial_tls(host, 322)`, then RFC-2326 line protocol over TLS.
  Sequence: OPTIONS → DESCRIBE (`Accept: application/sdp`) → SETUP
  (`Transport: RTP/AVP/TCP;unicast;interleaved=0-1`) → PLAY (`Range: npt=0.000-`).
  `User-Agent: open-bamboo-networking/0.1`.
- **Auth = user `bblp` / pass = access code**, stashed at `start()` as `I.user`/`I.passwd`.
  Two schemes auto-selected from the 401:
  - Basic: `Authorization: Basic base64(user:passwd)`
  - Digest (RFC 2617): `HA1=MD5(user:realm:passwd)`, `HA2=MD5(method:uri)`,
    `response=MD5(HA1:nonce:HA2)`.
- RTP/H.264 interleaved framing `'$' <chan:1> <len:2> <pkt>`, channel 0 = video,
  `decode_rtp_h264()` handles single NAL (1–23), STAP-A (24), FU-A (28 reassembly),
  emits **Annex-B** to Studio's decoder.
- **Keepalive:** background thread sends `GET_PARAMETER … RTSP/1.0` every 15 s (with a
  fresh Authorization header) to dodge the printer's ~30 s idle teardown.

Other stub files of interest: `stubs/tls_socket.cpp` (TLS), `stubs/image_io.cpp`
(JPEG), `stubs/rtsp_passthrough.cpp`, `stubs/dshow_filter.cpp` (Windows DirectShow),
`stubs/live555.cpp` (live555 integration). macOS camera needs an Obj-C `BambuPlayer`
class that is **not shipped** (camera broken on macOS).

---

## 3. Auth crypto — what it gives us, and the gaps

### Cloud login (`src/cloud_auth.cpp`) — fully implemented, no client-side signing
- Browser OAuth: localhost callback → `POST /v1/user-service/user/ticket/<TICKET>`
  body `{"ticket":"<TICKET>"}`, against `https://api.bambulab.com`
  (or `https://api.bambulab.cn` for CN).
- 200 returns `accessToken`, `refreshToken`, `expiresIn`, `loginType`.
- Token used as `Authorization: Bearer <access_token>`.
- Refresh: `POST /v1/user-service/user/refreshtoken` body `{"refreshToken":"…"}`.
- **No HMAC/RSA/SHA request signing** — relies purely on TLS + bearer token.
- `src/auth.cpp` is **only session persistence** (`obn.auth.json`, chmod 0600, atomic
  write, ISO8601 expiry) — no crypto.

### MQTT broker auth — concrete username/password construction
- **Cloud MQTT** (`src/cloud_session.cpp`):
  - host `us.mqtt.bambulab.com` (or `cn.mqtt.bambulab.com`), TLS, `tls_insecure=false`.
  - `username = "u_" + user_id`
  - **`password = access_token`** (the cloud bearer access token, passed straight in).
  - `client_id = "u_" + user_id + "_" + hex(rng64)`.
- **LAN MQTT** (`src/lan_session.cpp`):
  - host = printer IP, port `8883` TLS (`1883` plaintext), verified against
    `printer.cer` with `tls_verify_hostname = <serial>`.
  - **username `bblp` / password = access code** (per README; passed in as
    `username,password` args).
  - `client_id = "obn-" + hex(rng) + "-" + steady_clock_ns`.
- Routing is **LAN-first**: try LAN session for the `dev_id`, fall back to cloud.

### What ClusterM does **NOT** provide (still our gaps)
- **No `BambuFarm` username, no `u_<uid>` farm variant, no 49-char farm secret,
  no farm MQTT signing** — these strings/algorithms appear **nowhere** in the repo.
  ClusterM only knows `u_<uid>`+access_token (cloud) and `bblp`+access_code (LAN).
- **No MQTT command RSA signing.** `src/print_job.cpp build_project_file_json()`
  emits **plaintext** `{"print":{"command":"project_file",…}}` with **no** `header`
  signature block, and it does **not** produce a real `url_enc`. The
  `NETWORK_PLUGIN.md` sample shows `url_enc:"<AES+RSA-encrypted url>"` only as a
  *captured* field; the code never generates it. No OpenSSL EVP signing is used in
  print_job (only `md5_of_file()` for the `md5` field). Consequence: non-Developer-Mode
  printing is rejected with `84033543 "MQTT Command verification failed"`. Their
  documented **workaround is Developer Mode** (LAN-Only + LAN-Only-liveview + Developer
  mode on the printer), which skips printer-side signature verification.
- **No device-cert / `enc_msg` / bind-secret crypto.** `cloud_session.cpp` explicitly
  has *no* `enc_msg`, `bind_ticket`, or device-certificate logic. Binding (per
  STATUS.md) is just SSDP + cloud bind/unbind REST calls.

---

## 4. `ft_*` file transfer

- **LAN native over port 6000 (BambuTunnelLocal): YES, fully implemented.**
  Files: `src/abi_ft.cpp`, `src/tunnel_local.cpp`, `src/tunnel_upload.cpp`.
  Handshake `ft_tunnel_create` → `ft_tunnel_connect`, BBLP access-code exchange + TLS
  (same port-6000 socket family as the camera). `ft_job` selectors:
  - `cmd_type=7` capability probe
  - `cmd_type=5` chunked upload (dest `"emmc"` or `"udisk"`),
    `get_send_finished_event()` completion
  - `cmd_type=4` memory/preview download (Printer Preview, JPEG `ff d8 ff … JFIF`).
  Pipelined chunks verified byte-identical to the stock plugin on P2S.
- **Cloud FT tunnel / TUTK URLs: NO.** Cloud/TUTK URLs return `FT_EIO`; Studio falls
  back to FTPS where it can. So the **cloud FT wire protocol is not reverse-engineered
  here.**
- **Cloud 3MF → S3** (`src/cloud_print.cpp`) IS implemented as a 6-step HTTP/S3 flow
  (create project → presigned PUT config.3mf → notify → poll → PATCH placeholder URL →
  presigned PUT main.3mf → PATCH real URL). But S3 presign is **server-issued** (client
  just PUTs); no client-side AWS signing of substance.
- Plain **FTPS** also implemented: `src/ftps.cpp`, `include/obn/ftps_parse.hpp`.

---

## 5. What we'd still be MISSING for a full reimplementation

ClusterM closes the **local camera** gap completely (both 6000-MJPEG and 322-RTSPS,
auth + framing + depacketization) and pins down the **MQTT credential construction**
(`u_<uid>`/access_token cloud, `bblp`/access_code LAN). It does **not** close:

1. **Cloud/remote camera** (TUTK Kalay handshake, Agora BRTC App ID / license /
   cloud-issued p2p token) — fully absent, intentional.
2. **MQTT command signing** — the `header` RSA-SHA256 block and the real `url_enc`
   (AES+RSA) needed to print on stock (non-Developer-Mode) firmware. ClusterM
   sidesteps this with Developer Mode rather than reproducing the obfuscated
   per-install RSA keys.
3. **`BambuFarm` / farm-secret (49-char) MQTT auth** — not present at all; if our
   target uses a farm-mode broker identity, ClusterM offers no algorithm.
4. **Device-cert / `enc_msg` / bind crypto** — absent.
5. **Cloud FT tunnel wire protocol** (TUTK-tunneled file transfer) — absent
   (`FT_EIO`).

Useful companion docs in the repo: `NETWORK_PLUGIN.md` (full ABI + init + print
envelope), `STATUS.md` (feature/ABI matrix), `README.md` (Developer-Mode setup, env
vars like `OBN_LOG_*`, `OBN_SKIP_TLS_VERIFY`, `OBN_BAMBUSOURCE_LOG_FILE`).
