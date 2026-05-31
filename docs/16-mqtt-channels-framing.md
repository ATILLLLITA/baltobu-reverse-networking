# Bambu networking SDK — MQTT framing wire format + channel layer

Static RE only (no network capture). Two binaries cross-referenced:

- **Linux `libbambu_networking.so` v01.07.01.04** — full demangled symbols, readable
  Paho-C++/Paho-C code. Older architecture (`BBL::BambuNetworkAgent` + `BBL::TunnelMqtt`
  + `BBL::LocalClient` + cloud/local `*_conn_callback`). No `Framer` classes.
- **Windows `bnet.bin` v02.06.00.50** — VMProtect-unpacked **memory dump** (PE section
  `SizeOfRawData`/`PointerToRawData` are all 0; file offsets ≠ RVAs). Symbol-stripped
  except MSVC RTTI. This is the only image that contains the
  `BBL::SimpleFramer` / `BBL::JsonOrJsonBinFramer` classes.

> The MQTT connection state machine (Connecting/Connected/Reconnecting/OnConnectionLost/
> OnAutoReconnection) and the LAN↔cloud arbiter 5-state model are already documented in
> baltobu fork **07-internals-flow** — NOT repeated here. This file covers only the
> **on-wire framing / message body format** and the bytes that distinguish JSON from binary.

---

## 1. The framer classes (v02.06 only)

Recovered from MSVC RTTI type descriptors in `bnet.bin`:

| Class | RTTI string (file off) |
|-------|------------------------|
| `BBL::SimpleFramer`        | `.?AVSimpleFramer@BBL@@` @ 0x7AE478 |
| `BBL::JsonOrJsonBinFramer` | `.?AVJsonOrJsonBinFramer@BBL@@` @ 0x7AE4AC |

The dump is symbol-stripped (only RTTI present, **no exported framer methods, no vtable
labels**) and the sections are dumped contiguously with non-uniform per-section deltas, so
file offset → RVA is not a single constant. COL/vtable chasing to the framer code was not
reliably resolvable with the available tooling (`od`/`grep` on a packed dump; no
disassembler / PE-rebuild). The framer **method byte stream could not be decoded**, so the
encode/decode internals below are reconstructed from surrounding evidence and the v01.07
protocol and are marked **[INFERRED]** where not directly read from bytes.

**No `cbor`/`msgpack`/`ubjson`/`bson` string literals exist in either binary**, and the
Linux symtab exposes **no** nlohmann `to_cbor`/`to_msgpack`/`binary_writer` symbols
(nlohmann binary writers are header-only templates — MSVC inlines them and emits no symbol,
so their presence cannot be confirmed or denied from symbols alone). The baltobu claim that
the binary variant is `j.to_cbor`/`to_msgpack` is therefore **[UNCONFIRMED by these
binaries]** — see §3.

---

## 2. The printer control channel carries PLAIN JSON (confirmed)

This is the channel baltobu/farm code cares about: `device/<sn>/report` (subscribe) /
`device/<sn>/request` (publish). Topic templates are byte-confirmed in **both** images as
boost::format strings:

```
device/%1%/report      (subscribe)   bnet.bin @0x6004A0 ; so @0x4BA6D8
device/%1%/request     (publish)     bnet.bin @0x6004B8 ; so @0x4BA728
```

The payloads on this channel are **UTF-8 JSON text** — no magic byte, no length prefix, no
binary header. Confirmed by the JSON field-name string blobs adjacent to the topic
templates in `bnet.bin` (file ~0x5FA4E8): `sequence_id`, `command`, `print`,
`extrusion_cali_get`, … These are JSON object keys, not struct fields. The publish QoS is
the standard QoS0/QoS1 passed straight to `MQTTAsync_send`.

`sequence_id` is a **JSON field** (correlation id), NOT a binary frame sequence header.

So on the printer report/request channel, **there is no binary framing** — `JsonOrJsonBin`
does *not* apply here. It applies to a different (job/tunnel) channel; see §3.

---

## 3. JsonOrJsonBinFramer — what it actually frames

RTTI sitting immediately next to the framers in `bnet.bin` names the message types the
framer is generic over (file ~0x7AE4BC onward):

```
.?AV?$variant@UJobResp@BBL@@UJobFail@2@@std@@     -> std::variant<BBL::JobResp, BBL::JobFail>
JobMsg / JobResp / JobFail                        (struct names @ ~0x7AF23D..0x7AF2A2)
```

→ The framer wraps a **`JobMsg` request / `JobResp`|`JobFail` reply** RPC, not the printer
report stream. This is the cloud **"tunnel"/job** transport (the v02.06 successor to the
v01.07 `BBL::TunnelMqtt` RPC — see §5), used for request/response operations (start_push,
binds, file/camera tunnel setup) rather than the free-running telemetry report.

**`JsonOrJsonBinFramer` = "this message body is EITHER JSON text OR a JSON-equivalent
binary blob."** Its job is to tag/detect which encoding a given frame uses. [INFERRED] from
the name + the JobMsg variant context. The two candidate encodings:

- **JSON variant**: UTF-8 `{...}` (same nlohmann `json` object used everywhere else).
- **"JsonBin" variant**: a binary serialization of the *same* nlohmann json value.

### Distinguishing JSON vs binary — the detection rule [INFERRED]

The most defensible reading, consistent with nlohmann + the "JsonOrJsonBin" name and with
how such framers are universally written:

- A **leading discriminator byte / tag** selects the branch. The natural and zero-ambiguity
  choice (and the one nlohmann's own binary formats make trivial) is the **first byte**:
  - first byte `0x7B` = ASCII `{` → JSON text branch (objects always start with `{`).
  - any other leading byte → binary branch.
- The binary branch is a single nlohmann binary blob (`json::to_*` / `from_*`). **Which**
  nlohmann format (CBOR vs MsgPack vs UBJSON) is **[UNCONFIRMED]** — no format string or
  symbol survives. If a capture is ever obtained, identify by the first non-`{` byte:
  - CBOR map: `0xA0–0xBF` (or `0xBF` indefinite) / tag bytes
  - MsgPack fixmap: `0x80–0x8F`, map16 `0xDE`, map32 `0xDF`
  - UBJSON object: `0x7B` `{` (collides with JSON — so UBJSON is unlikely to be the binary
    variant precisely because it would defeat first-byte detection → leans CBOR/MsgPack).

> **Header layout:** no evidence of a multi-byte magic, version, or explicit length field
> in front of the body — MQTT already frames length, so the framer relies on the MQTT
> payload boundary plus the single leading discriminator byte. [INFERRED — could not read
> the compare-immediate bytes from the packed dump.]

### SimpleFramer

`BBL::SimpleFramer` is the degenerate sibling: **no JSON/binary discrimination** — it ships
the body as-is (raw bytes / plain JSON, length given by the MQTT payload length). It is the
base/fallback framer; `JsonOrJsonBinFramer` is the variant that adds the dual-encoding tag.
[INFERRED from naming pair; bytes not decoded.]

---

## 4. Cloud vs local channel (byte-confirmed from v01.07 .so)

Both cloud and local are **MQTT over TLS on port 8883** via the same Paho client; they
differ only in broker address, credentials, and cert policy.

### Cloud subscribe/publish channel
- Broker (rodata @0x4B9DF8): `ssl://cn.mqtt.bambulab.com:8883`,
  `ssl://us.mqtt.bambulab.com:8883` (region-selected). TLS, 8883.
- Client-id template `slicer:%1%:%2%`  (`slicer:<user_id>:<nonce>`) @0x4C9CD6
- Username template `u_%1%`  (`u_<user_id>`) @0x4C9CF6 ; password = cloud access token.
- Server cert verified against the public CA chain (`set_ssl` with normal verification).
- Callback class `BBL::cloud_conn_callback` (vtable @0x5F3548): `connected`,
  `connection_lost`, `message_arrived(shared_ptr<mqtt::message const>)`, `on_success`,
  `on_failure`. Log string `connect_cloud_mqtt, exception` @0x4C9D20.

### Local (LAN) subscribe/publish channel
- Broker `ssl://<printer-LAN-ip>:8883` (printer's own broker). TLS, 8883.
- Username `bblp`, password = printer **access code** (LAN access code). [INFERRED for
  exact literal — `bblp` not in this build's rodata; established Bambu LAN convention.]
- TLS uses a **relaxed/self-signed** policy set via
  `BBL::AccountManager::set_local_mqtt_ssl_opt()` (@so 0x1DCDE0) — server cert auth
  disabled / pinned, since the printer presents a self-signed cert.
  Predicate `BBL::AccountManager::is_mqtt_local()` selects this path.
- Callback class `BBL::local_conn_callback` (vtable @0x5F3690) — same method set as cloud.
- Same topic templates (`device/<sn>/report` sub, `device/<sn>/request` pub).

> `BBL::LocalClient` (connect/send/recv/publish/disconnect) is a **separate raw-TCP** client
> (the LAN file/SSDP path), NOT the local MQTT channel. The local MQTT channel is the same
> Paho async_client as cloud, re-pointed at the printer IP with `local_conn_callback`.

In v02.06 these become `MqttCloudSubscribeChannel` / `MqttLocalSubscribeChannel`
(both : `MqttSubscribeChannel` : `ISubscribeChannel`), driven by `MqttConnectionManager`
with a `MqttChannelListener`. The cloud-vs-local distinction (broker, creds, cert policy)
is identical to the v01.07 split above.

---

## 5. Paho usage & RPC plumbing (v01.07, confirmed)

- C++ layer: **`mqtt::async_client`** (Paho C++) over **Paho C `MQTTAsync_*`** (confirmed
  symbols: `MQTTAsync_create/connect/send/subscribe/disconnect/getVersionInfo`,
  `MQTTAsync_setConnected/MessageArrived...`, `MQTTAsync_global_init`). The brief's "Paho C
  1.3.9" is the C core; this .so statically links the C++ wrapper on top.
- `connect_options` setters used: `set_ssl`, `set_user_name`, `set_password`,
  `set_automatic_reconnect(min,max)`, `set_keep_alive`, `set_will`, `set_mqtt_version`,
  `set_servers`, `set_token`. → auto-reconnect + LWT + keepalive are all driver-level (Paho),
  matching the state machine in baltobu 07.
- RPC: `BBL::TunnelMqtt::send_request(int seq, json body, callback(int, json, uchar const*))`
  — the `unsigned char const*` trailing arg is the **raw binary payload pointer** that
  accompanies the JSON, i.e. the v01.07 ancestor of the JsonOrJsonBin split: a JSON header
  + optional binary tail. `TunnelMqtt::reconnect(lock, attempt)`, `start()`, `start_push()`,
  `handle_response(lock, __Bambu_Sample const&)` (the `__Bambu_Sample` shows TunnelMqtt also
  carries the camera/sample stream).

---

## Open items (could not resolve statically)

1. **CBOR vs MsgPack vs other** for the JsonBin branch — no string/symbol survives in either
   binary; needs a packet capture or a disassembler pass on `bnet.bin` framer code.
2. Exact discriminator byte value and whether any 1-byte type/version prefix precedes the
   body — needs the framer's compare-immediate bytes (packed dump blocked clean decode).
3. The `JobMsg`/`JobResp`/`JobFail` field schemas (the variant the framer carries).

No secrets, tokens, or keys included.
