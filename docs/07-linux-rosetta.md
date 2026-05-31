---
Linux Rosetta + live-DLL findings (complementary to this repo)

Findings that complement this project, obtained from two independent sources the DLL-only effort doesn't use:

1. an unprotected Linux build (libbambu_networking.so v01.07.01.04) that still ships a full symbol table — real .text bodies, named, with addresses/sizes; and
2. a runtime dump of the live Windows DLL (02.06.00.50) taken from a running bambu-studio.exe, where VMProtect has already decrypted the image in memory.

▎ Clean-room note. All of this comes from a symbol-bearing build and a process memory dump + standard objdump — not from reading this repo's decompilation. Facts only (symbol names, signatures, struct sizes, URL templates), suitable for an MIT-licensed, AGPL-compliance reimplementation.

1. bambu_network_create_agent — body recovered

04-public-api.md notes this one as "body recovery blocked by an anti-disassembly stub." On the Linux build it is clean:

void* bambu_network_create_agent(void) {
    if (g_bambu_network_agent) return g_bambu_network_agent;   // idempotent singleton
    auto* p = operator new(0x10);                              // agent object = 16 bytes
    BBL::BambuNetworkAgent::BambuNetworkAgent(p);              // ctor
    g_bambu_network_agent = p;
    // boost::nowide::utf8_codecvt<wchar_t,4> -> boost::filesystem::path::imbue(locale)
    return g_bambu_network_agent;
}

Class is BBL::BambuNetworkAgent; the agent is a 16-byte singleton stored in a global.

2. Export → C++ method signatures (exact demangled types)

Every public export is a thin wrapper: if (agent == *g_singleton) return agent->method(...). The delegated BBL::BambuNetworkAgent methods carry the exact C++ parameter types, so the C ABI is fully recovered. Highlights:

┌────────────────────────────────────────────┬────────────────────────────────────────────────────────────────────┐
│                   Export                   │                         Delegate signature                         │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_bind                         │ start_bind(string serial, string, string, string, bool,            │
│                                            │ function<void(int,int,string)> cb)                                 │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_unbind                       │ start_unbind(string serial)                                        │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_query_bind_status            │ query_bind_status(vector<string> dev_ids, unsigned* out, string*   │
│                                            │ out)                                                               │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_request_bind_ticket          │ request_bind_ticket(string* out)                                   │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_connect_printer              │ connect_printer(string, string, string, string, bool)              │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_connect_server               │ connect_server()                                                   │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_send_message_to_printer      │ send_message_to_printer(string dev_id, string json, int)           │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_send_message                 │ send_message(string dev_id, string json, int)                      │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_start_subscribe /            │ start_subscribe(string dev_id) / stop_subscribe(string)            │
│ stop_subscribe                             │                                                                    │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_build_login_cmd /            │ build_login_cmd()->string / build_login_info()->string             │
│ build_login_info                           │                                                                    │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_change_user                  │ change_user(string)                                                │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_check_user_report            │ check_user_report(int*, bool*, string*)                            │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_set_on_user_login_fn         │ set_on_user_login_fn(function<void(int,bool)>)                     │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_set_on_printer_connected_fn  │ set_on_printer_connected_fn(function<void(string)>)                │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_set_on_server_connected_fn   │ set_on_server_connected_fn(function<void(int,int)>)                │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_set_on_local_connect_fn      │ set_on_local_connect_fn(function<void(int,string,string)>)         │
├────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
│ bambu_network_start_print                  │ takes a BBL::PrintParams struct (fields incl. ams_mapping,         │
│                                            │ bed_leveling, bed_type, …)                                         │
└────────────────────────────────────────────┴────────────────────────────────────────────────────────────────────┘

(79 of 170 exports resolve to a direct BBL::BambuNetworkAgent/BBL::PrintParams delegate by static disassembly; full table available.)

3. ft_* tunnel/file-transfer API — confirmed (21 functions)

Absent from v01.07.01.04, present and decrypted in the v02.06.00.50 memory image — matches this repo's 21-function ft_* count:

ft_abi_version, ft_free,
ft_tunnel_create, ft_tunnel_retain, ft_tunnel_release, ft_tunnel_shutdown,
ft_tunnel_set_status_cb, ft_tunnel_start_connect, ft_tunnel_sync_connect, ft_tunnel_start_job,
ft_job_create, ft_job_cancel, ft_job_retain, ft_job_release,
ft_job_get_msg, ft_job_try_get_msg, ft_job_get_result,
ft_job_set_msg_cb, ft_job_set_result_cb, ft_job_msg_destroy, ft_job_result_destroy

A refcounted (retain/release) tunnel + async job model with status/msg/result callbacks.

4. Cloud REST endpoint catalog (50 templates)

Decrypted URL templates (%1% = host) across iot-service (18), user-service (16), design-service / design-user-service, comment-service, autotest-report-service, analysis-st. Examples:

%1%/iot-service/api/user/bind
%1%/iot-service/api/user/bind_list?dev_ids=%2%
%1%/iot-service/api/user/device/info
%1%/iot-service/api/user/device/version?dev_id=%2%
%1%/iot-service/api/user/print?force=true
%1%/v1/iot-service2/api/user/device/file_download?dev_id=%2%&type=print_fail_snapshot&name=%3%

How to reproduce (no vendor binaries shared)

- Linux: readelf -sW libbambu_networking.so for the symbol table; objdump -d --start-address=… -M intel per export.
- Windows: dump the loaded module region of bambu-studio.exe via ReadProcessMemory over [ModuleBase, +ModuleMemorySize] (VMProtect is already decrypted in memory); strings / disassemble the dump.

No secrets, tokens, certs, or keys are included; cloud endpoints are URL templates only.

---
