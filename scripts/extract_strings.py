#!/usr/bin/env python3
"""
Pull plaintext strings out of bambu_networking.dll and bucket them by
section. Useful for confirming the packer hypothesis (most strings live
inside the unpacker-managed region) and for cherry-picking interesting
URLs, error messages, or version strings.

The script writes:
    output/strings_all.txt          - every printable run length >= MIN
    output/strings_interesting.txt  - filtered for URL / json / error /
                                      bambu / mqtt / tls / cloud / etc.
    output/strings_per_section.txt  - histogram of where strings live

Run:
    python3 scripts/extract_strings.py
"""
from __future__ import annotations
import re
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import pe_analyze  # noqa: E402

MIN_LEN = 6

INTERESTING = re.compile(
    r"(https?://|\.bambulab\.com|mqtt|tls|x509|json|websocket|"
    r"bambu_|ft_|/api/|cdn|cloud|certificate|error|version|"
    r"public.key|private.key|MQTT|/v[0-9]/|"
    r"User-Agent|Content-Type|Authorization|Bearer)",
    re.IGNORECASE,
)

PRINTABLE = re.compile(rb"[\x20-\x7e\t]{%d,}" % MIN_LEN)
WIDE_RUN = re.compile(rb"(?:[\x20-\x7e]\x00){%d,}" % MIN_LEN)


def section_for_offset(off: int, sections):
    for s in sections:
        if s["rsize"] == 0:
            continue
        if s["rptr"] <= off < s["rptr"] + s["rsize"]:
            return s
    return None


def main():
    data = Path("unpacking/bambu_networking.dll").read_bytes()
    info = pe_analyze.parse(data)
    sections = info["sections"]

    raw_hits: list[tuple[int, str, str]] = []
    for m in PRINTABLE.finditer(data):
        sec = section_for_offset(m.start(), sections)
        raw_hits.append((
            m.start(),
            sec["name"] if sec else "<header/none>",
            m.group(0).decode("latin-1"),
        ))
    for m in WIDE_RUN.finditer(data):
        sec = section_for_offset(m.start(), sections)
        s = m.group(0).decode("utf-16-le", errors="replace")
        raw_hits.append((
            m.start(),
            sec["name"] if sec else "<header/none>",
            f"<wide> {s}",
        ))
    raw_hits.sort()

    out_dir = Path("output")
    (out_dir / "strings_all.txt").write_text(
        "\n".join(f"0x{o:08x} [{sec:<8}] {s}" for o, sec, s in raw_hits)
        + "\n"
    )

    interesting = [h for h in raw_hits if INTERESTING.search(h[2])]
    (out_dir / "strings_interesting.txt").write_text(
        "\n".join(f"0x{o:08x} [{sec:<8}] {s}" for o, sec, s in interesting)
        + "\n"
    )

    counts: dict[str, int] = {}
    for _, sec, _ in raw_hits:
        counts[sec] = counts.get(sec, 0) + 1
    per_sec = "\n".join(
        f"{sec:<12}  {n:>8}" for sec, n in sorted(
            counts.items(), key=lambda kv: -kv[1]
        )
    )
    (out_dir / "strings_per_section.txt").write_text(per_sec + "\n")

    print(f"Total printable runs >= {MIN_LEN}: {len(raw_hits)}")
    print(f"Interesting (URL/json/bambu/etc.): {len(interesting)}")
    print("By section:")
    print(per_sec)


if __name__ == "__main__":
    main()
