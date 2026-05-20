# `bambu_network_request_bind_ticket`

**Group:** device binding
**RVA:** `0x1bb540` &nbsp; **VA:** `0x1801bb540`
**Body size:** 245 bytes &nbsp; **Direct callees:** 5

## Signature

```c
int bambu_network_request_bind_ticket(agent_t *agent, std::string dev_id, std::string *out);
```

## Behavior

Request a one-time bind ticket from Bambu's cloud for the printer identified by `dev_id`. The ticket (an opaque short string) is stored in `*out`. The slicer presents it to the printer (manually or via LAN handshake) to start the bind handshake.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

First step of the bind flow: cloud → ticket → printer challenge → cloud confirmation.
