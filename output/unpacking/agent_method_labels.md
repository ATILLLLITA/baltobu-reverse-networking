# Inferred agent method names

Each row maps an internal helper function (`FUN_180XXXX`) to
the C-ABI export(s) that dispatch to it. The export's name and
documented behaviour is the strongest hint we have for what
the helper actually does. Treat these as **inferred** labels,
not confirmed, clean-room implementers can pick any internal
naming they like.

Helpers reachable from multiple exports usually mean the same
underlying method is shared (e.g. `FUN_180566489` is the MSVC
`std::string::assign` body, used wherever an MSVC `std::string`
is copied from a literal).

| Helper VA | Inferred role | Called by |
|---|---|---|
| `FUN_1800b6470` | shared helper called by 42 exports (e.g. `bind`) | `bambu_network_bind`, `bambu_network_bind_detect`, `bambu_network_change_user`, `bambu_network_connect_printer`, `bambu_network_delete_setting`, +37 more |
| `FUN_1800b6550` | `ft.job_create()` | `ft_job_create` |
| `FUN_1800b89c0` | shared helper called by 37 exports (e.g. `bind`) | `bambu_network_bind`, `bambu_network_bind_detect`, `bambu_network_change_user`, `bambu_network_connect_printer`, `bambu_network_delete_setting`, +32 more |
| `FUN_1800ba580` | `ft.tunnel_start_job()` | `ft_tunnel_start_job` |
| `FUN_1800bf3f0` | `ft.job_get_result()` | `ft_job_get_result` |
| `FUN_1800bf410` | shared helper called by 4 exports (e.g. `add_subscribe`) | `bambu_network_add_subscribe`, `bambu_network_del_subscribe`, `bambu_network_put_model_mall_rating`, `bambu_network_query_bind_status` |
| `FUN_1800bfba0` | `ft.job_create`, `ft.tunnel_create` (shared) | `ft_job_create`, `ft_tunnel_create` |
| `FUN_1800ce1c0` | `set_extra_http_header()` | `bambu_network_set_extra_http_header` |
| `FUN_1800e6ca0` | `ft.job_create()` | `ft_job_create` |
| `FUN_1800eaa50` | `ft.job_create()` | `ft_job_create` |
| `FUN_1800ed510` | `ft.job_create`, `ft.job_get_msg`, `ft.job_try_get_msg` (shared) | `ft_job_create`, `ft_job_get_msg`, `ft_job_try_get_msg` |
| `FUN_1800ef260` | `ft.job_create`, `ft.job_get_msg`, `ft.job_try_get_msg` (shared) | `ft_job_create`, `ft_job_get_msg`, `ft_job_try_get_msg` |
| `FUN_1800f48e0` | `ft.job_create()` | `ft_job_create` |
| `FUN_1800fa1f0` | `ft.job_create()` | `ft_job_create` |
| `FUN_1800fd640` | `ft.job_create`, `ft.job_get_result`, `ft.tunnel_start_job` (shared) | `ft_job_create`, `ft_job_get_result`, `ft_tunnel_start_job` |
| `FUN_1801136a0` | `set_extra_http_header()` | `bambu_network_set_extra_http_header` |
| `FUN_18011e530` | shared helper called by 4 exports (e.g. `add_subscribe`) | `bambu_network_add_subscribe`, `bambu_network_del_subscribe`, `bambu_network_put_model_mall_rating`, `bambu_network_query_bind_status` |
| `FUN_1801207d0` | shared helper called by 5 exports (e.g. `start_local_print`) | `bambu_network_start_local_print`, `bambu_network_start_local_print_with_record`, `bambu_network_start_print`, `bambu_network_start_sdcard_print`, `bambu_network_start_send_gcode_to_sdcard` |
| `FUN_180124f30` | `ft.job_get_result()` | `ft_job_get_result` |
| `FUN_180124fa0` | `ft.job_release`, `ft.tunnel_start_job` (shared) | `ft_job_release`, `ft_tunnel_start_job` |
| `FUN_180125080` | `ft.job_get_result()` | `ft_job_get_result` |
| `FUN_1801250f0` | `ft.job_release()` | `ft_job_release` |
| `FUN_180125360` | shared helper called by 5 exports (e.g. `start_local_print`) | `bambu_network_start_local_print`, `bambu_network_start_local_print_with_record`, `bambu_network_start_print`, `bambu_network_start_sdcard_print`, `bambu_network_start_send_gcode_to_sdcard` |
| `FUN_180125c10` | `start_publish()` | `bambu_network_start_publish` |
| `FUN_180126170` | `get_user_tasks()` | `bambu_network_get_user_tasks` |
| `FUN_18017c470` | `ft.job_get_result()` | `ft_job_get_result` |
| `FUN_18019dc7c` | `ft.job_create()` | `ft_job_create` |
| `FUN_1801a6ae0` | `destroy_agent()` | `bambu_network_destroy_agent` |
| `FUN_1801a6cc0` | `add_subscribe()` | `bambu_network_add_subscribe` |
| `FUN_1801a6d20` | `build_login_cmd()` | `bambu_network_build_login_cmd` |
| `FUN_1801a6d40` | `build_login_info()` | `bambu_network_build_login_info` |
| `FUN_1801a7090` | `build_logout_cmd()` | `bambu_network_build_logout_cmd` |
| `FUN_1801a70b0` | `change_user()` | `bambu_network_change_user` |
| `FUN_1801a7160` | `refresh_connection()` | `bambu_network_refresh_connection` |
| `FUN_1801a7170` | `check_user_report()` | `bambu_network_check_user_report` |
| `FUN_1801a7180` | `check_user_task_report()` | `bambu_network_check_user_task_report` |
| `FUN_1801a7190` | `connect_printer()` | `bambu_network_connect_printer` |
| `FUN_1801a73e0` | `connect_server()` | `bambu_network_connect_server` |
| `FUN_1801a73f0` | `del_rating_picture_oss()` | `bambu_network_del_rating_picture_oss` |
| `FUN_1801a7400` | `del_subscribe()` | `bambu_network_del_subscribe` |
| `FUN_1801a7460` | `delete_setting()` | `bambu_network_delete_setting` |
| `FUN_1801a7510` | `disconnect_printer()` | `bambu_network_disconnect_printer` |
| `FUN_1801a7520` | `enable_multi_machine()` | `bambu_network_enable_multi_machine` |
| `FUN_1801a7530` | `get_bambulab_host()` | `bambu_network_get_bambulab_host` |
| `FUN_1801a7550` | `get_camera_url()` | `bambu_network_get_camera_url` |
| `FUN_1801a7640` | `get_camera_url_for_golive()` | `bambu_network_get_camera_url_for_golive` |
| `FUN_1801a77a0` | `get_design_staffpick()` | `bambu_network_get_design_staffpick` |
| `FUN_1801a7830` | `get_hms_snapshot()` | `bambu_network_get_hms_snapshot` |
| `FUN_1801a78c0` | `get_model_instance_id`, `get_model_rating_id` (shared) | `bambu_network_get_model_instance_id`, `bambu_network_get_model_rating_id` |
| `FUN_1801a78d0` | `get_model_mall_detail_url()` | `bambu_network_get_model_mall_detail_url` |
| `FUN_1801a7980` | `get_model_mall_home_url()` | `bambu_network_get_model_mall_home_url` |
| `FUN_1801a7990` | `get_model_mall_rating()` | `bambu_network_get_model_mall_rating` |
| `FUN_1801a79a0` | `get_model_publish_url()` | `bambu_network_get_model_publish_url` |
| `FUN_1801a79c0` | `get_mw_user_4ulist()` | `bambu_network_get_mw_user_4ulist` |
| `FUN_1801a7a50` | `get_mw_user_preference()` | `bambu_network_get_mw_user_preference` |
| `FUN_1801a7ad0` | `get_my_message()` | `bambu_network_get_my_message` |
| `FUN_1801a7ae0` | `get_my_profile()` | `bambu_network_get_my_profile` |
| `FUN_1801a7ba0` | `get_my_token()` | `bambu_network_get_my_token` |
| `FUN_1801a7c60` | `get_oss_config()` | `bambu_network_get_oss_config` |
| `FUN_1801a7d30` | `get_printer_firmware()` | `bambu_network_get_printer_firmware` |
| `FUN_1801a7df0` | `get_setting_list`, `get_setting_list2` (shared) | `bambu_network_get_setting_list`, `bambu_network_get_setting_list2` |
| `FUN_1801a7fa0` | `get_slice_info()` | `bambu_network_get_slice_info` |
| `FUN_1801a80e0` | `get_studio_info_url()` | `bambu_network_get_studio_info_url` |
| `FUN_1801a8100` | `get_subtask()` | `bambu_network_get_subtask` |
| `FUN_1801a8190` | `get_subtask_info()` | `bambu_network_get_subtask_info` |
| `FUN_1801a8250` | `get_task_plate_index()` | `bambu_network_get_task_plate_index` |
| `FUN_1801a8300` | `get_user_info()` | `bambu_network_get_user_info` |
| `FUN_1801a83a0` | `get_user_presets()` | `bambu_network_get_user_presets` |
| `FUN_1801a84c0` | `get_user_print_info()` | `bambu_network_get_user_print_info` |
| `FUN_1801a84d0` | `get_user_tasks()` | `bambu_network_get_user_tasks` |
| `FUN_1801a8590` | `init_log()` | `bambu_network_init_log` |
| `FUN_1801a85a0` | `install_device_cert()` | `bambu_network_install_device_cert` |
| `FUN_1801a8650` | `is_server_connected()` | `bambu_network_is_server_connected` |
| `FUN_1801a8660` | `is_user_login()` | `bambu_network_is_user_login` |
| `FUN_1801a8670` | `start()` | `bambu_network_start` |
| `FUN_1801a8690` | `modify_printer_name()` | `bambu_network_modify_printer_name` |
| `FUN_1801a8840` | `put_model_mall_rating()` | `bambu_network_put_model_mall_rating` |
| `FUN_1801a8970` | `put_rating_picture_oss()` | `bambu_network_put_rating_picture_oss` |
| `FUN_1801a8a60` | `put_setting()` | `bambu_network_put_setting` |
| `FUN_1801a8bb0` | `query_bind_status()` | `bambu_network_query_bind_status` |
| `FUN_1801a8c10` | `request_bind_ticket()` | `bambu_network_request_bind_ticket` |
| `FUN_1801a8cb0` | `request_setting_id()` | `bambu_network_request_setting_id` |
| `FUN_1801a8e30` | `send_message()` | `bambu_network_send_message` |
| `FUN_1801a8f70` | `send_message_to_printer()` | `bambu_network_send_message_to_printer` |
| `FUN_1801a90b0` | `set_cert_file()` | `bambu_network_set_cert_file` |
| `FUN_1801a91e0` | `set_config_dir()` | `bambu_network_set_config_dir` |
| `FUN_1801a9300` | `set_country_code()` | `bambu_network_set_country_code` |
| `FUN_1801a93b0` | `set_extra_http_header()` | `bambu_network_set_extra_http_header` |
| `FUN_1801a9440` | `set_get_country_code_fn()` | `bambu_network_set_get_country_code_fn` |
| `FUN_1801a94c0` | `set_on_http_error_fn()` | `bambu_network_set_on_http_error_fn` |
| `FUN_1801a9540` | `set_on_local_connect_fn()` | `bambu_network_set_on_local_connect_fn` |
| `FUN_1801a95c0` | `set_on_local_message_fn()` | `bambu_network_set_on_local_message_fn` |
| `FUN_1801a9640` | `set_on_message_fn()` | `bambu_network_set_on_message_fn` |
| `FUN_1801a96c0` | `set_on_printer_connected_fn()` | `bambu_network_set_on_printer_connected_fn` |
| `FUN_1801a9740` | `set_on_server_connected_fn()` | `bambu_network_set_on_server_connected_fn` |
| `FUN_1801a97c0` | `set_on_ssdp_msg_fn()` | `bambu_network_set_on_ssdp_msg_fn` |
| `FUN_1801a9870` | `set_on_subscribe_failure_fn()` | `bambu_network_set_on_subscribe_failure_fn` |
| `FUN_1801a98f0` | `set_on_user_login_fn()` | `bambu_network_set_on_user_login_fn` |
| `FUN_1801a9970` | `set_on_user_message_fn()` | `bambu_network_set_on_user_message_fn` |
| `FUN_1801a99f0` | `set_queue_on_main_fn()` | `bambu_network_set_queue_on_main_fn` |
| `FUN_1801a9a70` | `set_server_callback()` | `bambu_network_set_server_callback` |
| `FUN_1801a9af0` | `set_user_selected_machine()` | `bambu_network_set_user_selected_machine` |
| `FUN_1801a9ba0` | `bind()` | `bambu_network_bind` |
| `FUN_1801a9e40` | `bind_detect()` | `bambu_network_bind_detect` |
| `FUN_1801a9f80` | `start_discovery()` | `bambu_network_start_discovery` |
| `FUN_1801a9fa0` | `start_local_print()` | `bambu_network_start_local_print` |
| `FUN_1801aa0d0` | `start_local_print_with_record()` | `bambu_network_start_local_print_with_record` |
| `FUN_1801aa250` | `ping_bind()` | `bambu_network_ping_bind` |
| `FUN_1801aa300` | `start_print()` | `bambu_network_start_print` |
| `FUN_1801aa480` | `start_publish()` | `bambu_network_start_publish` |
| `FUN_1801aa600` | `report_consent()` | `bambu_network_report_consent` |
| `FUN_1801aa6b0` | `start_sdcard_print()` | `bambu_network_start_sdcard_print` |
| `FUN_1801aa7e0` | `start_send_gcode_to_sdcard()` | `bambu_network_start_send_gcode_to_sdcard` |
| `FUN_1801aa910` | `start_subscribe()` | `bambu_network_start_subscribe` |
| `FUN_1801aa9c0` | `unbind()` | `bambu_network_unbind` |
| `FUN_1801aaa70` | `stop_subscribe()` | `bambu_network_stop_subscribe` |
| `FUN_1801aab20` | `track_enable()` | `bambu_network_track_enable` |
| `FUN_1801aab40` | `track_event()` | `bambu_network_track_event` |
| `FUN_1801aac70` | `track_get_property()` | `bambu_network_track_get_property` |
| `FUN_1801aadb0` | `track_header()` | `bambu_network_track_header` |
| `FUN_1801aae60` | `track_remove_files()` | `bambu_network_track_remove_files` |
| `FUN_1801aae80` | `track_update_property()` | `bambu_network_track_update_property` |
| `FUN_1801ab030` | `update_cert()` | `bambu_network_update_cert` |
| `FUN_1801ab040` | `get_user_avatar()` | `bambu_network_get_user_avatar` |
| `FUN_1801ab090` | `get_user_id()` | `bambu_network_get_user_id` |
| `FUN_1801ab0e0` | `user_logout()` | `bambu_network_user_logout` |
| `FUN_1801ab0f0` | `get_user_name()` | `bambu_network_get_user_name` |
| `FUN_1801ab140` | `get_user_nickanme()` | `bambu_network_get_user_nickanme` |
| `FUN_1801ab190` | `get_user_selected_machine()` | `bambu_network_get_user_selected_machine` |
| `FUN_1801ad500` | `bind()` | `bambu_network_bind` |
| `FUN_1801ad820` | `unbind()` | `bambu_network_unbind` |
| `FUN_1801adb40` | `ping_bind()` | `bambu_network_ping_bind` |
| `FUN_1801ade60` | shared helper called by 5 exports (e.g. `bind_detect`) | `bambu_network_bind_detect`, `bambu_network_put_setting`, `bambu_network_start_print`, `bambu_network_track_event`, `bambu_network_update_cert` |
| `FUN_1801ae180` | shared helper called by 4 exports (e.g. `get_my_token`) | `bambu_network_get_my_token`, `bambu_network_send_message`, `bambu_network_track_enable`, `bambu_network_track_header` |
| `FUN_1801ae6b0` | `get_user_info()` | `bambu_network_get_user_info` |
| `FUN_1801ae9d0` | shared helper called by 6 exports (e.g. `connect_server`) | `bambu_network_connect_server`, `bambu_network_delete_setting`, `bambu_network_get_my_profile`, `bambu_network_get_oss_config`, `bambu_network_get_user_tasks`, +1 more |
| `FUN_1801aecf0` | `connect_printer()` | `bambu_network_connect_printer` |
| `FUN_1801af010` | `get_setting_list()` | `bambu_network_get_setting_list` |
| `FUN_1801af330` | `get_setting_list2`, `query_bind_status`, `start_local_print` (shared) | `bambu_network_get_setting_list2`, `bambu_network_query_bind_status`, `bambu_network_start_local_print` |
| `FUN_1801af650` | shared helper called by 4 exports (e.g. `disconnect_printer`) | `bambu_network_disconnect_printer`, `bambu_network_start_sdcard_print`, `bambu_network_track_get_property`, `bambu_network_track_remove_files` |
| `FUN_1801af970` | shared helper called by 5 exports (e.g. `get_model_rating_id`) | `bambu_network_get_model_rating_id`, `bambu_network_get_user_print_info`, `bambu_network_modify_printer_name`, `bambu_network_request_bind_ticket`, `bambu_network_set_server_callback` |
| `FUN_1801afc90` | `get_printer_firmware()` | `bambu_network_get_printer_firmware` |
| `FUN_1801affb0` | shared helper called by 5 exports (e.g. `get_model_instance_id`) | `bambu_network_get_model_instance_id`, `bambu_network_get_model_mall_rating`, `bambu_network_get_model_publish_url`, `bambu_network_put_model_mall_rating`, `bambu_network_track_update_property` |
| `FUN_1801b02d0` | `del_rating_picture_oss`, `put_rating_picture_oss` (shared) | `bambu_network_del_rating_picture_oss`, `bambu_network_put_rating_picture_oss` |
| `FUN_1801b05f0` | `get_model_mall_home_url`, `send_message_to_printer` (shared) | `bambu_network_get_model_mall_home_url`, `bambu_network_send_message_to_printer` |
| `FUN_1801b0910` | `get_model_mall_detail_url()` | `bambu_network_get_model_mall_detail_url` |
| `FUN_1801b0c30` | `start_send_gcode_to_sdcard()` | `bambu_network_start_send_gcode_to_sdcard` |
| `FUN_1801b0f50` | `start_local_print_with_record()` | `bambu_network_start_local_print_with_record` |
| `FUN_1801fe910` | `ft.tunnel_create()` | `ft_tunnel_create` |
