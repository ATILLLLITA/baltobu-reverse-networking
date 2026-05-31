# bambu_networking — HTTP client layer

Recovered from the symbol-bearing Linux `01.07.01.04` build + the `02.06.00.50` memory image.

## Implementation

All cloud REST traffic goes through **`BBL::Http`** — a thin libcurl wrapper:

| Symbol | Role |
|--------|------|
| `BBL::Http::get(string url, …)` @ 0x19cdf0 | GET |
| `BBL::Http::post(string url, …)` | POST |
| `BBL::Http::priv::http_perform()` @ 0x137e46 | the `curl_easy_perform` core |
| `BBL::Http::priv::curl_error(CURLcode)` | CURLcode → message |
| `BBL::Sftp::priv::curl_error(CURLcode)` | a parallel `BBL::Sftp` (libcurl SFTP) for file transfer |

libcurl is statically linked (`Curl_*`, `curl_url_get/set`, `curl_mime_*`). PUT/POST helpers
for specific resources live on `AccountManager` (`put_setting`, `put_notification`,
`put_rating_picture_oss`, `put_model_mall_rating`, …).

## Request headers

Every authenticated request carries:

| Header | Value | Build |
|--------|-------|-------|
| `Authorization` | `Bearer <access token>` | both (token «REDACTED») |
| `User-Agent` | `BambuStudio/<ver>` (slicer UA) | both |
| `Accept-Encoding` | gzip/deflate | both |
| `X-BBL-Language` | UI language | both |
| `X-BBL-Client-ID` | client id | **02.06** |
| `X-BBL-Device-ID` | device id | **02.06** |
| `X-BBL-Agent-OS-Type` | OS type | **02.06** |
| `X-BBL-Agent-Version` | agent version | **02.06** |
| `X-BBL-Executable-info` | host executable info | **02.06** |

The `X-BBL-*` suite expanded from just `X-BBL-Language` (01.07) to the full set (02.06). Extra
headers can be injected via `bambu_network_set_extra_http_header(map<string,string>)`.

## Auth retry / token refresh

The client interposes a **refresh-on-401** loop (strings: `401`, `unauthorized`,
`refresh_token`/`refreshtoken`, `retry_request`, `retry_count`):

```
perform → if HTTP 401/unauthorized:
            AccountManager::request_refreshtoken()  → POST /user-service/user/refreshtoken
            re-sign with new Bearer token
            retry_request (bounded by retry_count)
```

`get_token_str(bool)` supplies the current `Bearer` value; a near-expiry token is refreshed
pre-emptively (see `login-json.md` / `cloud-auth.md`).

## Notable curl options

- `CURLOPT_SSLVERSION` — pinned TLS version.
- `CURLOPT_RESOLVE` — **custom host→IP override** (region routing / LAN), so a hostname like
  `api.bambulab.com` can be force-resolved; pairs with `AccountManager::load_servers_from_region`.
- `CURLOPT_PIPEWAIT`, `CURLOPT_SOCKS`, `CURLOPT_FTPSSLAUTH` (the SFTP/FTP path).

## Cross-links

- Hosts the requests target: `cloud-auth.md` (host matrix) + `cloud-endpoints.md` (50 paths).
- The OSS/S3 upload path signs its own requests (AWS-SigV4 / Aliyun-OSS) on top of this client —
  see `oss-upload-sigv4`.
- TLS verification is custom-pinned — see `security-device-cert.md`.

> Tokens, the actual `Authorization` value, and any credentials are «REDACTED» — header names
> and the retry/refresh mechanics only.
