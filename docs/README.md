# Documentation index

`bambu_networking.dll` is the networking core of Bambu Studio. The
slicer itself is published under AGPLv3 and dynamically loads this
library to perform every interaction with the outside world. The
library ships as an opaque binary, which is the licence violation that
baltobu exists to address. This documentation describes the library's
public ABI, internal architecture, wire protocol, and the methods used
to recover that information from the protected binary.

Read these files in order if you are new to the project. Each one
links to the next. They are short and self-contained enough to be read
front to back in about an hour.

1. [`01-binary.md`](01-binary.md) covers the binary as it exists on
   disk, the PE structure, the VMProtect 3.x protector, and the
   anti-analysis features that have to be defeated before the rest is
   readable.
2. [`02-unpacking.md`](02-unpacking.md) covers the unpacking method
   that produced the analysis dump, the dump's quality, the post-unpack
   inventory, and how to repeat the analysis pipeline against a future
   build.
3. [`03-architecture.md`](03-architecture.md) is the high-level system
   map: subsystem diagram, internal class hierarchy, the agent
   singleton, the lifecycle, the two-channel MQTT pub/sub, the REST
   endpoint inventory, and the threading model.
4. [`04-public-api.md`](04-public-api.md) catalogs the 128 exported
   functions by functional family and documents the cross-cutting
   patterns that recur across the surface.
5. [`05-wire-protocol.md`](05-wire-protocol.md) gives the JSON message
   schemas with field names, types, optionality, and error codes for
   every protocol the library speaks.
6. [`06-internals.md`](06-internals.md) covers what is inside the
   library beyond the ABI surface: the FT `cmd_type` enum, the RTTI
   hierarchy, the helper map, the subsystem log catalog, and the
   agent struct layout.
7. [`07-internals-flow.md`](07-internals-flow.md) covers runtime
   behaviour end to end: the layered architecture, the UML class
   diagram, the call hierarchy from C ABI through helpers to
   subsystems, sequence diagrams for the seven main flows (startup,
   login, MQTT publish, push reception, LAN-cloud arbiter, FT
   upload, token refresh, telemetry), the four state machines
   (bind, MQTT connection, LAN-cloud arbiter, FT job), the JSON
   build and parse pipeline, and the worker-to-GUI callback
   marshalling.

The per-export reference is at [`exports/`](exports/) with one
markdown file per exported function. The index there groups exports by
family.

## Quick links

- The dump and committed analysis artefacts are under
  [`../output/`](../output/).
- The scripts used to re-derive every committed artefact are under
  [`../scripts/`](../scripts/).
- The Ghidra headless scripts are under [`../ghidra/scripts/`](../ghidra/scripts/).
- The repository's top-level [`../README.md`](../README.md) covers
  layout, provenance, and reproduction.

## Companion analysis (Linux-Rosetta + live-DLL, 08–12)

Independent cross-source findings (symbol-bearing Linux `01.07.01.04` build + a
`ReadProcessMemory` dump of the live `02.06.00.50` DLL). Complementary to 01–07; cross-verified
against this repo's `05`/`06`.

8.  [`08-linux-rosetta.md`](08-linux-rosetta.md) — `create_agent` body recovered, 79 exact
    demangled C++ ABI signatures, cloud host matrix, cross-verification of 05/06.
9.  [`09-camera-tutk-brtc.md`](09-camera-tutk-brtc.md) — camera/liveview: ThroughTek Kalay
    (TUTK) + Agora (`BRTC`) media plane, `ttcode` flow, URL-scheme transport selector, H.264.
10. [`10-ft-abi.md`](10-ft-abi.md) — full C ABI signatures of the 21 `ft_*` exports
    (handle/refcount model, callback types) — complements the `cmd_type`/structs in 05/06.
11. [`11-print-params.md`](11-print-params.md) — `BBL::PrintParams`/`PublishParams` field
    layout, dual snake/camel serialization, version deltas (ams_mapping2, plate_md5…).
12. [`12-login-json.md`](12-login-json.md) — login payload schemas: `studio_userlogin`
    (`{command,sequence_id,data:{name,avatar}}`), printer-login (`login/command/wifi/tutk`),
    HTTP sign-in/ticket-token flow, cloud host matrix.

### Companion analysis, wave 2 (13–16)

13. [`13-error-codes.md`](13-error-codes.md) — library internal return codes (0/-1/-2/-4/-6/-17/-18/-26…) and the Bambu `enc_msg` application codes (`0x05022647–4C`, `84033545` = need-reset-device-pub-key); plus the HTTP/Paho/X509 pass-through code spaces.
14. [`14-device-report-fields.md`](14-device-report-fields.md) — the SDK does **not** parse `push_status` (forwarded opaquely to the slicer); documents what the SDK *does* read/write: device cache, print/upload command keys, config/login-report keys, with 01.07→02.06 deltas.
15. [`15-oss-upload-sigv4.md`](15-oss-upload-sigv4.md) — OSS/S3 STS credential schema, full AWS-SigV4 + Aliyun-OSS-v1 signing details, upload flow, and the `my/task` / design-publish camelCase payloads.
16. [`16-mqtt-channels-framing.md`](16-mqtt-channels-framing.md) — cloud vs local MQTT channels (both TLS:8883, client-id/username/cert-policy differences), report stream = plain JSON, the JsonOrJsonBinFramer cloud-tunnel framing (wire format flagged as open).

### Companion analysis, wave 3 (17–19)

17. [`17-ssdp-discovery.md`](17-ssdp-discovery.md) — LAN discovery: `BBL::SsdpDiscovery` on lssdp, `urn:bambulab-com:device:3dprinter:1`, the `Dev*.bambu.com` header set (01.07→02.06 deltas).
18. [`18-device-cert-security.md`](18-device-cert-security.md) — printer auth: `app_cert_install`/`enc_msg` device-cert handshake, custom pinned X509 chain verify, the `aes256`-wrapped cert fetch, `sec_link`.
19. [`19-http-client.md`](19-http-client.md) — `BBL::Http` libcurl wrapper, `X-BBL-*` header suite, the refresh-on-401 token-retry loop, pinned TLS + `CURLOPT_RESOLVE` region routing.

### Companion analysis, wave 4 (20–22)

20. [`20-pushstatus-schema.md`](20-pushstatus-schema.md) — the FULL printer `push_status` JSON schema (the SDK forwards it opaquely; schema lives in the open-source AGPL slicer `DeviceManager.cpp`): all categories, `gcode_state` enum, HMS error bit-decode.
21. [`21-small-subsystems.md`](21-small-subsystems.md) — SFTP/FTPS (`BBL::Sftp`, `bblp`+access-code), telemetry (`track_*`→TrackingManager), DeviceSubscribeManager (local/cloud arbiter), the bind/login state machine.
22. [`22-overview.md`](22-overview.md) — index + architecture summary of the whole 08–22 companion series; notes the cloud broker is TLS 1.3 (framer wire-format left open).

### Companion analysis, wave 5 (23)

23. [`23-live-decrypt-validation.md`](23-live-decrypt-validation.md) — live capture decrypted: OpenSSL TLS 1.2 master-secret extraction via the `session_id` anchor; confirms the local report channel is plain JSON (no binary framing) and validates the `push_status` schema against real on-wire data; cloud channel is TLS 1.3 (framer left open).

### Companion analysis, wave 6 (24)

24. [`24-studio-source-confirmation.md`](24-studio-source-confirmation.md) — cross-check against the open-source (AGPL) BambuStudio SDK headers (`bambu_networking.hpp`, `FileTransferUtils.hpp`): confirms the reversed `PrintParams` (44 fields) and `ft_*` ABI byte-for-byte; resolves the framer (JSON control + opaque `bin` side-channel, no CBOR/msgpack); decodes error -3030 = cloud OSS 3MF-upload failure (curl-60 = environmental cert-verify).

### Companion analysis, wave 7 (25–26)

25. [`25-camera-local-protocol.md`](25-camera-local-protocol.md) — the LOCAL camera is fully reimplementable: TCP-6000 MJPEG (80-byte `bblp`+access-code auth, 16-byte frame header) and RTSPS-H.264 (P1S/X1) — no vendor keys. Remote (TUTK Kalay / Agora) is cloud-minted and vendor-locked (Kalay license is the hard blocker; Agora is token-gated).
26. [`26-ref-clusterm-impl.md`](26-ref-clusterm-impl.md) — cross-reference to ClusterM/open-bamboo-networking (working AGPL clean-room): it closes the local camera (both transports) and MQTT credentials (cloud password = access_token; `u_<uid>` / `bblp`); it leaves open the MQTT-command RSA-SHA256 signing (bypassed via Developer Mode, else 84033543), the farm secret, remote camera, and the cloud FT tunnel.

### Companion analysis, wave 8 (27)

27. [`27-mqtt-sec-app-cert.md`](27-mqtt-sec-app-cert.md) — the MQTT-Sec / app_cert command-security crypto that closes the last Tier-1 gap (absent from this repo, ClusterM, and OpenBambuAPI): **RSA-SHA256 signature with the *app* private key** (`header` object) + RSA field-encryption with the *device* public key (`url_enc`/`param_enc`); app key is cloud-issued via `bootstrap_secret`. Corrected against, and cites, the SFC `reverse-networking` `authorization-control` branch — `app_cert_install` provisions `printer_cert`; the login challenge is RSA-encrypted with that cert (CertId = decimal serial); once secure, each command's `sequence_id`/`url` is sent RSA-encrypted as `*_enc`. Recovered from the official server + a working farm clone (implemented, not yet wire-verified against a secure printer).

### Companion analysis, wave 9 (28–29)

Linux `.so` recovery of the same `02.07` family + live on-wire validation of the print path.

28. [`28-linux-so-recovery.md`](28-linux-so-recovery.md) — methods for the **Linux** `libbambu_networking.so` (the Windows DLL's sibling, same VMProtect family): companion `libBambuSource.so` statically links curl+OpenSSL (`tutk_third_curl_*`/`TUTKSSL_*`) so `LD_PRELOAD` cannot interpose; **VMProtect anti-debug SIGKILLs the process under any `ptrace`** (gdb attach *and* launch), so only debugger-free channels work — external `/proc/<pid>/mem` heap scraping, a `dlopen`+call-exports **behavioral oracle** (recovers exact REST URL shapes by scanning the harness's own heap), and `tcpdump` + heap TLS-key extraction. Tabulates the recovered GET URLs (`my/tasks?deviceId&limit&offset&status`, `ping_bind`→`my/pincode/<code>`, `get_subtask_info`→`api/user/task/<id>`, `my/token`→`user/ticket/<t>`, …) and the `u_{uid}`/`us.mqtt.bambulab.com:8883` identity.
29. [`29-cloud-print-live.md`](29-cloud-print-live.md) — live-captured **`POST /my/task`** task-create body (byte-exact, lexicographic camelCase: `amsDetailMapping`/`amsMapping2`/`mode:"cloud_file"`/`profileId`-as-number/`sequence_id:"20000"`/…) and the **`project_file` `url` scheme matrix by delivery mode** — cloud `https…s3…SigV4`, LAN/offline `ftp:///<file>`, eMMC store-then-print over the `cmdtype 5/7` `mtype:12289` brtc file channel, legacy sdcard `file:///mnt/sdcard`. `sequence_id` cycles 20000→90000. Notes the live `app_cert_install` + `X-BBL-*` proof headers confirming the [`27`](27-mqtt-sec-app-cert.md) command-security layer is exercised on the wire.

### Companion analysis, wave 10 (30)

30. [`30-cloud-ft-tunnel.md`](30-cloud-ft-tunnel.md) — resolves the open cloud "tunnel" framer from [`16`](16-mqtt-channels-framing.md): the cloud file-transfer **data plane is ThroughTek Kalay P2P (`BambuTunnelTutk`)** — the same vendor-locked transport as the remote camera, not a binary MQTT framer to decode. Maps the **two-plugin split** (`libbambu_networking` owns `ft_*`/control; **`libBambuSource` owns the `Bambu_*` transport + the Kalay SDK *and the embedded license*** — `TUTK_SDK_Set_License_Key`/`CheckLicenseKeyIsValid`). Concludes the realistic open path is a **hybrid**: keep the genuine `libBambuSource`, have the open plugin's `ft_*` cloud branch open a `Bambu_*` tunnel and run the **same brtc `cmdtype 4/5/7` protocol it already uses for LAN** over `Bambu_SendMessage`/`RecvMessage` — no Kalay reimplementation or license extraction. Cloud *print* (S3/OSS) and LAN transfer remain fully open; only the two Kalay/Agora P2P features are vendor-bound.

### Companion analysis, wave 11 (31)

31. [`31-status-and-remaining-gate.md`](31-status-and-remaining-gate.md) — **status capstone.** Consolidates the implementation state across the series: every protocol is recovered and the entire **local** surface (LAN control/transfer, cloud print, local camera, command-security signing) is open and owner-controlled; command-security is now **wire-validated byte-identical** to genuine (updating [`27`](27-mqtt-sec-app-cert.md)) and the cloud FT tunnel works via the [`30`](30-cloud-ft-tunnel.md) hybrid. Names the **single remaining gate**: application-certificate *acquisition* is refused server-side with application `code 101` even for a byte-identical request, so the decision is server policy (not a client bug); the only way past it is the client's **embedded, globally-shared application key** — a vendor client-identity secret (direct analogue of the Kalay license), whose extraction is **out of scope** for a clean-room interop project. The series therefore treats the app cert+key as **caller-provided input** (as a TLS library takes a cert, not ships one) and shows why the gate blocks none of the project's real goals — verification is done via captured-traffic equivalence plus a **local mock acquisition endpoint with your own CA**, and a self-hosted control plane needs no vendor key by construction.
