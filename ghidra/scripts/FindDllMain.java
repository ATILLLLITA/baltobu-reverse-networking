// Hunt for _DllMainCRTStartup and the user-written DllMain inside the
// unpacked dump. Strategy:
//   1. List any symbol named DllMain*, *CRTStartup*, _initterm, _scrt_*,
//      __dllonexit (CRT signatures).
//   2. List functions that contain `cmp edx, 1` early in their bodies
//      followed by another comparison to 0 or 2 (the dispatch shape).
//   3. List functions whose decompilation contains the literal
//      string "DllMain" via a debug log call.
//   4. List functions referenced from the OEP wrapper at 0x1801b72917
//      via any reachable indirect transfer.
//
// Output goes to output/ghidra/dllmain_candidates.tsv.
//
// @category baltobu
// @author baltobu

import ghidra.app.decompiler.DecompInterface;
import ghidra.app.decompiler.DecompileOptions;
import ghidra.app.decompiler.DecompileResults;
import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;
import ghidra.program.model.address.AddressIterator;
import ghidra.program.model.listing.Function;
import ghidra.program.model.listing.FunctionIterator;
import ghidra.program.model.listing.Listing;
import ghidra.program.model.listing.Program;
import ghidra.program.model.mem.Memory;
import ghidra.program.model.mem.MemoryBlock;
import ghidra.program.model.symbol.Symbol;
import ghidra.program.model.symbol.SymbolIterator;
import ghidra.program.model.symbol.SymbolTable;

import java.io.File;
import java.io.FileWriter;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.List;
import java.util.TreeMap;

public class FindDllMain extends GhidraScript {

    @Override
    public void run() throws Exception {
        Program program = currentProgram;
        SymbolTable st = program.getSymbolTable();
        Memory mem = program.getMemory();

        File out = new File(System.getProperty("user.dir")
            + "/output/ghidra/dllmain_candidates.tsv");
        out.getParentFile().mkdirs();
        PrintWriter w = new PrintWriter(new FileWriter(out));
        w.println("category\tva\tname\tnote");

        println("=== 1. Symbol-table hunt ===");
        String[] keywords = new String[]{
            "DllMain", "CRTStartup", "_initterm", "_scrt_", "__scrt",
            "__dllonexit", "_CRT_INIT", "_init_thread_footer",
            "__security_init_cookie", "_DllMainCRTStartup"
        };
        SymbolIterator si = st.getAllSymbols(true);
        while (si.hasNext()) {
            if (monitor.isCancelled()) break;
            Symbol s = si.next();
            String n = s.getName();
            for (String kw : keywords) {
                if (n.contains(kw)) {
                    String va = String.format("0x%x",
                        s.getAddress().getOffset());
                    println("  symbol " + n + " @ " + va);
                    w.printf("symbol\t%s\t%s\tmatches keyword '%s'%n",
                        va, n, kw);
                }
            }
        }

        println();
        println("=== 2. .text byte-pattern scan ===");
        // Look for the canonical x64 _DllMainCRTStartup prologue:
        //   40 53                push rbx
        //   48 83 EC 20          sub  rsp, 20h
        //   41 8B D8             mov  ebx, r8d   (or 41 8B F8 = mov edi, r8d)
        // followed shortly by `test edx, edx` (85 D2) or `cmp edx, ??`
        // We scan .text for the four-byte prefix and report each hit.
        MemoryBlock text = null;
        for (MemoryBlock b : mem.getBlocks()) {
            if (b.getName().equals(".text")) { text = b; break; }
        }
        if (text == null) {
            println("  no .text block found");
            w.close();
            return;
        }
        long lo = text.getStart().getOffset();
        long hi = text.getEnd().getOffset();
        println("  scanning .text " + String.format("0x%x..0x%x", lo, hi));

        byte[] need1 = new byte[]{0x40, 0x53, 0x48, (byte)0x83};   // push rbx; sub rsp, ...
        byte[] need2 = new byte[]{0x41, (byte)0x8B, (byte)0xD8};   // mov ebx, r8d
        byte[] need3 = new byte[]{(byte)0x85, (byte)0xD2};         // test edx, edx
        int count = 0;
        for (long va = lo; va < hi - 16; va++) {
            if (monitor.isCancelled()) break;
            Address a = program.getAddressFactory()
                .getDefaultAddressSpace().getAddress(va);
            byte[] window = new byte[24];
            try { mem.getBytes(a, window); } catch (Exception e) { continue; }
            if (window[0] != need1[0]) continue;
            if (window[1] != need1[1]) continue;
            if (window[2] != need1[2]) continue;
            if (window[3] != need1[3]) continue;
            // Now look for need2 (mov ebx, r8d) within first 20 bytes
            boolean has_n2 = false;
            int n2_off = -1;
            for (int k = 4; k < 16; k++) {
                if (window[k] == need2[0]
                    && window[k+1] == need2[1]
                    && window[k+2] == need2[2]) {
                    has_n2 = true;
                    n2_off = k;
                    break;
                }
            }
            if (!has_n2) continue;
            // Look for need3 (test edx, edx) right after
            boolean has_n3 = false;
            for (int k = n2_off + 3; k < 22; k++) {
                if (window[k] == need3[0] && window[k+1] == need3[1]) {
                    has_n3 = true;
                    break;
                }
            }
            if (!has_n3) continue;
            // Strong candidate: looks like a 3-arg function that loads
            // r8d into ebx (preserving across call) and tests edx.
            count++;
            String va_str = String.format("0x%x", va);
            String hex = "";
            for (int k = 0; k < 16; k++) {
                hex += String.format("%02x ", window[k] & 0xff);
            }
            println("  candidate @ " + va_str + "  " + hex.trim());
            w.printf("byte-pattern\t%s\t-\tpush rbx;sub rsp;mov ebx,r8d;test edx,edx prologue%n",
                va_str);
            if (count >= 32) {
                println("  ... stopped at " + count);
                w.println("byte-pattern\t-\t-\ttruncated at 32 hits");
                break;
            }
        }

        println();
        println("=== 3. Functions whose body decompiles to a switch on a "
            + "small dispatch arg ===");
        // For each function in .text with body size between 24 and 4096,
        // decompile it and check whether the decompilation contains
        // string fragments like "case 1:" "case 2:" or comparisons of
        // a parameter to 0/1/2/3. Too expensive to run on all 24k
        // functions; instead limit to those where the function's body
        // is in .text and the function has 3+ params.
        DecompInterface d = new DecompInterface();
        d.setOptions(new DecompileOptions());
        d.setSimplificationStyle("decompile");
        d.openProgram(program);

        FunctionIterator fi = program.getListing().getFunctions(true);
        int scanned = 0;
        int matched = 0;
        while (fi.hasNext()) {
            if (monitor.isCancelled()) break;
            Function f = fi.next();
            long fva = f.getEntryPoint().getOffset();
            if (fva < lo || fva >= hi) continue;
            long body = f.getBody().getNumAddresses();
            if (body < 24 || body > 4096) continue;
            int npar = f.getParameterCount();
            if (npar < 2) continue;
            // Decompile (this is expensive; limit to first 200 candidates)
            if (scanned > 600) break;
            scanned++;
            DecompileResults dr = d.decompileFunction(f, 15, monitor);
            if (dr == null || dr.getDecompiledFunction() == null) continue;
            String c = dr.getDecompiledFunction().getC();
            // Heuristic: a function that compares its 2nd arg to 1/2/3
            // and either returns 1 or 0, and is short, is a DllMain
            // dispatch. Count the literal occurrences.
            int eq1 = countSubstr(c, "== 1)");
            int eq2 = countSubstr(c, "== 2)");
            int eq3 = countSubstr(c, "== 3)");
            int ret1 = countSubstr(c, "return 1");
            int ret0 = countSubstr(c, "return 0");
            int has_case1 = countSubstr(c, "case 1:");
            int has_case2 = countSubstr(c, "case 2:");
            int score = eq1 + eq2 + eq3 + (has_case1 + has_case2) * 2;
            if (score >= 3 && (ret1 + ret0) >= 1) {
                matched++;
                String va_str = String.format("0x%x", fva);
                println("  cand @ " + va_str + " size=" + body
                    + " npar=" + npar + " score=" + score);
                w.printf("dispatch\t%s\t%s\tsize=%d npar=%d score=%d%n",
                    va_str, f.getName(), body, npar, score);
            }
        }
        d.dispose();
        println("  scanned " + scanned + " candidates, matched " + matched);

        w.close();
        println();
        println("wrote " + out);
    }

    private static int countSubstr(String hay, String needle) {
        int c = 0, i = 0;
        while ((i = hay.indexOf(needle, i)) != -1) { c++; i += needle.length(); }
        return c;
    }
}
