# `bambu_network_get_user_nickanme`

**Group:** identity
**RVA:** `0x1ba2d0` &nbsp; **VA:** `0x1801ba2d0`
**Body size:** 64 bytes &nbsp; **Direct callees:** 1

## Signature

```c
void bambu_network_get_user_nickanme(std::string *out, agent_t *agent);
```

## Behavior

Returns the user's display nickname (sic. Bambu's typo in the export name). Empty when no user is logged in. via the hidden first parameter. On global-mismatch, constructs an **empty** `std::string` into `*out` (length=0, capacity=15) and returns. Otherwise dispatches to the agent's internal getter to populate `*out`.

## Notes

Read-only accessor. The library does not return ownership of an internal buffer; the caller's storage is populated via copy.
