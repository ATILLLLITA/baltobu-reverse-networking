/*
 * Minimal Windows host process that loads bambu_networking.dll and
 * waits, so Scylla can read the unpacked image out of process memory.
 *
 * Build on Windows with the MSVC toolchain:
 *
 *     cl /Fe:host_loader.exe host_loader.c
 *
 * Or with MinGW (cross-compile from Linux is fine):
 *
 *     x86_64-w64-mingw32-gcc -o host_loader.exe host_loader.c -lkernel32
 *
 * Usage:
 *
 *     - Place host_loader.exe and bambu_networking.dll in the same
 *       directory (typically unpacking/ in this repository).
 *     - Run host_loader.exe directly. No debugger is attached: VMProtect
 *       3.x bypasses ScyllaHide via a private ntdll mapping, so the
 *       working flow is debugger-free.
 *     - The process loads the DLL, prints the load base, then parks in
 *       WaitForSingleObject. The full unpack completes in about one
 *       second on the test hardware.
 *     - Launch Scylla, select the host_loader.exe process and the
 *       bambu_networking.dll module, then IAT Autosearch + Get Imports
 *       + Dump.
 *
 * The host deliberately:
 *   - Loads the DLL via LoadLibraryA so the OS loader, TLS callback
 *     and DllMain all run normally.
 *   - Calls GetProcAddress on one export to force the IAT to be
 *     fully resolved (some protectors are lazy about resolving until
 *     the first GetProcAddress call).
 *   - Sleeps forever via INFINITE Wait so the DLL is not unloaded
 *     before Scylla has had a chance to dump it.
 *
 * No export is called with arguments. That would risk triggering
 * anti-debug paths that have not been characterised. The single
 * GetProcAddress is enough.
 */
#include <windows.h>
#include <stdio.h>

#define DLL_NAME "bambu_networking.dll"
#define PROBE_EXPORT "bambu_network_get_version"

int main(void)
{
    printf("[host_loader] loading %s\n", DLL_NAME);
    HMODULE mod = LoadLibraryA(DLL_NAME);
    if (!mod) {
        DWORD err = GetLastError();
        fprintf(stderr, "[host_loader] LoadLibrary failed: %lu\n", err);
        return 1;
    }
    printf("[host_loader] loaded at 0x%p\n", (void *)mod);

    /*
     * Force the IAT path. We don't call the function - just resolve it
     * so any deferred unpacking inside GetProcAddress fires.
     */
    FARPROC p = GetProcAddress(mod, PROBE_EXPORT);
    if (!p) {
        DWORD err = GetLastError();
        fprintf(stderr, "[host_loader] GetProcAddress(%s) failed: %lu\n",
                PROBE_EXPORT, err);
    } else {
        printf("[host_loader] %s @ 0x%p\n", PROBE_EXPORT, (void *)p);
    }

    printf("[host_loader] sleeping; attach Scylla and dump now.\n");
    /*
     * Wait indefinitely. The debugger keeps the process alive; Ctrl+C
     * or a debugger detach is the only way out.
     */
    WaitForSingleObject(GetCurrentProcess(), INFINITE);
    return 0;
}
