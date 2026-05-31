# MQTT-Sec / app_cert security — the command-encryption + challenge crypto

The one Tier-1 gap that baltobu, ClusterM, and OpenBambuAPI all leave open: how a **secured**
(non-developer-mode) printer authenticates the server and protects MQTT commands. Recovered by
reversing the **official** Bambu Farm Manager (Go) server and re-implemented in a working farm
clone (`bambu-farm-server-clone`). It is **RSA-PKCS#1 v1.5 encryption with the printer's own
certificate**, *not* an RSA-SHA256 signature header (ClusterM's guess).

> ⚠️ **Confidence:** reconstructed from the official server binary + a working clone, but our
> test printer ran **device-insecure** firmware (no MQTT-Sec), so the secured path is
> **implemented, not yet wire-verified against a secure printer**. Field names / algorithm are
> from the official server; treat byte-exactness as "high-confidence, unverified".

## Pieces

1. **`app_cert_install` / `app_cert_list`** (MQTT `security` namespace) — provisioning.
2. **The login challenge ticket** — RSA-encrypted challenge proving the server holds (talks to)
   the device cert.
3. **Secure payload** — once a session is "secure", each command's `sequence_id` (and the
   `project_file` `url`) is RSA-encrypted with the printer cert and sent as `*_enc`.

## 1. `app_cert_install` (security namespace, server → printer)

```json
{ "security": { "sequence_id": "<seq>", "command": "app_cert_install",
                "app_cert": "<PEM>", "crl": "<CRL>" } }
```
The printer replies in `security` with a result; on success the server records the
**`printer_cert`** (the printer's own X.509, returned/known from the exchange) and advances a
per-device `security_session` (tracking `app_cert_sequence_id`, install success/fail, sequence
mismatch). `app_cert_list` enumerates installed certs. (`mqtt-transport.js` handles
`app_cert_install`/`app_cert_list`, `printer_cert`, `security_result`, the `security_session`
state machine, and a sequence-mismatch guard.)

## 2. Login challenge ticket (RSA-encrypted with the device cert)

To prove identity, the server issues a ticket whose challenge is **RSA-PKCS#1 v1.5 encrypted
with the printer certificate's public key** (`bambu-ticket.js`):

```
org_challenge = random 8 chars from
   [a-zA-Z0-9!@#$%^*()-_=+[]{},<>?/]
enc_challenge = base64( RSA_PKCS1_encrypt( printer_cert.pubkey, org_challenge ) )

ticket_DTO   = { "challenge": enc_challenge, "cert_id": <CertId>, "ts": <unix_sec> }
ticket       = base64( JSON( ticket_DTO ) )
```

- **`CertId`** = the certificate **serial number as a DECIMAL big-integer string** (the official
  Go server uses `x509.Certificate.SerialNumber.String()`; a hex serial must be converted to
  decimal — a real interop gotcha).
- Verification: the printer decrypts the challenge with its private key and returns a response;
  the server checks `SHA256(response) == SHA256(org_challenge)` and that `cert_id` matches.

This is the concrete shape of the `enc_msg` / device-cert handshake (the failures of which are
the `0x05022647…` codes in `13-error-codes`).

## 3. Secure payload — per-command field encryption

Once `security_session.state == 3` and a `printer_cert` is known
(`shouldUseSecurePayload(printer)`), every outgoing command is rewritten: for each protected
key the plaintext value is removed and an RSA-encrypted `*_enc` sibling is added
(`secure-payload.js`):

```
for key in [print.sequence_id, security.sequence_id, liveview.sequence_id,
            update.sequence_id, upgrade.sequence_id, system.sequence_id,
            upload.sequence_id, camera.sequence_id]:
    value         = payload[key]
    payload[key+"_enc"] = base64( RSA_PKCS1_encrypt( printer_cert.pubkey, str(value) ) )
    del payload[key]
```

i.e. `print.sequence_id` → `print.sequence_id_enc`. The same construction applies to the
`project_file` URL (`url` → **`url_enc`**). So a "signed" command is really a command whose
`sequence_id`/`url` are **RSA-encrypted with the printer's cert**; a printer that can't decrypt
them (wrong/absent cert) rejects the command with a verification error (`84033543`).

## Why our printer didn't need this

In `LAN_FARM` our test printer reported **`device insecure`** (no MQTT-Sec), so commands were
plaintext JSON inside TLS and none of the above was engaged (`BFM_MQTT_SEC_REQUIRED=false`,
gated by a `report.flag3` MQTT-Sec bit). The mechanism above is what a **secured** printer
requires — implement it to support stock/secure firmware without Developer Mode.

## For a reimplementation

Combine this with the rest of the corpus and the LAN/farm library is feature-complete for
secure printers too:
- get the printer cert via `app_cert_install` → store `printer_cert`,
- gate on the MQTT-Sec capability bit,
- RSA-PKCS1-encrypt `sequence_id`/`url` per command (secure-payload),
- answer the cert challenge with the decimal-serial `cert_id`.

Remaining unknowns are now only: **wire-verification against a secure printer**, the **cloud
`ft_*` tunnel**, and the **remote camera** (vendor Kalay/Agora). No secret key material is
reproduced here — algorithm and field names only.
