# The binary

This document describes `bambu_networking.dll` as it exists on disk and as it
exists after the VMProtect 3.x runtime stub has finished unpacking it in
memory. It is the starting point for any future reader who wants to know
what kind of file they are looking at, what protection it carries, and what
mechanisms had to be defeated before its contents became readable.

## Provenance

| Field             | Value                                            |
|-------------------|--------------------------------------------------|
| File              | `unpacking/bambu_networking.dll`                 |
| Size              | 22,320,368 bytes (21.3 MiB)                      |
| SHA-256           | recorded in `output/dll.sha256`                  |
| PE timestamp      | 2026-04-17 07:24:10 UTC                          |
| Library version   | `02.06.00.50` (from `bambu_network_get_version`) |
| Code signer       | Shanghai Lunkuo Technology Co., Ltd, EV cert     |
| Signature chain   | GlobalSign GCC R45 EV CodeSigning CA 2020, under GlobalSign Root CA |
| Source            | Bambu Studio Windows installer (vendored)        |

The Authenticode signature is intact and validates against the OS trust
store. This is a genuine, untampered Bambu Lab build. The Shanghai Lunkuo
Technology entity is Bambu Lab's legal corporate body in Shanghai.

The matching Linux `libbambu_networking.so` and macOS
`libbambu_networking.dylib` have not yet been analyzed but the public ABI
should match across the three operating systems.

## PE structure

All numbers below come from `scripts/pe_analyze.py`. The archived run is
at `output/pe_analyze.txt`.

| Field             | Value                                                  |
|-------------------|--------------------------------------------------------|
| Machine           | `0x8664` (x86-64)                                      |
| Magic             | `0x20b` (PE32+)                                        |
| Image base        | `0x180000000`                                          |
| Entry point RVA   | `0x1b72917` (VA `0x181b72917`)                         |
| Section alignment | `0x1000` (4 KiB pages)                                 |
| File alignment    | `0x200` (512 bytes)                                    |
| Size of image     | `0x2b4e000` (about 43 MiB in memory)                   |
| File size on disk | `0x154e000` (21.3 MiB)                                 |
| Characteristics   | `EXECUTABLE_IMAGE \| LARGE_ADDRESS_AWARE \| DLL`       |

The image roughly doubles when loaded, consistent with a LZMA packed binary
that materialises bytes into originally empty sections.

### Data directories

Only the populated directories are listed.

| Directory  | RVA          | Size      | Note                                  |
|------------|--------------|-----------|---------------------------------------|
| Export     | `0x02ab2c50` | `0x140a`  | 128 named exports, the public ABI     |
| Import     | `0x02aae228` | `0x1f4`   | Thinned to 24 entries, 1 symbol each  |
| Resource   | `0x02b4c000` | `0x1d5`   | Tiny, version info stripped           |
| Exception  | `0x02b03060` | `0x485b8` | Unwind info for the protector's stubs |
| Security   | `0x01546200` | `0x32f0`  | Authenticode signature                |
| Reloc      | `0x02b4d000` | `0xc0`    | Almost empty, protector relocates itself |
| TLS        | `0x01b9a3d0` | `0x28`    | One TLS callback (used by the unpacker)  |
| LoadConfig | `0x02b02f20` | `0x138`   | Standard MSVC load-config             |
| IAT        | `0x01605000` | `0x188`   | Same size as Import directory; no real IAT |

The combination of an empty Reloc directory, a populated TLS directory,
and a real Security directory matches a signed VMProtect binary almost
exactly.

### Section table

| Name    | VSize       | VA          | RSize       | RawPtr      | Flags                  |
|---------|-------------|-------------|-------------|-------------|------------------------|
| `.text` | `0x5e342b`  | `0x001000`  | 0           | 0           | CODE, EXEC, READ       |
| `.rdata`| `0x1a5f12`  | `0x5e5000`  | 0           | 0           | IDATA, READ            |
| `.data` | `0x09fb08`  | `0x78b000`  | 0           | 0           | IDATA, READ, WRITE     |
| `.pdata`| `0x0456cc`  | `0x82b000`  | 0           | 0           | IDATA, READ            |
| `.o1}`  | `0xd93a9d`  | `0x871000`  | 0           | 0           | CODE, EXEC, READ       |
| `.2F;`  | `0x0001e8`  | `0x1605000` | `0x200`     | `0x400`     | IDATA, READ, WRITE     |
| `.1M@`  | `0x1545618` | `0x1606000` | `0x1545800` | `0x600`     | CODE, IDATA, EXEC, READ|
| `.rsrc` | `0x0001d5`  | `0x2b4c000` | `0x200`     | `0x1545e00` | IDATA, READ            |
| `.reloc`| `0x0000c0`  | `0x2b4d000` | `0x200`     | `0x1546000` | IDATA, READ, DISCARD   |

Four observations follow from this table. First, the four standard
sections carry no bytes on disk; only the section table describes them so
the Windows loader can allocate the right address ranges. Second, the
entry point falls inside `.1M@` rather than `.text`, so the first
instructions executed when Windows loads the DLL belong to the protector.
Third, `.1M@` is flagged as both code and initialised data, which no
ordinary compiler emits. Fourth, `.o1}` is a 14 MiB code section that
arrives empty on disk; the protector reserves that range and fills it from
a compressed payload inside `.1M@` at load time.

### Imports

The 24 import entries each carry exactly one symbol.

```
IPHLPAPI.DLL       GetAdaptersAddresses
CRYPT32.dll        CertOpenSystemStoreA
RPCRT4.dll         UuidCreate
ADVAPI32.dll       ReportEventW
bcrypt.dll         BCryptGenRandom
WS2_32.dll         <ordinal 4 = bind>
KERNEL32.dll       GetStartupInfoW
USER32.dll         GetProcessWindowStation
MSVCP140.dll       std::codecvt<...>::_Getcat
WINTRUST.dll       WTHelperGetProvCertFromChain
VCRUNTIME140.dll   memcmp
VCRUNTIME140_1.dll __CxxFrameHandler4
api-ms-win-crt-time-l1-1-0.dll        _localtime64_s
api-ms-win-crt-heap-l1-1-0.dll        _callnewh
api-ms-win-crt-stdio-l1-1-0.dll       __stdio_common_vswprintf_s
api-ms-win-crt-filesystem-l1-1-0.dll  _rmdir
api-ms-win-crt-runtime-l1-1-0.dll     _errno
api-ms-win-crt-math-l1-1-0.dll        _fdclass
api-ms-win-crt-string-l1-1-0.dll      strncpy_s
api-ms-win-crt-locale-l1-1-0.dll      localeconv
api-ms-win-crt-convert-l1-1-0.dll     strtol
api-ms-win-crt-utility-l1-1-0.dll     srand
api-ms-win-crt-environment-l1-1-0.dll getenv
api-ms-win-crt-multibyte-l1-1-0.dll   _mbspbrk
```

A C++ networking application of this size would normally pull several
hundred symbols. The single-anchor pattern is consistent with a protector
that resolves the real imports at runtime through its own
`LoadLibrary`/`GetProcAddress` mechanism, leaving just enough static
imports to ensure the system DLLs are mapped into the address space first.
The static IAT directory is `0x188` bytes, far too small for the function
pointers this kind of code actually uses.

The composition still tells us the API categories the binary touches.
`WS2_32.dll` and `IPHLPAPI.DLL` mean BSD sockets and adapter enumeration.
`bcrypt.dll`, `CRYPT32.dll`, and `WINTRUST.dll` mean Windows-native crypto
plus system cert store and Authenticode chain validation; this is a
library that talks TLS to bambulab.com and validates server certificates
against the OS trust store. `RPCRT4.dll!UuidCreate` generates random UUIDs,
most likely for MQTT client IDs and session tokens. The MSVCP140 plus
api-ms-win-crt split CRT plus `__CxxFrameHandler4` import indicates Visual
Studio 2019 or newer with C++ exceptions enabled. `ADVAPI32.dll!ReportEventW`
writes to the Windows Event Log. `USER32.dll!GetProcessWindowStation` is
typically used by anti-debug to check whether the process is interactive.

## The VMProtect 3.x protector

The DLL is packed and protected by VMProtect 3.x configured with packing,
anti-debug, anti-VM, and import-protection. The function-virtualization
feature is also enabled but, as discovered post-unpack, not applied to any
of the 128 public exports (at least in the currently analyzed version of the
DLL). Each of these features can be pinned to specific functions inside `.1M@`.

### Protector inventory

Ghidra's auto-analyzer reports 24,710 functions in the packed binary, but
only 42 of those are real, decompilable functions; the other 24,668 are
one-byte hallucinations Ghidra emits probably because the export table points
into uninitialised section memory. The 42 real functions are all inside `.1M@`
and total 16,422 bytes. Their roles, by size:

| Role                       | Address      | Size  |
|----------------------------|--------------|-------|
| LZMA-family decompressor   | `0x181b21164`| 5,346 |
| Import resolver            | `0x18163ec4b`| 4,211 |
| Larger helper              | `0x181b22685`| 1,272 |
| VM dispatcher              | `0x18161436d`| 694   |
| DLL entry point (OEP wrapper) | `0x181b72917`| 88 |
| Early bootstrap            | `0x181b4eaac`| 27    |
| TLS callback               | `0x181e36e5a`| 18    |
| Other handlers (35 funcs)  | various      | ~5 KiB|

### LZMA-family payload decompressor

`FUN_181b21164` is the packed-payload decompressor. The distinguishing
patterns are an initial probability table of 0x400 (1024) entries seeded
to `0x400`, a streaming bit-reader shifting incoming bytes into a 64-bit
range register, the magic constant `0x736` (size of the prob-table for
one of the context bands), and Markov state-machine indices that switch
between literal, match, and repeated-match contexts. These are LZMA's
signature elements.

Cross-checked from entropy: a 64 KiB rolling-entropy scan over `.1M@`
shows three contiguous high-entropy runs totalling 14.3 MiB on disk that
expand into roughly 22.2 MiB of virtual sections. A compression ratio
around 64 percent is consistent with LZMA on x86-64 binary code.

### VM dispatcher

`FUN_18161436d` is the canonical VMProtect VM-handler dispatcher loop. The
recovered shape decrypts the next VM instruction pointer from the stack,
byte-swaps it, XORs with a per-build random key, looks up a handler offset
indexed by the result, XOR-decrypts that handler offset with two more
per-build keys, and indirect-jumps to the resolved handler. The four
build-time keys observed in this binary are `0xf2be4085`, `0xc089cca5`,
`0x38b69c12`, and `0x66b2b901`. Every VMProtected binary uses different
keys; their presence in this exact pattern is the fingerprint. Ghidra's
"Could not recover jumptable. Too many branches" warning on this function
is exactly what a static analyzer produces when it hits a VM dispatcher
table.

### Anti-VM string scanner

`FUN_181b22685` walks a probe buffer looking for substrings that identify
hypervisors and virtual environments.

| Substring          | Identifies                          |
|--------------------|-------------------------------------|
| `QEMU`             | QEMU CPUID brand string             |
| `Oracle`           | VirtualBox manufacturer             |
| `innotek`          | VirtualBox manufacturer (older brand)|
| `VirtualBox`       | VirtualBox product name             |
| `Virtual Platform` | Hyper-V SMBIOS string               |
| `VMware`           | VMware tools and devices            |
| `Parallels`        | Parallels on macOS                  |
| `777777`           | VMware default serial prefix        |

This is the exact list VMProtect's anti-VM module probes. Coverage of
every major desktop hypervisor plus QEMU makes the use case unmistakable:
detect that the binary is running in a sandbox, and either refuse to
unpack or unpack a decoy.

### Anti-debug primitives

The OEP wrapper at `0x181b72917` issues an `IN` instruction, which is
illegal in ring 3 on Windows and faults unless a debugger intercepts
it. `FUN_181b4eaac` issues `OUT 0xff4dd057` and `INT 1`, both of which
complete cleanly only inside a debugger. Multiple functions terminate
in `halt_baddata`, a manufactured "bad instruction" that prevents
linear-sweep disassembly from continuing. Most large functions have no
static callees because their control flow is hidden inside the VM
dispatcher. Ghidra reports "Instruction at 0x181b7295a overlaps
instruction at 0x181b72959" inside the OEP wrapper, a classic
anti-disassembly trick. The user-written `DllMain` (recovered
post-unpack at `0x180565dc4` in `.text`) is similarly post-processed
with overlapping-instruction bait; see
[`02-unpacking.md`](02-unpacking.md) and
[`../output/unpacking/dllmain_chain.md`](../output/unpacking/dllmain_chain.md).

### Import protection

VMProtect's import-protection rewrites every `call [kernel32_X]` thunk
inside `.text` as a call into VMProtect's runtime resolver, which looks
up the API at execution time using a build-time-generated id table. The
result is that the on-disk IAT shrinks to a single bootstrap entry, and
after unpacking the IAT is still effectively empty: VMProtect calls into
Windows itself rather than letting Bambu's code do so directly. The
resolver lives at `FUN_18163ec4b`.

### TLS-callback bootstrap

`tls_callback_0` at `0x181e36e5a` is 18 bytes. On
`DLL_PROCESS_ATTACH` it calls the early-stage unpacker. Windows runs
TLS callbacks before the DLL entry point, which gives the protector
time to set up its state before the system's exception handler
reaches a normal posture.

### Conclusion

Two distinct things happen at load time. Packing decompresses the
LZMA-compressed `.text`, `.rdata`, `.data`, and `.pdata` from inside
`.1M@` and writes them at their original VAs (`0x180001000` onward); after
this step the standard sections look like ordinary compiled code in
memory. Virtualization replaces specific functions with bytecode for the
protector's stack-based VM. The post-unpack inventory (see
[`02-unpacking.md`](02-unpacking.md)) shows that, in this build, by chance
none of the 128 public exports were chosen for virtualization, so all of
them decompile cleanly from the dump.

## Strings

A `scripts/extract_strings.py` run on the packed binary finds 28,868
printable runs inside `.1M@`, 108 in headers, and 10 in `.rsrc`. The
"normal" data sections produce zero hits because the strings the unpacked
code needs are bundled into the protector blob and only emerge after
unpacking. Post-unpack, `.rdata` carries the expected mass of MQTT
broker URIs, log format strings, JSON key literals, and RTTI type
descriptors that the rest of this documentation references.
