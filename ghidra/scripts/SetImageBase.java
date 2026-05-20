// Pre-analysis Ghidra script: rebase the program to a fixed image base.
//
// The Scylla dump of bambu_networking.dll has its PE header rewritten with
// the runtime ASLR base (e.g. 0x7FFE22C50000), not the original
// 0x180000000. Re-basing on import keeps every RVA we already documented
// in docs/03/06/07 consistent across the packed and unpacked analyses.
//
// Usage (from run_headless.sh):
//     -preScript SetImageBase.java 0x180000000
//
// If no argument is given, falls back to the IMAGE_BASE environment
// variable. If neither is set, the script is a no-op.
//
// @category baltobu
// @author baltobu

import ghidra.app.script.GhidraScript;
import ghidra.program.model.address.Address;

public class SetImageBase extends GhidraScript {

    @Override
    public void run() throws Exception {
        String spec = null;
        String[] args = getScriptArgs();
        if (args != null && args.length > 0 && !args[0].isEmpty()) {
            spec = args[0];
        } else {
            spec = System.getenv("IMAGE_BASE");
        }
        if (spec == null || spec.isEmpty()) {
            println("[SetImageBase] no IMAGE_BASE; leaving base unchanged");
            return;
        }
        long target = Long.decode(spec).longValue();
        long current = currentProgram.getImageBase().getOffset();
        if (current == target) {
            println(String.format(
                "[SetImageBase] already at 0x%x; nothing to do", target));
            return;
        }
        Address newBase = currentProgram.getAddressFactory()
            .getDefaultAddressSpace().getAddress(target);
        println(String.format(
            "[SetImageBase] rebasing from 0x%x to 0x%x", current, target));
        currentProgram.setImageBase(newBase, true);
        println("[SetImageBase] done");
    }
}
