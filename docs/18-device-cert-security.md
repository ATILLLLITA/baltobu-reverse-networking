# bambu_networking — device-certificate security (app_cert / enc_msg)

Recovered from the symbol-bearing Linux `01.07.01.04` build + the `02.06.00.50` memory image.
This is the **printer authentication / secure-link** layer that backs the `enc_msg` application
error codes (`0x05022647–0x0502264C`, see `error-codes.md`).

## Two eras

- **Linux 01.07** has the building blocks: `crl`, `pub_key`, `cert_chain`, `challenge`,
  `sec_link`, and a custom TLS cert-chain verifier — but **no `enc_msg`/`app_cert_install`**.
- **Windows 02.06** adds the full **`enc_msg` / `app_cert_install`** device-cert handshake
  (88× `enc_msg`, 19× `app_cert`, `app_cert_install`, `device_cert`, `need reset device pub key`).
  This is the newer printer-auth feature; the matching error codes also appear only here.

## Components

| Symbol | Role |
|--------|------|
| `BambuNetworkAgent::set_cert_file(string cert, string key)` @ 0x25e5a0 | install the client cert + private key the SDK presents (mutual TLS) |
| `ssl_verify_cert_chain`, `SSL_CTX_set_cert_verify_callback`, `ssl_cert_set_cert_cb` | **custom** chain verification (pinning), not default PKI |
| bundled `X509_V_ERR_*` table | full OpenSSL verify-error enum (CERT_REVOKED, CERT_UNTRUSTED, SIGNATURE_FAILURE, AKID/SKID mismatch, CHAIN_TOO_LONG, …) used by the custom verifier |

The SDK pins to the Bambu chain (`bambulab.com`); a CRL (`crl`) is carried in-band.

## `app_cert_install` (security namespace, MQTT)

The server provisions the printer's application certificate over MQTT
(`device/<sn>/request`, namespace `security`):

```json
{ "security": { "sequence_id": "<seq>", "command": "app_cert_install",
                "app_cert": "<PEM cert chain>", "crl": "<CRL>" } }
```

Companion query `app_cert_list` (`{ "security": { "command": "app_cert_list", "type": "app" } }`)
enumerates installed certs. The printer replies in the `security` namespace with a result and,
on failure, an `err_code` from the `enc_msg` space:

| err_code | meaning |
|----------|---------|
| `0x05022647`–`0x05022649` | verification error 1–3 (`…49` = **need reset device pub key** → cert re-issue) |
| `0x0502264A`–`0x0502264C` | verification error 4–6 |

(High byte `0x05` = security category; see `error-codes.md`.)

## Device certificate fetch (cloud REST)

The device/application cert material is retrieved from the cloud, AES-256 wrapped:

```
GET %1%/iot-service/api/user/applications/<app_id>/cert?aes256=<key>&ver=1
```

`aes256=<key>` is the symmetric unwrap key for the returned cert blob; `<app_id>` identifies
the application. The same `aes256`-wrapped pattern appears in the camera `ttcode_enc` path
(see `camera-tutk-brtc.md`).

## `sec_link`

`sec_link` (also surfaced as the `Devseclink.bambu.com` SSDP header, see `ssdp-discovery.md`)
is the secure-link state — whether the printer↔endpoint link has completed the device-cert /
secure-channel negotiation. The per-device cache exposes it (`device-report-fields.md`).

## Relation to farm mode

In farm mode this printer reported `device insecure` (no MQTT-Sec), so the `enc_msg` device-cert
layer was **not** engaged — commands rode plaintext-in-TLS (see OpenBambuFarmAPI `farm-mqtt.md`).
This SDK layer is the cloud/secured-printer path; the farm broker bypasses it.

> No private keys, certs, CRLs, or `aes256` unwrap keys are reproduced here — schema/field
> names only. Actual cert/key material is «REDACTED» by omission.
