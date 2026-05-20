#!/usr/bin/env python3
"""
Compare exports between the packed bambu_networking.dll and its
Scylla-unpacked dump.

The post_unpack_validate.py script already confirms the *names* of all
128 exports survive the dump. This script answers the next question:
do their *RVAs* line up too? VMProtect's section layout means the
unpacker reconstitutes the original `.text` etc. at their original VAs
(0x180001000 onwards), so the answer should be "yes, RVA-for-RVA". A
mismatch here would indicate Scylla relocated something, which would
break every RVA in the docs.

Usage:
    python3 scripts/compare_exports.py unpacking/dumps/bambu_networking_dump.dll

Writes a markdown summary to output/unpacking/exports_diff.md when the
output directory exists. Returns exit 0 if every export survived with
identical RVA, exit 1 otherwise.
"""
from __future__ import annotations
import argparse
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import pe_analyze   # noqa: E402
import pe_exports   # noqa: E402


def compare(dump_path: Path) -> int:
    orig = Path("unpacking/bambu_networking.dll").read_bytes()
    dump = dump_path.read_bytes()
    orig_info = pe_analyze.parse(orig)
    dump_info = pe_analyze.parse(dump)

    orig_exports = pe_exports.parse_exports(orig, orig_info)
    dump_exports = pe_exports.parse_exports(dump, dump_info)

    orig_by_name = {name: (ordn, rva) for ordn, rva, name in orig_exports}
    dump_by_name = {name: (ordn, rva) for ordn, rva, name in dump_exports}

    only_orig = sorted(set(orig_by_name) - set(dump_by_name))
    only_dump = sorted(set(dump_by_name) - set(orig_by_name))
    common = sorted(set(orig_by_name) & set(dump_by_name))

    rva_diffs = []
    ord_diffs = []
    for name in common:
        o_ord, o_rva = orig_by_name[name]
        d_ord, d_rva = dump_by_name[name]
        if o_rva != d_rva:
            rva_diffs.append((name, o_rva, d_rva))
        if o_ord != d_ord:
            ord_diffs.append((name, o_ord, d_ord))

    print(f"Total exports: original {len(orig_exports)}, "
          f"dump {len(dump_exports)}, common {len(common)}")
    print(f"Only in original: {len(only_orig)}")
    print(f"Only in dump:     {len(only_dump)}")
    print(f"RVA mismatches:   {len(rva_diffs)}")
    print(f"Ordinal mismatches: {len(ord_diffs)}")
    if only_orig:
        print("  first missing:", only_orig[:5])
    if rva_diffs:
        print("  first RVA drift:", rva_diffs[:3])
    if ord_diffs:
        print("  first ord drift:", ord_diffs[:3])

    out_dir = Path("output/unpacking")
    if out_dir.is_dir():
        out = out_dir / "exports_diff.md"
        with out.open("w") as f:
            f.write("# Packed vs unpacked export parity\n\n")
            f.write(f"- Original DLL: `bambu_networking.dll`\n")
            f.write(f"- Unpacked dump: `{dump_path}`\n\n")
            f.write(f"| Set                 | Count |\n")
            f.write(f"|---------------------|------:|\n")
            f.write(f"| Original exports    | {len(orig_exports)} |\n")
            f.write(f"| Dump exports        | {len(dump_exports)} |\n")
            f.write(f"| In both             | {len(common)} |\n")
            f.write(f"| Only in original    | {len(only_orig)} |\n")
            f.write(f"| Only in dump        | {len(only_dump)} |\n")
            f.write(f"| RVA mismatches      | {len(rva_diffs)} |\n")
            f.write(f"| Ordinal mismatches  | {len(ord_diffs)} |\n\n")
            if only_orig:
                f.write("## Exports missing in dump\n\n")
                for n in only_orig:
                    f.write(f"- `{n}`\n")
                f.write("\n")
            if rva_diffs:
                f.write("## RVA drift\n\n| Export | Original | Dump |\n|---|---|---|\n")
                for n, o, d in rva_diffs:
                    f.write(f"| `{n}` | `0x{o:x}` | `0x{d:x}` |\n")
                f.write("\n")
            if ord_diffs:
                f.write("## Ordinal drift\n\n| Export | Original | Dump |\n|---|---:|---:|\n")
                for n, o, d in ord_diffs:
                    f.write(f"| `{n}` | {o} | {d} |\n")
            if not (only_orig or only_dump or rva_diffs or ord_diffs):
                f.write("All exports match: same names, same RVAs, "
                        "same ordinals. The unpacker reconstituted the "
                        "original `.text` layout in place.\n")
        print(f"Wrote {out}")
    return 0 if not (only_orig or rva_diffs) else 1


def main() -> int:
    ap = argparse.ArgumentParser()
    ap.add_argument("dump")
    args = ap.parse_args()
    return compare(Path(args.dump))


if __name__ == "__main__":
    raise SystemExit(main())
