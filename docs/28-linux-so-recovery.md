# Linux `.so` recovery: behavioral oracle, anti-debug, and live capture

Docs [`01-binary`](01-binary.md) and [`02-unpacking`](02-unpacking.md) cover the **Windows**
`bambu_networking.dll`. The matching **Linux** `libbambu_networking.so` (same `02.07` family) is
packed by the same VMProtect 3.x protector — no section headers, entry point past EOF, `strings`
empty, ~0.92 gzip ratio — so static analysis fails identically. This document records the methods
that *do* work on the Linux build, since they differ from the Windows unpacking pipeline and
together they confirm the public ABI matches across operating systems (the open hypothesis in the
top-level README).

## What is statically linked, and why interposition fails

The plugin's companion `libBambuSource.so` **statically links both curl and OpenSSL**, renamed to
avoid symbol clashes:

- curl → `tutk_third_curl_easy_init` / `_setopt` / `_perform` (exported `T`)
- OpenSSL → `TUTKSSL_read` / `TUTKSSL_write` (exported `T`)

The only **dynamically imported** symbols are libc: `connect`, `getaddrinfo`, `send`, `sendto`.
Consequences for analysis:

- `LD_PRELOAD` of `curl_easy_setopt` (or even the renamed `tutk_third_curl_easy_setopt`) does **not**
  intercept anything — intra-DSO calls bind locally, not through the PLT, so there is no symbol to
  interpose.
- `send`/`sendto` are interposable but carry **TLS ciphertext** (TLS is performed in-process by the
  static OpenSSL), so they yield nothing readable.

Plaintext therefore exists only (a) transiently inside the process heap, or (b) on the wire under
TLS. Both are recoverable without a debugger — see below.

## VMProtect anti-debug is fatal to `ptrace`

Any debugger on the live process — both `gdb -p <pid>` (attach) and `gdb --args bambu-studio`
(launch) — causes the process to die with **`SIGKILL`** as soon as the packed plugin becomes
active. This was reproduced repeatedly with plenty of free memory (OOM ruled out via `dmesg` and
`free`), and even after correctly relocating a breakpoint to `base + file_offset` (so the kill is
the protector's anti-debug, not a tooling error). Practical rule: **never attach or launch a
debugger against the real process** — `ptrace` is detected via `TracerPid`/self-attach and the
protector kills the process.

The implication is that every recovery channel below avoids `ptrace` entirely.

## Method A — external heap scraping (`/proc/<pid>/mem`)

Reading another process's memory through `/proc/<pid>/mem` uses no `ptrace`, sets no `TracerPid`,
and is therefore invisible to the anti-debug. VMProtect decrypts each string into a temporary heap
buffer at use-time, so the plaintext the protector hides in `.rodata` is present in the heap while
the code that uses it runs. A periodic scanner over the writable/anonymous maps recovers:

- long-lived / bursty data: every REST URL the client builds, `push_status` frames, the cloud
  task-history JSON, the command-security request headers, the OSS presigned URLs.
- **limitation:** a single one-shot command buffer (an MQTT publish such as `project_file`) is
  freed within ~20 ms, faster than a full-heap scan pass, so it is missed in practice. Bursty
  commands (`set_airduct`, repeated `pushall`) and persistent structures are caught reliably.

A carve of the plugin's *own* mapped regions yields the same string count as the on-disk packed
file — the module image is **not** decrypted in place; only the heap holds plaintext.

## Method B — behavioral oracle (`dlopen` + call the exports)

`dlopen()` the packed `.so` and call the exported `bambu_network_*` functions directly (they use
the C++ ABI: `std::string` by value, etc., so build the harness with the same libstdc++). The
protector decrypts code as it executes, so the **outputs are real**. This bypasses the packer
without unpacking it.

To recover **REST URL shapes** specifically: call a GET-builder export, then scan the *harness's
own* heap for the `/v1/…` string the in-process static-curl just assembled (the harness heap is
tiny and noise-free). The call reaches the network and fails TLS verification (no CA in the test
env), but the URL and headers are built before `perform`, so they are present regardless. Login is
satisfied by `change_user` with a `data.token`-bearing envelope (the getter mapping is
`get_user_id→uid`, `get_user_name→account`, `get_user_avatar→avatar`, `get_user_nickanme→name`).

Deterministically recovered this way (augmenting [`05-wire-protocol`](05-wire-protocol.md)):

| Export | Request |
|---|---|
| `get_user_print_info` | `GET /v1/iot-service/api/user/print?force=true` |
| `get_user_tasks` | `GET /v1/user-service/my/tasks?deviceId=<dev>&limit=<n>&offset=<n>&status=<n>` (keys alphabetical; `deviceId` omitted when empty; `limit`/`offset`/`status` always present) |
| `get_subtask_info` | `GET /v1/iot-service/api/user/task/<task_id>` |
| `query_bind_status` | `GET /v1/iot-service/api/user/bind_list?dev_ids=<comma-joined>` |
| `ping_bind` | `/v1/user-service/my/pincode/<code>` (code in the path) |
| `get_my_message` | `GET /v1/user-service/my/messages` (`type`/`after`/`limit`) |
| `get_setting_list` | `GET /v1/iot-service/api/slicer/setting` |
| `get_my_token` | `GET /v1/user-service/user/ticket/<ticket>` (ticket→token) |
| `get_my_profile` | `GET /v1/user-service/my/profile` |
| `get_studio_info_url` | `https://api.bambulab.com/v1/iot-service/api/slicer/resource` |

`start_print` itself cannot be exercised in a bare harness: it returns an early error code until the
agent holds a **bound, online device** (populated by a real `get_user_print_info` against a live
account), so it short-circuits before building any request. The task-create body is instead
recovered live — see [`29-cloud-print-live`](29-cloud-print-live.md).

## Method C — packet capture + key extraction (decrypt offline)

Because the wire is TLS and the keys are static-OpenSSL, capture the encrypted traffic with
`tcpdump` (the bytes persist in the pcap permanently, so transience is a non-issue) and recover the
per-session TLS secrets from the process heap via Method A, then decrypt offline with `tshark`. For
TLS 1.2 sessions the master secret can be located in heap and GCM-verified against the captured
client `Finished` record before it is trusted. This is the channel-agnostic fallback for the
one-shot commands Method A misses, and the only debugger-free way to read the printer-bound MQTT
that never appears in a slicer-side getter.

## MQTT identity (confirmed by oracle + heap)

- Broker `ssl://us.mqtt.bambulab.com:8883` (TLS); CN region uses `cn.mqtt.bambulab.com`.
- Username `u_{uid}` (the `uid` from `design-user-service/my/preference`); password is the access
  token. LAN uses `bblp` / the device access code. See [`16-mqtt-channels-framing`](16-mqtt-channels-framing.md).

## Cross-references

- [`01-binary`](01-binary.md), [`02-unpacking`](02-unpacking.md) — the Windows DLL equivalent.
- [`18-device-cert-security`](18-device-cert-security.md), [`27-mqtt-sec-app-cert`](27-mqtt-sec-app-cert.md)
  — command-security, confirmed present on the wire (an `app_cert_install` command is published when
  a secured print starts).
- [`29-cloud-print-live`](29-cloud-print-live.md) — the live-captured task-create body and the
  `project_file` URL-scheme matrix.
