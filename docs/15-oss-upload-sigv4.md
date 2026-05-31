# bambu_networking — cloud file upload, OSS/S3 credential flow, AWS-Sigv4 signing & design publish

Static-only RE (no network). Sources:

- **Linux `01.07.01.04`** `libbambu_networking.so` — full symbols + readable `.rodata` (`.rodata` VA == file offset). Primary reference; all byte offsets below are into this file unless tagged `[win]`.
- **Windows `02.06.00.50`** `bnet.bin` (VMProtect-unpacked, plaintext strings) — used to confirm the sigv4 block and to **extend** payload schema with newer keys (`nozzleMapping`, `nozzleInfos`, `amsMapping2`, `slot_id`, `cover`).
- Demangled symtab `libbambu_networking.so.symtab.txt`.

All credential/secret VALUES below are FORMAT strings only — no real keys were present in the binaries. Anywhere a real value would appear is marked «REDACTED».

---

## 1. Implementing functions (from symtab)

| VA | Symbol | Role |
|----|--------|------|
| `0x2166c0` | `BBL::AccountManager::get_oss_config(string&, string, uint&, string&)` | GET OSS/S3 creds → fills cred struct |
| `0x1e79e0` | `BBL::AccountManager::upload_file_to_oss(string bucket?, boost::filesystem::path, string, uint&, string&, std::function<void(Http::Progress,bool&)>)` | the signed PUT (with progress cb) |
| — | `BBL::AccountManager::upload_3mf_to_oss(...)` | wraps upload of the sliced 3MF |
| `0x264400` | `bambu_network_put_rating_picture_oss` / `BBL::BambuNetworkAgent::put_rating_picture_oss` / `BBL::AccountManager::...` | rating-image OSS upload (uses `image/` content-type) |
| `0x26d180` | `BBL::TrackingManager::upload_file()` | telemetry/event file upload (logs `Tracking get_url = {}`) |
| `0x25d350` | `BBL::BambuNetworkAgent::start_publish(BBL::PublishParams, cb, cancel_cb, string*)` → `BBL::AccountManager::start_publish(...)` | POST publish task |
| — | `BBL::AccountManager::get_model_publish_url(...)` / `BBL::BambuNetworkAgent::get_model_publish_url(...)` | builds MakerWorld publish URL |
| `0x20a100` | `BBL::AccountManager::get_design_info(string, string&, uint&, string&)` | GET design metadata (`design-service/model/`) |
| — | `BBL::AccountManager::get_design_staffpick(...)` / `can_publish(...)` / `get_user_upload(...)` | design browse / pre-publish gate |

The agent class `BBL::BambuNetworkAgent` is a thin facade; real logic is in `BBL::AccountManager`. JSON is built/parsed with **nlohmann::json**.

---

## 2. OSS / S3 credential flow

### 2.1 Cred-fetch endpoints (CONFIRMED)

`get_oss_config` issues a GET to (host template `%1%`):

```
%1%/user-service/my/ossconfig?useType=1     (off 4956508 linux; 6279010 [win])
%1%/user-service/my/s3config?useType=1      (off 4956548 linux; [win])
```

`useType` is a query param (selects upload purpose / bucket class). Both builds emit the
literal `useType=1`; the `=N` is a positional format slot in callers — treat `useType` as
1..N. `ossconfig` returns Aliyun OSS creds; `s3config` returns AWS S3 creds. The SDK has
**both** signing paths compiled in and picks per-region/endpoint.

### 2.2 Cred response schema (CONFIRMED, off 4965495)

JSON keys parsed into the cred struct, in `.rodata` order:

```
endpoint
accessKeyId
accessKeySecret
securityToken
expiration
bucketName
cdnUrl
```

```jsonc
{
  "endpoint":        "«REDACTED host, e.g. oss-cn-*.aliyuncs.com / s3.*.amazonaws.com»",
  "accessKeyId":     "«REDACTED»",
  "accessKeySecret": "«REDACTED»",
  "securityToken":   "«REDACTED STS session token»",
  "expiration":      "«REDACTED ISO8601»",
  "bucketName":      "«REDACTED»",
  "cdnUrl":          "«REDACTED CDN base»"
}
```

These are short-lived STS creds (token + expiration) — re-fetched when expired.

### 2.3 URL construction (off 4965595)

```
https://%1%.%2%%3%      →  https://<bucketName>.<endpoint><objectPath>   (virtual-hosted)
%1%%2%                  →  <cdnUrl><objectPath>                          (download via CDN)
```

Object content-type seed string `image/` present (rating-picture path forces `image/...`).

---

## 3. AWS-Sigv4 signing (CONFIRMED, off 5049096–5049560 linux; off 6373280 [win])

The signer is provider-parameterised on the tuple `aws:amz` (off 5049132): the first token
(`AWS`/`aws`) feeds the `%s4...` slots, the second (`amz`) feeds the `x-%s-...` header slots.
So with the live provider it produces the standard AWS scheme. Order of `.rodata` format
strings reflects the build order:

| Purpose | Format string (`.rodata`) | Resolves to |
|---------|---------------------------|-------------|
| amz date | `%Y%m%dT%H%M%SZ` | `20260531T120000Z` |
| signed headers | `content-type;host;x-%s-date` | `content-type;host;x-amz-date` |
| canonical headers (no body-hash hdr) | `host:%s\nx-%s-date:%s\n` | `host:..\nx-amz-date:..\n` |
| canonical headers (with ctype) | `content-type:%s\nhost:%s\nx-%s-date:%s\n` | (off 5049460) |
| **canonical request** | `%s\n%s\n%s\n%s\n%s\n%s` | `METHOD\nURI\nQUERY\nCANON_HEADERS\nSIGNED_HEADERS\nHASHED_PAYLOAD` |
| scope terminator | `%s4_request` | `aws4_request` |
| **credential scope** | `%s/%s/%s/%s` | `<date>/<region>/<service>/aws4_request` |
| **string-to-sign** | `%s4-HMAC-SHA256\n%s\n%s\n%s` | `AWS4-HMAC-SHA256\n<amzdate>\n<scope>\n<hash(canonReq)>` |
| signing-key seed | `%s4%s` | `AWS4<accessKeySecret>` |

Signing-key derivation (standard HMAC chain, seed `AWS4`+secret):
`HMAC(HMAC(HMAC(HMAC("AWS4"+secret, date), region), service), "aws4_request")`.

### 3.1 Authorization header (CONFIRMED, off 5049485)

```
Authorization: %s4-HMAC-SHA256 Credential=%s/%s, SignedHeaders=%s, Signature=%s\r\n
X-%s-Date: %s\r\n
```

→ live:

```
Authorization: AWS4-HMAC-SHA256 Credential=«REDACTED AKID»/<scope>, SignedHeaders=content-type;host;x-amz-date, Signature=«REDACTED»
X-Amz-Date: 20260531T120000Z
```

### 3.2 Validation / error strings (signer guards, off 5049160+)

`region can't be empty`, `service can't be empty`, `first provider can't be empty`,
`second provider can't be empty`, `service missing in parameters or hostname`,
`region missing in parameters or hostname` — i.e. region/service are derived from the
endpoint hostname when not passed explicitly.

### 3.3 Aliyun OSS path (parallel, off 4965662)

For `ossconfig` creds the signer uses the Aliyun OSS v1 scheme instead of sigv4:

```
PUT\n\n                       (canonical: VERB \n Content-MD5 \n Content-Type ...)
x-oss-security-token:         (canonical x-oss header line)
OSS                           (Authorization prefix → "OSS <AKID>:<sig>")
Host
x-oss-security-token          (header name)
Content-Length
x-amz-security-token   /  AWS  (the S3 twin of the above, off 4965716)
```

So: `Authorization: OSS «REDACTED AKID»:«REDACTED sig»` + `x-oss-security-token: «REDACTED»`
for Aliyun; `Authorization: AWS4-HMAC-SHA256 ...` + `x-amz-security-token: «REDACTED»` for AWS.
Date header format for both: `%a, %d %b %Y %H:%M:%S GMT` (off 4965618).

---

## 4. End-to-end upload flow

```
1. get_oss_config  → GET  {host}/user-service/my/{oss,s3}config?useType=N
                      ← { endpoint, accessKeyId, accessKeySecret, securityToken,
                          expiration, bucketName, cdnUrl }                       [§2.2]
2. upload_3mf_to_oss / upload_file_to_oss:
     - build object URL  https://<bucketName>.<endpoint>/<objectPath>            [§2.3]
     - canonicalise request, derive signing key, compute Signature              [§3]
     - PUT object with headers:
         Host, Content-Length, (Content-Type),
         x-amz-date / X-Amz-Date  (or Date: …GMT for OSS),
         x-amz-security-token / x-oss-security-token: <securityToken>,
         Authorization: AWS4-HMAC-SHA256 … (or OSS <AKID>:<sig>)
     - streams body; std::function<void(Http::Progress,bool&)> progress callback
       (cancellable via the bool& out-param)
3. notify / publish:
     - download/share URL = <cdnUrl><objectPath>                                 [§2.3]
     - start_publish → POST {host}/user-service/my/task   (slice/print task)     [§5.1]
       and/or model publish URL                                                   [§5.2]
```

Failure log strings observed: `open file {} is failed`, `Failed to get file size:`,
`File does not exist:`, `ftp upload failed, status={}, error={}` (separate FTP/LAN path,
not the OSS path).

---

## 5. Design / MakerWorld publish

### 5.1 Slice/print task publish — `start_publish` → `POST {host}/user-service/my/task`

Endpoint `%1%/user-service/my/task` (off 4964210). nlohmann::json body. Keys (`.rodata`
order, off 4964114). **Linux `01.07.01.04`** set:

```
modelId, title, deviceId, filamentSettingIds, profileId, plateIndex,
timelapse, bedType, bedLeveling, flowCali, vibrationCali, layerInspect,
amsMapping, amsDetailMapping, useAms, oriProfileId, oriModelId
```

**Windows `02.06.00.50`** adds/extends (off 6271000 [win]) — multi-nozzle / AMS-2 era:

```
slot_id, amsMapping2, nozzleMapping, amsDetailMapping, nozzleInfos, mode,
useAms, oriProfileId, oriModelId
```

```jsonc
{
  "modelId": "...", "title": "...", "deviceId": "...",
  "filamentSettingIds": [...], "profileId": "...",
  "plateIndex": 0, "timelapse": false,
  "bedType": "...", "bedLeveling": true, "flowCali": false,
  "vibrationCali": true, "layerInspect": true,
  "amsMapping": [...], "amsMapping2": [...],          // [win] dual-nozzle
  "amsDetailMapping": [...], "nozzleMapping": [...],  // [win]
  "nozzleInfos": [...], "mode": "...",                // [win]
  "useAms": true,
  "oriProfileId": "...", "oriModelId": "..."
}
```

Task lookup/update: `%1%/user-service/my/task/%2%` (off ~4963760).
Task-create reuses response keys `upload_url`, `upload_ticket` (off 4964230).

> Note on the camelCase publish cluster from the baltobu brief
> (`{modelId, designId, title, cover, amsMapping, useAms, nozzleMapping, bedType,
> plateIndex, deviceId}`): CONFIRMED as a union across two surfaces — most keys come from
> the `my/task` body above; `designId`/`designTitle`/`cover` belong to the MakerWorld
> design-publish surface (§5.2). `cover` and `nozzleMapping` are present in the Windows
> build (`cover` off 6237503 [win], `nozzleMapping` off 6271024 [win]); `nozzleMapping`
> is **absent** from the older Linux build.

### 5.2 Model / design publish URLs — `get_model_publish_url`

Two URL templates in `.rodata` (off 4956340):

```
models/%1%/publish?project_id=%2%&profile_id=%3%        (print-profile publish)
/my/profiles/%1%/publish?project_id=%2%&design_id=%3%   (design publish)
```

(Matches the templated `{host}/publish?project_id=…&profile_id=…` /
`…&design_id=…` in `cloud-endpoints.md`.)

### 5.3 Design metadata / browse — `design-service` (off 4955650, 4962704)

```
%1%/design-service/model/%2%                                  (get_design_info; models=, & params)
%1%/design-service/design/staffpick?offset=%2%&limit=%3%      (get_design_staffpick)
%1%/design-service/my/design/recommend?seed=%2%&limit=%3%
```

`get_design_info` response keys observed nearby: `designId`, `designTitle` (off 4965874),
plus `report_url`, `urls` (telemetry get_url). Token/refresh keys (`accessToken`,
`expiresIn`, `refreshExpiresIn`) live in the same string pool (shared auth struct).

---

## 6. Verification status

| Brief claim | Status |
|-------------|--------|
| `/my/ossconfig?useType=N`, `/my/s3config?useType=N` | CONFIRMED (linux + win) |
| cred schema `{accessKeyId, accessKeySecret, securityToken, bucketName, endpoint, cdnUrl, expiration}` | CONFIRMED (exact keys, linux off 4965495) |
| sigv4 header `"%s4-HMAC-SHA256 Credential=%s/%s, SignedHeaders=%s, Signature=%s"` | CONFIRMED (linux off 5049485, win off 6373xxx) |
| canonical request / scope / STS / signing-key formats | CONFIRMED + documented (§3) |
| publish camelCase cluster | CONFIRMED as union of `my/task` body + design-publish surface; `nozzleMapping`/`cover` win-only |
| design-service / iot-service / user-service paths | CONFIRMED + EXTENDED (§5.2/5.3, §2.1) |

**Inferred (not byte-literal):** the exact HMAC key-derivation chaining order in §3 (standard
AWS construction, inferred from the `%s4%s` seed + `%s/%s/%s/%s` scope + per-line guards);
the `useType` 1..N range (only `=1` literal seen, `=N` is a caller format slot); the
virtual-hosted vs CDN URL mapping in §2.3 (inferred from the two `https://%1%.%2%%3%` /
`%1%%2%` templates adjacent to the cred keys).

No real keys, secrets, or tokens were present in either binary — only format strings.
