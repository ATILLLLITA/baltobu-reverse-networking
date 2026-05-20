# `bambu_network_set_on_local_connect_fn`

**Group:** callback registration (local_connect)
**RVA:** `0x1bbf20` &nbsp; **VA:** `0x1801bbf20`
**Body size:** 153 bytes &nbsp; **Direct callees:** 1

## Signature

```c
using cb_t = std::function<void(const std::string &/* dev_id */, int /* status */)>;

int bambu_network_set_on_local_connect_fn(agent_t *agent, cb_t *fn);
```

The `fn` argument is a pointer to an MSVC `std::function<...>` (a 56-byte object: SBO buffer + heap-stored functor pointer + vtable for clone/destroy/invoke).

## Behavior

Fires when a LAN-mode session to a discovered printer connects, disconnects or fails. Paired with start_local_print.

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
| `-1` | `agent` mismatched the global singleton. |

## Notes

- Register before `bambu_network_start`. After `start`, the worker threads may fire this callback at any time.
- The library takes a **deep copy** of the `std::function`. Captured state in the lambda lives until the agent is destroyed or until another call to this setter replaces it.
