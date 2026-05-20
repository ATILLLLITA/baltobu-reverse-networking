# `bambu_network_create_agent`

**Group:** agent lifecycle
**RVA:** `0x1b6d00` &nbsp; **VA (image base 0x180000000):** `0x1801b6d00`
**Body size:** 5 bytes &nbsp; **Direct callees:** decoder failure (see below)

## Status

Body recovery is blocked by an anti-disassembly stub. The ABI shape is
nevertheless fully constrained by how the rest of the library uses the
returned handle.

## Anti-disassembly stub

The first instruction Ghidra disassembles at `0x1801b6d00` does not
form a valid x86-64 flow. The decompiler emits:

```
WARNING: Bad instruction - Truncating control flow here
halt_baddata();
```

and the recovered "function" is just a 5-byte stub that aborts
disassembly. This is the signature of one of VMProtect's
anti-disassembly techniques, most likely an overlapping-instruction
trick where the linker placed the real entry point a few bytes after
`0x1801b6d00` and prefixed it with garbage bytes that resemble the
start of a multi-byte instruction. A linear-sweep disassembler decodes
the garbage and steers past the real entry; the real flow enters from
a sibling `jmp` or via the protector's import-resolver bootstrap.

This is the only export in the dump that exhibits this pattern; the
other 129 entry points decompile cleanly. `create_agent` is the
singleton factory for the central agent object, which makes it the
obvious target for such a hook.

## Signature

From the surrounding pattern of `bambu_network_*` exports and from the
shape of how every agent-taking export uses the returned pointer:

```c
agent_t *bambu_network_create_agent(void);
```

`agent_t` is an opaque 16-byte handle. The handle returned is later
checked against the global at `.data` RVA `0x7a99e0` (`DAT_1807a99e0`)
by every other `bambu_network_*` export.

## Contract derived from cross-references

Reading `destroy_agent`, `init_log`, and `start` reveals that the agent
singleton lives in a global at `.data` RVA `0x7a99e0`. Every
agent-taking export checks `g_agent == passed_arg` and uses the global
from then on. From this we can constrain `create_agent`'s contract
even without recovering its body.

`create_agent` writes the freshly-allocated agent pointer to the
global at `0x7a99e0`. It returns the same pointer to the caller as a
self-check token for subsequent calls. The allocation size at
`destroy_agent`'s `operator delete` is `0x10` bytes, which indicates
the returned handle is a 16-byte wrapper that points at a separately
allocated 2744-byte agent state (the deep destructor calls
`operator delete(state, 0xab8)` on the inner allocation).

## Inferred behaviour

`create_agent` is expected to allocate and zero-initialise the agent's
state structure on the heap (a 16-byte wrapper plus a 2744-byte real
state), initialise the callback slots to null, and return the agent
handle. The lifecycle observations from the rest of the API require
`set_config_dir` to have been called first. Failure returns null.

## Recovering the body

Two paths recover the real body. The first is manual disassembly of
the bytes at `0x1801b6d00` with a reorienting disassembler (radare2's
`pd@x:..`, IDA's `c` command, or Ghidra after a manual instruction
override at the entry). The second is dynamic introspection via Frida:
hook the export, observe the return value, and read it back as a
pointer. The Frida path is the cheapest and yields the contract that
matters most for a clean-room implementation.
