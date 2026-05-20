# `bambu_network_set_server_callback`

**Group:** callback registration (generic server callback)
**RVA:** `0x1bc560` &nbsp; **VA:** `0x1801bc560`
**Body size:** 403 bytes &nbsp; **Direct callees:** 5

## Signature

```c
using cb_t = std::function<void(/* opaque */)>;

int bambu_network_set_server_callback(agent_t *agent, cb_t *fn);
```

The `fn` argument is a pointer to an MSVC `std::function<...>` (a 56-byte object: SBO buffer + heap-stored functor pointer + vtable for clone/destroy/invoke).

## Behavior

A generic 'server-side event' router; distinct from the typed set_on_*_fn family. Recovered body returns -6 (`0xFFFFFFFA`) on internal setter failure and emits an error log line. Exact callback shape is not yet pinned down.

Follows the standard callback-setter pattern shared across all 16 callback exports (see [bambu_network_set_on_http_error_fn](bambu_network_set_on_http_error_fn.md) for the canonical walkthrough):

1. Self-check `if (g_agent != agent) return -1;` (after destroying the caller's `std::function` regardless).
2. Clone the caller's `std::function` into a local 56-byte buffer via the source's vtable[0].
3. Hand the clone to the agent's internal setter helper.
4. Destroy the caller's `std::function` (vtable[4]).
5. Return `0` on success.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | Callback installed. |
| `-6` (`0xFFFFFFFA`) | Internal setter rejected the callback (logged as an error). |
| `-1` | `agent` mismatched the global singleton. |

## Notes

- Register before `bambu_network_start`. After `start`, the worker threads may fire this callback at any time.
- The library takes a **deep copy** of the `std::function`. Captured state in the lambda lives until the agent is destroyed or until another call to this setter replaces it.
