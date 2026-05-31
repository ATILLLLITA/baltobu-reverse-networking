# Bambu Networking SDK — `ft_*` (File-Transfer "ft" tunnel) C ABI

Static reverse-engineering of the 21 `ft_*` C exports. **No network / cloud / printer
interaction was performed** — analysis is purely on-disk against:

- **Windows** `bnet.bin` — flat in-memory image of `bambu_networking.dll`
  **v02.06.00.50** (VMProtect already decrypted in RAM).
  Live module base `0x7FF9E9E70000`; file is a valid PE mapped at its virtual
  addresses, so **file offset == RVA** (verified: `MZ` @0 / `PE\0\0` @0x80 / ImageBase
  field = `0x7FF9E9E70000`). This is the authoritative source for the signatures below.
- **macOS** `libbambu_networking.dylib` (brtc-2026-05-16 and mac_01.07.01.04) — confirms the
  same 21 exports as **plain C symbols** (`_ft_*`, no C++ mangling). The dylib carries no
  extra type info (implementation classes are stripped), so it does not refine the prototypes.

All 21 export names live contiguously in the PE export-name table at RVA
`0x1ad805a … 0x1ad81b9`, immediately after the `bambu_network_*` block (alphabetical).
Export directory @ RVA `0x1ad6cf8` (OrdinalBase 1, 133 functions). Name→ordinal map is
identity; function RVAs taken from the EAT. No secrets present; nothing to redact.

## Confidence legend
- **(R)** firmly recovered — arg registers / return constants read directly from the disassembly.
- **(I)** inferred — argument *count* is firm from register usage; the *role/type* of a pointer
  (e.g. which side is in vs out, exact struct type) is deduced from the refcounted-handle
  C-API idiom, the store/copy pattern, and baltobu's cmd_type/struct docs.

## Handle / refcount model

`ft_tunnel_t` and `ft_job_t` are **opaque, intrusively ref-counted handles** (pointer to a
C++ object whose **first dword is an atomic refcount**). Evidence:

- `ft_*_retain`  : `test rcx,rcx; jz; mov eax,1; lock xadd [rcx],eax; ret` — atomic inc, null-safe.
- `ft_*_release` : `test rcx,rcx; jz; ... lock xadd ...` then, when the count hits the floor,
  releases the held sub-objects (member callbacks/strings/buffers via their own
  retain/release vtable slots `+8`/`+0x10`) and frees the object.
- `ft_*_create`  : takes an input descriptor + an **out-handle pointer**, allocates the object,
  stores it through the out pointer, returns 0/neg.

Return-code convention (recovered from immediates):
`0` = success; `-1` (`0xFFFFFFFF`, via `lea eax,[rcx-1]`) = **null/invalid handle**;
`-2` (`0xFFFFFFFE`) = e.g. **empty/would-block** (`ft_job_try_get_msg`);
`-4` (`0xFFFFFFFC`) / `-6` (`0xFFFFFFFA`) = other tunnel/job error states.
Return type is `int32_t` for everything except the `void` retain/release/destroy/free helpers.

Suggested opaque typedefs:
```c
typedef struct ft_tunnel* ft_tunnel_t;   /* refcounted; 1st dword = atomic refcount */
typedef struct ft_job*    ft_job_t;       /* refcounted; 1st dword = atomic refcount */
typedef struct ft_msg     ft_msg_t;       /* 16-byte POD copied out; has heap buf @ +8 */
typedef struct ft_result  ft_result_t;    /* has heap buf @ +8 freed by *_result_destroy */
```

## Signature table

| # | Export | RVA | Recovered C signature | Conf. | Evidence |
|---|--------|-----|-----------------------|-------|----------|
| 1 | `ft_abi_version`        | `0x24de00` | `int ft_abi_version(void)` | **R** | body = `mov eax,1; ret` (returns ABI ver **1**) |
| 2 | `ft_free`               | `0x24de10` | `void ft_free(void* ptr)` | **R** | `jmp [import free]` tail-call; frees `rcx` |
| 3 | `ft_tunnel_create`      | `0x24e910` | `int ft_tunnel_create(const void* cfg /*rcx*/, ft_tunnel_t* out /*rdx*/)` | R(args)/I(roles) | tests both rcx,rdx≠NULL; `mov [rdx],created` |
| 4 | `ft_tunnel_retain`      | `0x24ed80` | `void ft_tunnel_retain(ft_tunnel_t t)` | **R** | `mov eax,1; lock xadd [rcx]` null-safe |
| 5 | `ft_tunnel_release`     | `0x24ec90` | `void ft_tunnel_release(ft_tunnel_t t)` | **R** | atomic dec + member teardown (+0x10/+0x20/+0x30) |
| 6 | `ft_tunnel_shutdown`    | `0x24ef40` | `int ft_tunnel_shutdown(ft_tunnel_t t)` | **R** | 6-byte stub `mov eax,-1; ret` (returns -1 in this build) |
| 7 | `ft_tunnel_set_status_cb` | `0x24ed90` | `int ft_tunnel_set_status_cb(ft_tunnel_t t /*rcx*/, ft_status_cb cb /*rdx*/, void* user /*r8*/)` | R(args)/I(types) | `mov rsi,rdx; mov rdi,r8`; stores cb+user; null→-1 |
| 8 | `ft_tunnel_start_connect` | `0x24ef50` | `int ft_tunnel_start_connect(ft_tunnel_t t /*rcx*/, ft_connect_cb cb /*rdx*/, void* user /*r8*/)` | R(args)/I(types) | same 3-arg shape as set_status_cb; allocs closure {rdx,r8}; async |
| 9 | `ft_tunnel_sync_connect` | `0x24f6d0` | `int ft_tunnel_sync_connect(ft_tunnel_t t)` | **R** | 1 arg; calls `[t+8]` then checks state `[obj+0x290]==3` → 0, else -4/-6 |
| 10 | `ft_tunnel_start_job`   | `0x24f0d0` | `int ft_tunnel_start_job(ft_tunnel_t t /*rcx*/, ft_job_t job /*rdx*/)` | R(args)/I(types) | tests rcx,rdx≠NULL; reads job fields `[rdx+..]`, submits |
| 11 | `ft_job_create`         | `0x24df40` | `int ft_job_create(const void* desc /*rcx*/, ft_job_t* out /*rdx*/)` | R(args)/I(roles) | tests both≠NULL; switch on type byte (jump table) builds job; `mov [rdx],job` |
| 12 | `ft_job_retain`         | `0x24e800` | `void ft_job_retain(ft_job_t job)` | **R** | `mov eax,1; lock xadd [rcx]` null-safe |
| 13 | `ft_job_release`        | `0x24e700` | `void ft_job_release(ft_job_t job)` | **R** | atomic dec; releases members @ +0x100/+0x108/+0x110 |
| 14 | `ft_job_cancel`         | `0x24de20` | `int ft_job_cancel(ft_job_t job)` | **R** | 1 arg; null→-1; builds cancel msg, submits; →0 |
| 15 | `ft_job_get_msg`        | `0x24e3b0` | `int ft_job_get_msg(ft_job_t job /*rcx*/, uint32_t timeout_ms /*edx*/, ft_msg_t* out /*r8*/)` | R(args)/I(timeout) | `mov rdi,r8; mov esi,edx; mov rbx,rcx`; **blocking** get, copies 16B to out |
| 16 | `ft_job_try_get_msg`    | `0x24e870` | `int ft_job_try_get_msg(ft_job_t job /*rcx*/, ft_msg_t* out /*rdx*/)` | **R** | **2 args** (no timeout); `mov rdi,rdx`; →0 / -2 (empty) / -6 |
| 17 | `ft_job_get_result`     | `0x24e480` | `int ft_job_get_result(ft_job_t job /*rcx*/, uint32_t timeout_ms /*edx*/, ft_result_t* out /*r8*/)` | R(args)/I(timeout) | `mov r12,r8`; waits on state `[job+0x40+0xb8]`, copies result to out |
| 18 | `ft_job_set_msg_cb`     | `0x24e810` | `int ft_job_set_msg_cb(ft_job_t job /*rcx*/, ft_msg_cb cb /*rdx*/, void* user /*r8*/)` | **R** | `mov [job+0xe8],rdx; mov [job+0xf0],r8`; null→-1, →0 |
| 19 | `ft_job_set_result_cb`  | `0x24e840` | `int ft_job_set_result_cb(ft_job_t job /*rcx*/, ft_result_cb cb /*rdx*/, void* user /*r8*/)` | **R** | `mov [job+0xd8],rdx; mov [job+0xe0],r8`; null→-1, →0 |
| 20 | `ft_job_msg_destroy`    | `0x24e6d0` | `void ft_job_msg_destroy(ft_msg_t* msg)` | **R** | null-safe; frees heap buf `[msg+8]`, nulls it |
| 21 | `ft_job_result_destroy` | `0x24e7c0` | `void ft_job_result_destroy(ft_result_t* result)` | **R** | null-safe; frees `[result+8]`, zeroes +8/+0x10/+0x18 |

## Callback signatures

All three callbacks are stored as `(fn_ptr, void* user)` pairs inside the handle and follow
the conventional `(owner_handle, payload*, void* user)` C idiom. The **fn-ptr + user-ctx
slot pairs are firmly recovered**; the exact payload struct passed is inferred from how the
slots are read at the (async) call sites and from baltobu's struct docs.

```c
/* Tunnel status / connect-completion callbacks.
   Stored by ft_tunnel_set_status_cb (cb,user) and ft_tunnel_start_connect (cb,user). */
typedef void (*ft_status_cb )(ft_tunnel_t tunnel, int status /*state enum*/, void* user);  /* (I) */
typedef void (*ft_connect_cb)(ft_tunnel_t tunnel, int result /*0=ok*/,      void* user);  /* (I) */

/* Job message / result callbacks.
   Stored at job+0xE8/+0xF0 (msg cb,user) and job+0xD8/+0xE0 (result cb,user). */
typedef void (*ft_msg_cb   )(ft_job_t job, const ft_msg_t*    msg,    void* user);  /* (I) */
typedef void (*ft_result_cb)(ft_job_t job, const ft_result_t* result, void* user);  /* (I) */
```

Firm facts about the callbacks (R):
- `set_status_cb`, `start_connect`, `set_msg_cb`, `set_result_cb` each take exactly
  **3 args**: `(handle = rcx, fn_ptr = rdx, void* user = r8)`.
- The handle keeps the `user` pointer adjacent to the fn-ptr (offsets above) and invokes
  `fn_ptr(...)` with the saved `user` as the trailing argument.
- The first callback parameter being the owning handle, and the middle being a pointer to
  the freshly produced message/result POD, is the **(I)** part — consistent with
  `ft_job_get_msg`/`ft_job_get_result` copying a 16-byte / result POD out by pointer.

## Notes / corroboration
- `ft_msg_t` is a small (≈16-byte) POD copied by value out of `ft_job_get_msg` /
  `ft_job_try_get_msg` (`movups`), with an owned heap buffer at `+8` that
  `ft_job_msg_destroy` frees — matches a `{type/len, char* data}` message shape.
- `ft_result_t` similarly owns a heap buffer at `+8` (freed by `ft_job_result_destroy`,
  which also clears `+0x10/+0x18`) — a `{code, char* data, size_t len}`-style result.
- `ft_tunnel_shutdown` is a non-functional stub (`return -1`) in v02.06.00.50; teardown is
  driven entirely through `ft_tunnel_release`.
- macOS dylibs (brtc-2026-05-16 + mac_01.07.01.04) export the identical 21 plain-C `_ft_*`
  symbols, confirming the surface is stable across Windows and macOS builds.

_Method: PE export table parsed from the flat image (`od`), 21 EAT RVAs resolved, each
function disassembled by hand from raw byte dumps (objdump/python were unavailable in the
sandbox; x86-64 decode done manually). All offsets are file-offset == RVA in `bnet.bin`._
