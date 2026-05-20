# DllMain chain recovery

Recovered statically from the unpacked dump
`unpacking/dumps/bambu_networking_dump.dll`. All addresses are with
the image base forced to `0x180000000`. The MSVC CRT helper names
come from Ghidra's FidDB matches against the Visual Studio 2019
Release runtime library.

## The chain

```
OS loader
   |
   v
OEP wrapper at RVA 0x1b72917 (inside .1M@, VMProtect stub)
   |
   |  (anti-debug, unpack, then transfer)
   v
_DllMainCRTStartup at 0x180565614   (.text, MSVC CRT)
   |
   |  on DLL_PROCESS_ATTACH only:
   +-- __security_init_cookie at 0x180565d18 (stack-cookie seed)
   |
   v
dllmain_dispatch at 0x1805654bc     (.text, MSVC CRT)
   |
   +-- dllmain_raw at 0x1805652d0   (CRT init/teardown reason dispatcher)
   |     |
   |     +-- on DLL_PROCESS_ATTACH (reason==1) calls FUN_180565320
   |     |   (runs _initterm of __xc_a..__xc_z C++ static constructors,
   |     |    __scrt_dllmain_after_initialize_c, etc.)
   |     +-- on DLL_PROCESS_DETACH (reason==0) calls FUN_180565438
   |     |   (runs _initterm of __xt_a..__xt_z C++ destructors,
   |     |    __scrt_uninitialize_crt)
   |     +-- on DLL_THREAD_ATTACH (reason==2) calls FUN_180564a98
   |     +-- on DLL_THREAD_DETACH (reason==3) calls FUN_180564ac0
   |
   +-- USER DllMain at 0x180565dc4  (.text, Bambu Lab code)
         |
         |  reads as: BOOL DllMain(HINSTANCE, DWORD reason, LPVOID) { return TRUE; }
         |  the user-written body returns TRUE unconditionally.
         |
         |  VMProtect has post-processed it to call into the protector
         |  on the first DLL_PROCESS_ATTACH:
         v
   call 0x1812dbbfc                 (.o1}, protector hook)
```

## CRT helpers identified by Ghidra (Visual Studio 2019 Release)

| Symbol                                | VA            |
|---------------------------------------|---------------|
| `__scrt_acquire_startup_lock`         | `0x180564a10` |
| `__scrt_dllmain_after_initialize_c`   | `0x180564a4c` |
| `__scrt_dllmain_exception_filter`     | `0x180564ad8` |
| `__scrt_dllmain_uninitialize_c`       | `0x180564b38` |
| `__scrt_initialize_crt`               | `0x180564b7c` |
| `__scrt_is_nonwritable_in_current_image` | `0x180564c54` |
| `__scrt_release_startup_lock`         | `0x180564cec` |
| `__scrt_uninitialize_crt`             | `0x180564d10` |
| `dllmain_dispatch`                    | `0x1805654bc` |
| `_DllMainCRTStartup`                  | `0x180565614` |
| `__scrt_is_ucrt_dll_in_use`           | `0x180565a40` |
| `__scrt_unhandled_exception_filter`   | `0x180565c50` |

The C++-mangled symbol Ghidra recovered for the dispatcher is
`?dllmain_dispatch@@YAHQEAUHINSTANCE__@@KQEAX@Z`, which demangles to
`int __cdecl dllmain_dispatch(struct HINSTANCE__ * __ptr64 const, unsigned long, void * __ptr64 const)`.

## Decompilation of `_DllMainCRTStartup`

```c
void _DllMainCRTStartup(HINSTANCE__ *param_1, ulong param_2, void *param_3)
{
  if (param_2 == 1) {            // DLL_PROCESS_ATTACH
    FUN_180565d18();             // __security_init_cookie
  }
  dllmain_dispatch(param_1, param_2, param_3);
  return;
}
```

This is the unmodified standard MSVC entry. The OEP wrapper in `.1M@`
ultimately transfers control here after VMProtect's setup completes.

## Decompilation of `dllmain_dispatch`

```c
int __cdecl dllmain_dispatch(HINSTANCE__ *hinst, ulong reason, void *reserved)
{
  if ((reason == 0) && (g_init_flag < 1)) return 0;     // detach without prior attach: bail
  if (reason - 1 < 2) {                                 // attach or thread-attach
    if (_guard_dispatch_icall(...) == 0) return 0;      // CFG-guarded call (CRT init)
    if (dllmain_raw(hinst, reason, reserved) == 0)      // CRT init: _initterm, C++ ctors
      return 0;
  }
  int r = user_DllMain(hinst, reason, reserved);        // FUN_180565dc4
  if ((reason == 1) && (r == 0)) {
    user_DllMain(hinst, 0, reserved);                   // attach-failed rollback
    FUN_180565438(reserved != NULL);                    // CRT teardown
    _guard_dispatch_icall(hinst, 0, reserved);
  }
  if ((reason == 0) || (reason == 3)) {                 // detach or thread-detach
    if (dllmain_raw(hinst, reason, reserved) != 0)
      r = _guard_dispatch_icall(hinst, reason, reserved);
    else
      r = 0;
  }
  return r;
}
```

This is the standard MSVC `dllmain_dispatch` template, unmodified.
The pattern is the canonical attach/detach lifecycle with rollback on
attach failure.

## Decompilation of `dllmain_raw` (`FUN_1805652d0`)

```c
ulonglong dllmain_raw(undefined1 hinst, int reason, longlong reserved)
{
  if (reason == 0) {                          // DLL_PROCESS_DETACH
    return FUN_180565438(reserved != 0);      // CRT teardown
  }
  if (reason != 1) {                          // not DLL_PROCESS_ATTACH
    if (reason == 2) return FUN_180564a98();  // DLL_THREAD_ATTACH (dyn TLS init)
    if (reason != 3) return 1;
    return FUN_180564ac0();                   // DLL_THREAD_DETACH (dyn TLS dtor)
  }
  return FUN_180565320(hinst, reserved);      // DLL_PROCESS_ATTACH (CRT init)
}
```

`dllmain_raw` is the CRT's reason-dispatched setup/teardown. Not Bambu's
code; standard MSVC emission. The reason-table maps each lifecycle event
to a CRT helper.

## The user-written DllMain

`user_DllMain` lives at `0x180565dc4` and is **36 bytes including the
`int3` padding byte**. Decompiled by Ghidra to:

```c
undefined8 user_DllMain(void)
{
  return 1;
}
```

Manual instruction-by-instruction decode of the raw bytes
`48 83 ec 28 83 fa 01 75 10 48 83 3d 1b d3 15 00 00 75 06 e8 20 5e d7 00 78 b8 01 00 00 00 48 83 c4 28 c3 cc`:

| Offset | Bytes                         | Disassembly                                                 |
|-------:|-------------------------------|-------------------------------------------------------------|
| `+0x00`| `48 83 EC 28`                 | `sub rsp, 0x28`                                             |
| `+0x04`| `83 FA 01`                    | `cmp edx, 1`         (reason == DLL_PROCESS_ATTACH?)        |
| `+0x07`| `75 10`                       | `jne RET_1` (offset 0x19, the `mov eax, 1` block)           |
| `+0x09`| `48 83 3D 1B D3 15 00 00`     | `cmp qword [rip+0x15D31B], 0`  (`.rdata` flag at `0x1806C30F0`) |
| `+0x11`| `75 06`                       | `jne RET_1` (offset 0x19)                                   |
| `+0x13`| `E8 20 5E D7 00`              | `call 0x1812DBBFC`   (call into `.o1}` protector hook)      |
| `+0x18`| `78 B8`                       | `js -0x48` (anti-disassembly bait; not part of the live path) |
| `+0x1A`| `01 00 00 00`                 | junk bytes (covered by the JS that never lands)             |
| `+0x19`| `B8 01 00 00 00`              | `mov eax, 1`         (the RET_1 entry, reached by the JNEs) |
| `+0x1E`| `48 83 C4 28`                 | `add rsp, 0x28`                                             |
| `+0x22`| `C3`                          | `ret`                                                       |
| `+0x23`| `CC`                          | `int3`               (alignment padding)                    |

The bytes at `+0x18` (`78 B8 01 00 00 00`) are overlapping with the
real RET_1 entry at `+0x19` (`B8 01 00 00 00`). The JNE branches at
`+0x07` and `+0x11` jump to `+0x19`, the second byte of the spurious
JS instruction. This is the classic overlapping-instruction trick
that VMProtect inserts to confuse linear-sweep disassemblers; Ghidra's
recursive-descent analysis follows the correct path, which is why its
simplified decompilation reads as `return 1`.

Reading the live path only:

```c
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
    if (reason == DLL_PROCESS_ATTACH && !g_protector_hook_done) {
        vmp_runtime_hook();   // call rel32 into 0x1812DBBFC inside .o1}
    }
    return TRUE;
}
```

The flag at `0x1806C30F0` is the protector's "have I already done my
first-attach setup" guard. Once set to non-zero by the protector hook,
subsequent attach events skip the call.

## Implication for the clean-room implementation

Bambu Lab's source-level `DllMain` is a no-op that returns `TRUE`. All
library initialisation happens through the explicit C ABI exports:

```c
bambu_network_create_agent();    // construct the singleton
bambu_network_set_config_dir(...);
bambu_network_set_country_code(...);
bambu_network_set_cert_file(...);
bambu_network_set_queue_on_main_fn(...);
... // register every callback
bambu_network_init_log(...);
bambu_network_start(...);        // spawn workers
```

A clean-room implementation therefore needs:

```c
BOOL WINAPI DllMain(HINSTANCE hinst, DWORD reason, LPVOID reserved)
{
    return TRUE;
}
```

The `call 0x1812DBBFC` into `.o1}` is VMProtect's runtime hook for
its own anti-debug or licence-check setup. It is part of the
protector's instrumentation, not Bambu's design, and must not be
replicated in the clean-room library.

## How this was recovered

1. Inspected the OEP wrapper bytes at RVA `0x1b72917` in the dump.
   The wrapper is in `.1M@` (the protector section), not `.text`. It
   begins with `41 54 9C` (`push r12; pushfq`) followed by VMProtect
   junk and ends in `halt_baddata`. Following its tail calls is not
   tractable statically.
2. Queried the symbol table for any name containing the keywords
   `DllMain`, `CRTStartup`, `_scrt_`. Ghidra's FidDB had labelled
   `_DllMainCRTStartup`, `dllmain_dispatch`, and the full
   `__scrt_*` helper chain from a Visual Studio 2019 Release library
   signature match.
3. Decompiled `_DllMainCRTStartup` and walked the call graph to
   `dllmain_dispatch`, then to the two non-CRT callees.
4. Confirmed via cross-reference: `FUN_180565dc4` is called from
   exactly two callsites, both inside `dllmain_dispatch` (one for
   attach, one for detach). This is the `dllmain_raw` user-DllMain
   slot in MSVC's dispatcher template.
5. Manually decoded the 36-byte body, observing the overlapping-
   instruction trick that hides the call into `.o1}`.

The helper Ghidra script for the symbol-table query is committed at
`ghidra/scripts/FindByName.java`; the xref lister is at
`ghidra/scripts/ListXrefs.java`. Both are reusable for future tracing.
