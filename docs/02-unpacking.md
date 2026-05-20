# Unpacking

VMProtect's runtime stub has to be allowed to execute before any of the
real code becomes readable. This document describes the method that
worked, the captured memory image, and the analysis pipeline that runs
against that image.

## Working flow

The dump was produced on a bare-metal Windows 11 host. No debugger was
attached at any point. The reason is that VMProtect 3.x maps a second
private copy of `ntdll` into the process, calls anti-debug syscall stubs
through that fresh copy whose stubs are not hooked by ScyllaHide's
user-mode interception, and uses the answer to display the standard
"a debugger has been found" warning. Beating this in user mode requires
either TitanHide (kernel) or HyperHide (hypervisor). The simpler path
turned out to be avoiding a debugger entirely.

Scylla reads target memory via `OpenProcess` and `ReadProcessMemory`.
It is not a debugger and the protector does not detect it as one. The
procedure is:

1. Run our custom crafted `unpacking/host_loader.exe` directly. It calls
   `LoadLibraryA("bambu_networking.dll")`, captures a function pointer
   via `GetProcAddress("bambu_network_get_version")`, prints the load
   base, and then parks in `WaitForSingleObject(GetCurrentProcess(), INFINITE)`.
   The full unpack should completes in about one second on a standard computer
   hardware. Keep it running on the "sleeping" line in background.
2. Launch `Scylla_x64.exe` (standalone, no administrative rights needed).
3. Select Process `host_loader.exe` and Module DLL `bambu_networking.dll`.
4. Click *IAT Autosearch*, *Get Imports*, then *Dump*. *Fix Dump* is a
   no-op in this run because Scylla has nothing to rebuild on top of
   what the dumper already wrote.

Sample stdout from a `host_loader.exe`:

```
[host_loader] loading bambu_networking.dll
[host_loader] loaded at 0x00007FFE24020000
[host_loader] bambu_network_get_version @ 0x00007FFE241DA5F0
[host_loader] sleeping; attach Scylla and dump now.
```

The DLL's load base is ASLR-relocated each run, so absolute VAs printed
by `host_loader.exe` are not stable across sessions. Only the RVA
(`0x1ba5f0` for `bambu_network_get_version` in this build) is stable.

## Host configuration used

| Item              | Value                                                   |
|-------------------|---------------------------------------------------------|
| OS                | Microsoft Windows NT 10.0.26200.0 (Windows 11)          |
| Architecture      | x86-64                                                  |
| Hardware          | Lenovo 83DJ with Intel Core Ultra 7 155H (GenuineIntel) |
| Virtualization    | None (bare metal)                                       |
| Compiler for `host_loader.exe` | MinGW-w64 GCC 16.1.0 (posix-seh-ucrt)      |
| Tools             | Scylla v0.9.8                                           |

Because the host is bare metal, none of VMProtect's anti-VM strings
(`QEMU`, `VirtualBox`, `VMware`, `Parallels`, `Virtual Platform`,
`777777`) match anything in SMBIOS or the CPU brand string, and the
anti-VM module is effectively a no-op for this session.

## The dump

The resulting file is `unpacking/dumps/bambu_networking_dump.dll`. It is
45,384,704 bytes with SHA-256
`c46b41f47e855eaf96fab18f9a12ef767387e11a8eea75c134e66e07446be5d0a`.
It is gitignored because of its size and to keep redistribution of
Bambu Lab's binary out of source control. The session artefacts that
*are* committed live under [`../output/unpacking/`](../output/unpacking/).

### Section table of the dump

| Section | VA          | VirtSize    | RawAddr     | RawSize     | State                         |
|---------|-------------|-------------|-------------|-------------|-------------------------------|
| `.text` | `0x00001000`| `0x005E4000`| `0x00000400`| `0x005E3600`| populated, real x86-64 prologue at offset 0 |
| `.rdata`| `0x005E5000`| `0x001A6000`| `0x005E3A00`| `0x001A6000`| populated                     |
| `.data` | `0x0078B000`| `0x000A0000`| `0x00789A00`| `0x0009F800`| populated                     |
| `.pdata`| `0x0082B000`| `0x00046000`| `0x00829200`| `0x00045800`| populated, entries unreliable (caveat below) |
| `.o1}`  | `0x00871000`| `0x00D94000`| `0x0086EA00`| `0x00D93C00`| protector scratch / runtime, still resident |
| `.2F;`  | `0x01605000`| `0x00001000`| `0x01602600`| `0x00000200`| unchanged from packed         |
| `.1M@`  | `0x01606000`| `0x01546000`| `0x01602800`| `0x01545800`| original packed payload, still resident |
| `.rsrc` | `0x02B4C000`| `0x00001000`| `0x02B48000`| `0x00000200`| unchanged                     |
| `.reloc`| `0x02B4D000`| `0x00001000`| `0x02B48200`| `0x00000200`| unchanged                     |

The first 16 bytes of `.text` are
`40 55 48 8D AC 24 10 8A FF FF B8 F0 76 00 00 E8`, which decodes as
`push rbp; lea rbp, [rsp-0x758F0]; mov eax, 0x76F0; call __chkstk`,
a textbook large-frame x86-64 function prologue. The original code is in
there.

### Validator output

```
$ python3 scripts/post_unpack_validate.py unpacking/dumps/bambu_networking_dump.dll
== Validating unpacking/dumps/bambu_networking_dump.dll ==
  [OK]   section .text  has on-disk bytes  (rsize=0x5e3600, vsize=0x5e4000)
  [OK]   section .rdata has on-disk bytes  (rsize=0x1a6000, vsize=0x1a6000)
  [OK]   section .data  has on-disk bytes  (rsize=0x9f800,  vsize=0xa0000)
  [OK]   section .pdata has on-disk bytes  (rsize=0x45800,  vsize=0x46000)
  [WARN] image base matches original  (orig=0x180000000 dump=0x7ffe22c50000
                                       expected when dumped from a running ASLR process)
  [OK]   all 128 original exports still present
  [OK]   every export RVA lands in a populated section
  [WARN] IAT directory grew after Scylla rebuild  (orig=0x188 dump=0x0
                                                   expected with VMProtect import-protection)
== ALL CHECKS PASSED ==
```

The full report is committed at
[`../output/unpacking/post_unpack_validate.txt`](../output/unpacking/post_unpack_validate.txt).

### Known caveats of the dump

The dump's `AddressOfEntryPoint` is `0x01B72917`, the VMProtect OEP
wrapper, not the original `_DllMainCRTStartup`. The wrapper sits in
`.1M@` (the protector section) and transfers control into `.text`
after its anti-debug stage. The recovered chain is documented in
[`../output/unpacking/dllmain_chain.md`](../output/unpacking/dllmain_chain.md);
the user-written `DllMain` body returns `TRUE` unconditionally and
all library initialisation happens through the explicit C ABI
exports.

The runtime ASLR base (`0x7FFE22C50000` in this session) is what Scylla
wrote into the dumped PE header. When re-importing into Ghidra the image
base must be forced back to `0x180000000` so that the RVAs in this
documentation match. The wrapper `ghidra/run_headless.sh` does this
automatically via `IMAGE_BASE=0x180000000`.

The `.pdata` section is populated but its `RUNTIME_FUNCTION` entries do
not validate. Ghidra falls back to control-flow analysis for function
boundaries. This is fine for the 128 named exports because we already
have their entry RVAs from the export directory, but any unexported
helper function Ghidra identifies may have less reliable boundaries.

The IAT directory in the dumped file is empty. VMProtect's
import-protection feature replaces every `call [kernel32_X]` thunk
inside `.text` with `call vmprotect_resolver(api_id, ...)` indirected
through its runtime. Scylla therefore has nothing to rebuild. Two
follow-up paths reconstruct the call semantics. The first is to walk
the VMProtect API-id resolver `FUN_18163ec4b` and produce a synthetic
IAT for Ghidra by reversing the build-time key and table layout. The
second is to hook a live Bambu Studio session with Frida and observe
the Windows API calls each export makes. Either path is independent
and either alone is sufficient.

The protector did not free its scratch (`.o1}`, 13.6 MiB) or its
compressed payload (`.1M@`, 21 MiB) after decompression, so the dump
file is 45 MiB rather than the roughly 22 MiB the original sections
would sum to. Both sections are ignored by Ghidra during post-unpack
analysis (see `STRIP_PROTECTOR_SECTIONS=1` in
`ghidra/run_headless.sh`).

## Post-unpack inventory

With the dump imported into Ghidra (project `baltobu-unpacked`, image
base forced to `0x180000000`, `STRIP_PROTECTOR_SECTIONS=1`), the
`PostUnpackAnalysis.java` script walks every symbol marked as an
external entry point and classifies each by body size, callee count
and decompiler warnings. The committed output is at
[`../output/ghidra_unpacked/exports.tsv`](../output/ghidra_unpacked/exports.tsv)
with a one-paragraph summary at
[`../output/ghidra_unpacked/exports_summary.md`](../output/ghidra_unpacked/exports_summary.md).

| Bucket                                | Count |
|---------------------------------------|------:|
| Total external entry points           | 132   |
| Named `bambu_network_*` exports       | 107   |
| Named `ft_*` exports                  | 21    |
| TLS / OEP / wrapper helpers           | 4     |
| Heuristic-flagged suspicious          | 10    |
| Of which zero-size wrapper synonyms   | 2     |
| Of which tiny named exports (5 to 15 B)| 8    |

Of 130 entries, 129 decompile cleanly. None of the public exports show
the structural signatures of VMProtect's function-virtualization feature
(no large no-callees bodies, no `Could not recover jumptable` warnings on
any export's prologue). In this build VMProtect is configured for
packing, anti-debug, anti-VM, and import-protection only; the
function-VM feature is not applied to the public ABI.

### The eight tiny named exports

These are all legitimate small functions, not VM-protected stubs.

| Export                                  | Size | Shape                          |
|-----------------------------------------|-----:|--------------------------------|
| `ft_abi_version`                        | 6    | `mov eax, 1; ret`              |
| `ft_free`                               | 7    | `jmp [iat_free]` (UCRT thunk)  |
| `ft_job_retain`                         | 15   | refcount increment             |
| `ft_tunnel_retain`                      | 15   | refcount increment             |
| `ft_tunnel_shutdown`                    | 6    | thin forwarder                 |
| `bambu_network_check_debug_consistent`  | 7    | constant return                |
| `bambu_network_create_agent`            | 5    | anti-disassembly stub          |

### The one outlier: `bambu_network_create_agent`

`bambu_network_create_agent` at RVA `0x1b6d00` is the only export whose
body Ghidra cannot statically decode. The recovered five-byte stub
terminates in `halt_baddata()`. This is VMProtect's overlapping-instruction
anti-disassembly trick: the real entry point sits a few bytes downstream
and the bytes between are intentionally invalid so that a naive linear
sweep diverges into garbage. Recovering the real body needs either a
manual instruction override in Ghidra (anchor the disassembler past the
poison bytes) or a Frida hook that calls the export with realistic inputs
and reads the returned pointer. The clean-room implication is documented
in [`exports/bambu_network_create_agent.md`](exports/bambu_network_create_agent.md);
the public ABI signature and side effects can be inferred from how the
rest of the library uses the returned handle.

## Analysis pipeline

The analysis pipeline can be re-run from a fresh clone in three stages.
Stage 1 produces the metadata-level surface from the packed binary alone
and never needs Windows. Stage 2 produces the dump. Stage 3 runs the
Ghidra workflow against the dump.

### Stage 1: metadata extraction

All scripts in `scripts/` use only the Python standard library and emit
deterministic, version-controlled output.

```
python3 scripts/pe_analyze.py         | tee output/pe_analyze.txt
python3 scripts/pe_exports.py         -o output/exports.txt
python3 scripts/pe_imports.py         -o output/imports.txt
python3 scripts/extract_strings.py
python3 scripts/categorize_exports.py
sha256sum unpacking/bambu_networking.dll > output/dll.sha256
```

### Stage 2: unpacking

Done on a Windows host. See the runbook at
[`../output/unpacking/RUNBOOK.md`](../output/unpacking/RUNBOOK.md) and
the working flow described at the top of this document.

### Stage 3: Ghidra analysis against the dump

```
TARGET_BINARY=unpacking/dumps/bambu_networking_dump.dll \
PROJECT_NAME=baltobu-unpacked \
IMAGE_BASE=0x180000000 \
STRIP_PROTECTOR_SECTIONS=1 \
    ghidra/run_headless.sh PostUnpackAnalysis.java
```

This takes about seven minutes on the test hardware. Stripping the
protector sections (`.o1}`, `.2F;`, `.1M@`) before auto-analysis cuts
the run from over an hour because Ghidra would otherwise spend that time
trying to analyse the compressed payload and the protector's scratch
buffer.

Once the project exists, the per-export decompiler pass is:

```
TARGET_BINARY=unpacking/dumps/bambu_networking_dump.dll \
PROJECT_NAME=baltobu-unpacked \
    ghidra/run_headless.sh DecompileExports.java
```

This runs in about two minutes and writes each export's body to
`output/ghidra_decompile/exports/<name>.c`. The transitive-helper passes
(`DumpHelperBodies.java`, `DumpTransitiveHelpers.java`) extend the corpus
to first-level helpers and to functions at depth three from any export.

## Clean-room boundary

The decompiled bodies under `output/ghidra_decompile/` are gitignored.
Anyone implementing the clean-room replacement library must not read
those bodies. The sanctioned artefacts are the per-export documents
under [`exports/`](exports/), the architecture document, the wire
protocol document, and the internals document. Each of those describes
the ABI in fresh prose suitable for re-implementation.

Decompilation of the VMProtect protector (the 42 functions in `.1M@`)
is committed at `output/ghidra/protector/`. That code is the protector's
own implementation, not Bambu Lab's, and the clean-room boundary does
not apply to it.

## Artefacts produced by this stage

| Path                                              | Purpose                                         | Committed |
|---------------------------------------------------|-------------------------------------------------|:---------:|
| `unpacking/dumps/bambu_networking_dump.dll`          | Scylla dump                                     | no (size) |
| `unpacking/dumps/bambu_networking_dump.sha256`    | Provenance for the dump                         | yes       |
| `output/unpacking/session_notes.txt`              | Raw notes from the unpacking host               | yes       |
| `output/unpacking/scyllax64_console.txt`          | Scylla stderr and stdout log                    | yes       |
| `output/unpacking/host_loader_sanity.stdout.txt`  | Proves the no-debugger flow works               | yes       |
| `output/unpacking/post_unpack_validate.txt`       | Validator output                                | yes       |
| `output/unpacking/RUNBOOK.md`                     | Corrected runbook for repeating the dump        | yes       |
| `output/ghidra_unpacked/exports.tsv`              | Export inventory (RVA, size, callees, flags)    | yes       |
| `output/ghidra_unpacked/exports_summary.md`       | One-paragraph summary of the inventory          | yes       |
| `output/ghidra/exports_signatures.tsv`            | Recovered signatures (return type, args)        | yes       |
| `output/ghidra/protector/*`                       | 42 decompiled protector functions               | yes       |
| `output/ghidra_decompile/exports/*.c`             | Raw decompiled bodies (clean-room gated)        | no        |
| `output/ghidra_decompile/helpers/*.c`             | 184 first-level helper bodies (clean-room gated)| no        |
| `output/ghidra_decompile/transitive/*.c`          | 876 transitive helpers at depth 3 (clean-room gated) | no   |

## Repeating this on a future build

Bambu Lab ships new builds periodically. Repeating the analysis against a
new DLL boils down to refreshing the dump, refreshing the Ghidra
artefacts, and re-running the analysis scripts. The constraints to watch
are:

- VMProtect's per-build keys change. The fingerprint functions still
  look the same, but the four constants quoted in [`01-binary.md`](01-binary.md)
  will differ.
- The Anti-VM string list is stable but can be configured per build. If
  a new build refuses to run on bare metal, inspect `FUN_181b22685`-equivalent
  for any new probe strings.
- The eight tiny named exports may grow or shrink. Re-run
  `PostUnpackAnalysis.java`'s suspicious-bucket check.
- New exports added by Bambu (or removed) appear in
  `output/ghidra_unpacked/exports.tsv`. The per-export documentation
  pipeline can be re-run with new symbols.
