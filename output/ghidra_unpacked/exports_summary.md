# Post-unpack export inventory

- Total exports: **132**
- Suspicious (possible VM-protected): **10**

- Average export body size: **265** bytes
- Normal-looking exports: **122**

## Suspicious exports

| Export | RVA | Size | Callees | Why |
|---|---|---:|---:|---|
| `bambu_network_check_debug_consistent` | `0x1b6730` | 7 | 0 | tiny; |
| `bambu_network_create_agent` | `0x1b6d00` | 5 | 0 | tiny; |
| `ft_abi_version` | `0x2492b0` | 6 | 0 | tiny; |
| `ft_free` | `0x2492c0` | 7 | 0 | tiny; |
| `ft_job_retain` | `0x249cb0` | 15 | 0 | tiny; |
| `ft_tunnel_retain` | `0x24a230` | 15 | 0 | tiny; |
| `ft_tunnel_shutdown` | `0x24a3f0` | 6 | 0 | tiny; |
| `tls_callback_1` | `0x563e20` | 10 | 0 | tiny; |
| `EXT_181b72917` | `0x1b72917` | 0 | 0 | tiny; |
| `EXT_181e36e5a` | `0x1e36e5a` | 0 | 0 | tiny; |
