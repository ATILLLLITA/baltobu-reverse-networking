# `bambu_network_change_user`

**Group:** auth / login
**RVA:** `0x1b66e0` &nbsp; **VA:** `0x1801b66e0`
**Body size:** 78 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_change_user(agent_t *agent, std::string user_credentials);
```

## Behavior

Switch the active user to the one identified by `user_credentials` (opaque JSON blob: token + user ID + maybe refresh token). The string is passed by value; the callee destroys it. The function kicks off an asynchronous re-init of agent state under the new user. The login result arrives via `set_on_user_login_fn`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

This is also the entry point used during the *initial* login: the slicer calls `change_user` once it has obtained the credentials (via OAuth flow or password POST handled outside the library).
