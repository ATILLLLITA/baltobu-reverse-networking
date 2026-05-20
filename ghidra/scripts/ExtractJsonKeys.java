// For every function in .text that calls FUN_1800e6ca0 (nlohmann::json
// ::operator[]), record the literal string keys it passes. Output the
// per-function key vocabulary — the JSON schema each function touches.
//
// @category baltobu
// @author baltobu

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Data;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.listing.Program;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
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

public class ExtractJsonKeys extends GhidraScript {

    // Targets: nlohmann::json's operator[] and operator[] (const)
    private static final long[] JSON_OPS = {
        0x1800e6ca0L,   // operator[](string)
    };

    @Override
    public void run() throws Exception {
        Program program = currentProgram;
        Memory mem = program.getMemory();
        ReferenceManager refs = program.getReferenceManager();

        // Find every function that calls one of the JSON ops.
        TreeSet<Long> targetFuncs = new TreeSet<>();
        for (long op : JSON_OPS) {
            Address opAddr = program.getAddressFactory()
                .getDefaultAddressSpace().getAddress(op);
            Iterator<Reference> it = refs.getReferencesTo(opAddr).iterator();
            while (it.hasNext()) {
                Reference r = it.next();
                if (!r.getReferenceType().isCall()) continue;
                Function f = getFunctionContaining(r.getFromAddress());
                if (f != null) targetFuncs.add(f.getEntryPoint().getOffset());
            }
        }
        println("Functions calling json::operator[]: " + targetFuncs.size());

        DecompInterface d = new DecompInterface();
        d.setOptions(new DecompileOptions());
        d.setSimplificationStyle("decompile");
        d.openProgram(program);

        // For each target function, decompile + scrape FUN_1800e6ca0(_, "key") patterns
        Pattern litPat = Pattern.compile(
            "FUN_1800e6ca0\\s*\\([^,]+,\\s*\"([^\"]+)\"");
        Pattern datPat = Pattern.compile(
            "FUN_1800e6ca0\\s*\\([^,]+,\\s*&DAT_180([0-9a-fA-F]+)\\)");

        TreeMap<Long, TreeSet<String>> funcKeys = new TreeMap<>();
        int n = 0;
        for (long va : targetFuncs) {
            if (monitor.isCancelled()) break;
            Address addr = program.getAddressFactory()
                .getDefaultAddressSpace().getAddress(va);
            Function f = getFunctionAt(addr);
            if (f == null) continue;
            DecompileResults dr = d.decompileFunction(f, 30, monitor);
            if (dr == null || dr.getDecompiledFunction() == null) continue;
            String body = dr.getDecompiledFunction().getC();
            TreeSet<String> keys = new TreeSet<>();
            Matcher m = litPat.matcher(body);
            while (m.find()) keys.add(m.group(1));
            m = datPat.matcher(body);
            while (m.find()) {
                long rva = Long.parseLong(m.group(1), 16);
                String s = readCString(mem, rva);
                if (s != null) keys.add(s);
            }
            if (!keys.isEmpty()) funcKeys.put(va, keys);
            n++;
            if (n % 32 == 0)
                println("processed " + n + "/" + targetFuncs.size());
        }
        d.dispose();

        // Save
        File out = new File(System.getProperty("user.dir")
            + "/output/unpacking/json_keys_per_function.tsv");
        try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
            w.println("function_va\tn_keys\tkeys");
            for (var e : funcKeys.entrySet()) {
                w.printf("0x%x\t%d\t%s%n",
                    e.getKey(), e.getValue().size(),
                    String.join(",", e.getValue()));
            }
        }
        println("wrote " + out + " (" + funcKeys.size() + " functions)");
    }

    private String readCString(Memory mem, long rva) {
        try {
            // The dump is rebased to 0x180000000, so RVA is in the same
            // address space as the loaded image.
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
