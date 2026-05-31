# Bambu Networking SDK — Login / Auth Payload Schema (static RE)

Static analysis only. No network / cloud / printer calls were made.
All secret VALUES are redacted as «REDACTED»; this document is SCHEMA / KEY names only.

## ⚠️ CORRECTION (disassembly-verified, supersedes the inferred schema below)

The sandbox blocked objdump for this run, so the original schema below over-merged keys into
`studio_userlogin`. Disassembling the **real** function bodies (objdump, main host) corrects it:

- **`AccountManager::build_login_cmd` @ 0x21aed0** (942 b) — ordered rodata keys: `command`,
  `sequence_id`, `data`, `avatar`, `name`; literal `studio_userlogin` @ 0x4c1620; built with
  nlohmann `initializer_list`. So the cloud MQTT login is **minimal**, exactly matching baltobu:
  ```json
  { "command": "studio_userlogin", "sequence_id": "<seq>",
    "data": { "name": "<display>", "avatar": "<url>" } }
  ```
  The identity keys (`user_id/account/cli_id/dev_id/country_code/tutk`) are **NOT** in
  `studio_userlogin.data` — they live in a separate AccountInfo cluster.
- **`AccountManager::build_login_request` @ 0x1e3550** — ordered keys: `login`, `command`,
  `sequence_id`, `wifi`, `tutk` (+ `%d`). This is the **printer-login** command — it ties
  directly to the farm TCP-3002 `{"login":{"command":"login",…}}` handshake (OpenBambuFarmAPI),
  with added `wifi`/`tutk` fields.
- `build_login_info` @ 0x25ecc0 builds the same `studio_userlogin` shape.

Everything below is the original (string-clustering) recovery — keep for the HTTP sign-in /
ticket-token details, but treat the `studio_userlogin` key list there as superseded by the above.

## Sources
- Linux build w/ symbols: `…/Tzeny-CDN/linux_01.07.01.04/libbambu_networking.so` (v01.07.01.04)
- Demangled symtab: `…/analysis-work/tzeny-cdn/libbambu_networking.so.symtab.txt`
- Windows image (strings/keys): `/root/_bambu_new/live/collected/bnet/bnet.bin` (v02.06.00.50)
- Cross-ref: baltobu documented commands (`studio_userlogin`, `studio_useroffline`)

## Method & tooling caveat (IMPORTANT)
The sandbox **denied execution of `objdump`, `python3`, `strings`** against the
binaries, so I could **not** disassemble the function bodies. That means I could
NOT decode rip-relative `lea` → rodata pointers as instructions, nor decode the
`movabs reg, 0x<8 ascii>` **inlined SSO immediates** that nlohmann::json uses for
short keys. The recovery below is built from:
1. **rodata string constants** extracted with `grep -abo` (byte-offset clustering)
   from both the Linux `.so` and the Windows image, and
2. **symbol-table** function boundaries, and
3. **baltobu** cross-reference.

Keys that are present as rodata constants are marked **[rodata]**.
Keys that are required by the documented command shape but are NOT standalone
rodata constants (they are almost certainly nlohmann SSO `movabs` immediates that
I could not decode without a disassembler) are marked **[inlined-immediate, inferred]**.

### Function map (real bodies vs `.cold` clones)
The addresses in the task prompt are the `.cold` (exception/slow-path) clones.
Real bodies:

| Function | real body | size | .cold clone |
|---|---|---|---|
| `BBL::AccountManager::build_login_cmd[abi:cxx11]()` | `0x21aed0` | 942 | `0x1481c4` |
| `BBL::BambuNetworkAgent::build_login_cmd[abi:cxx11]()` | `0x25beb0` | 68 (tail-calls AccountManager) | — |
| `BBL::AccountManager::build_login_request(string, bool)` | `0x1e3550` | 1897 | `0x13c9bc` |
| `BBL::BambuNetworkAgent::build_login_info[abi:cxx11]()` | `0x25ecc0` | 1576 | `0x151ad4` |
| `BBL::AccountManager::build_logout_cmd[abi:cxx11]()` | `0x21b280` | 496 | `0x14824c` |
| `BBL::BambuNetworkAgent::build_logout_cmd[abi:cxx11]()` | `0x25bf00` | 68 | — |
| `BBL::AccountManager::get_token_str[abi:cxx11](bool)` | `0x2066a0` | 1042 | `0x1431f4` |
| `BBL::AccountManager::get_ticket(string,uint&,string&)` | `0x20df30` | 1149 | `0x145408` |
| `BBL::AccountManager::post_ticket(string,uint&,string&)` | `0x20ecd0` | 1496 | `0x14585e` |
| C entry points | `bambu_network_build_login_cmd@0x260ca0`, `…_build_login_info@0x260d80`, `…_build_logout_cmd@0x260d10` | | |

## rodata key clusters (evidence)

**Linux `.so` login cluster** (byte offsets, all contiguous in one .rodata region):
```
4960019 name              4960938 user_id
4960035 name              4960971 account
4960524 dev_id            4960979 refresh_token
4960585 dev_id            4961001 expires_in
4960815 country_code      4961040 avatar
4960938 user_id           4961068 sequence_id
                          4961085 tutk
4962456 Authorization     4962403/4962496 "Bearer "
4986400 studio_userlogin
4989991 cli_id
```
**Windows image login/sign-in cluster:**
```
6269248 account     6269264 expires_in   6269280 refresh_token
6275184 accessToken 6275200 expiresIn    6275216 refreshToken   (HTTP response camelCase)
6275347 dev_id      6275445 id
6275472 studio_userlogin   6275504 studio_useroffline   (adjacent, 32 B apart)
6294636/6296465 cli_id      6296555 tutk
```
Both `studio_userlogin` and `studio_useroffline` are confirmed present in the
Windows image; only `studio_userlogin` survives as a standalone constant in the
Linux `.so` (the offline name is built via concatenation / inlined there).

---

## 1) MQTT login command — `studio_userlogin`
Built by `AccountManager::build_login_cmd` (BambuNetworkAgent::build_login_cmd tail-calls it).
Confirms + extends baltobu's `{command, sequence_id, data:{name, avatar}}`.

```jsonc
{
  "command":     "studio_userlogin",   // [inlined-immediate, inferred]  envelope verb
  "sequence_id": "<seq>",              // [rodata] @4961068 — monotonic string counter
  "user_id":     "<uid>",              // [rodata] @4960938 — BBL user id
  "account":     "<email/login>",      // [rodata] @4960971
  "name":        "<display name>",     // [rodata] @4960019/35  (baltobu data.name)
  "avatar":      "<avatar url>",       // [rodata] @4961040     (baltobu data.avatar)
  "cli_id":      "<client/session id>",// [rodata] @4989991
  "dev_id":      "<device id>",        // [rodata] @4960524/85
  "country_code":"<ISO region>",       // [rodata] @4960815
  "tutk":        <0|1 / token>         // [rodata] @4961085 — TUTK P2P enable/credential flag
}
```
Notes:
- baltobu describes the public shape as `{command, sequence_id, data:{name,avatar}}`.
  In this SDK build the login-identity keys (`account, user_id, cli_id, country_code,
  dev_id, tutk`) appear as a **flat** key cluster in the same rodata region as
  `name`/`avatar`. Whether they nest under a `data` object or sit flat in the
  published frame could not be confirmed without disassembly — **the `data{}`
  nesting is marked inferred** (baltobu shows nesting; rodata shows a flat cluster).
- `command` itself is not a standalone rodata constant in the login region → it is
  emitted as an nlohmann SSO inlined immediate (`movabs … ,0x646e616d6d6f63` →
  "command") that I could not decode here.

## 2) Logout command — `studio_useroffline`
`AccountManager::build_logout_cmd` @0x21b280. Mirror of the login envelope.
```jsonc
{
  "command":     "studio_useroffline", // [rodata @6275504 in Win; inlined in Linux]
  "sequence_id": "<seq>",              // [rodata]
  "user_id":     "<uid>"               // [rodata]   (no name/avatar/tutk payload)
}
```
(`name`/`avatar`/`tutk` are inferred-absent for offline — only identity is sent.)

## 3) HTTP login request — `build_login_request(string url, bool)`
@0x1e3550. This is the cloud sign-in exchange. Endpoint family confirmed in rodata:
`/api/sign-in/ticket?to=` and the bearer header path.

Request side (token-based sign-in — the SDK signs in with an existing access
token, NOT username/password; no `password`/`client_id`/`2fa` keys appear in the
bnet rodata sign-in cluster):
- HTTP header: `Authorization: Bearer «REDACTED-access-token»`  (built by `get_token_str(true)`)
- Body fields present in the sign-in cluster: `account` [rodata @6269248], `token` [rodata].

Response body parsed by this function (camelCase variant @6275184 in Win image):
```jsonc
{
  "accessToken":  "«REDACTED»",   // [rodata] @6275184
  "refreshToken": "«REDACTED»",   // [rodata] @6275216
  "expiresIn":    <seconds>,      // [rodata] @6275200
  "tfaKey":       "«REDACTED»",   // [inferred] present in Win strings — 2FA challenge key
  "loginType":    "<verify|…>",   // [inferred] present in Win strings
  "code":         <int>,          // [rodata] api status code
  "error":        "<msg>",        // [rodata]
  "message":      "<msg>"         // [rodata]
}
```
The token store then persists the **snake_case** form (cluster @4960979/4961001
Linux, @6269264/6269280 Win):
```jsonc
{ "access_token":"«REDACTED»", "refresh_token":"«REDACTED»", "expires_in":<s> }
```
So the SDK reads camelCase off the HTTP response and writes snake_case into its
local credential store (the same names re-used by the MQTT side).

## 4) `build_login_info` payload — `BambuNetworkAgent::build_login_info()`
@0x25ecc0 (1576 B). Identity/credential blob assembled for the agent. Recovered
keys (same rodata region as the login cmd):
```jsonc
{
  "account":      "<email/login>",   // [rodata]
  "user_id":      "<uid>",           // [rodata]
  "name":         "<display name>",  // [rodata]
  "avatar":       "<url>",           // [rodata]
  "cli_id":       "<client id>",     // [rodata]
  "dev_id":       "<device id>",     // [rodata]
  "country_code": "<region>",        // [rodata]
  "tutk":         <flag>,            // [rodata]
  "access_token": "«REDACTED»",      // [rodata]
  "refresh_token":"«REDACTED»",      // [rodata]
  "expires_in":   <seconds>          // [rodata]
}
```
(Exact subset / nesting not disassembled; these are the constants the 1576-byte
builder draws from.)

## 5) Ticket / token flow
- `get_token_str(bool)` @0x2066a0 — returns the auth token; the `bool` selects
  whether the `"Bearer "` prefix is prepended (rodata `"Bearer "` @4962403/4962496,
  `"Authorization"` @4962456) for use as the HTTP `Authorization` header.
- `get_ticket(string, uint&, string&)` @0x20df30 — GETs a notification/upload
  ticket. Endpoints: `/api/user/notification?action=upload&ticket=`,
  `/api/sign-in/ticket?to=`, `/api/user/ttcode`. Returns `ticket` [rodata].
- `post_ticket(string, uint&, string&)` @0x20ecd0 — POSTs the ticket result back
  (notification upload confirm / sign-in `to=<target user>` exchange). Body uses
  `ticket`, `code`, `to_user_id`/`to=` [inferred from endpoint `?to=`].
- Response envelope keys throughout: `code`, `error`, `message`, `result`, `ticket`,
  `token` (all [rodata]).

---

## Confidence / limitations
- **High confidence (rodata-confirmed):** command name `studio_userlogin`,
  `studio_useroffline`; keys `sequence_id, user_id, account, name, avatar, cli_id,
  dev_id, country_code, tutk`; token keys `access_token/refresh_token/expires_in`
  and camelCase `accessToken/refreshToken/expiresIn`; endpoint `/api/sign-in/ticket?to=`;
  `Authorization: Bearer` header.
- **Inferred (could NOT disassemble):** the literal `"command"` envelope key and any
  other 1–8-char keys emitted as nlohmann SSO `movabs` immediates; the exact
  flat-vs-`data{}`-nested grouping of the login frame; `tfaKey`/`loginType` membership
  in the sign-in response; precise field subset of `build_login_info`.
- To finish the immediate-decoded keys and confirm nesting, re-run with `objdump`/`python3`
  enabled and disassemble `0x21aed0`, `0x1e3550`, `0x25ecc0`, `0x21b280`, decoding both
  `lea rip+disp → .rodata` (VA==fileoff, delta 0) and `movabs r,imm64` (LE ascii).
