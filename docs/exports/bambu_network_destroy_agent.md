# `bambu_network_destroy_agent`

**Group:** agent lifecycle
**RVA:** `0x1b83a0` &nbsp; **VA (image base 0x180000000):** `0x1801b83a0`
**Body size:** 74 bytes &nbsp; **Direct callees:** 2
**MSVC mangling:** none (plain C export)

## Signature

```c
int bambu_network_destroy_agent(agent_t *agent);
```

## Behavior

Validates and tears down the process's singleton networking agent.

1. Compare the caller-supplied `agent` against the global agent
   pointer `g_agent` (`.data` RVA `0x7a99e0`). If they differ,
   return `-1` without modifying any state.
2. If `g_agent` is non-NULL:
   - Call the agent's deep destructor (`FUN_1801a6ae0`). This is the
     C++ destructor that walks every internal smart pointer,
     callback holder, MQTT connection and worker handle, freeing
     them in the correct order.
   - Call `operator delete` (the small-buffer dealloc thunk
     `thunk_FUN_1805664dd`) on the wrapper, passing size `0x10`.
3. Write `NULL` to `g_agent`. Return `0`.

## The `0x10` size and the wrapper-handle hypothesis

The `operator delete` call is given a size of **16 bytes**, which
is far too small to hold the agent's full state (seventeen
callback function pointers alone would need 17 × 16 = 272 bytes,
plus the worker threads, MQTT client, login state, etc.).

The most plausible reading: `g_agent` points to a **16-byte wrapper
handle**, and the wrapper owns the real agent state via an internal
pointer. The deep destructor `FUN_1801a6ae0` handles the inner
state; the `operator delete` here releases the outer wrapper. We
do not yet have direct evidence of the wrapper's layout, we will
learn it once we recover `bambu_network_create_agent`'s body
(currently blocked by anti-disassembly).

An alternative reading is that the `0x10` size is a decompiler
artefact: the `thunk_FUN_1805664dd` deallocator may have a non-standard
calling convention that Ghidra mis-decoded. The wrapper hypothesis is
the more likely of the two, since it matches the deep destructor's
`operator delete(state, 0xab8)` on the inner allocation.

## Side effects

- Heap free of the agent wrapper.
- Indirect: tearing down the agent's children (threads stopped,
  MQTT disconnected, callbacks released).
- Writes `NULL` to the global `g_agent`.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | The global agent matched the supplied handle and was destroyed. |
| `-1` | The supplied handle did not match the current `g_agent`. |

## Notes

- The function does **not** check whether `agent == NULL`, if the
  caller passes NULL and the global is also NULL, the function
  returns `0` without doing anything. (Both branches reach the
  "do nothing" code path: the `g_agent != param_1` check is `0 == 0`
  → false → fall through; then `g_agent != 0` is false → skip
  destruction; then write NULL → return 0.) This is benign but
  surprising; clean-room callers should not rely on it.
- After `destroy_agent` succeeds, every other `bambu_network_*`
  function that takes an agent argument will return `-1` until a
  new `create_agent` runs. The library state is fully reset.
- This is **the** point in the API where the global is cleared.
  No other recovered export writes `NULL` to `DAT_1807a99e0`.

## Implementation notes for the clean-room

```c
int bambu_network_destroy_agent(agent_t *agent) {
    if (g_agent != agent) return -1;
    if (g_agent) {
        agent_destroy_internals(g_agent);   // walks SP / cb / threads
        delete g_agent;                      // free outer wrapper
    }
    g_agent = NULL;
    return 0;
}
```
