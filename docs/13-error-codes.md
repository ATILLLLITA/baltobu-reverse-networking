# Bambu networking SDK — error-code catalog

Static reverse-engineering only (no network). Cross-confirmed from three
independent sources:

1. **Linux .so w/ symbols** `libbambu_networking.so` (build `01.07.01.04`) —
   `.rodata` log format strings + demangled symtab. In this image `.rodata`
   VA == file offset (delta 0).
2. **Windows image** `bnet.bin` (v`02.06.00.50`) — newer build that contains
   the `enc_msg` security/verification handler and its numeric `err_code`
   literals. The Linux `01.07.01.04` build predates this feature (the
   `84033543..84033548` literals are absent there).
3. Prior recovered tables (`output/unpacking/error_codes.md`,
   `docs/05-wire-protocol.md`) — confirmed and extended below.

There are **two distinct numbering systems** plus several pass-through
systems the library does NOT interpret:

- **(1) Library-internal negative `int` return codes** — what every
  `bambu_network_*` / `ft_*` export returns to the caller. Not stored as
  strings; recovered from return-register constant assignments across the
  876-function transitive helper closure.
- **(2) Bambu application `enc_msg` error codes** — 32-bit positive
  integers in the encrypted-message (device-cert handshake) JSON
  `err_code` field, branched on by the `enc_msg` handler. Stored as
  literal strings in `.rdata` of `bnet.bin`.

Pass-through systems (logged but not interpreted by this library — listed
for disambiguation, not catalogued here): HTTP status codes (`set_on_http_error_fn`),
Paho MQTT `return code` / `reason code`, OpenSSL `X509_V_ERR_*` cert-verify
codes (present in `bnet.bin` .rdata as a separate enum-to-string table),
and arbitrary cloud `err_code` fields the library forwards verbatim.

---

## Table 1 — Library internal return codes (value → meaning)

Every `bambu_network_*` and `ft_*` export returns an `int`. `0` is success;
all error values are negative. "Sites/fns" = number of return-assignment
callsites / distinct functions emitting that constant (from the helper-closure scan).

| Code  | Hex (int32)  | Sites/fns | Meaning |
|------:|--------------|-----------|---------|
| `0`   | `0x00000000` | universal | **success** |
| `-1`  | `0xFFFFFFFF` | 98 / 84   | generic failure — global agent mismatch, NULL/invalid arg, or unhandled helper failure. By far the most common (used by `ft.job_cancel`, `ft.job_create`, `get_model_instance_id`, `get_model_mall_home_url`, `get_model_mall_rating`, +many) |
| `-2`  | `0xFFFFFFFE` | 8 / 8     | timeout / no message available (e.g. `ft_job_get_msg` queue-pop with empty queue) |
| `-3`  | `0xFFFFFFFD` | 1 / 1     | rare, specific failure path *(inferred)* |
| `-4`  | `0xFFFFFFFC` | 2 / 2     | session not connected (e.g. `ft_tunnel_sync_connect`, session state != 3) |
| `-5`  | `0xFFFFFFFB` | 2 / 2     | rare, specific failure *(inferred)* |
| `-6`  | `0xFFFFFFFA` | 1 / 1     | internal setter rejected (e.g. `set_server_callback` helper failure) |
| `-8`  | `0xFFFFFFF8` | 1 / 1     | specific failure *(inferred)* |
| `-9`  | `0xFFFFFFF7` | 2 / 2     | specific failure *(inferred)* |
| `-10` | `0xFFFFFFF6` | 1 / 1     | specific failure in a specific call path *(inferred)* |
| `-11` | `0xFFFFFFF5` | 2 / 2     | specific failure *(inferred)* |
| `-12` | `0xFFFFFFF4` | 1 / 1     | specific failure *(inferred)* |
| `-13` | `0xFFFFFFF3` | 1 / 1     | specific failure *(inferred)* |
| `-14` | `0xFFFFFFF2` | 1 / 1     | specific failure *(inferred)* |
| `-16` | `0xFFFFFFF0` | 1 / 1     | specific failure *(inferred)* |
| `-17` | `0xFFFFFFEF` | 2 / 2     | precondition not met *(inferred)* |
| `-18` | `0xFFFFFFEE` | 8 / 6     | invalid argument — emitted by several input validators |
| `-19` | `0xFFFFFFED` | 3 / 1     | related to `-18` (adjacent validator path) *(inferred)* |
| `-26` | `0xFFFFFFE6` | 5 / 5     | dev_id / device validation failed (e.g. `bambu_network_bind`) |

Notes:
- Codes are sparse (no `-7`, `-15`, `-20..-25` observed); the library does
  not use a contiguous enum.
- `-1` is the catch-all; treat any non-zero return as failure and only
  branch on the specific codes above where behaviour matters.
- The per-callsite attribution table is in
  `output/unpacking/error_codes.md`.

---

## Table 2 — Bambu application / `enc_msg` error codes (decimal, hex → meaning)

From the `enc_msg` (encrypted-message / device public-key handshake) handler
in `bnet.bin`. The handler reads a JSON `err_code` integer and branches on
these six values; the literals appear contiguously in `.rdata`
(file offsets `0x5E_xxxx`, decimal ~6213946+):

| Decimal   | Hex          | Meaning |
|----------:|--------------|---------|
| 84033543  | `0x05022647` | encrypted-message verification error 1 *(branched; specific text not logged — inferred generic verify failure)* |
| 84033544  | `0x05022648` | verification error 2 *(inferred)* |
| 84033545  | `0x05022649` | **need reset device pub key** — explicit log string; triggers device public-key reset / cert re-issue flow |
| 84033546  | `0x0502264A` | verification error 4 *(inferred)* |
| 84033547  | `0x0502264B` | verification error 5 *(inferred)* |
| 84033548  | `0x0502264C` | verification error 6 *(inferred)* |
| (other)   | —            | falls through `"enc_msg: unknown err_code, payload = {}"` — not interpreted, forwarded to caller |

Structure: 32-bit value `0x0502_26_4X`. High byte `0x05` = category
(only this category observed in these builds); low nibble = specific code
`0x7..0xC`. Only `84033545` carries an explicit human-readable consequence
in the log strings; the other five are branched individually but their log
emitters print only the numeric `err_code=NNNNNNNN`.

### Surrounding `enc_msg` handler diagnostics (context, not numeric codes)

Adjacent `.rdata` log literals from the same handler (these are
pre-checks / state messages, NOT `err_code` values):

```
enc_msg: mqtt_security_seq_id = {}
enc_msg: not a print command
enc_msg: no sequence_id in payload print command
enc_msg: not sequence_id in payload security command
enc_msg: not studio sequence_int = {}
enc_msg: show_msg = false
enc_msg: no err_code or invalid err_code type
enc_msg: err_code=84033543 ... 84033548
enc_msg: unknown err_code, payload = {}
enc_msg: add_encrypt info failed, dev_id = {}
enc_msg: add sign info failed
```

---

## Disambiguation: other code spaces seen in the binaries (pass-through)

These are present in the images but are **not** Bambu-defined; the library
logs them and forwards without interpreting (do not confuse with Tables 1/2):

- **HTTP status** — `code={}` / `code=%2%` in cloud upload / print-job /
  ticket flows (`get ticket failed, code={}`, `post ticket failed,code=`,
  `upload_3mf failed(%1%),code=%2%,body=%3%`, etc.). Standard 4xx/5xx.
- **MQTT (Paho)** — `return code=` and `reason code=` field keys logged
  during connect/login; Paho `MQTTReasonCodes`.
- **OpenSSL cert verify** — full `X509_V_OK` / `X509_V_ERR_*` enum-to-string
  table in `bnet.bin` `.rdata` (~offset 6320700). TLS chain validation only.
- **Login/bind status strings** (not numeric): `recv_ticket`, `get_ticket`,
  `post_ticket`, `wait_auth`, `wait_info`, `wait_printer`, `wait_time`,
  `recv_err`, `FAILURE`, `SUCCESS`, `login_report`.

No secrets, keys, or credentials are included; all values above are numeric
constants and log format strings recovered statically from the binaries.
