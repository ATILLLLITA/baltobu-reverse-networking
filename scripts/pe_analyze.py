#!/usr/bin/env python3
"""
PE header / section / data-directory dumper for bambu_networking.dll.

Pure-stdlib, no `pefile` dependency, so this works in any minimal Python 3.

Usage:
    python3 scripts/pe_analyze.py [path/to/dll]
Default path is ./unpacking/bambu_networking.dll relative to the current working dir.

Output is plain text on stdout. Pipe to output/pe_analyze.txt to archive.
"""
from __future__ import annotations
import datetime as _dt
import struct
import sys
from pathlib import Path


DATA_DIR_NAMES = [
    "Export", "Import", "Resource", "Exception", "Security", "Reloc",
    "Debug", "Arch", "GlobalPtr", "TLS", "LoadConfig", "BoundImport",
    "IAT", "DelayImport", "CLR", "Reserved",
]


def load(path: Path) -> bytes:
    return path.read_bytes()


def parse(data: bytes):
    """Return a dict of parsed PE fields useful for downstream analysis."""
    e_lfanew = struct.unpack_from("<I", data, 0x3C)[0]
    sig = data[e_lfanew : e_lfanew + 4]
    if sig != b"PE\x00\x00":
        raise ValueError(f"Bad PE signature: {sig!r}")
    coff_off = e_lfanew + 4
    machine, num_sec, ts, _, _, opt_size, characteristics = struct.unpack_from(
        "<HHIIIHH", data, coff_off
    )
    opt_off = coff_off + 20
    magic = struct.unpack_from("<H", data, opt_off)[0]
    if magic != 0x20B:
        raise ValueError(f"Not PE32+ (magic 0x{magic:x})")
    ep_rva = struct.unpack_from("<I", data, opt_off + 16)[0]
    img_base = struct.unpack_from("<Q", data, opt_off + 24)[0]
    section_alignment = struct.unpack_from("<I", data, opt_off + 32)[0]
    file_alignment = struct.unpack_from("<I", data, opt_off + 36)[0]
    size_of_image = struct.unpack_from("<I", data, opt_off + 56)[0]
    dd_off = opt_off + 112
    data_dirs = []
    for i, name in enumerate(DATA_DIR_NAMES):
        va, sz = struct.unpack_from("<II", data, dd_off + i * 8)
        data_dirs.append((name, va, sz))
    sec_off = opt_off + opt_size
    sections = []
    for i in range(num_sec):
        raw = data[sec_off + i * 40 : sec_off + (i + 1) * 40]
        name = raw[:8].decode("latin-1", errors="replace").rstrip("\x00")
        vsize, va, rsize, rptr = struct.unpack_from("<IIII", raw, 8)
        chars = struct.unpack_from("<I", raw, 36)[0]
        sections.append({
            "name": name, "vsize": vsize, "va": va, "rsize": rsize,
            "rptr": rptr, "chars": chars,
        })
    return {
        "machine": machine, "num_sec": num_sec, "timestamp": ts,
        "characteristics": characteristics, "magic": magic,
        "ep_rva": ep_rva, "img_base": img_base,
        "section_alignment": section_alignment,
        "file_alignment": file_alignment,
        "size_of_image": size_of_image,
        "data_dirs": data_dirs, "sections": sections,
    }


def rva_to_offset(rva: int, sections) -> int | None:
    """Translate a virtual RVA to file offset. None if the RVA falls in a
    section that has no raw data on disk (rsize == 0)."""
    for s in sections:
        end = s["va"] + max(s["vsize"], s["rsize"])
        if s["va"] <= rva < end:
            if s["rsize"] == 0:
                return None
            return s["rptr"] + (rva - s["va"])
    return None


def section_for_rva(rva: int, sections):
    for s in sections:
        end = s["va"] + max(s["vsize"], s["rsize"])
        if s["va"] <= rva < end:
            return s
    return None


def fmt_chars(c: int) -> str:
    flags = []
    if c & 0x20: flags.append("CODE")
    if c & 0x40: flags.append("IDATA")
    if c & 0x80: flags.append("UDATA")
    if c & 0x20000000: flags.append("EXEC")
    if c & 0x40000000: flags.append("READ")
    if c & 0x80000000: flags.append("WRITE")
    if c & 0x02000000: flags.append("DISCARD")
    if c & 0x04000000: flags.append("NOCACHE")
    if c & 0x10000000: flags.append("SHARED")
    return "|".join(flags) if flags else "-"


def main(argv):
    path = Path(argv[1]) if len(argv) > 1 else Path("unpacking/bambu_networking.dll")
    data = load(path)
    info = parse(data)
    ts = info["timestamp"]
    print(f"File: {path}  ({len(data):,} bytes)")
    print(f"Machine: 0x{info['machine']:04x}  (0x8664 = x86-64)")
    print(f"Sections: {info['num_sec']}")
    print(f"Timestamp: {ts}  ({_dt.datetime.fromtimestamp(ts, _dt.timezone.utc).isoformat()})")
    print(f"Characteristics: 0x{info['characteristics']:04x}")
    print(f"Magic: 0x{info['magic']:x}  (0x20b = PE32+)")
    print(f"Image base: 0x{info['img_base']:x}")
    print(f"Entry point RVA: 0x{info['ep_rva']:x}  (VA 0x{info['img_base']+info['ep_rva']:x})")
    print(f"Section alignment: 0x{info['section_alignment']:x}")
    print(f"File alignment: 0x{info['file_alignment']:x}")
    print(f"Size of image: 0x{info['size_of_image']:x}")
    print()
    print("Data directories (non-empty):")
    for n, va, sz in info["data_dirs"]:
        if va or sz:
            print(f"  {n:14s} VA=0x{va:08x}  Size=0x{sz:x}")
    print()
    print(f"{'Name':<10}{'VSize':>12}{'VA':>12}{'RSize':>12}{'RawPtr':>12}  Chars")
    for s in info["sections"]:
        print(f"{s['name']:<10}{s['vsize']:>12x}{s['va']:>12x}"
              f"{s['rsize']:>12x}{s['rptr']:>12x}  0x{s['chars']:08x} "
              f"{fmt_chars(s['chars'])}")
    print()
    ep_sec = section_for_rva(info["ep_rva"], info["sections"])
    print(f"Entry-point section: {ep_sec['name'] if ep_sec else '<none>'}")


if __name__ == "__main__":
    main(sys.argv)
