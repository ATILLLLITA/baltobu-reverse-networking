# `bambu_network_check_debug_consistent`

**Group:** misc / debug
**RVA:** `0x1b6730` &nbsp; **VA:** `0x1801b6730`
**Body size:** 7 bytes &nbsp; **Direct callees:** 0

## Signature

```c
uint8_t bambu_network_check_debug_consistent(uint8_t x);
```

## Behavior

Returns `x ^ 1`. No global agent check, no side effects.

This is a tiny debug-build linkage assertion: the slicer passes a known byte, the library XORs with `1` and returns. If the slicer calls this with `0` it expects `1` back, and with `1` it expects `0`. The check confirms that the library version the slicer compiled against matches the library actually loaded, useful for catching mismatched debug/release builds.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

6-byte body: `mov al, cl; xor al, 1; ret`. The 'suspicious' flag in `output/ghidra_unpacked/suspicious.tsv` is just because of the small size, not because of any anti-disassembly trick.
