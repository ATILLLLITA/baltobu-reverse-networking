// Find any function/symbol whose name matches a substring. Useful when
// Ghidra's PDB / FidDB heuristics have labelled standard CRT helpers
// and we need their addresses.
//
// Usage: ghidra/run_headless.sh FindByName.java dllmain
//
// @category baltobu
// @author baltobu

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.Function;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;

public class FindByName extends GhidraScript {
    @Override
    public void run() throws Exception {
        String[] args = getScriptArgs();
        if (args == null || args.length == 0) {
            println("usage: pass a substring");
            return;
        }
        String needle = args[0].toLowerCase();
        SymbolIterator si = currentProgram.getSymbolTable().getAllSymbols(true);
        int n = 0;
        while (si.hasNext()) {
            Symbol s = si.next();
            if (!s.getName().toLowerCase().contains(needle)) continue;
            println(String.format("0x%x\t%s\t%s",
                s.getAddress().getOffset(),
                s.getName(),
                s.getSymbolType()));
            n++;
        }
        println("matches: " + n);
    }
}
