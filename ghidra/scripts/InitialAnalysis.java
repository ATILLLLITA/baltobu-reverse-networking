// Headless Ghidra script: initial inventory of bambu_networking.dll.
//
// After Ghidra's auto-analysis has run, this script emits:
//   - output/ghidra/sections.txt        per-section function and byte counts
//   - output/ghidra/functions.txt       every function Ghidra found
//   - output/ghidra/exports_resolved.tsv each PE export -> Ghidra function +
//                                       size + section
//   - output/ghidra/summary.md           a human-readable summary
//
// We don't decompile anything here - the binary is packed and the bulk of
// .text isn't on disk. This script just answers "what did the auto-analyzer
// actually find?" so we can plan the next stage.
//
// @category baltobu
// @author baltobu

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Program;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import ghidra.program.model.symbol.SymbolTable;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

public class InitialAnalysis extends GhidraScript {

    private static final String OUTPUT_DIR =
        System.getProperty("user.dir") + "/output/ghidra";

    @Override
    public void run() throws Exception {
        Program program = currentProgram;
        long imageBase = program.getImageBase().getOffset();
        println("[InitialAnalysis] program: " + program.getName());
        println("[InitialAnalysis] image base: 0x" + Long.toHexString(imageBase));

        File outDir = new File(OUTPUT_DIR);
        outDir.mkdirs();

        // ---------- Sections / memory blocks ----------
        MemoryBlock[] blocks = program.getMemory().getBlocks();
        Map<String, long[]> sectionCounts = new LinkedHashMap<>();
        for (MemoryBlock b : blocks) {
            // [func count, byte count covered by functions]
            sectionCounts.put(b.getName(), new long[]{0L, 0L});
        }

        FunctionIterator funcIter = program.getFunctionManager().getFunctions(true);
        List<Function> allFuncs = new ArrayList<>();
        while (funcIter.hasNext()) {
            allFuncs.add(funcIter.next());
        }
        println("[InitialAnalysis] functions found: " + allFuncs.size());

        for (Function f : allFuncs) {
            Address ep = f.getEntryPoint();
            MemoryBlock blk = program.getMemory().getBlock(ep);
            if (blk == null) continue;
            long[] c = sectionCounts.get(blk.getName());
            if (c == null) {
                c = new long[]{0L, 0L};
                sectionCounts.put(blk.getName(), c);
            }
            c[0] += 1;
            c[1] += f.getBody().getNumAddresses();
        }

        try (PrintWriter w = new PrintWriter(new FileWriter(
                new File(outDir, "sections.txt")))) {
            w.println("# Section -> function inventory (Ghidra view)");
            w.println("# block_name  start            size       num_funcs  bytes_in_funcs");
            for (MemoryBlock b : blocks) {
                long[] c = sectionCounts.getOrDefault(b.getName(), new long[]{0,0});
                w.printf("%-10s  0x%016x  0x%-8x  %8d  %12d%n",
                    b.getName(),
                    b.getStart().getOffset(),
                    b.getSize(),
                    c[0], c[1]);
            }
        }

        // ---------- Full function table ----------
        try (PrintWriter w = new PrintWriter(new FileWriter(
                new File(outDir, "functions.txt")))) {
            w.println("# All functions Ghidra discovered.");
            w.println("# entry                section     size    name");
            for (Function f : allFuncs) {
                MemoryBlock blk = program.getMemory().getBlock(f.getEntryPoint());
                w.printf("0x%016x  %-10s  %6d  %s%n",
                    f.getEntryPoint().getOffset(),
                    blk == null ? "?" : blk.getName(),
                    f.getBody().getNumAddresses(),
                    f.getName());
            }
        }

        // ---------- Exports resolved ----------
        SymbolTable symtab = program.getSymbolTable();
        SymbolIterator allSyms = symtab.getAllSymbols(true);

        int exportCount = 0;
        int withFunc = 0;
        int tinyThunks = 0;
        try (PrintWriter w = new PrintWriter(new FileWriter(
                new File(outDir, "exports_resolved.tsv")))) {
            w.println("name\trva\tva\tsection\tsize_bytes\tis_function\tlikely_thunk");
            while (allSyms.hasNext()) {
                if (monitor.isCancelled()) break;
                Symbol s = allSyms.next();
                if (!s.isExternalEntryPoint()) continue;
                exportCount++;
                Address addr = s.getAddress();
                long va = addr.getOffset();
                long rva = va - imageBase;
                MemoryBlock blk = program.getMemory().getBlock(addr);
                String section = blk == null ? "?" : blk.getName();
                Function func = getFunctionAt(addr);
                if (func == null) {
                    func = getFunctionContaining(addr);
                }
                long size = func == null ? 0L : func.getBody().getNumAddresses();
                boolean isFunc = func != null && func.getEntryPoint().equals(addr);
                boolean likelyThunk = isFunc && size > 0 && size <= 16;
                if (isFunc) withFunc++;
                if (likelyThunk) tinyThunks++;
                w.printf("%s\t0x%x\t0x%x\t%s\t%d\t%s\t%s%n",
                    s.getName(), rva, va, section, size,
                    isFunc ? "yes" : (func != null ? "in_function" : "no"),
                    likelyThunk ? "yes" : "no");
            }
        }

        // ---------- Summary ----------
        try (PrintWriter w = new PrintWriter(new FileWriter(
                new File(outDir, "summary.md")))) {
            w.println("# Ghidra initial analysis summary\n");
            w.println("Generated by `ghidra/scripts/InitialAnalysis.java`.\n");
            w.println("- Program: `" + program.getName() + "`");
            w.println("- Image base: `0x" + Long.toHexString(imageBase) + "`");
            w.println("- Functions discovered by auto-analyzer: **"
                + allFuncs.size() + "**");
            w.println("- Exports seen by Ghidra: **" + exportCount + "**");
            w.println("- Exports whose address starts a Ghidra-recognised function: **"
                + withFunc + "**");
            w.println("- Exports that look like tiny thunks (<=16 bytes): **"
                + tinyThunks + "**");
            w.println();
            w.println("## Per-section function inventory\n");
            w.println("| Section | Start | Size | Funcs | Bytes in funcs |");
            w.println("|---|---|---|---:|---:|");
            for (MemoryBlock b : blocks) {
                long[] c = sectionCounts.getOrDefault(b.getName(), new long[]{0,0});
                w.printf("| `%s` | `0x%x` | `0x%x` | %d | %d |%n",
                    b.getName(), b.getStart().getOffset(), b.getSize(),
                    c[0], c[1]);
            }
            w.println();
            w.println("See `exports_resolved.tsv` for the per-export breakdown.");
        }

        println("[InitialAnalysis] wrote " + outDir);
    }
}
