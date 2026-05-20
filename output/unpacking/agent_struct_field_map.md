# Inferred `BambuNetworkAgent` internal-state struct layout

The exported `agent_t *` is a 16-byte wrapper whose first 8 bytes
point at the **real** agent state on the heap. Every internal
helper takes the wrapper pointer as `param_1` and dereferences
it (`lVar1 = *param_1`) to reach the real state.

This map is built by mining each helper for offsets of the form
`(<base> + 0xXXX)` where `<base>` is an 8-aligned variable bound
from `*param_1`. We restrict to helpers that demonstrably
dereference `*param_1` (i.e. operate on the real state, not on
auxiliary arguments). Offsets are 8-byte aligned and within
`[0x10, 0x4000]` to avoid noise from loop strides, vtable
slots and absolute addresses.

Reading this table:

- *High-confidence fields* are those touched by a single setter
  whose meaning the export name reveals (e.g. `0x958` →
  `set_config_dir` → almost certainly the `config_dir`
  `std::string`).
- *Internal/structural fields* are those touched by many
  helpers; they're probably hub members like the cloud HTTP
  client, the broker pointer, callback tables, etc.

| Offset | Hex | # helpers | Likely meaning (from caller exports) |
|---:|---|---:|---|
| 16 | `0x10` | 11 | `enable_multi_machine`, `ft.job_cancel`, `ft.job_create`, `ft.job_get_msg`, `ft.job_get_result`, +11 |
| 24 | `0x18` | 11 | `build_login_info`, `ft.job_cancel`, `ft.job_create`, `ft.job_get_msg`, `ft.job_get_result`, +7 |
| 32 | `0x20` | 36 | `add_subscribe`, `bind`, `build_login_info`, `del_subscribe`, `ft.job_create`, +34 |
| 40 | `0x28` | 56 | `bind`, `bind_detect`, `change_user`, `connect_printer`, `delete_setting`, +51 |
| 48 | `0x30` | 13 | `ft.job_cancel`, `ft.tunnel_create`, `ft.tunnel_start_job`, `get_model_instance_id`, `get_model_mall_home_url`, +16 |
| 56 | `0x38` | 1 | `ft.tunnel_set_status_cb` |
| 64 | `0x40` | 2 | `ft.job_cancel`, `start_publish` |
| 72 | `0x48` | 1 | `ft.job_cancel` |
| 88 | `0x58` | 1 | `ft.tunnel_create` |
| 96 | `0x60` | 3 | `get_user_id`, `start_local_print`, `start_local_print_with_record`, `start_print`, `start_publish`, +2 |
| 128 | `0x80` | 3 | `build_login_info`, `get_user_avatar`, `start_publish` |
| 160 | `0xa0` | 1 | `start_publish` |
| 176 | `0xb0` | 1 | `get_model_instance_id`, `get_model_mall_rating`, `get_model_publish_url`, `put_model_mall_rating`, `track_update_property` |
| 224 | `0xe0` | 1 | `ft.job_cancel` |
| 240 | `0xf0` | 1 | `ft.job_cancel` |
| 248 | `0xf8` | 1 | `ft.job_cancel` |
| 2392 | `0x958` | 1 | `set_config_dir` |
| 2552 | `0x9f8` | 1 | `get_user_selected_machine` |
| 2624 | `0xa40` | 1 | `get_user_presets` |
| 2640 | `0xa50` | 1 | `get_user_presets` |
