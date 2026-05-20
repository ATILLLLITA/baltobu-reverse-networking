# Unpacking helpers

Material specific to the dynamic-unpacking session that produces the
analysis dump from `bambu_networking.dll`. The protector's
identification, the rationale, the working flow, and the corrected
runbook are documented in
[`../docs/02-unpacking.md`](../docs/02-unpacking.md). This directory
holds the source for `host_loader.exe`, the small Windows program used
as the dump target.

## `host_loader.c`

A 60-line Windows program that does nothing but `LoadLibrary` the DLL
and sleep. It gives Scylla a tiny process to attach to, without
dragging in the full Bambu Studio application. Build on the Windows
side:

```
:: MSVC
cl /Fe:host_loader.exe host_loader.c

:: MinGW (works cross-compiled from Linux too)
x86_64-w64-mingw32-gcc -o host_loader.exe host_loader.c -lkernel32
```

Drop `host_loader.exe` next to `bambu_networking.dll`. Run it
directly without a debugger; it loads the DLL and parks in
`WaitForSingleObject`. Then launch Scylla, select the process and
module, click *IAT Autosearch*, *Get Imports*, *Dump*.

## Why there is no debugger in the flow

VMProtect 3.x maps a second private copy of `ntdll` into the process
and calls anti-debug syscall stubs through that fresh copy whose stubs
are not hooked by ScyllaHide's user-mode interception. The "a debugger
has been found" warning fires anyway. Defeating this in user mode
requires either TitanHide (kernel) or HyperHide (hypervisor). The
simpler path is to avoid attaching a debugger; Scylla reads target
memory via `OpenProcess` and `ReadProcessMemory` and is not detected.

## Validation

The dump can be validated from a Linux host:

```
python3 scripts/post_unpack_validate.py path/to/bambu_networking_dump.dll
```

Two `WARN` outcomes are expected and explained in
[`../docs/02-unpacking.md`](../docs/02-unpacking.md): the ASLR-relocated
header base and the empty IAT (a consequence of VMProtect
import-protection).

## Re-importing into Ghidra

```
TARGET_BINARY=unpacking/dumps/bambu_networking_dump.dll \
PROJECT_NAME=baltobu-unpacked \
IMAGE_BASE=0x180000000 \
STRIP_PROTECTOR_SECTIONS=1 \
    ghidra/run_headless.sh PostUnpackAnalysis.java
```

Forcing the image base back to `0x180000000` keeps the RVAs in the
documentation aligned with the dump. Stripping the protector sections
(`.o1}`, `.2F;`, `.1M@`) cuts the analysis time from over an hour to
about seven minutes; those sections are the protector's compressed
payload and scratch buffers, not code that needs auto-analysis.
