#!/usr/bin/env python3
"""
Dump the PE export directory (function names, ordinals, RVAs) for the target
DLL. Writes a deterministic text table that downstream tooling and docs can
diff or grep.

Usage:
    python3 scripts/pe_exports.py [path/to/dll] [-o output/exports.txt]

If -o is omitted, output goes to stdout.
"""
from __future__ import annotations
import argparse
import struct
import sys
from pathlib import Path

# Reuse the shared parser by importing pe_analyze from the same directory.
sys.path.insert(0, str(Path(__file__).resolve().parent))
import pe_analyze  # noqa: E402


def parse_exports(data: bytes, info: dict):
    sections = info["sections"]
    exp_va, exp_sz = info["data_dirs"][0][1:]
    if not exp_va:
        return []
    exp_off = pe_analyze.rva_to_offset(exp_va, sections)
    if exp_off is None:
        raise RuntimeError("Export directory RVA not in mapped file data")
    (characteristics, timestamp, mver, miver, name_rva, ord_base,
     num_funcs, num_names, addr_funcs_rva, addr_names_rva,
     addr_ords_rva) = struct.unpack_from("<IIHHIIIIIII", data, exp_off)
    name_table_off = pe_analyze.rva_to_offset(addr_names_rva, sections)
    ord_table_off = pe_analyze.rva_to_offset(addr_ords_rva, sections)
    func_table_off = pe_analyze.rva_to_offset(addr_funcs_rva, sections)
    if None in (name_table_off, ord_table_off, func_table_off):
        raise RuntimeError("One of the export sub-tables is unmapped")
    results = []
    for i in range(num_names):
        name_rva = struct.unpack_from("<I", data, name_table_off + i * 4)[0]
        n_off = pe_analyze.rva_to_offset(name_rva, sections)
        if n_off is None:
            name = f"<unmapped 0x{name_rva:x}>"
        else:
            end = data.find(b"\x00", n_off)
            name = data[n_off:end].decode("latin-1", errors="replace")
        ordn = struct.unpack_from("<H", data, ord_table_off + i * 2)[0]
        func_rva = struct.unpack_from(
            "<I", data, func_table_off + ordn * 4
        )[0]
        results.append((ordn + ord_base, func_rva, name))
    results.sort(key=lambda r: r[0])
    return results


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument("path", nargs="?", default="unpacking/bambu_networking.dll")
    ap.add_argument("-o", "--output", default=None)
    args = ap.parse_args(argv[1:])
    data = Path(args.path).read_bytes()
    info = pe_analyze.parse(data)
    exports = parse_exports(data, info)
    lines = [
        f"# PE exports of {args.path}",
        f"# Total: {len(exports)}",
        f"# Image base: 0x{info['img_base']:x}",
        "# ordinal  rva         name",
    ]
    for ordn, rva, name in exports:
        lines.append(f"{ordn:5d}  0x{rva:08x}  {name}")
    text = "\n".join(lines) + "\n"
    if args.output:
        Path(args.output).write_text(text)
        print(f"Wrote {len(exports)} exports to {args.output}")
    else:
        sys.stdout.write(text)


if __name__ == "__main__":
    main(sys.argv)
