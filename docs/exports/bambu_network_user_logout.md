# `bambu_network_user_logout`

**Group:** auth / login
**RVA:** `0x1be540` &nbsp; **VA:** `0x1801be540`
**Body size:** 41 bytes &nbsp; **Direct callees:** 1

## Signature

```c
int bambu_network_user_logout(agent_t *agent);
```

## Behavior

Initiate user logout. Calls into the agent's logout helper (`FUN_1801a...`), which publishes a logout MQTT command and clears cached identity state on success. Asynchronous; observe completion via the appropriate user-message callback.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

