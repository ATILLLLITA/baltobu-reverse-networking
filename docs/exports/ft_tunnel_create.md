# `ft_tunnel_create`

**Group:** `ft_*` lifecycle (tunnels)
**RVA:** `0x249dc0` &nbsp; **VA (image base 0x180000000):** `0x180249dc0`
**Body size:** 879 bytes &nbsp; **Direct callees:** ~6
**MSVC mangling:** none (plain C export)

## Signature

```c
int ft_tunnel_create(const char *spec_str, ft_tunnel **out_tunnel);
```

`spec_str` is a NUL-terminated string identifying the remote endpoint
(host / URL / identifier, likely a printer LAN address or cloud
endpoint). `out_tunnel` receives the new tunnel handle.

## Behavior

1. If either argument is NULL, returns `-1`.
2. **One-time global init**, guarded by an `std::once_flag` at
   `DAT_1807a9d38`. The first call into `ft_tunnel_create` (in the
   process lifetime) constructs two singleton I/O contexts (likely
   `boost::asio::io_context` or an internal equivalent) at
   `DAT_1807a9ca0` and `DAT_1807a9cf0`, bumps their refcounts, and
   registers an `atexit` handler (`FUN_1805e39c0`) to tear them
   down at process exit.
3. Wrap `spec_str` in a local `std::string`.
4. Allocate a **48-byte intermediate object** via `operator new(0x30)`
   that wraps the spec plus a flag (`0x100000`), likely a connect
   request descriptor.
5. Call into the I/O subsystem's `start_session`-style helper
   (`FUN_180240430`) which builds the actual transport-level
   session object and returns it.
6. Allocate the **56-byte `ft_tunnel`** via `operator new(0x38)` ,
   exactly the size released by
   [`ft_tunnel_release`](ft_tunnel_release.md).
7. Initialise the tunnel:
   - `refcount = 1`
   - six `uint64_t` smart-pointer slots zeroed
   - the transport session pointer is moved into the smart-pointer
     slot at `+0x10` (later visible to
     [`ft_tunnel_start_connect`](ft_tunnel_start_connect.md) and
     [`ft_tunnel_set_status_cb`](ft_tunnel_set_status_cb.md))
8. Write the new pointer to `*out_tunnel`. Drop the intermediate
   object's refcount. Return `0`.

## Side effects

- **First call only**: allocates two process-wide I/O singletons
  and registers an `atexit` handler. The singletons live for the
  duration of the process.
- Each call: heap allocations for the intermediate request
  descriptor and the tunnel struct itself.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | `*out_tunnel` set; the caller owns the reference. |
| `-1` | Either argument was NULL. |

## Notes

- The created tunnel is **not connected**, it is just a handle
  bound to the requested endpoint. Use
  [`ft_tunnel_sync_connect`](ft_tunnel_sync_connect.md) or
  [`ft_tunnel_start_connect`](ft_tunnel_start_connect.md) to
  actually open the transport.
- The two global I/O contexts likely correspond to (a) the
  background worker thread pool and (b) the I/O multiplexer; they
  are shared across **all** tunnels in the process.
- The `0x100000` flag set on the intermediate object is unidentified;
  it may be a buffer-size hint, an option set or a feature bitmask.
- One-time-init pattern: the
  `*(ThreadLocalStoragePointer + tls_index * 8 + 0x28) <
  DAT_1807a9d38` test, combined with the `FUN_180564ff0` /
  `FUN_180564f90` calls around the singleton construction, is the
  MSVC implementation of `std::call_once` / `std::once_flag`.
- The function does not validate the spec format. An invalid spec
  is presumably caught later, when connect is attempted.

## Implementation notes for the clean-room

```c
int ft_tunnel_create(const char *spec_str, ft_tunnel **out) {
    if (!spec_str || !out) return -1;
    std::call_once(g_io_init, init_io_singletons);
    auto *tunnel = new ft_tunnel{};
    tunnel->session = make_session(spec_str, /* opts */ 0x100000);
    *out = tunnel;
    return 0;
}
```
