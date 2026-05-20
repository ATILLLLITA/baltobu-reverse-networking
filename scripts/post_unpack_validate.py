#!/usr/bin/env python3
"""
Validate an unpacked (Scylla-dumped) bambu_networking.dll.

When the dynamic-unpacking session completes, we'll have a file along
the lines of `bambu_networking_dump.dll`. This script checks that the
dump actually has the original sections populated and the same exports
the protected DLL declares, so we know the dump is good before
spending Ghidra time on it.

Usage:
    python3 scripts/post_unpack_validate.py path/to/unpacked.dll

The original DLL must still be at ./unpacking/bambu_networking.dll so we can
diff the two.
"""
from __future__ import annotations
import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import pe_analyze  # noqa: E402
import pe_exports  # noqa: E402

EXPECTED_NORMAL_SECTIONS = (".text", ".rdata", ".data", ".pdata")


def report(name: str, ok: bool, detail: str = "", level: str = "FAIL") -> bool:
    """level is the severity if ok is False: "FAIL" or "WARN"."""
    if ok:
        mark = "OK"
    else:
        mark = level
    print(f"  [{mark}] {name}" + (f"  ({detail})" if detail else ""))
    return ok or level == "WARN"


def validate(dump_path: Path) -> int:
    print(f"== Validating {dump_path} ==")
    orig = Path("unpacking/bambu_networking.dll").read_bytes()
    dump = dump_path.read_bytes()
    orig_info = pe_analyze.parse(orig)
    dump_info = pe_analyze.parse(dump)

    all_ok = True
    notes: list[str] = []

    # --- 1. Standard sections should now have real bytes on disk.
    by_name = {s["name"]: s for s in dump_info["sections"]}
    for sec in EXPECTED_NORMAL_SECTIONS:
        s = by_name.get(sec)
        if not s:
            all_ok &= report(f"section {sec} present", False, "missing")
            continue
        all_ok &= report(
            f"section {sec} has on-disk bytes",
            s["rsize"] > 0,
            f"rsize=0x{s['rsize']:x}, vsize=0x{s['vsize']:x}",
        )

    # --- 2. Image base should match (or, if dumped at a relocated base,
    #       at least one of them should be consistent with what we saw).
    #       Scylla rewrites the header with the *runtime* (ASLR-relocated)
    #       base when dumping a live process, so a mismatch is expected and
    #       not a defect; flag it WARN, not FAIL.
    base_match = orig_info["img_base"] == dump_info["img_base"]
    all_ok &= report(
        "image base matches original",
        base_match,
        f"orig=0x{orig_info['img_base']:x} dump=0x{dump_info['img_base']:x}"
        + ("" if base_match else "; expected when dumped from a running ASLR process"),
        level="WARN",
    )
    if not base_match:
        notes.append(
            "Header image base is the ASLR-relocated runtime base, not the original "
            f"0x{orig_info['img_base']:x}. Ghidra import: set the base explicitly to "
            f"0x{orig_info['img_base']:x} so RVAs match the docs."
        )

    # --- 3. The export table should still parse and list the same names.
    try:
        orig_exports = pe_exports.parse_exports(orig, orig_info)
        dump_exports = pe_exports.parse_exports(dump, dump_info)
    except Exception as exc:
        all_ok &= report("export tables parse", False, str(exc))
        return 0 if all_ok else 1

    orig_names = {n for _, _, n in orig_exports}
    dump_names = {n for _, _, n in dump_exports}
    missing = orig_names - dump_names
    added = dump_names - orig_names
    all_ok &= report(
        f"all {len(orig_names)} original exports still present",
        not missing,
        f"missing: {sorted(missing)[:5]}{'...' if len(missing) > 5 else ''}"
        if missing else "",
    )
    if added:
        # Not fatal but worth noting (Scylla can re-add cleanup helpers).
        print(f"  [INFO] dump has {len(added)} extra exports: "
              f"{sorted(added)[:5]}{'...' if len(added) > 5 else ''}")

    # --- 4. Sanity: at least one export should now sit on a function
    #       larger than 1 byte. We can't verify this without Ghidra, but
    #       we can verify the RVA points into a section with rsize>0.
    bad = []
    for ordn, rva, name in dump_exports:
        sec = None
        for s in dump_info["sections"]:
            if s["va"] <= rva < s["va"] + max(s["vsize"], s["rsize"]):
                sec = s
                break
        if sec is None or sec["rsize"] == 0:
            bad.append((name, rva, sec["name"] if sec else "<?>"))
    all_ok &= report(
        "every export RVA lands in a populated section",
        not bad,
        f"{len(bad)} exports point into empty sections (first: {bad[0]})"
        if bad else "",
    )

    # --- 5. IAT directory size would normally grow substantially after a
    #       packer's unpack. VMProtect 3.x "import protection" replaces
    #       every `call [kernel32_X]` thunk with a call into the VMProtect
    #       runtime, so Scylla finds nothing to rebuild (see
    #       output/unpacking/session_notes.txt). Empty IAT means the
    #       Linux side will need to label imports another way (Frida-side
    #       hook log, or recovering the VMP runtime's API id table); it
    #       does not invalidate the dump itself. Flag WARN, not FAIL.
    orig_iat_size = orig_info["data_dirs"][12][2]  # entry 12 = IAT
    dump_iat_size = dump_info["data_dirs"][12][2]
    iat_grew = dump_iat_size > orig_iat_size * 4
    all_ok &= report(
        "IAT directory grew after Scylla rebuild",
        iat_grew,
        f"orig=0x{orig_iat_size:x} dump=0x{dump_iat_size:x}"
        + ("" if iat_grew else "; expected with VMProtect import-protection"),
        level="WARN",
    )
    if not iat_grew:
        notes.append(
            "IAT was not rebuilt: VMProtect's import-protection wiped the original "
            "thunks. Recover import semantics via Frida hook log or by reversing the "
            "VMP API-id resolver, not from the dump's IAT."
        )

    print()
    if notes:
        print("Notes:")
        for n in notes:
            print(f"  - {n}")
        print()
    print("== ALL CHECKS PASSED ==" if all_ok else "== SOME CHECKS FAILED ==")
    return 0 if all_ok else 1


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("dump", help="path to the Scylla-dumped DLL")
    args = ap.parse_args()
    return validate(Path(args.dump))


if __name__ == "__main__":
    raise SystemExit(main())
