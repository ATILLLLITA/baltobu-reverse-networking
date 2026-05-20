// Walk every agent-internal helper called by an export, decompile it,
// and extract every `*param_1 + offset` access pattern. That gives us a
// histogram of which struct offsets each helper touches — i.e. the
// agent's internal struct layout, derived from real code rather than
// from a debugger session.
//
// Output: output/ghidra/agent_struct_field_map.tsv
//   columns: offset_hex, helper_va, helper_name
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
import java.util.ArrayList;
import java.util.List;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class MapAgentStruct extends GhidraScript {

    // The 117 helper VAs derived from output/unpacking/agent_internal_helpers.txt.
    // We re-derive them here from the export table to keep the script
    // self-contained. We don't filter by name; we walk every called
    // function from every external entry point that touches the global
    // agent at 0x18007a99e0.

    private static final long AGENT_GLOBAL = 0x18007a99e0L;

    @Override
    public void run() throws Exception {
        Program program = currentProgram;
        DecompInterface d = new DecompInterface();
        d.setOptions(new DecompileOptions());
        d.setSimplificationStyle("decompile");
        d.openProgram(program);

        // Pattern matches: param_X[N] or *(... )(param_X + 0xXXX) or
        // (lVar1 + 0xXXX) where lVar1 came from *param_X. We grab the
        // simpler "+ 0xXXX" pattern from anywhere in the body where a
        // dereferenced struct base is used.
        Pattern p1 = Pattern.compile("\\+\\s*0x([0-9a-fA-F]+)\\b");

        TreeMap<Long, TreeSet<String>> offsetToHelpers = new TreeMap<>();
        List<long[]> helpers = new ArrayList<>();

        // Collect every called function from every external entry point.
        var iter = program.getSymbolTable().getAllSymbols(true);
        java.util.HashSet<Long> seen = new java.util.HashSet<>();
        while (iter.hasNext()) {
            var s = iter.next();
            if (!s.isExternalEntryPoint()) continue;
            String nm = s.getName();
            if (!nm.startsWith("bambu_network_") && !nm.startsWith("ft_")) continue;
            Function f = getFunctionAt(s.getAddress());
            if (f == null) continue;
            for (Function callee : f.getCalledFunctions(monitor)) {
                long va = callee.getEntryPoint().getOffset();
                // Heuristic: agent-internal helpers cluster around
                // 0x1801a0000..0x1801b0000 (the BambuNetworkAgent
                // method block) but also extend up into the rest of
                // .text. Limit to functions in .text proper (low GiB
                // range, away from EXT_).
                if (va < 0x180001000L || va > 0x180600000L) continue;
                if (seen.add(va)) helpers.add(new long[]{va, 0L});
            }
        }
        println("Collected " + helpers.size() + " callees to inspect.");

        int processed = 0;
        for (long[] entry : helpers) {
            if (monitor.isCancelled()) break;
            Address addr = program.getAddressFactory()
                .getDefaultAddressSpace().getAddress(entry[0]);
            Function f = getFunctionAt(addr);
            if (f == null) continue;
            DecompileResults dr = d.decompileFunction(f, 30, monitor);
            if (dr == null || dr.getDecompiledFunction() == null) continue;
            String body = dr.getDecompiledFunction().getC();
            // Only consider helpers that actually use the global agent;
            // otherwise we'd flood with offsets unrelated to the agent.
            // Look for the AGENT_GLOBAL constant (0x1807a99e0 in the
            // rebased program); Ghidra usually emits it as DAT_1807a99e0
            // or 0x1807a99e0.
            if (!body.contains("DAT_1807a99e0")) continue;
            Matcher m = p1.matcher(body);
            while (m.find()) {
                long off;
                try {
                    off = Long.parseLong(m.group(1), 16);
                } catch (NumberFormatException e) { continue; }
                // Filter: offsets > 0x10000 are usually code/data addresses
                // baked into the function (literal addresses), not struct
                // offsets. Real struct offsets stay under ~0x4000.
                if (off > 0x10000) continue;
                if (off < 0x10) continue;  // tiny offsets are usually loop counters
                offsetToHelpers.computeIfAbsent(off, k -> new TreeSet<>())
                    .add(String.format("0x%x:%s", entry[0], f.getName()));
            }
            processed++;
            if (processed % 16 == 0)
                println("processed " + processed + "/" + helpers.size());
        }
        d.dispose();

        File out = new File(System.getProperty("user.dir")
            + "/output/ghidra/agent_struct_field_map.tsv");
        out.getParentFile().mkdirs();
        try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
            w.println("offset_hex\toffset_dec\thelpers_count\thelpers");
            for (var e : offsetToHelpers.entrySet()) {
                long off = e.getKey();
                var s = e.getValue();
                w.printf("0x%x\t%d\t%d\t%s%n",
                    off, off, s.size(),
                    String.join(",", s));
            }
        }
        println("wrote " + out + " (" + offsetToHelpers.size() + " distinct offsets)");
    }
}
