# `ft_tunnel_sync_connect`

**Group:** `ft_*` connect (tunnels)
**RVA:** `0x24ab80` &nbsp; **VA (image base 0x180000000):** `0x18024ab80`
**Body size:** 62 bytes &nbsp; **Direct callees:** 1
**MSVC mangling:** none (plain C export)

## Signature

```c
int ft_tunnel_sync_connect(ft_tunnel *tunnel);
```

## Behavior

Blocking connect. Returns when the transport is either fully
connected or has failed.

1. If `tunnel == NULL`, returns `-1`.
2. Call into the underlying session's blocking connect helper
   (`FUN_18026fc00(tunnel->session)`). The call returns only when
   the I/O loop has reached a terminal state.
3. Inspect the session's internal state at offset `+0x290`. If
   it is `3` (the "connected" state), return `0`. Otherwise
   return `-4` (`0xFFFFFFFC`).

## Side effects

- Blocks the calling thread for the duration of the connect
  attempt. There is no caller-supplied timeout.
- Drives whatever I/O loop the session is bound to until the
  connect completes.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | Connected, the tunnel is ready to carry jobs. |
| `-1` | `tunnel` was NULL. |
| `-4` (`0xFFFFFFFC`) | Connect did not succeed (session ended in a non-3 state). |

## Notes

- The state-byte at session `+0x290` is the underlying transport's
  state machine. The value `3` is the "connected / ready" state.
  Other plausible states (numbers TBD) include
  *unconnected* / *connecting* / *failed* / *closed*.
- The function does not surface a granular error reason, `-4`
  collapses every failure mode (DNS failure, refused, TLS error,
  …) into a single return. To get the underlying error, the
  caller would presumably read it from the status callback
  registered via
  [`ft_tunnel_set_status_cb`](ft_tunnel_set_status_cb.md).
- Pair with [`ft_tunnel_start_connect`](ft_tunnel_start_connect.md)
  if you want the async variant.
- The function is unusually small (62 bytes) because all real
  work is inside `FUN_18026fc00`. This export is a thin wrapper.

## Implementation notes for the clean-room

```c
int ft_tunnel_sync_connect(ft_tunnel *tunnel) {
    if (!tunnel) return -1;
    ft_session_blocking_connect(tunnel->session);
    return tunnel->session->state == FT_STATE_CONNECTED ? 0 : -4;
}
```
