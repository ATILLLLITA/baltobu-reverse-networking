# MQTT-Sec / app_cert command-security crypto

The Tier-1 gap baltobu, ClusterM, and OpenBambuAPI leave open: how a **secured**
(non-developer-mode) printer authenticates the client and protects MQTT/HTTP commands.

> **Attribution & correction.** The authoritative mechanism below is documented in the **Software
> Freedom Conservancy `reverse-networking` repo, `authorization-control` branch**
> (`f.sfconservancy.org/j4k0xb/reverse-networking`, *Authorization Control/*: `4. App
> Certificates`, `5. MQTT`, `6. HTTP`, `3. Renewal`). An earlier reconstruction of ours (from a
> farm-server clone) modelled this as `sequence_id_enc` + an RSA challenge — that was **wrong**;
> the real scheme is an **RSA-SHA256 signature with the *app* private key** plus **field
> encryption with the *device* public key**, as below. Our test printer ran *device-insecure*
> firmware so we never exercised it; the SFC branch did.

## Capability gate

A secured printer advertises support via **`print.flag3` bit 16** in `push_status`. Only then do
clients sign/encrypt.

## App certificate acquisition (cloud) — `3. Renewal`

The **app private key is issued by the cloud**, not extracted from the binary:

```
GET https://api.bambulab.com/v1/iot-service/api/user/applications/{enc_secret_b64}/cert?aes256={wrapped_key_b64}&ver=1
```
(the same `/applications/{id}/cert?aes256=…&ver=1` endpoint noted in `18-device-cert-security`).

- **`bootstrap_secret`** = hardcoded in the client = the app-cert CN prefix
  (`GLOF…-…`) + 16 hex chars ("to hinder straightforward RE").
- It is **AES-256-GCM**-encrypted under a random session key; the session key is **RSA-wrapped**
  with the service's public key; both go base64 in the URL.
- The server returns the **cert chain (PEM)** + the **AES-256-GCM-encrypted app private key**,
  which the client decrypts with the session key and stores for signing/encryption.
- The app certificate is **shared across all installations** (not per-device): chain
  `application_root.bambulab.com` → intermediate → leaf, RSA-2048.

## MQTT command authorization — `5. MQTT`

Two independent layers:

**(a) Signature — the `header` object (all critical commands).** RSA-SHA256 over the **complete
JSON payload** (UTF-8 bytes), signed with the **app private key**:
```json
"header": {
  "sign_ver":   "v1.0",
  "sign_alg":   "RSA_SHA256",
  "sign_string":"<base64(RSA_sign(app_priv, utf8(json)))>",
  "cert_id":    "<app_cert.serialNumber.lower() + app_cert.issuer>",
  "payload_len":"<utf8 byte length of the payload>"
}
```
*"The `header` object proves the request content was produced by the holder of the app private key."*

**(b) Field encryption — `project_file` / `gcode_line` only.** The sensitive field is RSA-encrypted
with the **device certificate public key** and added as a parallel `*_enc` field (cleartext kept;
≤ ~245 bytes per RSA):
- `print.project_file.url` → **`url_enc`**
- `print.gcode_line.param` → **`param_enc`**

Order: field-encrypt first, then sign the whole object. Example:
```json
{ "print": { "command":"project_file", "url":"file:///…",
             "url_enc":"<base64 RSA(device_pub, url)>" },
  "header": { "sign_ver":"v1.0","sign_alg":"RSA_SHA256",
              "sign_string":"<…>","cert_id":"<…>","payload_len":123 } }
```

## HTTP authorization — `6. HTTP`

A possession token (not a content signature):
- **`x-bbl-device-security-sign`** = `base64( RSA_Encrypt_NoPadding( app_priv, utf8(unix_ms_decimal) ) )`
  — deterministic RSA (no padding) over the current millisecond timestamp.
- **`x-bbl-app-certification-id`** = `issuer + ":" + serialNumber.lower()`.

## For a reimplementation

To drive **stock/secure** printers (no Developer Mode):
1. Acquire the app cert + private key from the cloud (`bootstrap_secret` → applications/cert) — needs a Bambu cloud account; the `bootstrap_secret` is in the binary.
2. Get the device cert (printer's own, via `app_cert_install`).
3. Per command: `url_enc`/`param_enc` with the device pubkey (for `project_file`/`gcode_line`), then append the `header` RSA-SHA256 signature with the app private key.
4. For HTTP: add `x-bbl-device-security-sign` + `x-bbl-app-certification-id`.

This closes the command-security gap. The remaining unknowns are now only the **cloud `ft_*`
tunnel wire** and the **remote camera** (vendor Kalay/Agora). Full detail + the captured cert
chain/CRL are in the SFC `authorization-control` branch cited above; no secret key material is
reproduced here.
