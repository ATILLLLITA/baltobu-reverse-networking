# Cloud file-transfer tunnel: it is TUTK (Kalay), and the open path is a hybrid

[`16-mqtt-channels-framing`](16-mqtt-channels-framing.md) left the cloud "tunnel" framer wire
format open, and [`26-ref-clusterm-impl`](26-ref-clusterm-impl.md) listed the **cloud FT tunnel
wire protocol** as the last non-camera gap. This document resolves it: the cloud file-transfer
data plane is **ThroughTek Kalay P2P (TUTK)** — the *same* vendor-locked transport as the remote
camera — so there is no mystery binary framer to reverse. It then gives the realistic path to
cloud file transfer in an open networking plugin: **delegate the transport to Bambu's own
`libBambuSource`** rather than reimplement Kalay.

## The two-plugin split (what owns what)

Bambu Studio loads **two** native plugins and binds different ABIs to each:

| Plugin | ABI it exports | Role |
|---|---|---|
| `libbambu_networking.so` | `bambu_network_*` (control) **and** the `ft_*` file-transfer module | Cloud account / HTTP / MQTT control plane; owns the *file-transfer job/tunnel objects* |
| `libBambuSource.so` | `Bambu_*` (`Init`/`Create`/`Open`/`SendMessage`/`RecvMessage`/`ReadSample`/`StartStream`/`Close`) | Media + **transport** plane: the `BambuTunnel{Tutk,Agora,Local,Camera,Live555}` classes |

Confirmed from the shipped libraries' dynamic export tables:

- `libbambu_networking.so` exports the 21 `ft_*` entries (`ft_tunnel_create`, `ft_tunnel_start_job`,
  `ft_job_create`, …) and `bambu_network_*`. It exposes **no** `IOTC_*`/`TUTK_*`/`Bambu_*` symbols.
- `libBambuSource.so` exports `Bambu_*` **and** the Kalay SDK: `IOTC_Initialize`, `avClientStart`,
  `IOTC_Lan_Search`, `TUTKSSL_*`, plus the license entry points `TUTK_SDK_Set_License_Key`,
  `SetLicenseKey`, `CheckLicenseKeyIsValid`, `GetLicenseKeyState`. The Agora ("BRTC") backend lives
  here too (`onLicenseValidationFailure`, `createAgoraRtcEngine`).

So **the Kalay license check is inside `libBambuSource`, Bambu's own library** — not in the
networking plugin a clean-room project replaces.

**The delegation is runtime `dlopen`/`dlsym` (confirmed).** `libbambu_networking.so` imports
`dlopen` + `dlsym` (and lists `libdl.so.2` as `NEEDED`), while it has **no** load-time `NEEDED`
on `libBambuSource` and **no** undefined `Bambu_*`/`IOTC_*` symbols in its dynamic table (the
VMProtect packer strips section headers but the dynamic symbol/relocation tables stay intact for
the loader, so this is reliable). It therefore reaches the transport plane by
`dlopen("libBambuSource.so")` + `dlsym("Bambu_…")` at run time, not by linking it. Both `.so`s are
mapped together in the live Studio process. The practical consequence: an **open** networking
plugin can resolve the genuine `Bambu_*` transport the identical way.

## The cloud FT tunnel is TUTK

The file-transfer module tries transports in a fixed order (from the open-source Studio side,
`FileTransferUtils` + the Send-to-Printer flow): **`tcp` (LAN `:6000`) → `tutk` (cloud P2P) →
`ftp` (fallback)**. The LAN `tcp` leg is the brtc/file channel documented in
[`29-cloud-print-live`](29-cloud-print-live.md) (`cmdtype 7` ability, `cmdtype 5` chunked upload,
`cmdtype 4` download; `mtype 12289` replies). The **cloud leg is `tutk`** — `BambuTunnelTutk`, i.e.
ThroughTek Kalay P2P (UDP, relay-assisted; the library carries `AddUDPRelayConnectTask` /
`avClientStart` / the full IOTC/AVAPI surface).

Therefore the "open framer" question from [`16`](16-mqtt-channels-framing.md) resolves as:

- The `JsonOrJsonBinFramer` / `JobMsg`/`JobResp`/`JobFail` MQTT RPC is the **control plane** that
  *negotiates / sets up* the tunnel (and other request/response ops). It is not the bulk file
  format.
- The **bulk file bytes ride the Kalay P2P session**, not an exotic CBOR/MsgPack MQTT frame. There
  is nothing to gain from decoding a "binary framer": the data plane is Kalay.

This makes the cloud FT tunnel **vendor-locked by the embedded Kalay license**, exactly like the
remote camera (see [`25-camera-local-protocol`](25-camera-local-protocol.md) §Remote). A clean-room
reimplementation cannot mint the Kalay license; it is the one hard secret.

## The realistic open path: hybrid (keep Bambu's `libBambuSource`)

A fully clean-room replacement of *both* plugins necessarily loses TUTK (camera **and** cloud FT) —
this is intrinsic, not a missing feature. But cloud file transfer is still reachable without
cracking Kalay, because the license and transport are isolated in `libBambuSource`:

> Ship the **open** `libbambu_networking.so` and keep the **genuine** `libBambuSource.so` (the
> library Studio already loads for the camera). The open networking plugin's `ft_*` cloud branch
> opens a `Bambu_*` tunnel against the genuine source library and runs the **same brtc file
> protocol it already implements for LAN** (`cmdtype 4/5/7`) over `Bambu_SendMessage` /
> `Bambu_RecvMessage` instead of a raw `:6000` TLS socket.

What this needs, and what it does **not**:

- **Reuses** the brtc upload/ability/download state machine already written for the LAN tunnel —
  only the byte transport is swapped (direct TCP → `Bambu_*` tunnel).
- **Does not** require reimplementing Kalay, extracting the license key, or running a Kalay relay
  client — `libBambuSource` does all of that, license included.
- The delegation *mechanism* (`dlopen`+`dlsym` of `Bambu_*` in `libBambuSource`) is **confirmed**
  (above), and the exact file-tunnel call *sequence* has now been **runtime-traced** — see the next
  section. (The trace had to wrap **`dlsym` itself**, since the plugin resolves the transport via
  `dlsym`, not load-time linking, so a plain `LD_PRELOAD` of `Bambu_*` does not interpose.)

## Runtime-confirmed file-tunnel sequence (dlsym trace)

Confirmed by wrapping `dlsym` and driving a genuine off-LAN operation against an eMMC printer
(`device=<sn>`). The networking plugin drives `libBambuSource` exactly as a hybrid would need to —
identifying values (Kalay UID, authkey, passwd, serial, access code) are redacted:

1. **Transport URL.** `Bambu_Create("bambu:///tutk?uid=<KalayUID>&authkey=<8hex>&passwd=<6hex>&region=<r>&device=<sn>&net_ver=<plugin>&dev_ver=<fw>&refresh_url=1&cli_id=<uuid>&cli_ver=<studio>")`.
   Same `bambu:///tutk?…` scheme as the camera ([`09`](09-camera-tutk-brtc.md)); `uid` is the Kalay
   UID, `authkey`/`passwd` the cloud-minted session credentials. The plugin tries
   `bambu:///local/<ip>?port=6000&user=bblp&passwd=<access-code>` **first** (LAN tcp) and falls back
   to `tutk` only when the printer is off-LAN — the `tcp → tutk → ftp` order from
   `FileTransferUtils`.
2. **Connect.** `Bambu_Open(tunnel)` runs the Kalay P2P handshake (~1.2 s observed; returns 0).
3. **Open the brtc file channel.** `Bambu_StartStreamEx(tunnel, type=12289)`. `12289` is the brtc
   file/control channel id — the `mtype:12289` from [`29`](29-cloud-print-live.md). (The live camera
   instead uses `Bambu_StartStream(video=1)`; that is the only thing distinguishing a camera tunnel
   from a file tunnel at this layer — both share the `tutk` URL scheme.)
4. **Run the brtc protocol.** `Bambu_SendMessage(tunnel, ctrl=12289, frame, len)` carries the
   **same brtc `cmdtype` protocol used on the LAN `:6000` socket**, now as the tunnel message body.
   Captured frame (a download/query — `get_project_file` of an on-printer memory file):

   ```
   {"cmdtype":4,"req":{"mem_dl_param_size":98,"offset":0,"path":"mem:/<id>"},"sequence":2}
   <size-delimited>{"command":"get_project_file","file_rel_path":"","peer_host":"studio","sequence_id":2,"version":1}
   ```

   i.e. a **JSON transport frame** `{"cmdtype","req","sequence"}` immediately followed by a
   **length-sized sub-payload** (`req.mem_dl_param_size` = the sub-payload's byte length — here a
   98-byte command JSON). `cmdtype` is the same enum as LAN: `4` = download/get, `5` = chunked
   upload, `7` = storage-ability.

This **resolves the open framer questions from [`16`](16-mqtt-channels-framing.md)**: the
"JsonOrJsonBin" body is **not** CBOR/MsgPack/UBJSON. It is a JSON control frame plus a size-delimited
sub-payload — a JSON command, or raw bytes for a `cmdtype 5` chunk — transported over
`Bambu_SendMessage` on channel `12289`. There is no exotic binary serialization to decode.

**Hybrid recipe, now concrete.** OBN's `ft_*` cloud branch would: (a) fetch the `tutk` credential
bundle (`ttcode`) from the cloud over HTTPS — the same fetch the camera `get_camera_url` uses — and
build the `bambu:///tutk?…` URL; (b) `dlopen` the genuine `libBambuSource` and `dlsym`
`Bambu_Create`/`Open`/`StartStreamEx`/`SendMessage`/`Close`; (c) open channel `12289` and run its
**existing** brtc `cmdtype 5` chunked-upload code (the `tunnel_upload` state machine) over
`Bambu_SendMessage` instead of the raw `:6000` socket. No Kalay reimplementation and no license
handling — both stay inside `libBambuSource`.

## What is implemented vs. blocked, after this

| Path | Transport | Status in an open plugin |
|---|---|---|
| Cloud **print** (`start_print`) | HTTPS PUT 3MF → OSS/S3, printer pulls signed URL | **Open / works** (no tunnel; see [`15`](15-oss-upload-sigv4.md), [`24`](24-studio-source-confirmation.md)) |
| LAN file transfer / eMMC store | TLS `:6000` brtc (`cmdtype 4/5/7`) | **Open / works** ([`29`](29-cloud-print-live.md)) |
| Remote send-to-storage (off-LAN) | **TUTK Kalay P2P** | **Vendor-locked** — only via genuine `libBambuSource` (hybrid above) |
| Remote camera / liveview | **TUTK Kalay / Agora** | **Vendor-locked** — same ([`25`](25-camera-local-protocol.md)) |

Net: the genuinely-open surface is functionally complete for printing and LAN transfer; the only
remaining gaps are the two **Kalay/Agora P2P** features, which collapse into a single vendor
dependency (`libBambuSource`) rather than separate protocols to reverse.

## Cross-references

- [`16-mqtt-channels-framing`](16-mqtt-channels-framing.md) — the framer this resolves (control,
  not bulk).
- [`09-camera-tutk-brtc`](09-camera-tutk-brtc.md), [`25-camera-local-protocol`](25-camera-local-protocol.md)
  — the same TUTK/Kalay + Agora transport, from the camera side.
- [`24-studio-source-confirmation`](24-studio-source-confirmation.md) — the `ft_*` ABI and the
  S3/OSS cloud-print path.
- [`29-cloud-print-live`](29-cloud-print-live.md) — the brtc `cmdtype` file protocol the hybrid
  reuses.
