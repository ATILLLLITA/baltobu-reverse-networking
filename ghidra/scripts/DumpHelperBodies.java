// Bulk-decompile every function that's called by a bambu_network_* or
// ft_* export and write each body to a file. Plus a single concatenated
// dump for grep-based analysis.
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

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.HashSet;
import java.util.TreeMap;

public class DumpHelperBodies extends GhidraScript {

    @Override
    public void run() throws Exception {
        Program program = currentProgram;
        DecompInterface d = new DecompInterface();
        d.setOptions(new DecompileOptions());
        d.setSimplificationStyle("decompile");
        d.openProgram(program);

        // Collect callees of every named C export.
        HashSet<Long> seen = new HashSet<>();
        TreeMap<Long, Function> helpers = new TreeMap<>();
        var iter = program.getSymbolTable().getAllSymbols(true);
        while (iter.hasNext()) {
            var s = iter.next();
            if (!s.isExternalEntryPoint()) continue;
            String nm = s.getName();
            if (!nm.startsWith("bambu_network_") && !nm.startsWith("ft_")) continue;
            Function f = getFunctionAt(s.getAddress());
            if (f == null) continue;
            for (Function callee : f.getCalledFunctions(monitor)) {
                long va = callee.getEntryPoint().getOffset();
                if (va < 0x180001000L || va > 0x180600000L) continue;
                if (seen.add(va)) helpers.put(va, callee);
            }
        }
        println("Decompiling " + helpers.size() + " helpers...");

        String outDir = System.getProperty("user.dir")
            + "/output/ghidra_decompile/helpers";
        new File(outDir).mkdirs();
        File concat = new File(System.getProperty("user.dir")
            + "/output/ghidra_decompile/all_helpers.c");

        try (PrintWriter all = new PrintWriter(new FileWriter(concat))) {
            int n = 0;
            for (var e : helpers.entrySet()) {
                if (monitor.isCancelled()) break;
                Function f = e.getValue();
                DecompileResults dr = d.decompileFunction(f, 30, monitor);
                if (dr == null || dr.getDecompiledFunction() == null) continue;
                String body = dr.getDecompiledFunction().getC();
                String header = String.format(
                    "// ===== %s @ 0x%x (size %d) =====%n",
                    f.getName(), e.getKey(),
                    f.getBody().getNumAddresses());
                File out = new File(outDir, String.format("%s_0x%x.c",
                    f.getName(), e.getKey()));
                try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
                    w.print(header);
                    w.print(body);
                }
                all.print(header);
                all.print(body);
                all.println();
                n++;
                if (n % 32 == 0) println("decompiled " + n + "/" + helpers.size());
            }
        }
        d.dispose();
        println("done.");
    }
}
