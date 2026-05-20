// List every reference TO a given VA, with the calling function and the
// from-address. Useful when we need to confirm that a candidate
// function is only reachable from one site.
//
// Usage: ghidra/run_headless.sh ListXrefs.java 0x180565dc4
//
// @category baltobu
// @author baltobu

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Reference;
import ghidra.program.model.symbol.ReferenceManager;

import java.util.Iterator;

public class ListXrefs extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args == null || args.length == 0) {
            println("usage: pass a VA, e.g. 0x180565dc4");
            return;
        }
        long va = Long.decode(args[0]);
        Address addr = currentProgram.getAddressFactory()
            .getDefaultAddressSpace().getAddress(va);
        ReferenceManager rm = currentProgram.getReferenceManager();
        Iterator<Reference> it = rm.getReferencesTo(addr).iterator();
        int n = 0;
        while (it.hasNext()) {
            Reference r = it.next();
            Function f = getFunctionContaining(r.getFromAddress());
            String fn = (f == null) ? "<no fn>" : (f.getName()
                + " @ 0x" + Long.toHexString(f.getEntryPoint().getOffset()));
            println(String.format("from 0x%x  type=%s  in %s",
                r.getFromAddress().getOffset(),
                r.getReferenceType(),
                fn));
            n++;
        }
        println("xrefs: " + n);
    }
}
