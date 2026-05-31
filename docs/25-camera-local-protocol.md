# Bambu camera — local protocol (reimplementable) vs remote (vendor-locked)

Consolidated from four independent sources: OpenBambuAPI (`video.md`), the **working clean-room
code in ClusterM/open-bamboo-networking**, the open-source BambuStudio (`MediaPlayCtrl.cpp`),
and our `libBambuSource` binary (`BambuTunnel*` class map). The earlier `09-camera-tutk-brtc`
established the stack; this doc nails the **reimplementable local path**.

## Verdict

| Path | Reimplementable? | Why |
|------|------------------|-----|
| **Local MJPEG** (A1 / A1 mini) — TCP **6000** / TLS | ✅ **yes** | protocol fully known + working ClusterM code; only credential is the printer **Access Code** |
| **Local H.264** (P1S / X1 / P2S / X2D …) — **RTSPS** | ✅ **yes** | RTSP over TLS, `bblp`/access-code, RTP-H.264; ClusterM implements it |
| **Remote** (cloud, off-LAN) — TUTK Kalay / Agora BRTC | ❌ **no (clean-room)** | app-id/token/channel + Kalay UID/license are minted by the **closed** plugin from the Bambu cloud `ttcode`; never in OSS |

## Local MJPEG — TCP 6000 / TLS (A1, A1 mini)

Studio builds (`MediaPlayCtrl::Play`):
```
bambu:///local/<DEV_IP>.?port=6000&user=bblp&passwd=<ACCESS_CODE>&device=<DEV_ID>&net_ver=<agent>&dev_ver=<fw>&cli_id=<uuid>&cli_ver=<studio_ver>
```
`port` is fixed 6000, `user` is always literal `bblp`, `passwd` = the printer **LAN Access Code**.

Wire (TLS to `<ip>:6000`) — **80-byte auth packet** sent immediately on connect:

| Offset | Size | Field |
|--------|------|-------|
| 0 | 4 | `0x40` (payload size = 64) — LE |
| 4 | 4 | `0x3000` (type) — LE |
| 8 | 8 | padding (0) |
| 16 | 32 | username `bblp`, NUL-padded |
| 48 | 32 | password = access code, NUL-padded |

Then the server streams **JPEG frames**, each with a **16-byte header**:

| Offset | Size | Field |
|--------|------|-------|
| 0 | 4 | JPEG payload size (LE) |
| 4 | 4 | itrack (0) |
| 8 | 4 | flags (1) |
| 12 | 4 | reserved (0) |

followed by the JPEG (`FF D8 … FF D9`, 1280×720, up to ~1 MB, may be fragmented → concatenate).

## Local H.264 — RTSPS (P1S, X1, X2D, …)

Newer chamber cams use RTSP-over-TLS instead of the 6000 framing. Studio URL form:
```
bambu:///rtsps___bblp:<ACCESS_CODE>@<DEV_IP>/streaming/live/1?proto=rtsps
```
ClusterM's `rtsp_client.cpp`: TLS dial → `OPTIONS / DESCRIBE / SETUP / PLAY`, auth `bblp` +
access-code (Basic or Digest, `HA1 = MD5(user:realm:pass)`), interleaved RTP, `decode_rtp_h264`
(single-NAL / STAP-A / FU-A) → Annex-B, 15 s `GET_PARAMETER` keepalive. `bambu:///camera_h264`
seen in the binary is an internal idle/logo placeholder, **not** a real endpoint.

Both local transports are exposed through the open `Bambu_*` C tunnel API
(`Bambu_Open` → `Bambu_StartStream` → `Bambu_ReadSample` → `Bambu_Close`, header
`Printer/BambuTunnel.h`). The `libBambuSource` class map confirms the split:
`BambuTunnelLocal` (the above), `BambuTunnelTutk` / `BambuTunnelLive` (remote), `BambuTunnelLogo`.

## Remote camera — why it's vendor-locked

Studio does **not** construct the remote URL; it calls the closed plugin:
```cpp
agent->get_camera_url("<dev_id>|<fw_ver>|<proto>", cb);   // proto ∈ "", "tutk", "agora"
```
The plugin returns a ready-made opaque `bambu:///tutk?…` / `bambu:///agora?…`. The Agora
**app-id / token / channel** and the **TUTK UID / license key** are minted at runtime inside
the closed plugin from Bambu's cloud (the account / IoT `ttcode` camera-url endpoint) and never
appear in the open source. Remote-Agora availability is gated by `home_flag` **bit 30**
(`is_support_agora = (flag >> 30) & 1`, `DeviceManager.cpp`).

**Nuance:** the only *hard* secret is the TUTK **Kalay license key** (needed for `IOTC_Initialize` /
`TUTK_SDK_Set_License_Key`). The Agora path is per-channel **token-gated** — if you have a Bambu
cloud account, the cloud *issues* the token + app-id in the `ttcode` response, so an Agora client
could in principle join using cloud-issued credentials (you don't need Agora's secret App
Certificate to *consume* a token). So: **Agora remote = possible with a cloud account + decoding
the ttcode response; TUTK/Kalay remote = blocked by the embedded license.** Neither is needed for
the local path.

## Practical takeaway
For any on-LAN viewer (the common case) the camera is **fully reimplementable today** — connect to
the printer with the access code (6000-MJPEG or RTSPS-H.264). Remote/off-LAN streaming is the only
part that needs Bambu's cloud + vendor SDKs.
