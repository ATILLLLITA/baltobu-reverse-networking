// Find functions that reference a given string-literal VA, decompile
// each, write the bodies. Used to drill into specific subsystems by
// known log message.
//
// Usage: ghidra/run_headless.sh FindXrefsAndDecompile.java <string_va>
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
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.Iterator;
import java.util.TreeSet;

public class FindXrefsAndDecompile extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args == null || args.length == 0) {
            println("usage: pass VA of the .rdata string");
            return;
        }
        long va = Long.decode(args[0]);
        Program program = currentProgram;
        Address addr = program.getAddressFactory()
            .getDefaultAddressSpace().getAddress(va);

        ReferenceManager refs = program.getReferenceManager();
        TreeSet<Long> funcs = new TreeSet<>();
        Iterator<Reference> it = refs.getReferencesTo(addr).iterator();
        while (it.hasNext()) {
            Reference r = it.next();
            Function f = getFunctionContaining(r.getFromAddress());
            if (f != null) funcs.add(f.getEntryPoint().getOffset());
        }
        println("functions referencing 0x" + Long.toHexString(va) + ": " + funcs.size());

        DecompInterface d = new DecompInterface();
        d.setOptions(new DecompileOptions());
        d.openProgram(program);

        File outDir = new File(System.getProperty("user.dir")
            + "/output/ghidra_decompile/xrefs");
        outDir.mkdirs();
        for (long fva : funcs) {
            if (monitor.isCancelled()) break;
            Function f = getFunctionAt(program.getAddressFactory()
                .getDefaultAddressSpace().getAddress(fva));
            if (f == null) continue;
            DecompileResults dr = d.decompileFunction(f, 60, monitor);
            if (dr == null || dr.getDecompiledFunction() == null) continue;
            File out = new File(outDir, String.format("0x%x.c", fva));
            try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
                w.printf("// Function at 0x%x referencing 0x%x%n%n", fva, va);
                w.print(dr.getDecompiledFunction().getC());
            }
            println("wrote " + out);
        }
        d.dispose();
    }
}
