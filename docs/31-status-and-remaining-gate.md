# Status capstone: what is closed, and the one remaining gate

This document consolidates the implementation status across the whole
series and names the single server-side gate that remains. It is the
capstone to [`26`](26-ref-clusterm-impl.md) (which first listed the open
items), [`27`](27-mqtt-sec-app-cert.md) (command-security crypto),
[`29`](29-cloud-print-live.md) (live print path), and
[`30`](30-cloud-ft-tunnel.md) (cloud FT tunnel).

The short version: every protocol the library speaks has been recovered
and re-implemented in clean room, and the entire **local** surface is
functionally complete and owner-controlled. One cloud operation —
acquiring the *application* certificate — is refused by the vendor's
server, and the only known way past that refusal is a secret that is out
of scope for an interop project. Crucially, that gate blocks only the
"talk to the vendor cloud as the vendor's own client" path; it does not
block printing, LAN control, a self-hosted control plane, or the
completeness of the library itself.

## What is closed

| Area | State | Where |
|---|---|---|
| Binary recovery + buildable source | The protected DLL/`.so` is fully mapped; an open networking plugin builds from source for Linux and Windows | [`01`](01-binary.md), [`02`](02-unpacking.md), [`28`](28-linux-so-recovery.md) |
| Public ABI (`bambu_network_*`, 21× `ft_*`) | Catalogued with exact signatures and struct layouts | [`04`](04-public-api.md), [`10`](10-ft-abi.md), [`11`](11-print-params.md) |
| Wire protocol (JSON schemas, error codes) | Documented field-by-field, validated against live capture | [`05`](05-wire-protocol.md), [`13`](13-error-codes.md), [`23`](23-live-decrypt-validation.md) |
| Config blob crypto (`*.conf`) | AES-128-ECB under the published network key; interop-proven (decrypts a genuine config) | [`06`](06-internals.md) |
| MQTT credentials (cloud + local) | cloud password = `access_token`, `u_<uid>` / `bblp`+access-code | [`16`](16-mqtt-channels-framing.md), [`26`](26-ref-clusterm-impl.md) |
| LAN control + file transfer | MQTT `:8883`, FTPS, and the brtc `cmdtype 4/5/7` file channel — fully open, owner-controlled with the printer access code | [`21`](21-small-subsystems.md), [`29`](29-cloud-print-live.md) |
| Cloud **print** (`start_print`) | HTTPS PUT 3MF → OSS/S3 (SigV4); printer pulls a signed URL. No tunnel needed | [`15`](15-oss-upload-sigv4.md), [`24`](24-studio-source-confirmation.md) |
| Local camera | TCP-6000 MJPEG + RTSPS-H.264 — reimplementable, no vendor keys | [`25`](25-camera-local-protocol.md) |
| **command-security** (RSA-SHA256 signing + device-pubkey field encryption) | Implemented and now **wire-validated byte-identical** to the genuine client (updates [`27`](27-mqtt-sec-app-cert.md)'s "not yet wire-verified"); the HTTP proof header uses PKCS#1 v1.5, see [`27`](27-mqtt-sec-app-cert.md) | [`27`](27-mqtt-sec-app-cert.md), [`29`](29-cloud-print-live.md) |
| **Cloud FT tunnel** (TUTK/Kalay) | Resolved as a vendor transport, not a binary framer; the open path is the **hybrid** (delegate `Bambu_*` to the genuine `libBambuSource`). Transport bring-up and the reply-read pattern are runtime-confirmed against a real off-LAN printer | [`30`](30-cloud-ft-tunnel.md) |

Two findings worth pinning, because they corrected earlier drafts:

- **The HTTP proof signature is PKCS#1 v1.5, not raw/`NO_PADDING` RSA.**
  An implementation that left-pads the timestamp and signs with no
  padding produces the wrong block; the genuine client signs the raw
  timestamp under type-1 padding. (Detail in [`27`](27-mqtt-sec-app-cert.md).)
- **The app-cert acquisition URL is base64url, not standard base64.**
  The `enc_secret` path segment and the `aes256=` query are base64url
  (raw, `-`/`_`); standard base64 + percent-encoding routes to a 404
  because `/` and `%2F` break the `/applications/{id}/cert` path. This is
  a real interop detail any reimplementation must get right.

## The one remaining gate: application-certificate acquisition

command-security signs with an **application** certificate and its
private key (distinct from the per-printer *device* certificate of
[`18`](18-device-cert-security.md)). [`27`](27-mqtt-sec-app-cert.md)
describes how that material is provisioned: a renewal request to
`…/iot-service/api/user/applications/{enc_secret}/cert?aes256={wrapped_session_key}`,
where `enc_secret` is the AES-GCM-sealed bootstrap secret (base64url) and
`wrapped_session_key` is the session key RSA-wrapped to the service's
public key. The response carries the app cert, the (encrypted) app
private key, and a CRL.

A standalone reimplementation can build that request **byte-identically**
to the genuine client — the base64url encoding, the `IV‖ciphertext‖tag`
GCM framing, the RSA wrap size, the full `X-BBL-*` header suite
([`19`](19-http-client.md)), the user-agent, and a valid account token all
match. The server nonetheless refuses it with application code **`101`
("application outdated")**. The refusal is *identical* across every
public key in the shipped certificate chain, which places the decision in
the server's request-authorization layer **before** the encrypted payload
is processed — i.e. it is a server-side policy decision, **not** a client
encoding bug. (The same account token is accepted on ordinary endpoints
such as profile/task queries, so the token itself is valid; the gate is
specific to issuing application certificates.)

> **2026-06-07 update — the "outdated" label is a red herring; the gate is
> client *identity*, not client *version*.** Live testing with a fresh,
> valid account token confirms the server does **no** version gating on the
> reachable surface: a deliberately ancient claimed version
> (`bambu_network_agent/01.05.00.00`, X-BBL-*-Version `01.05.00.00`) is
> accepted exactly like the current `02.07.01.51` on both the
> unauthenticated `…/iot-service/api/slicer/resource` check and the
> authenticated `…/iot-service/api/user/bind` device list — every response is
> `code:null` success. The `slicer/resource` endpoint merely *advertises*
> `software.version=02.07.01.57, force_update=false` (purely advisory). So
> bumping the version cannot be what clears `code 101`: changing it changes
> nothing on every endpoint we can reach. This **strengthens** the conclusion
> above — the `get_app_cert` refusal is an *identity* gate (the embedded
> application key), and "application outdated" is a misleading server message,
> not a literal version complaint. The reimplementation now advertises the
> genuine `02.07.01.x` fingerprint anyway (agent `02.07.01.51` / Studio
> `02.07.01.57`, plus the previously-missing `X-BBL-Executable-Env: false`)
> as correctness hygiene, so any *client-side* self-check also passes — see
> the `cloud-client-fingerprint-02.07.01` branch in the companion
> `open-bamboo-networking` repo. The `get_app_cert` acquisition endpoint
> itself still needs an authenticated end-to-end run to confirm `101`
> persists with the synced fingerprint.

The only known way past `code 101` is to present the client's **embedded
application private key** — a single, globally-shared secret deliberately
hidden inside the binary, used so the vendor's server will accept the
caller as its own official client. This is the direct analogue of the
embedded **Kalay license** that locks the remote-P2P features
([`30`](30-cloud-ft-tunnel.md) §"the one hard secret"): a vendor
client-identity secret, not a protocol detail.

**Extracting that embedded key is out of scope for a clean-room interop
project, by design.** It is the vendor's client-authentication secret;
recovering it to defeat the server's refusal would be circumventing an
access control, not documenting a protocol. This series therefore stops
at *describing the gate* and treats the application certificate and key
as **caller-provided input** — exactly as a TLS library accepts a cert
and key rather than shipping someone else's.

## Why this gate does not block the project's purpose

| Concern | Resolution without the embedded key |
|---|---|
| "Is the library complete?" | A library implements the *algorithm* and takes keys as input (OpenSSL signs with *your* key; it ships nobody's). The command-security codec is implemented and byte-identical to genuine — that is wire-completeness. Possessing the vendor's secret is not a property of the library. |
| "How do we prove it works end to end?" | Two ways, both stronger than poking the production cloud: (1) **byte-identical equivalence** against captured genuine traffic — already done; (2) a **local mock of the acquisition endpoint** with your own CA and service keypair, against which the existing `acquire → store → sign → ttcode → FT` path runs green. The mock proves the client code is correct (i.e. that `101` is server policy, not a client defect) and is reproducible with no credentials or rate limits. |
| "We want off the vendor cloud anyway" | A self-hosted control plane uses *your* trust root, so the vendor's key is irrelevant there by construction. The local mock above is the first brick of exactly that. |
| "Day-to-day operation" | The LAN control plane and local file transfer are fully functional with the printer access code, with no cloud in the path at all. |

## Status table (final)

| Path | Transport | Status |
|---|---|---|
| LAN control (status/commands) | MQTT `:8883`, `bblp`+access-code | **Open / owner-controlled** |
| LAN file transfer / eMMC store | brtc `cmdtype 4/5/7` over TLS `:6000` | **Open** |
| Cloud print (`start_print`) | HTTPS PUT → OSS/S3 SigV4 | **Open** |
| Local camera | MJPEG / RTSPS | **Open** |
| command-security signing | RSA-SHA256 app-key + device-pubkey field enc | **Open (caller provides cert+key)** |
| App-cert *acquisition* from vendor cloud | HTTPS renewal request | **Gated server-side (`code 101`); needs the embedded app key — out of scope** |
| Remote send-to-storage (off-LAN) | TUTK Kalay P2P | **Vendor-locked transport** — reachable only via the hybrid (genuine `libBambuSource`), [`30`](30-cloud-ft-tunnel.md) |
| Remote camera / liveview | TUTK Kalay / Agora | **Vendor-locked transport** — same hybrid dependency |

Net: the open surface is functionally complete for printing, LAN control,
and (via the hybrid) cloud file transfer. What remains are vendor-held
secrets — the embedded application key behind `code 101`, and the Kalay
license behind the two P2P features — neither of which is a protocol to
reverse, and both of which a self-hosted deployment simply does not need.

## Cross-references

- [`26-ref-clusterm-impl`](26-ref-clusterm-impl.md) — the open-item list this closes out.
- [`27-mqtt-sec-app-cert`](27-mqtt-sec-app-cert.md) — the command-security crypto and the acquisition request shape.
- [`29-cloud-print-live`](29-cloud-print-live.md) — the live print path and the proof headers on the wire.
- [`30-cloud-ft-tunnel`](30-cloud-ft-tunnel.md) — the TUTK resolution and the hybrid transport.
