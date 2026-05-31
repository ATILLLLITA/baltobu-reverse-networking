# bambu_networking — LAN discovery (SSDP)

Recovered from the symbol-bearing Linux `01.07.01.04` build + the `02.06.00.50` memory image.

## Implementation

LAN discovery is `BBL::SsdpDiscovery`, built on the open-source **lssdp** library (statically
linked). Symbols:

| Symbol | Role |
|--------|------|
| `BBL::SsdpDiscovery::start()` @ 0x138ff6 | spawn the discovery thread (`start_discover`) |
| `BBL::SsdpDiscovery::stop()` @ 0x138f1a | stop |
| `BBL::SsdpDiscovery::ssdp_thread()` @ 0x1392da | the socket loop |
| `BBL::SsdpDiscovery::parse_sdp_message(char*…)` @ 0x138ad4 | parse a received NOTIFY/response |
| `BBL::on_packet_received(lssdp_ctx*)` / `show_neighbor_list` | lssdp callbacks |
| `lssdp_socket_create/read/close`, `lssdp_send_msearch`, `lssdp_send_notify`, `lssdp_packet_parser`, `send_multicast_data`, `lssdp_network_interface_update`, `lssdp_neighbor_check_timeout` | the bundled lssdp core |

The public entry is `bambu_network_start_discovery` → `BambuNetworkAgent::start_discovery`;
results are delivered through `set_on_ssdp_msg_fn(std::function<void(std::string)>)` — the raw
SSDP message text is handed to the host app.

## Wire details

- **Multicast group:** `239.255.255.250` (standard SSDP). Port is set programmatically (not a
  string literal); our live farm capture observed the printer NOTIFY on UDP **1990** and
  **2021** — consistent with lssdp + Bambu's custom ports. *(port value inferred)*
- **Device URN:** `urn:bambulab-com:device:3dprinter:1` (a truncated
  `urn:bambulab-com:device:3dprinter…slicer_service…` URN is also present for the slicer service).
- **SSDP verbs/headers parsed:** `NOTIFY`, `M-SEARCH`, `HTTP/1.1`, `NT:`, `NTS:ssdp:alive`,
  `USN:`, `Location:`, `CACHE-CONTROL:max-age`. A region guard `NTS_check` / `NTS_check_CN` is
  present.

## Bambu custom headers (`Dev*.bambu.com`)

These header **names** (not domains the printer contacts) carry the device inventory:

| Header | Meaning | Build |
|--------|---------|-------|
| `DevName.bambu.com` | friendly name | both |
| `DevModel.bambu.com` | model code (e.g. C12) | both |
| `DevBind.bambu.com` | bind state (`free` / bound) | both |
| `DevConnect.bambu.com` | connection kind | both |
| `DevSignal.bambu.com` | Wi-Fi RSSI | both |
| `Devseclink.bambu.com` | secure-link / TLS state | both |
| `DevVersion.bambu.com` | firmware version | **02.06 only** |
| `DevInf.bambu.com` | interface/extra info | **02.06 only** |

(Our farm capture also saw `DevCap.bambu.com`; it is firmware-emitted but not referenced by
either SDK build here.) `DevBind = free` is what the manager uses to offer an unbound printer.

## Relation to farm discovery

This is the **printer-side** discovery (`urn:…:device:3dprinter:1`). The Farm Manager *server*
advertises a different URN (`urn:bambulab-com:farm:server:1`) — see OpenBambuFarmAPI
`farm-discovery.md`. Both ride the same `239.255.255.250` multigroup; the SDK here is the
client/slicer half that finds printers on the LAN.
