// For every subsystem-tagged log string in .rdata, locate the function
// that references it. The referencing function is the subsystem method
// that emits that log. Output a labeled method graph per subsystem.
//
// @category baltobu
// @author baltobu

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressSetView;
import ghidra.program.model.data.StringDataType;
import ghidra.program.model.data.TerminatedStringDataType;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.listing.Program;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.TreeMap;
import java.util.TreeSet;

public class MapLogStringsToMethods extends GhidraScript {

    // Hand-curated set of subsystem prefixes we want to map.
    private static final String[] PREFIXES = {
        "DeviceSubscribeManager:",
        "DeviceSubscribeManager::",
        "DeviceSubscribeManager update:",
        "MqttConnectionManager:",
        "MqttConnectionManager::",
        "MqttConnectionManager create!",
        "MqttConnectionManager destroy!",
        "MqttClientKeeper:",
        "GenericSubscriptionManager:",
        "AccountManager::",
        "FTJob failed",
        "MqttCloudSubscribeChannel",
        "MqttLocalSubscribeChannel",
        "MqttSubscribeChannel",
        "ChannelOptions",
        "JsonOrJsonBinFramer",
        "SimpleFramer",
        "DirectUploadingTrackingManager",
    };

    @Override
    public void run() throws Exception {
        Program program = currentProgram;
        Memory mem = program.getMemory();
        ReferenceManager refs = program.getReferenceManager();
        Listing listing = program.getListing();

        // .rdata is where the log literals live. Find that block.
        var rdata = mem.getBlock(".rdata");
        if (rdata == null) {
            // The dump's .rdata might be named differently; find by name
            for (var b : mem.getBlocks()) {
                if (b.getName().equals(".rdata")) { rdata = b; break; }
            }
        }
        if (rdata == null) {
            println("No .rdata block found");
            return;
        }
        println(".rdata: " + rdata.getStart() + " .. " + rdata.getEnd());

        // Scan .rdata for null-terminated strings matching our prefixes.
        // Limit to printable ASCII regions to keep it fast.
        Address start = rdata.getStart();
        Address end = rdata.getEnd();
        long startOff = start.getOffset();
        long endOff = end.getOffset();
        long maxLen = endOff - startOff;
        byte[] block = new byte[(int)Math.min(maxLen, 0x300000L)];
        mem.getBytes(start, block);

        TreeMap<String, List<long[]>> hits = new TreeMap<>();
        // hits: prefix -> list of [string_va, function_va]

        int i = 0;
        while (i < block.length - 16) {
            // Look for ASCII start: any of our prefix first characters
            if (block[i] < 0x20 || block[i] > 0x7e) { i++; continue; }
            // Read up to 250 chars or until NUL/non-printable
            int j = i;
            while (j < block.length && j - i < 250
                   && block[j] >= 0x20 && block[j] <= 0x7e) j++;
            if (j < block.length && block[j] == 0 && j - i >= 20) {
                String s = new String(block, i, j - i, "ISO-8859-1");
                for (String p : PREFIXES) {
                    if (s.startsWith(p)) {
                        long va = startOff + i;
                        Address strAddr = start.getNewAddress(va);
                        // Find xrefs to this address
                        Iterator<Reference> it = refs.getReferencesTo(strAddr).iterator();
                        while (it.hasNext()) {
                            Reference r = it.next();
                            Address from = r.getFromAddress();
                            Function f = getFunctionContaining(from);
                            if (f != null) {
                                hits.computeIfAbsent(p, k -> new ArrayList<>())
                                    .add(new long[]{va, f.getEntryPoint().getOffset(),
                                                    s.length() < 200 ? s.length() : 200});
                                // Also store the string itself
                                if (!stringMap.containsKey(va))
                                    stringMap.put(va, s);
                            }
                        }
                        break;
                    }
                }
            }
            i = j + 1;
        }

        // Aggregate: for each prefix, group by function VA to learn how many
        // logs that function emits.
        File out = new File(System.getProperty("user.dir")
            + "/output/unpacking/subsystem_method_map.md");
        try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
            w.println("# Subsystem method map\n");
            w.println("Each row maps a subsystem method (an address in `.text`) to");
            w.println("the log strings it emits. Two methods that emit the same set");
            w.println("of strings are very likely the same one. This lets us pin");
            w.println("down the entry points of `DeviceSubscribeManager` and");
            w.println("`MqttConnectionManager` even when the C++ vtables aren't");
            w.println("statically resolvable.\n");
            for (String prefix : new TreeSet<>(hits.keySet())) {
                List<long[]> entries = hits.get(prefix);
                // Group by function VA
                TreeMap<Long, TreeSet<Long>> byFunc = new TreeMap<>();
                for (long[] e : entries) {
                    byFunc.computeIfAbsent(e[1], k -> new TreeSet<>()).add(e[0]);
                }
                w.printf("## `%s` (%d distinct methods, %d total log-string xrefs)%n%n",
                    prefix, byFunc.size(), entries.size());
                w.println("| Method VA | # logs | Sample log strings |");
                w.println("|---|---:|---|");
                for (var be : byFunc.entrySet()) {
                    long funcVa = be.getKey();
                    TreeSet<Long> strs = be.getValue();
                    StringBuilder sb = new StringBuilder();
                    int k = 0;
                    for (long sva : strs) {
                        String s = stringMap.getOrDefault(sva, "?");
                        if (s.length() > 90) s = s.substring(0, 87) + "…";
                        sb.append("`").append(s.replace("`", "\\`")).append("`");
                        if (++k >= 2) break;
                        sb.append(" • ");
                    }
                    if (strs.size() > 2) sb.append(" • …");
                    w.printf("| `0x%x` | %d | %s |%n", funcVa, strs.size(), sb.toString());
                }
                w.println();
            }
        }
        println("wrote " + out);
    }

    private TreeMap<Long, String> stringMap = new TreeMap<>();
}
