#!/usr/bin/env python3
"""
Dump the PE import directory: imported DLL names and the symbols pulled in
from each. Useful for inferring what subsystems the target uses (TLS,
sockets, crypto, MSVC runtime, etc.).

Usage:
    python3 scripts/pe_imports.py [path/to/dll] [-o output/imports.txt]
"""
from __future__ import annotations
import argparse
import struct
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import pe_analyze  # noqa: E402


def read_c_string(data: bytes, off: int) -> str:
    end = data.find(b"\x00", off)
    return data[off:end].decode("latin-1", errors="replace")


def parse_imports(data: bytes, info: dict):
    sections = info["sections"]
    imp_va, imp_sz = info["data_dirs"][1][1:]
    if not imp_va:
        return []
    imp_off = pe_analyze.rva_to_offset(imp_va, sections)
    if imp_off is None:
        raise RuntimeError("Import directory RVA not in mapped file data")
    out = []
    cursor = imp_off
    while True:
        (orig_thunk_rva, ts, fwd_chain, name_rva,
         first_thunk_rva) = struct.unpack_from("<IIIII", data, cursor)
        cursor += 20
        if not (orig_thunk_rva or name_rva or first_thunk_rva):
            break
        dll_off = pe_analyze.rva_to_offset(name_rva, sections)
        dll = read_c_string(data, dll_off) if dll_off is not None else "<?>"
        syms = []
        thunk_rva = orig_thunk_rva or first_thunk_rva
        thunk_off = pe_analyze.rva_to_offset(thunk_rva, sections)
        if thunk_off is not None:
            i = 0
            while True:
                entry = struct.unpack_from("<Q", data, thunk_off + i * 8)[0]
                if entry == 0:
                    break
                if entry & (1 << 63):
                    syms.append(("ordinal", entry & 0xFFFF, None))
                else:
                    hint_off = pe_analyze.rva_to_offset(entry & 0x7FFFFFFF, sections)
                    if hint_off is None:
                        syms.append(("name?", entry, None))
                    else:
                        hint = struct.unpack_from("<H", data, hint_off)[0]
                        sym = read_c_string(data, hint_off + 2)
                        syms.append(("name", hint, sym))
                i += 1
        out.append((dll, syms))
    return out


def main(argv):
    ap = argparse.ArgumentParser()
    ap.add_argument("path", nargs="?", default="unpacking/bambu_networking.dll")
    ap.add_argument("-o", "--output", default=None)
    args = ap.parse_args(argv[1:])
    data = Path(args.path).read_bytes()
    info = pe_analyze.parse(data)
    imports = parse_imports(data, info)
    lines = [
        f"# PE imports of {args.path}",
        f"# DLLs: {len(imports)}",
        f"# Total symbols: {sum(len(s) for _, s in imports)}",
    ]
    for dll, syms in imports:
        lines.append(f"\n[{dll}]  ({len(syms)} symbol{'s' if len(syms) != 1 else ''})")
        for kind, a, b in syms:
            if kind == "ordinal":
                lines.append(f"  ord {a}")
            elif kind == "name":
                lines.append(f"  {b}")
            else:
                lines.append(f"  <{kind} 0x{a:x}>")
    text = "\n".join(lines) + "\n"
    if args.output:
        Path(args.output).write_text(text)
        print(f"Wrote import table to {args.output}")
    else:
        sys.stdout.write(text)


if __name__ == "__main__":
    main(sys.argv)
