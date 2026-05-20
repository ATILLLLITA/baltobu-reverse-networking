# Per-export reference

This directory contains one document per exported function of
`bambu_networking.dll`. The documents describe the ABI as Bambu's
library presents it to the slicer, not the implementation. The
clean-room boundary is preserved: the raw decompiled bodies under
`output/ghidra_decompile/exports/` are gitignored; anyone implementing
the clean-room replacement must read only the documents in this
directory.

Each document is organised the same way:

- The functional group and the relative virtual address inside the
  unpacked dump.
- The recovered C signature with named, typed parameters.
- The behaviour at the ABI level.
- A table of return values where applicable.
- Notes that affect a clean-room implementation but do not belong in
  the formal contract.

For the cross-cutting patterns (agent singleton self-check,
`std::function`-clone callback registration, pass-by-value
`std::string`, refcount discipline in `ft_*`), see
[`../04-public-api.md`](../04-public-api.md).

## Index by family

### Agent lifecycle (5)

- [`bambu_network_create_agent`](bambu_network_create_agent.md) (partial; body recovery blocked by anti-disassembly stub)
- [`bambu_network_destroy_agent`](bambu_network_destroy_agent.md)
- [`bambu_network_init_log`](bambu_network_init_log.md)
- [`bambu_network_start`](bambu_network_start.md)
- [`bambu_network_get_version`](bambu_network_get_version.md)

### Configuration (8)

- [`bambu_network_set_config_dir`](bambu_network_set_config_dir.md)
- [`bambu_network_set_country_code`](bambu_network_set_country_code.md)
- [`bambu_network_set_cert_file`](bambu_network_set_cert_file.md)
- [`bambu_network_set_extra_http_header`](bambu_network_set_extra_http_header.md)
- [`bambu_network_set_user_selected_machine`](bambu_network_set_user_selected_machine.md)
- [`bambu_network_enable_multi_machine`](bambu_network_enable_multi_machine.md)
- [`bambu_network_update_cert`](bambu_network_update_cert.md)
- [`bambu_network_install_device_cert`](bambu_network_install_device_cert.md)

### Callback registration (13)

- [`bambu_network_set_get_country_code_fn`](bambu_network_set_get_country_code_fn.md)
- [`bambu_network_set_on_http_error_fn`](bambu_network_set_on_http_error_fn.md)
- [`bambu_network_set_on_local_connect_fn`](bambu_network_set_on_local_connect_fn.md)
- [`bambu_network_set_on_local_message_fn`](bambu_network_set_on_local_message_fn.md)
- [`bambu_network_set_on_message_fn`](bambu_network_set_on_message_fn.md)
- [`bambu_network_set_on_printer_connected_fn`](bambu_network_set_on_printer_connected_fn.md)
- [`bambu_network_set_on_server_connected_fn`](bambu_network_set_on_server_connected_fn.md)
- [`bambu_network_set_on_ssdp_msg_fn`](bambu_network_set_on_ssdp_msg_fn.md)
- [`bambu_network_set_on_subscribe_failure_fn`](bambu_network_set_on_subscribe_failure_fn.md)
- [`bambu_network_set_on_user_login_fn`](bambu_network_set_on_user_login_fn.md)
- [`bambu_network_set_on_user_message_fn`](bambu_network_set_on_user_message_fn.md)
- [`bambu_network_set_queue_on_main_fn`](bambu_network_set_queue_on_main_fn.md)
- [`bambu_network_set_server_callback`](bambu_network_set_server_callback.md)

### Authentication and login (6)

- [`bambu_network_build_login_cmd`](bambu_network_build_login_cmd.md)
- [`bambu_network_build_login_info`](bambu_network_build_login_info.md)
- [`bambu_network_build_logout_cmd`](bambu_network_build_logout_cmd.md)
- [`bambu_network_change_user`](bambu_network_change_user.md)
- [`bambu_network_is_user_login`](bambu_network_is_user_login.md)
- [`bambu_network_user_logout`](bambu_network_user_logout.md)

### Identity and profile (8)

- [`bambu_network_get_user_id`](bambu_network_get_user_id.md)
- [`bambu_network_get_user_name`](bambu_network_get_user_name.md)
- [`bambu_network_get_user_nickanme`](bambu_network_get_user_nickanme.md)
- [`bambu_network_get_user_avatar`](bambu_network_get_user_avatar.md)
- [`bambu_network_get_user_info`](bambu_network_get_user_info.md)
- [`bambu_network_get_my_profile`](bambu_network_get_my_profile.md)
- [`bambu_network_get_my_token`](bambu_network_get_my_token.md)
- [`bambu_network_get_user_selected_machine`](bambu_network_get_user_selected_machine.md)

### User data (6)

- [`bambu_network_get_user_presets`](bambu_network_get_user_presets.md)
- [`bambu_network_get_user_print_info`](bambu_network_get_user_print_info.md)
- [`bambu_network_get_user_tasks`](bambu_network_get_user_tasks.md)
- [`bambu_network_get_my_message`](bambu_network_get_my_message.md)
- [`bambu_network_get_mw_user_4ulist`](bambu_network_get_mw_user_4ulist.md)
- [`bambu_network_get_mw_user_preference`](bambu_network_get_mw_user_preference.md)

### Telemetry consent (3)

- [`bambu_network_check_user_report`](bambu_network_check_user_report.md)
- [`bambu_network_check_user_task_report`](bambu_network_check_user_task_report.md)
- [`bambu_network_report_consent`](bambu_network_report_consent.md)

### Settings (5)

- [`bambu_network_get_setting_list`](bambu_network_get_setting_list.md)
- [`bambu_network_get_setting_list2`](bambu_network_get_setting_list2.md)
- [`bambu_network_put_setting`](bambu_network_put_setting.md)
- [`bambu_network_delete_setting`](bambu_network_delete_setting.md)
- [`bambu_network_request_setting_id`](bambu_network_request_setting_id.md)

### Device binding (7)

- [`bambu_network_request_bind_ticket`](bambu_network_request_bind_ticket.md)
- [`bambu_network_bind`](bambu_network_bind.md)
- [`bambu_network_bind_detect`](bambu_network_bind_detect.md)
- [`bambu_network_ping_bind`](bambu_network_ping_bind.md)
- [`bambu_network_query_bind_status`](bambu_network_query_bind_status.md)
- [`bambu_network_unbind`](bambu_network_unbind.md)
- [`bambu_network_modify_printer_name`](bambu_network_modify_printer_name.md)

### Printer connection (5)

- [`bambu_network_connect_printer`](bambu_network_connect_printer.md)
- [`bambu_network_disconnect_printer`](bambu_network_disconnect_printer.md)
- [`bambu_network_connect_server`](bambu_network_connect_server.md)
- [`bambu_network_is_server_connected`](bambu_network_is_server_connected.md)
- [`bambu_network_refresh_connection`](bambu_network_refresh_connection.md)

### Pub/Sub (5)

- [`bambu_network_add_subscribe`](bambu_network_add_subscribe.md)
- [`bambu_network_del_subscribe`](bambu_network_del_subscribe.md)
- [`bambu_network_start_subscribe`](bambu_network_start_subscribe.md)
- [`bambu_network_stop_subscribe`](bambu_network_stop_subscribe.md)
- [`bambu_network_start_publish`](bambu_network_start_publish.md)

### Messaging (2)

- [`bambu_network_send_message`](bambu_network_send_message.md)
- [`bambu_network_send_message_to_printer`](bambu_network_send_message_to_printer.md)

### Print job submission (9)

- [`bambu_network_start_print`](bambu_network_start_print.md)
- [`bambu_network_start_local_print`](bambu_network_start_local_print.md)
- [`bambu_network_start_local_print_with_record`](bambu_network_start_local_print_with_record.md)
- [`bambu_network_start_sdcard_print`](bambu_network_start_sdcard_print.md)
- [`bambu_network_start_send_gcode_to_sdcard`](bambu_network_start_send_gcode_to_sdcard.md)
- [`bambu_network_get_slice_info`](bambu_network_get_slice_info.md)
- [`bambu_network_get_subtask`](bambu_network_get_subtask.md)
- [`bambu_network_get_subtask_info`](bambu_network_get_subtask_info.md)
- [`bambu_network_get_task_plate_index`](bambu_network_get_task_plate_index.md)

### Cloud media (5)

- [`bambu_network_get_oss_config`](bambu_network_get_oss_config.md)
- [`bambu_network_put_rating_picture_oss`](bambu_network_put_rating_picture_oss.md)
- [`bambu_network_del_rating_picture_oss`](bambu_network_del_rating_picture_oss.md)
- [`bambu_network_put_model_mall_rating`](bambu_network_put_model_mall_rating.md)
- [`bambu_network_get_model_mall_rating`](bambu_network_get_model_mall_rating.md)

### Service URLs and Makerworld endpoints (8)

- [`bambu_network_get_bambulab_host`](bambu_network_get_bambulab_host.md)
- [`bambu_network_get_studio_info_url`](bambu_network_get_studio_info_url.md)
- [`bambu_network_get_model_mall_home_url`](bambu_network_get_model_mall_home_url.md)
- [`bambu_network_get_model_mall_detail_url`](bambu_network_get_model_mall_detail_url.md)
- [`bambu_network_get_model_publish_url`](bambu_network_get_model_publish_url.md)
- [`bambu_network_get_model_rating_id`](bambu_network_get_model_rating_id.md)
- [`bambu_network_get_model_instance_id`](bambu_network_get_model_instance_id.md)
- [`bambu_network_get_design_staffpick`](bambu_network_get_design_staffpick.md)

### Camera streaming (2)

- [`bambu_network_get_camera_url`](bambu_network_get_camera_url.md)
- [`bambu_network_get_camera_url_for_golive`](bambu_network_get_camera_url_for_golive.md)

### Device firmware and HMS (2)

- [`bambu_network_get_printer_firmware`](bambu_network_get_printer_firmware.md)
- [`bambu_network_get_hms_snapshot`](bambu_network_get_hms_snapshot.md)

### Discovery (1)

- [`bambu_network_start_discovery`](bambu_network_start_discovery.md)

### Telemetry `track_*` (6)

- [`bambu_network_track_enable`](bambu_network_track_enable.md)
- [`bambu_network_track_event`](bambu_network_track_event.md)
- [`bambu_network_track_header`](bambu_network_track_header.md)
- [`bambu_network_track_update_property`](bambu_network_track_update_property.md)
- [`bambu_network_track_get_property`](bambu_network_track_get_property.md)
- [`bambu_network_track_remove_files`](bambu_network_track_remove_files.md)

### Debug and misc (1)

- [`bambu_network_check_debug_consistent`](bambu_network_check_debug_consistent.md)

### `ft_*` namespace (21)

- [`ft_abi_version`](ft_abi_version.md)
- [`ft_free`](ft_free.md)
- [`ft_job_cancel`](ft_job_cancel.md)
- [`ft_job_create`](ft_job_create.md)
- [`ft_job_get_msg`](ft_job_get_msg.md)
- [`ft_job_get_result`](ft_job_get_result.md)
- [`ft_job_msg_destroy`](ft_job_msg_destroy.md)
- [`ft_job_release`](ft_job_release.md)
- [`ft_job_result_destroy`](ft_job_result_destroy.md)
- [`ft_job_retain`](ft_job_retain.md)
- [`ft_job_set_msg_cb`](ft_job_set_msg_cb.md)
- [`ft_job_set_result_cb`](ft_job_set_result_cb.md)
- [`ft_job_try_get_msg`](ft_job_try_get_msg.md)
- [`ft_tunnel_create`](ft_tunnel_create.md)
- [`ft_tunnel_release`](ft_tunnel_release.md)
- [`ft_tunnel_retain`](ft_tunnel_retain.md)
- [`ft_tunnel_set_status_cb`](ft_tunnel_set_status_cb.md)
- [`ft_tunnel_shutdown`](ft_tunnel_shutdown.md)
- [`ft_tunnel_start_connect`](ft_tunnel_start_connect.md)
- [`ft_tunnel_start_job`](ft_tunnel_start_job.md)
- [`ft_tunnel_sync_connect`](ft_tunnel_sync_connect.md)

## What is not in this directory

This per-export reference describes the C ABI surface. The wire
protocols (MQTT topic shapes, REST endpoint paths, JSON message
schemas) are documented in
[`../05-wire-protocol.md`](../05-wire-protocol.md). The internal struct
layouts (beyond what destructors made visible) are documented in
[`../06-internals.md`](../06-internals.md). The Linux `.so` and macOS
`.dylib` variants have not yet been acquired; the hypothesis is that
the three operating systems share a single C ABI, which is yet to be
confirmed.
