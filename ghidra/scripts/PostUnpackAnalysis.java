// Headless Ghidra script: post-unpack export inventory.
//
// Run this against the *unpacked* DLL (the Scylla-rebuilt dump) after
// loading it into Ghidra. It emits a per-export summary so we can see
// at a glance which exports decompiled into normal-looking C and which
// ones are suspiciously short / hostile (= candidates for VM-protected
// functions).
//
// Outputs (written under output/ghidra_unpacked/ so we don't clobber the
// packed-DLL artefacts under output/ghidra/):
//   output/ghidra_unpacked/exports.tsv
//   output/ghidra_unpacked/exports_summary.md
//   output/ghidra_unpacked/suspicious.tsv  (exports that look VM'd)
//
// Heuristics for "suspicious":
//   - body smaller than 16 bytes (probably a stub)
//   - or body larger than 64 KiB (probably an inlined VM dispatcher)
//   - or zero callees with size > 256 bytes (the VM-handler shape)
//   - or contains the "Could not recover jumptable" decompiler warning
//
// @category baltobu
// @author baltobu

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Program;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import ghidra.program.model.symbol.SymbolTable;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.Set;

public class PostUnpackAnalysis extends GhidraScript {

    private static final String OUT_DIR =
        System.getProperty("user.dir") + "/output/ghidra_unpacked";

    private static final int MIN_REAL_SIZE = 16;
    private static final int MAX_REAL_SIZE = 65536;
    private static final int LARGE_NO_CALLEES = 256;

    private static class Row {
        String name;
        long rva;
        long va;
        long size;
        int callees;
        boolean hasJumpTableWarning;
        boolean suspicious;
        String why;
    }

    @Override
    public void run() throws Exception {
        Program program = currentProgram;
        long base = program.getImageBase().getOffset();
        File outDir = new File(OUT_DIR);
        outDir.mkdirs();

        SymbolTable symtab = program.getSymbolTable();
        DecompInterface decomp = new DecompInterface();
        decomp.setOptions(new DecompileOptions());
        decomp.setSimplificationStyle("decompile");
        decomp.openProgram(program);

        List<Row> rows = new ArrayList<>();
        SymbolIterator it = symtab.getAllSymbols(true);
        while (it.hasNext()) {
            if (monitor.isCancelled()) break;
            Symbol s = it.next();
            if (!s.isExternalEntryPoint()) continue;
            if (s.getName().startsWith("Ordinal_")) continue;
            Row r = new Row();
            r.name = s.getName();
            r.va = s.getAddress().getOffset();
            r.rva = r.va - base;
            Function f = getFunctionAt(s.getAddress());
            if (f == null) f = getFunctionContaining(s.getAddress());
            r.size = f == null ? 0 : f.getBody().getNumAddresses();
            Set<Function> callees = f == null ? null
                : f.getCalledFunctions(monitor);
            r.callees = callees == null ? 0 : callees.size();
            if (f != null) {
                DecompileResults dr = decomp.decompileFunction(f, 30, monitor);
                String err = dr == null ? "" : dr.getErrorMessage();
                String c = dr == null || dr.getDecompiledFunction() == null
                    ? ""
                    : dr.getDecompiledFunction().getC();
                r.hasJumpTableWarning = (c != null && c.contains(
                    "Could not recover jumptable"))
                    || (err != null && err.contains(
                        "Could not recover jumptable"));
            }
            // Heuristics
            StringBuilder why = new StringBuilder();
            if (r.size < MIN_REAL_SIZE) why.append("tiny;");
            if (r.size > MAX_REAL_SIZE) why.append("huge;");
            if (r.callees == 0 && r.size > LARGE_NO_CALLEES)
                why.append("no-callees;");
            if (r.hasJumpTableWarning) why.append("jumptable-warning;");
            r.suspicious = why.length() > 0;
            r.why = why.toString();
            rows.add(r);
        }
        decomp.dispose();

        rows.sort((a, b) -> Long.compare(a.rva, b.rva));

        try (PrintWriter w = new PrintWriter(new FileWriter(
                new File(outDir, "exports.tsv")))) {
            w.println("name\trva\tva\tsize\tcallees\tjumptable_warn\tsuspicious\twhy");
            for (Row r : rows) {
                w.printf("%s\t0x%x\t0x%x\t%d\t%d\t%s\t%s\t%s%n",
                    r.name, r.rva, r.va, r.size, r.callees,
                    r.hasJumpTableWarning ? "yes" : "no",
                    r.suspicious ? "yes" : "no",
                    r.why);
            }
        }

        List<Row> suspicious = new ArrayList<>();
        for (Row r : rows) if (r.suspicious) suspicious.add(r);

        try (PrintWriter w = new PrintWriter(new FileWriter(
                new File(outDir, "suspicious.tsv")))) {
            w.println("name\trva\tsize\tcallees\twhy");
            for (Row r : suspicious) {
                w.printf("%s\t0x%x\t%d\t%d\t%s%n",
                    r.name, r.rva, r.size, r.callees, r.why);
            }
        }

        try (PrintWriter w = new PrintWriter(new FileWriter(
                new File(outDir, "exports_summary.md")))) {
            w.println("# Post-unpack export inventory\n");
            w.println("- Total exports: **" + rows.size() + "**");
            w.println("- Suspicious (possible VM-protected): **"
                + suspicious.size() + "**");
            w.println();
            int normal = rows.size() - suspicious.size();
            int avgSize = 0;
            for (Row r : rows) avgSize += r.size;
            avgSize = rows.isEmpty() ? 0 : avgSize / rows.size();
            w.println("- Average export body size: **" + avgSize + "** bytes");
            w.println("- Normal-looking exports: **" + normal + "**");
            w.println();
            if (!suspicious.isEmpty()) {
                w.println("## Suspicious exports\n");
                w.println("| Export | RVA | Size | Callees | Why |");
                w.println("|---|---|---:|---:|---|");
                for (Row r : suspicious) {
                    w.printf("| `%s` | `0x%x` | %d | %d | %s |%n",
                        r.name, r.rva, r.size, r.callees, r.why);
                }
            }
        }
        println("[PostUnpackAnalysis] wrote " + outDir);
        println("[PostUnpackAnalysis] " + rows.size() + " exports, "
            + suspicious.size() + " suspicious");
    }
}
