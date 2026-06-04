# baltobu-reverse

Reverse engineering of `bambu_networking.dll` for the **baltobu**
project initiated by the Software Freedom Conservancy. Bambu Lab
ships this library as an opaque binary with Bambu Studio even though
Bambu Studio is AGPLv3 and the library is dynamically linked into it.
That is a licence violation. The goal of baltobu is to recover the
public ABI by clean-room reverse engineering so that a free, drop-in
replacement library can be written.

This is licence-compliance and interoperability work, not piracy. The
ABI recovered here is the boundary Bambu themselves expose to their
AGPLv3 application; rebuilding a free library against that boundary is
exactly what AGPLv3 exists to enable.

The matching Linux `libbambu_networking.so` has since been
cross-analyzed (a later `02.07`-family build under the same VMProtect
protector; see [`docs/28-linux-so-recovery.md`](docs/28-linux-so-recovery.md)),
which confirms the public ABI matches between Windows and Linux. The
macOS `libbambu_networking.dylib` has not yet been analyzed but is
expected to match.

## Start here

The high-level system map is at
[`docs/03-architecture.md`](docs/03-architecture.md). It synthesises
everything that has been recovered into one coherent picture:
subsystem diagram, internal class hierarchy, the agent singleton,
lifecycle, the two-channel MQTT pub/sub, REST endpoint inventory, and
threading model.

For the rest of the documentation in reading order, see
[`docs/README.md`](docs/README.md).

## Repository layout

```
.
├── README.md                  this file
├── docs/                      written documentation
│   ├── README.md              documentation index
│   ├── 01-binary.md           PE structure, sections, VMProtect 3.x identification
│   ├── 02-unpacking.md        unpacking method, dump quality, analysis pipeline
│   ├── 03-architecture.md     system map, classes, threading, lifecycle
│   ├── 04-public-api.md       128 exports, family grouping, cross-cutting patterns
│   ├── 05-wire-protocol.md    JSON schemas, types, optionality, error codes
│   ├── 06-internals.md        FT cmd_type, RTTI, helper map, agent struct layout
│   └── exports/               one markdown file per exported function
├── scripts/                   Python 3 analysis tools (stdlib only)
│   ├── pe_analyze.py
│   ├── pe_exports.py
│   ├── pe_imports.py
│   ├── extract_strings.py
│   ├── categorize_exports.py
│   ├── extract_signature.py
│   └── post_unpack_validate.py
├── unpacking/                 the binary and the unpacking session
│   ├── bambu_networking.dll       the target packed binary (vendored)
│   ├── dumps/                     unpacked image and its checksum
│   │   ├── bambu_networking_dump.dll      Scylla-produced unpacked image (gitignored)
│   │   └── bambu_networking_dump.sha256
│   ├── host_loader.c              LoadLibrary stub for the dump session
│   └── README.md                  how to run the session
├── ghidra/                    Ghidra material
│   ├── scripts/               headless analysis scripts (.java)
│   ├── projects/              local Ghidra project store (gitignored)
│   └── run_headless.sh        wrapper around analyzeHeadless
└── output/                    generated artefacts (kept under VCS so that
                               findings are stable across machines)
```

## Provenance of the binary

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

The Authenticode signature is intact and validates against the OS
trust store. The analyzed DLL is a genuine, untampered Bambu Lab build,
but this was expected as gathered automatically by running official Bambu Studio.

The table above is the **primary** analyzed artefact. The companion
analysis (docs 08–31) additionally draws on the sources below — analysis
inputs only; no vendor binary is redistributed here:

| Companion source | Provenance |
|---|---|
| Linux `libbambu_networking.so` | a later `02.07`-family build under the same VMProtect 3.x protector; used for cross-source ABI confirmation ([`docs/28-linux-so-recovery.md`](docs/28-linux-so-recovery.md)) |
| Symbol-bearing Linux build `01.07.01.04` | retains demangled C++ symbols; used to recover exact ABI signatures ([`docs/08-linux-rosetta.md`](docs/08-linux-rosetta.md)) |
| Live `02.07.x` account / printer captures | on-wire validation of the login, print, command-security, and file-transfer flows ([`docs/29-cloud-print-live.md`](docs/29-cloud-print-live.md)) |

A consolidated, step-by-step reproduction guide for every method below is
in [`docs/32-reproduction-methods.md`](docs/32-reproduction-methods.md).

## What has been recovered

The library exposes a C ABI of 128 functions across two namespaces:
`bambu_network_*` (107) for the cloud, user, and printer API; and
`ft_*` (21) for the refcounted file-transfer subsystem.

The binary is packed and protected by VMProtect 3.x configured with
packing, anti-debug, anti-VM, and import-protection. A dynamic dump
produced by Scylla against a bare-metal Windows 11 host gave a
faithful in-memory image of the unpacked binary. All 128 exports land
in populated `.text`; 129 of 130 entry points decompile cleanly.
The single outlier is `bambu_network_create_agent`, whose body is
hidden behind a five-byte anti-disassembly stub.

Documentation against the dump now covers every export, the complete
internal class structure (from RTTI), the JSON message schemas with
types and optionality, the library-internal and Bambu application
error codes, the login and bind state machine, the FT command enum,
and the agent's internal struct layout.

## Companion analysis and live validation (docs 08–31)

The series has since been extended well beyond the initial static
analysis. Later documents cross-check the findings against the
symbol-bearing Linux `libbambu_networking.so` of the same family,
validate the wire protocol against live on-wire captures, and map the
subsystems the first pass left open: the camera/file media plane
(ThroughTek Kalay + Agora), the OSS/S3 cloud-print upload, the MQTT
command-security layer (app-certificate RSA-SHA256 signing), and the
cloud file-transfer tunnel.
[`docs/31-status-and-remaining-gate.md`](docs/31-status-and-remaining-gate.md)
is the capstone: it states what is closed and the single server-side
gate that remains — application-certificate *acquisition*, which depends
on a vendor-embedded client key that is out of scope for clean-room
interop, exactly as the Kalay license is for the P2P transports. All of
this is fresh-prose documentation under the same clean-room boundary
described below; see [`docs/README.md`](docs/README.md) for the full
reading order (waves 08–31).

## Quick start

To re-derive every artefact in `output/` from the packed DLL:

```
python3 scripts/pe_analyze.py | tee output/pe_analyze.txt
python3 scripts/pe_exports.py -o output/exports.txt
python3 scripts/pe_imports.py -o output/imports.txt
python3 scripts/extract_strings.py
python3 scripts/categorize_exports.py
python3 scripts/extract_signature.py
sha256sum unpacking/bambu_networking.dll > output/dll.sha256
```

To run the Ghidra headless analyses (idempotent; reuses the project
after the first import), point `GHIDRA_HOME` at a Ghidra installation
and then:

```
ghidra/run_headless.sh InitialAnalysis.java     # function and section inventory
ghidra/run_headless.sh ProtectorAnalysis.java   # decompile the 42 protector funcs
```

The unpacking flow and the post-unpack Ghidra analysis are documented
in [`docs/02-unpacking.md`](docs/02-unpacking.md).

## Clean-room boundary

Raw decompiled bodies (under `output/ghidra_decompile/`) are
gitignored. Anyone implementing the clean-room replacement library
must not read those bodies. The better artefacts are the markdown
documents under `docs/`, including the per-export reference under
`docs/exports/`. Those documents describe the ABI in fresh prose
suitable for re-implementation.

Decompilation of the VMProtect protector (the 42 functions in `.1M@`)
is committed at `output/ghidra/protector/`. That code is the
protector's own implementation, not Bambu Lab's, and the clean-room
boundary does not apply to it. No further analysis was done on that part.

## License

The documentation, scripts, and analysis artefacts in this repository
are released under the MIT License. See [`LICENSE`](LICENSE).

This license covers only the original work in this repository (the
written documentation, the Python and Ghidra scripts, and the derived
analysis artefacts). It does not cover `bambu_networking.dll` itself,
which remains Bambu Lab's proprietary binary and is not redistributed
here.
