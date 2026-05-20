// Pre-analysis Ghidra script: remove VMProtect-only sections from the
// program before auto-analysis runs.
//
// The Scylla dump of bambu_networking.dll retains the protector's
// scratch / compressed-payload sections — `.o1}` (~13.6 MiB),
// `.2F;` (~4 KiB), `.1M@` (~21 MiB) — in addition to the populated
// originals (`.text`, `.rdata`, `.data`, `.pdata`). The auto-analyzer
// has no way to know those are dead-on-arrival and will happily spend
// hours chasing references inside them, blowing up the analysis time
// to 1h+ and producing thousands of garbage functions. We saw this
// the first run.
//
// Dropping those memory blocks before analysis means the analyzer only
// touches the real code/data and any references from `.text` into the
// dead regions become unresolved (which is what we want — they were
// always garbage). The export RVAs are unaffected.
//
// Usage (from run_headless.sh): -preScript StripProtectorSections.java
//
// @category baltobu
// @author baltobu

import ghidra.app.script.GhidraScript;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;

import java.util.ArrayList;
import java.util.List;

public class StripProtectorSections extends GhidraScript {

    private static final String[] DROP = {
        ".o1}",
        ".2F;",
        ".1M@",
    };

    @Override
    public void run() throws Exception {
        Memory mem = currentProgram.getMemory();
        List<MemoryBlock> toRemove = new ArrayList<>();
        for (String name : DROP) {
            MemoryBlock b = mem.getBlock(name);
            if (b == null) {
                println("[StripProtectorSections] " + name + " not present (skip)");
                continue;
            }
            toRemove.add(b);
        }
        for (MemoryBlock b : toRemove) {
            println(String.format(
                "[StripProtectorSections] removing %s @ %s..%s (size 0x%x)",
                b.getName(), b.getStart(), b.getEnd(), b.getSize()));
            mem.removeBlock(b, monitor);
        }
        println("[StripProtectorSections] " + toRemove.size()
            + " block(s) removed");
    }
}
