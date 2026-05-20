// Decompile a single function by VA, write to stdout.
//
// Usage: ghidra/run_headless.sh DecompileOne.java 0x18023cda0
//
// @category baltobu
// @author baltobu

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;

public class DecompileOne extends GhidraScript {

    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args == null || args.length == 0) {
            println("DecompileOne: pass a VA, e.g. 0x18023cda0");
            return;
        }
        long va = Long.decode(args[0]);
        Address addr = currentProgram.getAddressFactory()
            .getDefaultAddressSpace().getAddress(va);

        Function f = getFunctionAt(addr);
        if (f == null) f = getFunctionContaining(addr);
        if (f == null) {
            println("no function at " + addr);
            return;
        }
        DecompInterface d = new DecompInterface();
        d.setOptions(new DecompileOptions());
        d.openProgram(currentProgram);
        DecompileResults dr = d.decompileFunction(f, 60, monitor);
        d.dispose();

        String body = dr.getDecompiledFunction().getC();
        String name = f.getName();
        String tag = String.format("0x%x_%s", va, name);
        File out = new File(System.getProperty("user.dir")
            + "/output/ghidra_decompile/oneshot/" + tag + ".c");
        out.getParentFile().mkdirs();
        try (PrintWriter w = new PrintWriter(new FileWriter(out))) {
            w.print(body);
        }
        println("wrote " + out);
        // Also print the body to stdout for live use
        println("---begin---");
        for (String line : body.split("\n")) println(line);
        println("---end---");
    }
}
