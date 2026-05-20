#!/usr/bin/env python3
"""
Extract the Authenticode signature (WIN_CERTIFICATE blob) from
bambu_networking.dll, save the raw PKCS#7 to disk, and (if openssl is
installed) print a human-readable certificate chain.

Why we care: the signature identifies who signed the DLL. Confirming that
the publisher is Bambu Lab (and not, say, a redistributor) is part of
provenance documentation. We will *not* try to forge or strip it.

Outputs:
    output/dll_signature.der         - raw PKCS#7
    output/dll_signature_certs.txt   - openssl -text dump (if available)

Run:
    python3 scripts/extract_signature.py
"""
from __future__ import annotations
import shutil
import struct
import subprocess
import sys
from pathlib import Path

sys.path.insert(0, str(Path(__file__).resolve().parent))
import pe_analyze  # noqa: E402

OUT_DER = Path("output/dll_signature.der")
OUT_TXT = Path("output/dll_signature_certs.txt")


def main():
    data = Path("unpacking/bambu_networking.dll").read_bytes()
    info = pe_analyze.parse(data)
    sec_va, sec_sz = info["data_dirs"][4][1:]
    if not sec_va or not sec_sz:
        print("No Security directory; binary is unsigned.")
        return
    # Security directory's VA is unusual: it's a *file offset*, not an RVA.
    blob = data[sec_va : sec_va + sec_sz]
    # WIN_CERTIFICATE header: dwLength (4) | wRevision (2) | wCertType (2)
    length, revision, cert_type = struct.unpack_from("<IHH", blob)
    print(f"WIN_CERTIFICATE  length={length}  rev=0x{revision:04x}  type=0x{cert_type:04x}")
    if cert_type != 0x0002:  # WIN_CERT_TYPE_PKCS_SIGNED_DATA
        print(f"Unexpected cert type 0x{cert_type:04x} (expected 0x0002)")
        return
    der = blob[8 : length]
    OUT_DER.write_bytes(der)
    print(f"Wrote {len(der):,} bytes of PKCS#7 to {OUT_DER}")

    openssl = shutil.which("openssl")
    if not openssl:
        print("openssl not in PATH; skipping certificate dump.")
        return
    res = subprocess.run(
        [openssl, "pkcs7", "-inform", "DER", "-in", str(OUT_DER),
         "-print_certs", "-text", "-noout"],
        capture_output=True, text=True,
    )
    OUT_TXT.write_text(res.stdout)
    # Print just the subject / issuer / dates for quick inspection.
    interesting = [
        line.strip() for line in res.stdout.splitlines()
        if any(k in line for k in ("Subject:", "Issuer:", "Not Before",
                                   "Not After"))
    ]
    print("\nCertificate summary:")
    for line in interesting:
        print("  " + line)
    print(f"\nFull dump: {OUT_TXT}")


if __name__ == "__main__":
    main()
