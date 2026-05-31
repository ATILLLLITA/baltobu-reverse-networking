# Documentation index

`bambu_networking.dll` is the networking core of Bambu Studio. The
slicer itself is published under AGPLv3 and dynamically loads this
library to perform every interaction with the outside world. The
library ships as an opaque binary, which is the licence violation that
baltobu exists to address. This documentation describes the library's
public ABI, internal architecture, wire protocol, and the methods used
to recover that information from the protected binary.

Read these files in order if you are new to the project. Each one
links to the next. They are short and self-contained enough to be read
front to back in about an hour.

1. [`01-binary.md`](01-binary.md) covers the binary as it exists on
   disk, the PE structure, the VMProtect 3.x protector, and the
   anti-analysis features that have to be defeated before the rest is
   readable.
2. [`02-unpacking.md`](02-unpacking.md) covers the unpacking method
   that produced the analysis dump, the dump's quality, the post-unpack
   inventory, and how to repeat the analysis pipeline against a future
   build.
3. [`03-architecture.md`](03-architecture.md) is the high-level system
   map: subsystem diagram, internal class hierarchy, the agent
   singleton, the lifecycle, the two-channel MQTT pub/sub, the REST
   endpoint inventory, and the threading model.
4. [`04-public-api.md`](04-public-api.md) catalogs the 128 exported
   functions by functional family and documents the cross-cutting
   patterns that recur across the surface.
5. [`05-wire-protocol.md`](05-wire-protocol.md) gives the JSON message
   schemas with field names, types, optionality, and error codes for
   every protocol the library speaks.
6. [`06-internals.md`](06-internals.md) covers what is inside the
   library beyond the ABI surface: the FT `cmd_type` enum, the RTTI
   hierarchy, the helper map, the subsystem log catalog, and the
   agent struct layout.
7. [`07-internals-flow.md`](07-internals-flow.md) covers runtime
   behaviour end to end: the layered architecture, the UML class
   diagram, the call hierarchy from C ABI through helpers to
   subsystems, sequence diagrams for the seven main flows (startup,
   login, MQTT publish, push reception, LAN-cloud arbiter, FT
   upload, token refresh, telemetry), the four state machines
   (bind, MQTT connection, LAN-cloud arbiter, FT job), the JSON
   build and parse pipeline, and the worker-to-GUI callback
   marshalling.

The per-export reference is at [`exports/`](exports/) with one
markdown file per exported function. The index there groups exports by
family.

## Quick links

- The dump and committed analysis artefacts are under
  [`../output/`](../output/).
- The scripts used to re-derive every committed artefact are under
  [`../scripts/`](../scripts/).
- The Ghidra headless scripts are under [`../ghidra/scripts/`](../ghidra/scripts/).
- The repository's top-level [`../README.md`](../README.md) covers
  layout, provenance, and reproduction.

## Companion analysis (Linux-Rosetta + live-DLL, 08–12)

Independent cross-source findings (symbol-bearing Linux `01.07.01.04` build + a
`ReadProcessMemory` dump of the live `02.06.00.50` DLL). Complementary to 01–07; cross-verified
against this repo's `05`/`06`.

8.  [`08-linux-rosetta.md`](08-linux-rosetta.md) — `create_agent` body recovered, 79 exact
    demangled C++ ABI signatures, cloud host matrix, cross-verification of 05/06.
9.  [`09-camera-tutk-brtc.md`](09-camera-tutk-brtc.md) — camera/liveview: ThroughTek Kalay
    (TUTK) + Agora (`BRTC`) media plane, `ttcode` flow, URL-scheme transport selector, H.264.
10. [`10-ft-abi.md`](10-ft-abi.md) — full C ABI signatures of the 21 `ft_*` exports
    (handle/refcount model, callback types) — complements the `cmd_type`/structs in 05/06.
11. [`11-print-params.md`](11-print-params.md) — `BBL::PrintParams`/`PublishParams` field
    layout, dual snake/camel serialization, version deltas (ams_mapping2, plate_md5…).
12. [`12-login-json.md`](12-login-json.md) — login payload schemas: `studio_userlogin`
    (`{command,sequence_id,data:{name,avatar}}`), printer-login (`login/command/wifi/tutk`),
    HTTP sign-in/ticket-token flow, cloud host matrix.
