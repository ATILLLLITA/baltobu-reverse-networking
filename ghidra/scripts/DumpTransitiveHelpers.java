// Bulk-decompile every function reachable from a bambu_network_* / ft_*
// export within N levels of call edges. Saves bodies + a transitive
// helper map for offline analysis.
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
import java.util.ArrayDeque;
import java.util.HashSet;
import java.util.TreeMap;
import java.util.TreeSet;

public class DumpTransitiveHelpers extends GhidraScript {

    private static final int MAX_DEPTH = 3;   // covers most real-work helpers
    private static final long TEXT_LO = 0x180001000L;
    private static final long TEXT_HI = 0x180600000L;

    @Override
    public void run() throws Exception {
        Program program = currentProgram;

        // BFS from each export
        HashSet<Long> seen = new HashSet<>();
        TreeMap<Long, Function> queue = new TreeMap<>();
        TreeMap<Long, Integer> depth = new TreeMap<>();
        var iter = program.getSymbolTable().getAllSymbols(true);
        while (iter.hasNext()) {
            var s = iter.next();
            if (!s.isExternalEntryPoint()) continue;
            String nm = s.getName();
            if (!nm.startsWith("bambu_network_") && !nm.startsWith("ft_")) continue;
            Function f = getFunctionAt(s.getAddress());
            if (f == null) continue;
            long va = f.getEntryPoint().getOffset();
            if (seen.add(va)) {
                queue.put(va, f);
                depth.put(va, 0);
            }
        }
        ArrayDeque<Function> bfs = new ArrayDeque<>(queue.values());
        while (!bfs.isEmpty()) {
            if (monitor.isCancelled()) break;
            Function f = bfs.pollFirst();
            long va = f.getEntryPoint().getOffset();
            int dep = depth.get(va);
            if (dep >= MAX_DEPTH) continue;
            for (Function callee : f.getCalledFunctions(monitor)) {
                long cva = callee.getEntryPoint().getOffset();
                if (cva < TEXT_LO || cva > TEXT_HI) continue;
                if (seen.add(cva)) {
                    depth.put(cva, dep + 1);
                    bfs.addLast(callee);
                }
            }
        }
        println("transitive helpers: " + seen.size());

        DecompInterface d = new DecompInterface();
        d.setOptions(new DecompileOptions());
        d.setSimplificationStyle("decompile");
        d.openProgram(program);

        File outDir = new File(System.getProperty("user.dir")
            + "/output/ghidra_decompile/transitive");
        outDir.mkdirs();
        File catFile = new File(System.getProperty("user.dir")
            + "/output/ghidra_decompile/all_transitive.c");

        int n = 0;
        try (PrintWriter cat = new PrintWriter(new FileWriter(catFile))) {
            for (long va : new TreeSet<>(seen)) {
                if (monitor.isCancelled()) break;
                Function f = getFunctionAt(program.getAddressFactory()
                    .getDefaultAddressSpace().getAddress(va));
                if (f == null) continue;
                DecompileResults dr = d.decompileFunction(f, 30, monitor);
                if (dr == null || dr.getDecompiledFunction() == null) continue;
                String body = dr.getDecompiledFunction().getC();
                String header = String.format(
                    "// ===== FUN_%x @ 0x%x (depth %d, size %d) =====%n",
                    va, va, depth.getOrDefault(va, -1),
                    f.getBody().getNumAddresses());
                File out = new File(outDir, String.format("FUN_%x.c", va));
                try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
                    w.print(header);
                    w.print(body);
                }
                cat.print(header);
                cat.print(body);
                cat.println();
                n++;
                if (n % 64 == 0) println("decompiled " + n);
            }
        }
        d.dispose();
        println("done: " + n + " functions");
    }
}
