# Bambu Studio (open-source, AGPL) — Cloud Print / File-Transfer source notes

Source: https://github.com/bambulab/BambuStudio (branch `master`), fetched 2026-05-31 via
raw.githubusercontent.com. This is public AGPL source. No secrets included.

Key takeaway: the **SDK ABI is present in the open source**. Two vendored headers carry it:

- `src/slic3r/Utils/bambu_networking.hpp` — declares `BBL::PrintParams`, `BBL::PublishParams`,
  the full `BAMBU_NETWORK_ERR_*` code table (incl. **-3030**), and the `func_*` function-pointer
  typedefs (`func_start_print`, `func_start_local_print_with_record`,
  `func_start_send_gcode_to_sdcard`, ...).
- `src/slic3r/Utils/FileTransferUtils.hpp` — declares the newer **`ft_*` file-transfer ABI**
  (`FileTransferModule` function-pointer table, `FT_TunnelHandle`/`FT_JobHandle`, `ft_job_result`,
  `ft_job_msg`, `ft_err`).

The error *strings* and the actual cert handling live inside the **closed-source** `BambuNetwork`
plugin (the `libbambu_networking` shared lib the `func_*`/`ft_*` pointers are bound to). Studio
only defines the ABI and the numeric error codes; it does not contain the curl/OpenSSL call sites.

---

## 1. Error -3030 and the "unable to get local issuer certificate" (CURL error 60)

### What -3030 means

`bambu_networking.hpp` (namespace `BBL`, ~line 7) defines the "Start Print" (SP) error range
-3010..-3140. Line ~95:

```cpp
#define BAMBU_NETWORK_ERR_PRINT_SP_UPLOAD_3MF_CONFIG_TO_OSS_FAILED  -3030
```

Surrounding SP codes (cloud start-print pipeline, in execution order):

```
-3010  ERR_PRINT_SP_REQUEST_PROJECT_ID_FAILED        // create/allocate cloud project id
-3020  ERR_PRINT_SP_CHECK_MD5_FAILED                  // md5 mismatch check
-3030  ERR_PRINT_SP_UPLOAD_3MF_CONFIG_TO_OSS_FAILED   // <-- upload the *config* 3mf to cloud OSS/S3
-3040  ERR_PRINT_SP_PUT_NOTIFICATION_FAILED
-3050  ERR_PRINT_SP_GET_NOTIFICATION_TIMEOUT
-3060  ERR_PRINT_SP_GET_NOTIFICATION_FAILED
-3070  ERR_PRINT_SP_FILE_NOT_EXIST
-3080  ERR_PRINT_SP_GET_USER_UPLOAD_FAILED
-3090  ERR_PRINT_SP_FILE_OVER_SIZE
-3100  ERR_PRINT_SP_UPLOAD_3MF_TO_OSS_FAILED          // upload the full 3mf to cloud OSS/S3
-3110  ERR_PRINT_SP_PATCH_PROJECT_FAILED
-3120  ERR_PRINT_SP_POST_TASK_FAILED
-3130  ERR_PRINT_SP_WAIT_PRINTER_FAILED
-3140  NETOWRK_ERR_PRINT_SP_ENC_FLAG_NOT_READY        // (sic: "NETOWRK" typo in source)
```

So **-3030 = the cloud "start print" path failed while HTTPS-uploading the 3MF *config* object to
Bambu's object storage (OSS / S3-compatible bucket)**. It is the first network op in the SP
pipeline that talks to OSS over TLS (after project-id allocation and the local md5 check). -3100
is the same failure for the full 3MF payload.

The parallel range -2010..-2140 is `ERR_PRINT_WR_*` ("print with record", i.e.
`start_local_print_with_record`); -2030 is the WR analogue
`ERR_PRINT_WR_UPLOAD_3MF_CONFIG_TO_OSS_FAILED`.

### Why CURL error 60 / "unable to get local issuer certificate" triggers -3030

CURL error **60 = `CURLE_PEER_FAILED_VERIFICATION`** ("unable to get local issuer certificate")
means libcurl, inside the closed `BambuNetwork` plugin, could not verify the OSS endpoint's TLS
chain against a trusted CA. Because the OSS upload is exactly the step that errors as -3030, a
cert-verification failure on that HTTPS PUT surfaces to the user as **-3030 "Failed to send print
job"** (Studio's generic fallback string — see below; Studio does not special-case -3030, so it is
shown as the default send-failure message rather than a cert-specific one).

Triggers for the curl-60 on this path (all environmental — the code itself is fine):

- **Missing / wrong CA bundle.** The plugin's libcurl was built against OpenSSL and looks for the
  system CA store. On Linux/AppImage and on misconfigured Windows boxes the CA path can be empty
  or stale, so the OSS cert chain can't be verified.
- **`OPENSSL_CONF` / `SSL_CERT_FILE` / `SSL_CERT_DIR` / `CURL_CA_BUNDLE` env sensitivity.** If
  `OPENSSL_CONF` points at a config that disables/loads providers oddly, or `SSL_CERT_*` points at
  a non-existent file, OpenSSL fails to build the trust store and curl returns 60. This is the
  classic "works until some other app set OPENSSL_CONF globally" failure.
- **Corporate MITM / proxy** injecting a TLS cert whose issuer isn't in the local store.
- **Clock skew** can also manifest as chain-build failure, though it usually reports curl-60's
  cousins (error 58/77/35). The "local issuer certificate" wording specifically = the intermediate
  or root needed to complete the chain is not present in the trust store.

Net: -3030 is a *cloud-OSS-upload* failure code; the curl-60 cert error is the *root cause* on
machines whose OpenSSL/curl trust store is missing or overridden. Studio's own source contains
**no** mention of curl/SSL/CA/OpenSSL — that logic is entirely inside the binary `BambuNetwork`
plugin. Mitigation is environmental: provide a valid CA bundle (`CURL_CA_BUNDLE` /
`SSL_CERT_FILE`), and clear any hostile `OPENSSL_CONF`.

---

## 2. Cloud-print + file-transfer call flow (Studio side)

### Entry: `src/slic3r/GUI/Jobs/PrintJob.cpp`

Includes (top of file) — these pin where the ABI lives:

```cpp
#include "bambu_networking.hpp"                  // BBL::PrintParams + error codes
#include "slic3r/GUI/DeviceCore/DevManager.h"
#include "slic3r/GUI/DeviceCore/DevUtil.h"
#include "slic3r/Utils/FileTransferUtils.hpp"    // ft_* file-transfer ABI
```

`PrintJob::process()` builds a `BBL::PrintParams` and dispatches by connection mode / print type:

1. **LAN with cloud record:** `m_agent->start_local_print_with_record(params, update_fn,
   cancel_fn, wait_fn)` — uploads to printer via FTP **and** records the task in the cloud.
2. **Cloud fallback / cloud print:** `m_agent->start_print(params, update_fn, cancel_fn, wait_fn)`
   — this is the **cloud OSS upload + cloud task POST** path that produces the -30xx codes
   (including **-3030**).
3. **Send to SD / "send G-code to sdcard":** `m_agent->start_send_gcode_to_sdcard(...)` /
   `start_sdcard_print(...)`.

`m_agent` is a `NetworkAgent*` (held by `MachineObject` in `DeviceManager.hpp` as
`NetworkAgent *m_agent`). MQTT publishing of the actual print *command* is via
`MachineObject::publish_json()` / `cloud_publish_json()` / `local_publish_json()`.

### Cloud "send print with download" — high-level sequence (cloud path, `start_print`)

```
1. request cloud project id            -> fail -3010
2. local md5 of 3mf                     -> fail -3020
3. HTTPS PUT 3mf *config* to OSS bucket -> fail -3030   *** curl-60 cert failure lands here ***
4. HTTPS PUT full 3mf to OSS bucket     -> fail -3100
5. PATCH project metadata              -> fail -3110
6. POST print task (cloud)             -> fail -3120
7. put/await printer notification      -> fail -3040 / -3050 / -3060
8. wait for printer to accept          -> fail -3130
```

The printer then pulls ("with download") the 3MF from OSS using the signed URL the cloud handed it
via the task/notification — i.e. Studio uploads to OSS, posts the task, and the *printer* downloads
from OSS. Studio does not stream the file to the printer in cloud mode.

### Error → message mapping in PrintJob.cpp

```cpp
if (result < 0) {
    curr_percent = -1;
    if (result == BAMBU_NETOWRK_ERR_PRINT_SP_ENC_FLAG_NOT_READY) {
        msg_text = _L("Retrieving printer information, please try again later.");
    }
    else if (result == BAMBU_NETWORK_ERR_PRINT_WR_FILE_NOT_EXIST ||
             result == BAMBU_NETWORK_ERR_PRINT_SP_FILE_NOT_EXIST) {
        msg_text = FILE_IS_NOT_EXISTS_STR;
    } else if (result == BAMBU_NETWORK_ERR_PRINT_SP_FILE_OVER_SIZE ||
               result == BAMBU_NETWORK_ERR_PRINT_WR_FILE_OVER_SIZE) {
        msg_text = FILE_OVER_SIZE_STR;
    }
    // ... FTP-upload-failed cases ...
    // -3030 is NOT special-cased -> falls through to:
    //     msg_text = SEND_PRINT_FAILED_STR;   ("Failed to send the print job")
}
```

So -3030 reaches the user as the generic **"Failed to send the print job"** dialog (with the raw
code in the log), not a cert-specific message.

---

## 3. SDK ABI header — struct field lists (CONFIRMS reversed layouts)

### `BBL::PrintParams` — `src/slic3r/Utils/bambu_networking.hpp` (~line 180)

Exact field order as declared:

```cpp
struct PrintParams {
    std::string  dev_id;
    std::string  task_name;
    std::string  project_name;
    std::string  preset_name;
    std::string  filename;            // local 3mf path
    std::string  config_filename;
    int          plate_index;
    std::string  ftp_folder;
    std::string  ftp_file;
    std::string  ftp_file_md5;
    std::string  nozzle_mapping;
    std::string  ams_mapping;
    std::string  ams_mapping2;
    std::string  ams_mapping_info;
    std::string  nozzles_info;
    std::string  connection_type;     // string, e.g. "lan"/"cloud"
    std::string  comments;
    int          origin_profile_id = 0;
    int          stl_design_id = 0;
    std::string  origin_model_id;
    std::string  print_type;          // string, e.g. "print"/"resume"
    std::string  dst_file;
    std::string  dev_name;
    std::string  dev_ip;
    bool         use_ssl_for_ftp;
    bool         use_ssl_for_mqtt;
    std::string  username;            // "bblp" for LAN
    std::string  password;            // LAN access code
    bool         task_bed_leveling;
    bool         task_flow_cali;
    bool         task_vibration_cali;
    bool         task_layer_inspect;
    bool         task_record_timelapse;
    bool         task_timelapse_use_internal;
    bool         task_use_ams;
    std::string  task_bed_type;
    std::string  extra_options;
    int          auto_bed_leveling{ 0 };
    int          auto_flow_cali{ 0 };
    int          auto_offset_cali{ 0 };
    int          extruder_cali_manual_mode{ -1 };
    bool         task_ext_change_assist;
    bool         try_emmc_print;
};
```

Note: `connection_type` and `print_type` are **`std::string`**, not enums. No separate
connection/print-type enum in this header; string constants like `"printer"`, `"filament"`,
`"print"` are used.

### `BBL::PublishParams` — `bambu_networking.hpp` (~line 230)

```cpp
struct PublishParams {
    std::string  project_name;
    std::string  project_3mf_file;
    std::string  preset_name;
    std::string  project_model_id;
    std::string  design_id;
    std::string  config_filename;
};
```

### NetworkAgent function-pointer typedefs — `src/slic3r/Utils/NetworkAgent.hpp`

The agent binds these from the closed plugin (`PrintParams` passed **by value**):

```cpp
typedef int (*func_start_print)(void *agent, PrintParams params,
        OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn);
typedef int (*func_start_local_print_with_record)(void *agent, PrintParams params,
        OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn);
typedef int (*func_start_send_gcode_to_sdcard)(void *agent, PrintParams params,
        OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn, OnWaitFn wait_fn);
typedef int (*func_start_local_print)(void *agent, PrintParams params,
        OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn);
typedef int (*func_start_sdcard_print)(void *agent, PrintParams params,
        OnUpdateStatusFn update_fn, WasCancelledFn cancel_fn);
```

Matching `NetworkAgent` methods: `start_print`, `start_local_print_with_record`,
`start_send_gcode_to_sdcard`, `start_local_print`, `start_sdcard_print`.

---

## 4. `ft_*` file-transfer ABI — `src/slic3r/Utils/FileTransferUtils.hpp`

This is the newer file-transfer tunnel module (separate dynamic lib, `dlopen`/`LoadLibrary`-loaded
via `InitFTModule()` / `UnloadFTModule()` / `module()`). Namespace `Slic3r` (+ `Slic3r::detail`).
Opaque handles: `FT_TunnelHandle`, `FT_JobHandle`. Calling convention macro `FT_CALL`.

### Result / message structs (framing-relevant)

```c
struct ft_job_result {
    int         ec;          // error code
    int         resp_ec;     // remote/response error code
    const char *json;        // result payload as a JSON string
    const void *bin;         // optional binary blob (opaque)
    uint32_t    bin_size;
};

struct ft_job_msg {
    int         kind;        // message-kind discriminator
    const char *json;        // message payload as a JSON string
};

typedef enum {
    FT_OK        =  0,
    FT_EINVAL    = -1,
    FT_ESTATE    = -2,
    FT_EIO       = -3,
    FT_ETIMEOUT  = -4,
    FT_ECANCELLED= -5,
    FT_EXCEPTION = -6,
    FT_EUNKNOWN  = -128
} ft_err;
```

### Function-pointer typedefs (exact)

```cpp
using fn_ft_abi_version        = int     (FT_CALL *)();
using fn_ft_free               = /* free helper */;
using fn_ft_job_result_destroy = /* destroy ft_job_result */;
using fn_ft_job_msg_destroy    = /* destroy ft_job_msg */;

using fn_ft_tunnel_create      = ft_err (FT_CALL *)(const char *url, FT_TunnelHandle **out);
using fn_ft_tunnel_retain      = /* retain */;
using fn_ft_tunnel_release     = /* release */;
using fn_ft_tunnel_start_connect = ft_err (FT_CALL *)(FT_TunnelHandle *,
        void (FT_CALL *)(void *user, int ok, int err, const char *msg), void *user);
using fn_ft_tunnel_sync_connect  = ft_err (FT_CALL *)(FT_TunnelHandle *);
using fn_ft_tunnel_set_status_cb = ft_err (FT_CALL *)(FT_TunnelHandle *,
        void (FT_CALL *)(void *user, int old_status, int new_status, int err, const char *msg),
        void *user);
using fn_ft_tunnel_shutdown    = /* shutdown */;

using fn_ft_job_create         = ft_err (FT_CALL *)(const char *params_json, FT_JobHandle **out);
using fn_ft_job_retain         = /* retain */;
using fn_ft_job_release        = /* release */;
using fn_ft_job_set_result_cb  = ft_err (FT_CALL *)(FT_JobHandle *,
        void (FT_CALL *)(void *user, ft_job_result result), void *user);
using fn_ft_job_get_result     = ft_err (FT_CALL *)(FT_JobHandle *, uint32_t timeout_ms,
        ft_job_result *out_result);
using fn_ft_tunnel_start_job   = ft_err (FT_CALL *)(FT_TunnelHandle *, FT_JobHandle *);
using fn_ft_job_cancel         = /* cancel */;
using fn_ft_job_set_msg_cb     = ft_err (FT_CALL *)(FT_JobHandle *,
        void (FT_CALL *)(void *user, ft_job_msg msg), void *user);
using fn_ft_job_try_get_msg    = /* non-blocking get */;
using fn_ft_job_get_msg        = /* blocking get */;
```

`FileTransferModule` is the aggregate of all 21 of these pointers; wrappers `FileTransferTunnel`
and `FileTransferJob` manage retain/release lifetime and dispatch the status/result/msg callbacks.

### Tunnel/job model

- `ft_tunnel_create(url, &tunnel)` — a **tunnel** is created from a URL (the printer/cloud
  endpoint). `ft_tunnel_sync_connect` (blocking) or `ft_tunnel_start_connect` (async w/ callback)
  establishes it; `ft_tunnel_set_status_cb` reports state transitions
  `(old_status, new_status, err, msg)`.
- `ft_job_create(params_json, &job)` — a **job** is parameterized by a **JSON string** (e.g. an
  upload/download descriptor). `ft_tunnel_start_job(tunnel, job)` runs it on the tunnel.
- Results via `ft_job_get_result(job, timeout_ms, &result)` (poll) or `ft_job_set_result_cb`
  (push); progress/intermediate via `ft_job_set_msg_cb` + `ft_job_try_get_msg`/`ft_job_get_msg`.

This `ft_*` tunnel is the modern replacement for the older FTP-based `start_local_print*`
transfer; it is request/response over a persistent tunnel rather than per-file FTP.

---

## 5. Cloud message framing hints (Studio side)

- **`ft_*` jobs/results/messages are JSON-string framed**: `ft_job_create(const char *params_json,
  ...)`, and both `ft_job_result.json` and `ft_job_msg.json` are `const char *` JSON. There is an
  optional **opaque binary side-channel** in `ft_job_result` (`const void *bin` + `uint32_t
  bin_size`) for raw payloads, but no CBOR/msgpack encoding is named anywhere in the headers — the
  control plane is plain JSON, the data plane is opaque bytes.
- **MQTT control commands** (the actual "print" command after upload) are JSON: `MachineObject`
  uses `publish_json()` / `cloud_publish_json()` / `local_publish_json()` and `parse_json()`.
- **Cloud file transfer** itself is HTTPS PUT to OSS/S3 (hence the curl/TLS dependency behind
  -3030/-3100), with the printer pulling the object via a signed URL handed over in the cloud task
  notification.

No binary/CBOR/msgpack framing is present on the Studio side for control messages; everything
Studio emits is JSON (over MQTT for commands, over the `ft_*` tunnel for transfer jobs).

---

## File path index (all paths in github.com/bambulab/BambuStudio, branch master)

| What | Path |
|------|------|
| PrintParams, PublishParams, BAMBU_NETWORK_ERR_* (incl. -3030 @ ~L95) | `src/slic3r/Utils/bambu_networking.hpp` |
| `func_start_print` etc. typedefs + NetworkAgent methods | `src/slic3r/Utils/NetworkAgent.hpp` |
| `ft_*` ABI, FileTransferModule, ft_job_result/msg, ft_err | `src/slic3r/Utils/FileTransferUtils.hpp` |
| Cloud/LAN send dispatch + error→message mapping | `src/slic3r/GUI/Jobs/PrintJob.cpp` |
| MachineObject, NetworkAgent* m_agent, publish_json | `src/slic3r/GUI/DeviceManager.hpp` |

(Note: `src/slic3r/GUI/Jobs/SendToPrinterJob.cpp` returned 404 on master — that job appears
renamed/relocated; the send flow is fully covered by `PrintJob.cpp`.)
