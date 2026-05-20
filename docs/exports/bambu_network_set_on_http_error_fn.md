# `bambu_network_set_on_http_error_fn`

**Group:** callback registration (HTTP)
**RVA:** `0x1bbe80` &nbsp; **VA:** `0x1801bbe80`
**Body size:** 122 bytes &nbsp; **Direct callees:** ~2

## Signature

```c
typedef std::function<void(int status, const std::string &url, const std::string &body)>
        on_http_error_fn_t;  // exact signature inferred from the name; payload TBD

int bambu_network_set_on_http_error_fn(agent_t *agent,
                                       on_http_error_fn_t *fn);
```

The `fn` argument is a pointer to an MSVC `std::function<...>` (56-byte
object, small-buffer optimisation slot + heap pointer + vtable).
The callee copies the functor into the agent's internal storage via
its `std::function` clone-vtable; the caller's copy is destroyed
before return.

## Behavior

Standard callback-setter pattern (shared across all 16 callback
exports). See [the family overview](README.md#callback-setter-family).

1. Self-check: `if (g_agent != agent) return -1;` (after first
   destroying the caller's `std::function` so the cleanup happens
   regardless of the error path).
2. Clone the caller's `std::function` into a local 56-byte buffer
   via the source's vtable[0] (`std::function::__clone__`).
3. Hand the clone to the agent's internal setter
   (`FUN_1801a94c0`).
4. Destroy the caller's `std::function` (vtable[4]).
5. Return `0`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | Callback installed. |
| `-1` | `agent` mismatched the global singleton. |

## Notes

- Fires when an outbound HTTP request from the library hits an
  error response (HTTP ≥ 400) or a transport failure. Exact
  payload shape (status code, URL, response body) is inferable
  from the family but not directly confirmed by the body.
- Register before `bambu_network_start`. After `start`, the
  worker threads may fire this callback at any time.
- The library takes a **deep copy** of the `std::function`. Any
  captured state the caller put in the lambda will live for the
  lifetime of the agent or until replaced by another
  `set_on_http_error_fn` call.
