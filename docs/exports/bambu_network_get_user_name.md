# `bambu_network_get_user_name`

**Group:** identity
**RVA:** `0x1ba290` &nbsp; **VA:** `0x1801ba290`
**Body size:** 64 bytes &nbsp; **Direct callees:** 1

## Signature

```c
void bambu_network_get_user_name(std::string *out, agent_t *agent);
```

## Behavior

Returns the user's account name (e.g. an email address or username, depending on how they signed up). Empty when no user is logged in. via the hidden first parameter. On global-mismatch, constructs an **empty** `std::string` into `*out` (length=0, capacity=15) and returns. Otherwise dispatches to the agent's internal getter to populate `*out`.

## Notes

Read-only accessor. The library does not return ownership of an internal buffer; the caller's storage is populated via copy.
