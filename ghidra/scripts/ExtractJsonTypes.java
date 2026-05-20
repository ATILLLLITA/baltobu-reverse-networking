// For every json::operator[] callsite, look back at the immediately
// preceding "local_X[0] = <N>" tag store. nlohmann::json's internal
// representation stores a value_t tag at offset 0 of the json node, and
// MSVC emits that tag literal just before constructing the value.
//
// nlohmann::json::value_t enum:
//   0=null  1=object  2=array  3=string  4=boolean
//   5=int   6=uint    7=float  8=binary  15=discarded
//
// We grep the decompiled C body for the sequence:
//   <var>[0] = <N>;            // type tag
//   ...                        // construct value
//   FUN_1800e6ca0(_, "key")    // assign into j["key"]
//
// And output (function_va, key, inferred_type).
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
import ghidra.program.model.mem.Memory;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.Iterator;
import java.util.TreeMap;
import java.util.TreeSet;
import java.util.regex.Matcher;
import java.util.regex.Pattern;

public class ExtractJsonTypes extends GhidraScript {

    private static final long JSON_OP = 0x1800e6ca0L;

    private static final String[] TYPE_NAMES = {
        "null", "object", "array", "string", "boolean",
        "int", "uint", "float", "binary", "?", "?", "?",
        "?", "?", "?", "discarded"
    };

    @Override
    public void run() throws Exception {
        Program program = currentProgram;
        ReferenceManager refs = program.getReferenceManager();
        Address opAddr = program.getAddressFactory()
            .getDefaultAddressSpace().getAddress(JSON_OP);

        TreeSet<Long> targets = new TreeSet<>();
        Iterator<Reference> it = refs.getReferencesTo(opAddr).iterator();
        while (it.hasNext()) {
            Reference r = it.next();
            if (!r.getReferenceType().isCall()) continue;
            Function f = getFunctionContaining(r.getFromAddress());
            if (f != null) targets.add(f.getEntryPoint().getOffset());
        }
        println("targets: " + targets.size());

        DecompInterface d = new DecompInterface();
        d.setOptions(new DecompileOptions());
        d.setSimplificationStyle("decompile");
        d.openProgram(program);

        // Patterns:
        //   "local_38[0] = 3;"               <- tag assignment (number 0..15)
        //   FUN_1800e6ca0(_, "key")          <- index by literal key
        //   FUN_1800e6ca0(_, &DAT_180XXXXX)  <- index by .rdata literal
        // Match both Ghidra forms:
        //   local_38[0] = 3;
        //   local_38._0_1_ = 3;
        Pattern tagPat = Pattern.compile(
            "(\\w+)(?:\\[0\\]|\\._0_1_)\\s*=\\s*(\\d+)\\s*;");
        Pattern callLit = Pattern.compile("FUN_1800e6ca0\\s*\\([^,]+,\\s*\"([^\"]+)\"");
        Pattern callDat = Pattern.compile("FUN_1800e6ca0\\s*\\([^,]+,\\s*&DAT_180([0-9a-fA-F]+)\\)");

        TreeMap<Long, TreeMap<String, String>> funcKeys = new TreeMap<>();
        Memory mem = program.getMemory();
        int n = 0;
        for (long va : targets) {
            if (monitor.isCancelled()) break;
            Address a = program.getAddressFactory()
                .getDefaultAddressSpace().getAddress(va);
            Function f = getFunctionAt(a);
            if (f == null) continue;
            DecompileResults dr = d.decompileFunction(f, 30, monitor);
            if (dr == null || dr.getDecompiledFunction() == null) continue;
            String body = dr.getDecompiledFunction().getC();

            // Walk lines, tracking the most recent tag assignment.
            String[] lines = body.split("\\n");
            String currentTag = null;
            TreeMap<String, String> keyTypes = new TreeMap<>();
            for (String line : lines) {
                Matcher tm = tagPat.matcher(line);
                if (tm.find()) {
                    int tag = Integer.parseInt(tm.group(2));
                    if (tag >= 0 && tag <= 15) {
                        currentTag = TYPE_NAMES[tag];
                    }
                }
                Matcher cm = callLit.matcher(line);
                if (cm.find()) {
                    String key = cm.group(1);
                    if (currentTag != null) {
                        // Stick the most recent tag against this key
                        keyTypes.merge(key, currentTag, (old, neu) ->
                            old.equals(neu) ? old : old + "|" + neu);
                    } else {
                        keyTypes.putIfAbsent(key, "?");
                    }
                }
                Matcher dm = callDat.matcher(line);
                if (dm.find()) {
                    long rva = Long.parseLong(dm.group(1), 16);
                    String key = readCString(mem, rva);
                    if (key != null) {
                        if (currentTag != null) {
                            keyTypes.merge(key, currentTag, (old, neu) ->
                                old.equals(neu) ? old : old + "|" + neu);
                        } else {
                            keyTypes.putIfAbsent(key, "?");
                        }
                    }
                }
            }
            if (!keyTypes.isEmpty()) funcKeys.put(va, keyTypes);
            n++;
            if (n % 32 == 0) println("processed " + n + "/" + targets.size());
        }
        d.dispose();

        File out = new File(System.getProperty("user.dir")
            + "/output/unpacking/json_keys_typed.tsv");
        try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
            w.println("function_va\tkey\tinferred_type");
            for (var e : funcKeys.entrySet()) {
                for (var ke : e.getValue().entrySet()) {
                    w.printf("0x%x\t%s\t%s%n", e.getKey(),
                        ke.getKey(), ke.getValue());
                }
            }
        }
        println("wrote " + out + " (" + funcKeys.size() + " functions)");
    }

    private String readCString(Memory mem, long rva) {
        try {
            Address a = currentProgram.getAddressFactory()
                .getDefaultAddressSpace().getAddress(0x180000000L + rva);
            byte[] buf = new byte[120];
            mem.getBytes(a, buf);
            int end = 0;
            while (end < buf.length && buf[end] != 0) end++;
            if (end < 1) return null;
            String s = new String(buf, 0, end, "ISO-8859-1");
            if (s.matches("[\\x20-\\x7e]+")) return s;
        } catch (Exception ignored) {}
        return null;
    }
}
