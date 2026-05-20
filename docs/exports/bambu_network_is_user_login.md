# `bambu_network_is_user_login`

**Group:** auth / login
**RVA:** `0x1ba710` &nbsp; **VA:** `0x1801ba710`
**Body size:** 23 bytes &nbsp; **Direct callees:** 1

## Signature

```c
bool bambu_network_is_user_login(agent_t *agent);
```

## Behavior

Returns `true` (non-zero) when a user is currently logged in, `false` otherwise. The global self-check is implicit: if `g_agent != agent`, the function returns `false`.

## Notes

Cheap synchronous accessor. After `change_user` succeeds and the login callback has fired with status=0, this flips to `true`.
