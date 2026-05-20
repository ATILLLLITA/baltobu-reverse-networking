# Packed vs unpacked export parity

- Original DLL: `bambu_networking.dll`
- Unpacked dump: `unpacking/dumps/bambu_networking_dump.dll`

| Set                 | Count |
|---------------------|------:|
| Original exports    | 128 |
| Dump exports        | 128 |
| In both             | 128 |
| Only in original    | 0 |
| Only in dump        | 0 |
| RVA mismatches      | 0 |
| Ordinal mismatches  | 0 |

All exports match, same names, same RVAs, same ordinals. The unpacker reconstituted the original `.text` layout in place.
