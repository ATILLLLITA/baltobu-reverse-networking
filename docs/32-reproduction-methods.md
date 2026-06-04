# Reproduction methods: how to repeat this analysis end to end

This document consolidates **every method** used across the series into a
single practical guide, so a new analyst can reproduce the findings
without rediscovering the dead ends. Each section says what the method is
for, what it needs, the gotcha that wastes the most time, and which
detailed document carries the full procedure.

The analysis runs in three complementary modes — **static** (the Windows
DLL), **cross-source** (symbol-bearing and open-source siblings), and
**dynamic / live** (the Linux `.so` and on-wire capture). No single mode
is sufficient; the ABI and wire protocol documented here are the
*intersection* of all three, which is why each independently confirms the
others.

> **Clean-room note.** Every method below recovers the *public ABI and
> wire protocol* — the boundary Bambu's AGPLv3 application already speaks
> to. None of them extracts or redistributes vendor secrets. The
> deliberately-out-of-scope items are listed at the end; they are **not**
> reproduction targets.

## Toolchain and environment

| Need | Used for |
|---|---|
| Bare-metal Windows 11 host (not a VM) | Scylla dynamic dump of the Windows DLL — VMProtect's anti-VM trips inside hypervisors |
| Scylla (x64) | Reconstruct the unpacked image + rebuild the import table |
| Ghidra (headless) | Function/section inventory and decompilation of the dump |
| Python 3 (stdlib only) | The `scripts/` PE/exports/imports/strings/signature pipeline |
| A Linux host running the genuine slicer | Dynamic recovery of the Linux `.so`; live capture |
| `root` (for `/proc/<pid>/mem`) | Debugger-free heap reads (see Method C) |
| `tcpdump` + Wireshark | On-wire capture; decrypt with a recovered TLS key (Method D) |

## Method A — Static unpacking of the Windows DLL

**For:** the export table, RTTI class hierarchy, and decompilable bodies.
**Procedure:** identify the protector (VMProtect 3.x — packing, anti-debug,
anti-VM, import-protection), run the target under a `LoadLibrary` stub
(`unpacking/host_loader.c`), let it self-unpack, then dump the in-memory
image with **Scylla** and rebuild imports. Validate the dump with
`scripts/post_unpack_validate.py`, then drive Ghidra headless
(`ghidra/run_headless.sh InitialAnalysis.java`). 129/130 entry points
decompile cleanly; `bambu_network_create_agent` hides behind a five-byte
anti-disassembly stub.
**Gotcha:** the dump must come from **bare metal** — anti-VM corrupts the
unpack inside a hypervisor. Full procedure: [`01-binary`](01-binary.md),
[`02-unpacking`](02-unpacking.md).

## Method B — Cross-source signature recovery

**For:** turning decompiled-but-mangled C++ into *exact* demangled ABI
signatures, and sanity-checking field layouts.
**Procedure:** two independent sources.
- A **symbol-bearing Linux build** (`01.07.01.04`) still carries demangled
  C++ symbols; lift exact signatures from it ([`08-linux-rosetta`](08-linux-rosetta.md)).
- The **open-source (AGPL) BambuStudio** headers (`bambu_networking.hpp`,
  `FileTransferUtils.hpp`) pin the public structs and the `ft_*` ABI
  byte-for-byte ([`24-studio-source-confirmation`](24-studio-source-confirmation.md)).
**Gotcha:** versions drift — reconcile field deltas (e.g. `PrintParams`)
against the version you actually run, not the oldest source.

## Method C — Linux `.so` dynamic recovery (static fails identically)

The Linux `libbambu_networking.so` is packed by the same VMProtect 3.x
protector, so static analysis fails the same way. The recovery methods
differ from the Windows pipeline and are the most reusable part of this
work. Three constraints shape everything:

1. **VMProtect anti-debug SIGKILLs the process under any `ptrace`** — gdb
   attach *and* launch. So no debugger: only debugger-free channels work.
2. The companion `libBambuSource.so` **statically links curl and OpenSSL**
   (renamed `tutk_third_curl_*` / `TUTKSSL_*`). Intra-DSO calls bind
   locally, so `LD_PRELOAD` of curl/OpenSSL interposes **nothing**.
3. The only dynamic imports are libc (`connect`/`getaddrinfo`/`send`/
   `sendto`), and `send`/`sendto` carry **TLS ciphertext**.

The techniques that *do* work:

- **`/proc/<pid>/mem` heap scraping (no `ptrace`).** Read the live slicer's
  heap directly for transient plaintext — REST URL shapes, JSON command
  and report bodies — which exist in the clear only momentarily before TLS.
- **`dlopen` + call-exports behavioral oracle.** Load the plugin in your
  own harness, call the exports, and scan **your harness's own heap** for
  the URL/JSON each call assembles. This recovers exact request shapes
  without touching the protected control flow.
- **`dlsym`-wrap `LD_PRELOAD` shim.** The plugin resolves the transport
  (`Bambu_*` in `libBambuSource`) via `dlsym`, not load-time linking, so a
  plain `Bambu_*` preload never fires. Wrap **`dlsym` itself** to capture
  the resolved pointers and trace the cross-DSO calls.
**Gotcha:** don't reach for a debugger — constraint (1) will just kill the
process and cost you an hour. Full procedure: [`28-linux-so-recovery`](28-linux-so-recovery.md),
[`30-cloud-ft-tunnel`](30-cloud-ft-tunnel.md).

## Method D — On-wire capture and decryption

**For:** validating the documented JSON schemas against real traffic.
**Procedure:** capture with `tcpdump`; recover the **TLS 1.2 master secret
from the process heap** using the `session_id` as the anchor, feed it to
Wireshark, and decrypt. The **local** report channel is plain JSON (no
binary framing), which validates the `push_status` schema directly; the
task-create body and `project_file` URL schemes were pinned byte-exact
this way.
**Gotcha:** the **cloud** broker is TLS 1.3 — the session-key recovery
differs, and the cloud file-transfer data plane is Kalay P2P, not a TLS
stream to read (see Method-scope note below). Full procedure:
[`23-live-decrypt-validation`](23-live-decrypt-validation.md),
[`29-cloud-print-live`](29-cloud-print-live.md).

## Method E — Config-blob crypto (interop check)

**For:** confirming a clean-room implementation is byte-compatible with the
genuine `*.conf`. **Procedure:** the config blob is **AES-128-ECB** under a
network key that is a static, publicly-known string in the slicer; decrypt
a genuine config and round-trip it. **Gotcha:** zero-padding — strip
trailing NULs after decrypt, and treat non-block-aligned input as legacy
plaintext. Detail: [`06-internals`](06-internals.md).

## Reproduction checklist

1. Acquire the genuine plugin by running official Bambu Studio (provenance
   in the top-level README). Record the SHA-256.
2. **Static (Windows):** Method A → export table + decompiled bodies.
3. **Cross-source:** Method B → exact signatures + struct layouts.
4. **Dynamic (Linux):** Method C → URL/JSON shapes the static read leaves
   ambiguous; transport call sequence.
5. **Live:** Method D → decrypt real traffic, validate every schema.
6. **Interop:** Method E and the per-subsystem docs → confirm a retyped
   implementation matches the wire byte-for-byte.

Each artefact under `output/` is regenerated by the `scripts/` commands in
the top-level README's *Quick start*; the documentation is the
re-implementation source, not the raw decompiled bodies (see the
README's *Clean-room boundary*).

## Out of scope — not reproduction targets

Two vendor secrets are deliberately **not** covered by any method above,
because extracting them would be circumventing an access control rather
than documenting a protocol boundary:

- The **Kalay (TUTK) license** that gates the remote camera and the cloud
  file-transfer P2P transport ([`25`](25-camera-local-protocol.md),
  [`30`](30-cloud-ft-tunnel.md)). The open path keeps the genuine
  `libBambuSource` rather than reproducing the license.
- The **embedded application key** behind the `code 101` app-certificate
  acquisition gate ([`31`](31-status-and-remaining-gate.md)). A clean-room
  implementation takes the application certificate and key as
  **caller-provided input**, exactly as a TLS library takes a cert.

These are documented as *boundaries*, with the same status as any other
vendor-held secret: identified, explained, and left to the operator to
supply legitimately — never extracted or redistributed here.

## Cross-references

- [`01-binary`](01-binary.md), [`02-unpacking`](02-unpacking.md) — Method A.
- [`08-linux-rosetta`](08-linux-rosetta.md), [`24-studio-source-confirmation`](24-studio-source-confirmation.md) — Method B.
- [`28-linux-so-recovery`](28-linux-so-recovery.md), [`30-cloud-ft-tunnel`](30-cloud-ft-tunnel.md) — Method C.
- [`23-live-decrypt-validation`](23-live-decrypt-validation.md), [`29-cloud-print-live`](29-cloud-print-live.md) — Method D.
- [`31-status-and-remaining-gate`](31-status-and-remaining-gate.md) — what is closed vs. the out-of-scope boundaries.
