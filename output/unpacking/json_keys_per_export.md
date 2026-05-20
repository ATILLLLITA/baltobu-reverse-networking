# JSON keys referenced per export (recovered from helpers)

For each `bambu_network_*` / `ft_*` export, the set of JSON
string keys its primary helper(s) reference via
`nlohmann::json::operator[]` (`FUN_1800e6ca0`).

These are the **literal field names** the library puts in the
messages it builds and parses. They're the concrete wire schema
each export touches.

Caveats:

- Some keys are passed via `&DAT_18...` pointers (literals
  in `.rdata`); they've been resolved to their string value.
- Helpers shared across many exports show the union of all keys.
- Keys read indirectly (through computed strings or vtable
  dispatch) are not captured.

## `bambu_network_build_login_info` (5 keys)

- `"avatar"`
- `"command"`
- `"data"`
- `"name"`
- `"sequence_id"`

