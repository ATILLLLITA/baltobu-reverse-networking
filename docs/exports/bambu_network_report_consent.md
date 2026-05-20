# `bambu_network_report_consent`

**Group:** telemetry consent
**RVA:** `0x1bb3d0` &nbsp; **VA:** `0x1801bb3d0`
**Body size:** 366 bytes &nbsp; **Direct callees:** 7

## Signature

```c
int bambu_network_report_consent(agent_t *agent, std::string consent_json);
```

## Behavior

Submit the user's updated consent decision. The JSON blob `consent_json` carries the new consent flags; the function destroys it after copying. The library forwards the decision to the cloud and updates its local cache.

## Return values

| Return | Meaning |
|------:|---------|
| `0` | success |
| `-1` | `agent` mismatched the global singleton. |

