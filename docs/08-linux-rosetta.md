---
  Linux Rosetta + live-DLL findings (complementary to this repo)

  Findings that complement this project, from two independent sources the DLL-only effort doesn't use:

  1. an unprotected Linux build (libbambu_networking.so v01.07.01.04) with a full symbol table — real .text bodies, named, with addresses/sizes; and
  2. a runtime dump of the live Windows DLL (02.06.00.50) from a running bambu-studio.exe, where VMProtect is already decrypted in memory.

  ▎ Clean-room note. All from a symbol-bearing build + a process memory dump + objdump — not from this repo's decompilation. Facts only (symbols, signatures, struct sizes, URL templates).

  1. bambu_network_create_agent — body recovered

  04-public-api.md marks this "body recovery blocked by an anti-disassembly stub." Clean on Linux:

  void* bambu_network_create_agent(void) {
      if (g_bambu_network_agent) return g_bambu_network_agent;   // idempotent singleton
      auto* p = operator new(0x10);                              // agent = 16 bytes
      BBL::BambuNetworkAgent::BambuNetworkAgent(p);              // ctor
      g_bambu_network_agent = p;
      // boost::nowide::utf8_codecvt<wchar_t,4> -> boost::filesystem::path::imbue(locale)
      return g_bambu_network_agent;
  }

  2. Full export → BBL::BambuNetworkAgent method map (79 resolved)

  Each export is a thin wrapper: if (agent == *g_singleton) return agent->method(...). Signatures are exact demangled C++ types (method names relative to BBL::BambuNetworkAgent:: unless noted):

  ┌─────────────────────────────────────────────┬────────────────────────────────────────────────────────────────────┐
  │                   Export                    │                         Delegate signature                         │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_bind                          │ start_bind(std::string, std::string, std::string, std::string,     │
  │                                             │ bool, std::function<void (int, int, std::string)>)                 │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_build_login_cmd               │ build_login_cmd()                                                  │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_build_login_info              │ build_login_info()                                                 │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_build_logout_cmd              │ build_logout_cmd()                                                 │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_change_user                   │ change_user(std::string)                                           │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_check_user_report             │ check_user_report(int*, bool*, std::string*)                       │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_check_user_task_report        │ check_user_task_report(int*, bool*)                                │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_connect_printer               │ connect_printer(std::string, std::string, std::string,             │
  │                                             │ std::string, bool)                                                 │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_connect_server                │ connect_server()                                                   │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_create_agent                  │ BambuNetworkAgent()                                                │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_delete_setting                │ delete_setting(std::string)                                        │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_del_rating_picture_oss        │ del_rating_picture_oss(std::string&, std::string&, unsigned int&,  │
  │                                             │ std::string&)                                                      │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_destroy_agent                 │ ~BambuNetworkAgent()                                               │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_disconnect_printer            │ disconnect_printer()                                               │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_bambulab_host             │ get_bambulab_host()                                                │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_camera_url                │ get_camera_url(std::string, std::function<void (std::string)>)     │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_design_staffpick          │ get_design_staffpick(int, int, std::function<void (std::string)>)  │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_model_instance_id         │ get_model_instance_id(int&, int, unsigned int&, std::string&)      │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_model_mall_detail_url     │ get_model_mall_detail_url(std::string*, std::string)               │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_model_mall_home_url       │ get_model_mall_home_url(std::string*)                              │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_model_publish_url         │ get_model_publish_url(std::string*)                                │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_model_rating_id           │ get_model_instance_id(int&, int, unsigned int&, std::string&)      │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_my_profile                │ get_my_profile(std::string, unsigned int*, std::string*)           │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_oss_config                │ get_oss_config(std::string&, std::string, unsigned int&,           │
  │                                             │ std::string&)                                                      │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_printer_firmware          │ get_printer_firmware(std::string, unsigned int*, std::string*)     │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_setting_list              │ get_setting_list(std::string, std::function<void (int)>,           │
  │                                             │ std::function<bool ()>)                                            │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_slice_info                │ get_slice_info(std::string, std::string, int, std::string*)        │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_studio_info_url           │ get_studio_info_url()                                              │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_subtask                   │ get_subtask(BBL::BBLModelTask*)                                    │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_task_plate_index          │ get_task_plate_index(std::string, int*)                            │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_user_avatar               │ user_avatar()                                                      │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_user_id                   │ user_id()                                                          │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_user_info                 │ get_user_info(int*)                                                │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_user_name                 │ user_name()                                                        │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_user_nickanme             │ user_nickanme() (sic — vendor typo)                                │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_user_presets              │ get_user_presets(std::map<std::string, std::map<std::string,       │
  │                                             │ std::string>>*)                                                    │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_user_print_info           │ get_user_print_info(unsigned int*, std::string*)                   │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_get_user_selected_machine     │ user_selected_machine()                                            │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_init_log                      │ init_log()                                                         │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_modify_printer_name           │ modify_printer_name(std::string, std::string)                      │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_put_model_mall_rating         │ put_model_mall_rating(int, int, std::string,                       │
  │                                             │ std::vector<std::string>, unsigned int&, std::string&)             │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_put_rating_picture_oss        │ put_rating_picture_oss(std::string&, std::string&, std::string,    │
  │                                             │ int, unsigned int&, std::string&)                                  │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_put_setting                   │ put_setting(std::string, std::string, std::map<std::string,        │
  │                                             │ std::string>*, unsigned int*)                                      │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_query_bind_status             │ query_bind_status(std::vector<std::string>, unsigned int*,         │
  │                                             │ std::string*)                                                      │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_refresh_connection            │ check_server_connection()                                          │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_request_bind_ticket           │ request_bind_ticket(std::string*)                                  │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_request_setting_id            │ request_setting_id(std::string, std::map<std::string,              │
  │                                             │ std::string>*, unsigned int*)                                      │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_send_message                  │ send_message(std::string, std::string, int)                        │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_send_message_to_printer       │ send_message_to_printer(std::string, std::string, int)             │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_cert_file                 │ set_cert_file(std::string, std::string)                            │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_config_dir                │ set_config_dir(std::string)                                        │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_country_code              │ set_country_code(std::string)                                      │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_extra_http_header         │ set_extra_http_header(std::map<std::string, std::string>)          │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_get_country_code_fn       │ set_get_country_code_fn(std::function<std::string ()>)             │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_on_http_error_fn          │ set_on_http_error_fn(std::function<void (unsigned int,             │
  │                                             │ std::string)>)                                                     │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_on_local_connect_fn       │ set_on_local_connect_fn(std::function<void (int, std::string,      │
  │                                             │ std::string)>)                                                     │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_on_local_message_fn       │ set_on_local_message_fn(std::function<void (std::string,           │
  │                                             │ std::string)>)                                                     │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_on_message_fn             │ set_on_message_fn(std::function<void (std::string, std::string)>)  │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_on_printer_connected_fn   │ set_on_printer_connected_fn(std::function<void (std::string)>)     │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_on_server_connected_fn    │ set_on_server_connected_fn(std::function<void (int, int)>)         │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_on_ssdp_msg_fn            │ set_on_ssdp_msg_fn(std::function<void (std::string)>)              │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_on_user_login_fn          │ set_on_user_login_fn(std::function<void (int, bool)>)              │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_queue_on_main_fn          │ set_queue_on_main_fn(std::function<void (std::function<void ()>)>) │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_set_user_selected_machine     │ set_user_selected_machine(std::string)                             │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_start                         │ load_config()                                                      │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_start_local_print             │ start_local_print(BBL::PrintParams, …) (ctor PrintParams(const&)   │
  │                                             │ inlined first)                                                     │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_start_local_print_with_record │ start_local_print_with_record(BBL::PrintParams, …)                 │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_start_print                   │ start_print(BBL::PrintParams, …)                                   │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_start_publish                 │ start_publish(BBL::PublishParams, std::function<void (int, int,    │
  │                                             │ std::string)>, std::function<bool ()>, std::string*)               │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_start_send_gcode_to_sdcard    │ start_send_gcode_to_sdcard(BBL::PrintParams, …)                    │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_start_subscribe               │ start_subscribe(std::string)                                       │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_stop_subscribe                │ stop_subscribe(std::string)                                        │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_track_enable                  │ track_enable(bool)                                                 │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_track_event                   │ track_event(std::string, std::string)                              │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_track_get_property            │ track_get_property(std::string, std::string&, std::string)         │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_track_header                  │ track_header(std::string)                                          │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_track_update_property         │ track_update_property(std::string, std::string, std::string)       │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_unbind                        │ start_unbind(std::string)                                          │
  ├─────────────────────────────────────────────┼────────────────────────────────────────────────────────────────────┤
  │ bambu_network_user_logout                   │ user_logout()                                                      │
  └─────────────────────────────────────────────┴────────────────────────────────────────────────────────────────────┘

  Notable: set_cert_file(string,string), set_extra_http_header(map), set_config_dir, set_country_code (config surface); the start_*print* family all take a BBL::PrintParams struct; start_publish takes BBL::PublishParams; get_subtask(BBL::BBLModelTask*).

  3. ft_* tunnel/file-transfer API — confirmed (21)

  Absent in v01.07.01.04, present/decrypted in the v02.06.00.50 memory image (matches this repo's count):

  ft_abi_version, ft_free,
  ft_tunnel_create, ft_tunnel_retain, ft_tunnel_release, ft_tunnel_shutdown,
  ft_tunnel_set_status_cb, ft_tunnel_start_connect, ft_tunnel_sync_connect, ft_tunnel_start_job,
  ft_job_create, ft_job_cancel, ft_job_retain, ft_job_release,
  ft_job_get_msg, ft_job_try_get_msg, ft_job_get_result,
  ft_job_set_msg_cb, ft_job_set_result_cb, ft_job_msg_destroy, ft_job_result_destroy

  Refcounted (retain/release) tunnel + async job model with status/msg/result callbacks.

  4. Cloud REST endpoint catalog (50 templates)

  %1% = host. Decrypted from the v02.06.00.50 memory image.

  iot-service
  %1%/iot-service/api/slicer/resource
  %1%/iot-service/api/slicer/setting
  %1%/iot-service/api/slicer/setting/%2%
  %1%/iot-service/api/user/applications/%2%/cert?aes256=%3%&ver=1
  %1%/iot-service/api/user/bind
  %1%/iot-service/api/user/bind_list?dev_ids=%2%
  %1%/iot-service/api/user/device/info
  %1%/iot-service/api/user/device/version?dev_id=%2%
  %1%/iot-service/api/user/notification
  %1%/iot-service/api/user/notification?action=upload&ticket=%2%
  %1%/iot-service/api/user/print?force=true
  %1%/iot-service/api/user/profile/%2%
  %1%/iot-service/api/user/project
  %1%/iot-service/api/user/project/%2%
  %1%/iot-service/api/user/task/%2%
  %1%/iot-service/api/user/ttcode
  %1%/iot-service/api/user/upload?%2%
  %1%/iot-service/api/user/upload?event_track/slicer=%2%

  user-service
  %1%/user-service/my/logout
  %1%/user-service/my/messages/type
  %1%/user-service/my/ossconfig?useType=1
  %1%/user-service/my/pincode/%2%
  %1%/user-service/my/profile
  %1%/user-service/my/s3config?useType=1
  %1%/user-service/my/setting
  %1%/user-service/my/task
  %1%/user-service/my/task/%2%
  %1%/user-service/my/tasks?%2%
  %1%/user-service/my/tasks?deviceId=%2%&limit=%3%
  %1%/user-service/my/ticket/%2%
  %1%/user-service/user/consent
  %1%/user-service/user/refreshtoken
  %1%/user-service/user/ticket
  %1%/user-service/user/ticket/%2%

  design-service / design-user-service
  %1%/design-service/design/staffpick?offset=%2%&limit=%3%
  %1%/design-service/model/%2%
  %1%/design-service/my/design/recommend?seed=%2%&limit=%3%
  %1%/design-user-service/filament/config
  %1%/design-user-service/my/filament/v2
  %1%/design-user-service/my/filament/v2/batch
  %1%/design-user-service/my/preference

  other services
  %1%/comment-service/rating/

other services
%1%/comment-service/rating/
%1%/comment-service/rating/inst/%2%
%1%/analysis-st/tag/?UID=%2%
%1%/autotest-report-service/api/user_last_task?user_id=%2%
%1%/v1/iot-service2/api/user/device/file_download?dev_id=%2%&type=print_fail_snapshot&name=%3%
%1%/publish?project_id=%2%&design_id=%3%
%1%/publish?project_id=%2%&profile_id=%3%
%1%/request
%1%/report

How to reproduce (no vendor binaries shared)

- Linux: readelf -sW libbambu_networking.so; objdump -d --start-address=… -M intel per export.
- Windows: ReadProcessMemory over [ModuleBase, +ModuleMemorySize] of bambu-studio.exe (VMProtect already decrypted in memory); strings/disassemble.

No secrets, tokens, certs, or keys included; cloud endpoints are URL templates only.

---
