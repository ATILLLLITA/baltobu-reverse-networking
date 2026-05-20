# `bambu_network_set_user_selected_machine`

**Group:** device selection
**RVA:** `0x1bc700` &nbsp; **VA:** `0x1801bc700`
**Body size:** 78 bytes &nbsp; **Direct callees:** 3

## Signature

```c
int bambu_network_set_user_selected_machine(agent_t *agent,
                                         std::string dev_id);
```

## Behavior

Pass-by-value `std::string`. Marks the named printer as the currently-selected one, this becomes the default target for subsequent agent operations that don't explicitly name a printer. Self-check + destroy on both paths.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

## Notes

Paired with `bambu_network_get_user_selected_machine` (reader). The selection is persisted to disk under the config directory so it survives restarts.
