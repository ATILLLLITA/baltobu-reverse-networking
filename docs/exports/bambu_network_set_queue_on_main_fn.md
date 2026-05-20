# `bambu_network_set_queue_on_main_fn`

**Group:** callback registration (UI thread trampoline)
**RVA:** `0x1bc4c0` &nbsp; **VA:** `0x1801bc4c0`
**Body size:** 153 bytes &nbsp; **Direct callees:** 1

## Signature

```c
using cb_t = std::function<void(std::function<void()> /* task */)>;

int bambu_network_set_queue_on_main_fn(agent_t *agent, cb_t *fn);
```

The `fn` argument is a pointer to an MSVC `std::function<...>` (a 56-byte object: SBO buffer + heap-stored functor pointer + vtable for clone/destroy/invoke).

## Behavior

Trampoline the slicer hands the library so it can post tasks back onto the GUI thread. Whenever the library has a callback that must run on the slicer's main thread, it calls queue_on_main(task) instead of invoking task() directly. Critical for any UI work.

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
- The slicer MUST register this before issuing any call that may schedule UI work; otherwise the library has no way to dispatch back onto the GUI thread.
