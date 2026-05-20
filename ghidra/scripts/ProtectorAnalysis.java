// Headless Ghidra script: decompile the 42 real functions inside `.1M@`.
//
// These are the only functions in the binary that have actual bytes on
// disk - everything else is hallucinated by the auto-analyzer because the
// protector's section table reserves virtual ranges that are empty in the
// file. The 42 functions here together comprise the protector / unpacker
// stub.
//
// Output: output/ghidra/protector/
//   - protector_all.c          all 42 functions concatenated
//   - protector_inventory.tsv  one row per function (entry, size, callees)
//   - F_<addr>.c               per-function decompilation
//
// We deliberately decompile *only* this region. The 1-byte phantom
// functions in `.text` / `.o1}` are not real code and would crash or
// timeout the decompiler.
//
// @category baltobu
// @author baltobu

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Program;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import java.util.TreeSet;

public class ProtectorAnalysis extends GhidraScript {

    private static final String OUT_DIR =
        System.getProperty("user.dir") + "/output/ghidra/protector";

    // Only decompile functions in this block (the packed section).
    private static final String TARGET_BLOCK = ".1M@";

    // Skip the tiny 1-byte hallucinations.
    private static final int MIN_SIZE = 2;

    @Override
    public void run() throws Exception {
        Program program = currentProgram;
        File outDir = new File(OUT_DIR);
        outDir.mkdirs();
        println("[ProtectorAnalysis] writing to " + outDir);

        // Collect target functions.
        List<Function> targets = new ArrayList<>();
        FunctionIterator iter = program.getFunctionManager().getFunctions(true);
        while (iter.hasNext()) {
            Function f = iter.next();
            MemoryBlock blk = program.getMemory().getBlock(f.getEntryPoint());
            if (blk == null || !TARGET_BLOCK.equals(blk.getName())) continue;
            if (f.getBody().getNumAddresses() < MIN_SIZE) continue;
            targets.add(f);
        }
        targets.sort((a, b) -> a.getEntryPoint().compareTo(b.getEntryPoint()));
        println("[ProtectorAnalysis] " + targets.size() +
                " real functions in " + TARGET_BLOCK);

        DecompInterface decomp = new DecompInterface();
        decomp.setOptions(new DecompileOptions());
        decomp.setSimplificationStyle("decompile");
        if (!decomp.openProgram(program)) {
            println("[ProtectorAnalysis] decompiler failed to open: "
                    + decomp.getLastMessage());
            return;
        }

        ReferenceManager refs = program.getReferenceManager();

        PrintWriter allW = new PrintWriter(new FileWriter(
                new File(outDir, "protector_all.c")));
        PrintWriter invW = new PrintWriter(new FileWriter(
                new File(outDir, "protector_inventory.tsv")));
        invW.println("address\tsize\tname\tnum_callees\tnum_xrefs_in");

        int ok = 0;
        int fail = 0;
        for (Function f : targets) {
            if (monitor.isCancelled()) break;

            // Inventory row
            Set<Function> callees = f.getCalledFunctions(monitor);
            int xrefIn = 0;
            for (Iterator<Reference> rit = refs.getReferencesTo(f.getEntryPoint()).iterator();
                 rit.hasNext(); rit.next()) {
                xrefIn++;
            }
            invW.printf("0x%x\t%d\t%s\t%d\t%d%n",
                f.getEntryPoint().getOffset(),
                f.getBody().getNumAddresses(),
                f.getName(),
                callees == null ? 0 : callees.size(),
                xrefIn);

            // Per-function decompilation
            DecompileResults res = decomp.decompileFunction(f, 120, monitor);
            String header = "// ========== " + f.getName() +
                " @ 0x" + Long.toHexString(f.getEntryPoint().getOffset()) +
                "  size=" + f.getBody().getNumAddresses() + " ==========";
            allW.println(header);
            if (res.getDecompiledFunction() != null) {
                String c = res.getDecompiledFunction().getC();
                allW.println(c);
                File f1 = new File(outDir,
                    String.format("F_0x%x.c", f.getEntryPoint().getOffset()));
                PrintWriter pw = new PrintWriter(new FileWriter(f1));
                pw.println("// " + f.getName());
                pw.println("// 0x" + Long.toHexString(f.getEntryPoint().getOffset())
                    + "  size=" + f.getBody().getNumAddresses());
                pw.println();
                pw.println(c);
                pw.close();
                ok++;
            } else {
                allW.println("// (decompilation failed: " + res.getErrorMessage() + ")");
                fail++;
            }
            allW.println();
        }

        allW.close();
        invW.close();
        decomp.dispose();
        println("[ProtectorAnalysis] decompiled OK=" + ok + " FAIL=" + fail);
    }
}
