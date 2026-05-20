// Headless Ghidra script: decompile every named export and emit a
// committed signatures TSV + per-function .c sources (gitignored).
//
// Clean-room boundary policy (see top-level README + .gitignore):
//   - `output/ghidra_decompile/` is gitignored. Raw decompiled C sources
//     for *Bambu* code (i.e. the exports) MUST NOT be committed; this
//     script writes them there. Anyone implementing the clean-room
//     replacement must not read these files.
//   - The signatures TSV (function name + arg/return types + size) is
//     committed under `output/ghidra/`. Signatures describe the ABI;
//     the ABI is what AGPLv3 entitles us to recover. Bodies are not.
//
// Outputs:
//   output/ghidra_decompile/exports/<name>.c          (gitignored)
//   output/ghidra/exports_signatures.tsv              (committed)
//
// @category baltobu
// @author baltobu

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Parameter;
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

public class DecompileExports extends GhidraScript {

    private static final String ROOT = System.getProperty("user.dir");
    private static final String C_DIR = ROOT + "/output/ghidra_decompile/exports";
    private static final String SIG_TSV = ROOT + "/output/ghidra/exports_signatures.tsv";
    private static final int DECOMP_TIMEOUT_SEC = 60;

    private static class Row {
        String name;
        long rva;
        long va;
        long size;
        int callees;
        String returnType;
        String args;
        String calleeNames;
        boolean decompiled;
        String error;
    }

    @Override
    public void run() throws Exception {
        Program program = currentProgram;
        long base = program.getImageBase().getOffset();
        new File(C_DIR).mkdirs();
        new File(SIG_TSV).getParentFile().mkdirs();

        SymbolTable symtab = program.getSymbolTable();
        DecompInterface decomp = new DecompInterface();
        decomp.setOptions(new DecompileOptions());
        decomp.setSimplificationStyle("decompile");
        decomp.openProgram(program);

        List<Row> rows = new ArrayList<>();
        SymbolIterator it = symtab.getAllSymbols(true);
        int count = 0;
        while (it.hasNext()) {
            if (monitor.isCancelled()) break;
            Symbol s = it.next();
            if (!s.isExternalEntryPoint()) continue;
            String name = s.getName();
            if (name.startsWith("Ordinal_")) continue;
            if (name.startsWith("EXT_")) continue;
            // Some Ghidra-synthesized labels we don't want.
            if (name.equals("entry") || name.startsWith("FUN_")) continue;

            Row r = new Row();
            r.name = name;
            r.va = s.getAddress().getOffset();
            r.rva = r.va - base;

            Function f = getFunctionAt(s.getAddress());
            if (f == null) f = getFunctionContaining(s.getAddress());
            if (f == null) {
                r.error = "no-function";
                rows.add(r);
                continue;
            }

            r.size = f.getBody().getNumAddresses();
            Set<Function> callees = f.getCalledFunctions(monitor);
            r.callees = callees == null ? 0 : callees.size();
            StringBuilder cn = new StringBuilder();
            if (callees != null) {
                int i = 0;
                for (Function c : callees) {
                    if (i++ > 0) cn.append(",");
                    cn.append(c.getName());
                    if (cn.length() > 256) { cn.append("..."); break; }
                }
            }
            r.calleeNames = cn.toString();

            DecompileResults dr = decomp.decompileFunction(
                f, DECOMP_TIMEOUT_SEC, monitor);
            if (dr == null || dr.getDecompiledFunction() == null) {
                r.error = "decompile-null";
                if (dr != null) r.error = "decompile:" + dr.getErrorMessage();
            } else {
                r.decompiled = true;
                String body = dr.getDecompiledFunction().getC();
                File out = new File(C_DIR, sanitize(name) + ".c");
                try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
                    w.print(body);
                }
            }

            // Signature: trust the Function's recovered prototype over
            // the decompiler's signature string — the latter sometimes
            // includes the body parenthetically.
            Parameter[] params = f.getParameters();
            r.returnType = f.getReturn() != null
                && f.getReturn().getDataType() != null
                    ? f.getReturn().getDataType().getName()
                    : "?";
            StringBuilder ab = new StringBuilder();
            for (int i = 0; i < params.length; i++) {
                if (i > 0) ab.append(", ");
                Parameter p = params[i];
                String t = p.getDataType() != null
                    ? p.getDataType().getName() : "?";
                String pn = p.getName();
                ab.append(t).append(" ").append(pn);
            }
            r.args = ab.toString();

            rows.add(r);
            count++;
            if (count % 16 == 0) {
                println(String.format(
                    "[DecompileExports] processed %d/%d (latest: %s, %d bytes)",
                    count, rows.size(), name, r.size));
            }
        }
        decomp.dispose();

        rows.sort((a, b) -> Long.compare(a.rva, b.rva));

        try (PrintWriter w = new PrintWriter(new FileWriter(SIG_TSV))) {
            w.println("name\trva\tva\tsize\tcallees\treturn_type\targs\tcallee_names\tdecompiled\terror");
            for (Row r : rows) {
                w.printf("%s\t0x%x\t0x%x\t%d\t%d\t%s\t%s\t%s\t%s\t%s%n",
                    r.name,
                    r.rva, r.va,
                    r.size, r.callees,
                    r.returnType == null ? "" : r.returnType,
                    r.args == null ? "" : r.args,
                    r.calleeNames == null ? "" : r.calleeNames,
                    r.decompiled ? "yes" : "no",
                    r.error == null ? "" : r.error);
            }
        }

        int ok = 0, bad = 0;
        for (Row r : rows) {
            if (r.decompiled) ok++; else bad++;
        }
        println(String.format(
            "[DecompileExports] %d exports, %d decompiled, %d failed",
            rows.size(), ok, bad));
        println("[DecompileExports] signatures: " + SIG_TSV);
        println("[DecompileExports] bodies (gitignored): " + C_DIR);
    }

    private static String sanitize(String s) {
        // Replace any chars that aren't safe for filenames. Our exports
        // are all [a-z0-9_], so this is mostly defensive.
        return s.replaceAll("[^A-Za-z0-9._-]", "_");
    }
}
