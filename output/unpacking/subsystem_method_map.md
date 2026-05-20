# Subsystem method map

Each row maps a subsystem method (an address in `.text`) to
the log strings it emits. Two methods that emit the same set
of strings are very likely the same one. This lets us pin
down the entry points of `DeviceSubscribeManager` and
`MqttConnectionManager` even when the C++ vtables aren't
statically resolvable.

## `AccountManager::` (23 distinct methods, 43 total log-string xrefs)

| Method VA | # logs | Sample log strings |
|---|---:|---|
| `0x180001000` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x1800082c0` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x18000ee80` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x180015ad0` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x18001c6b0` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x1800234d0` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x18002a310` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x180031190` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x180038170` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x18003ed90` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x180045a80` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x18004c800` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x180053470` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x18005a060` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x180060ce0` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x180067900` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x18006e580` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x1800751a0` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x18007c120` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x180082d80` | 2 | `AccountManager::load_from_json() failed! exception={}` • `AccountManager::request_project_id, body={}` |
| `0x18012c4e0` | 1 | `AccountManager::request_project_id, body={}` •  |
| `0x180143d40` | 1 | `AccountManager::request_project_id, body={}` •  |
| `0x180591de0` | 1 | `AccountManager::load_from_json() failed! exception={}` •  |

## `DeviceSubscribeManager update:` (20 distinct methods, 20 total log-string xrefs)

| Method VA | # logs | Sample log strings |
|---|---:|---|
| `0x180001000` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x1800082c0` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x18000ee80` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x180015ad0` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x18001c6b0` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x1800234d0` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x18002a310` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x180031190` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x180038170` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x18003ed90` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x180045a80` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x18004c800` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x180053470` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x18005a060` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x180060ce0` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x180067900` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x18006e580` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x1800751a0` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x18007c120` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |
| `0x180082d80` | 1 | `DeviceSubscribeManager update: dev_id = {}, dev_ip = {}, access_code = {}` •  |

## `DeviceSubscribeManager:` (29 distinct methods, 421 total log-string xrefs)

| Method VA | # logs | Sample log strings |
|---|---:|---|
| `0x180001000` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x1800082c0` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x18000ee80` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x180015ad0` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x18001c6b0` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x1800234d0` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x18002a310` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x180031190` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x180038170` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x18003ed90` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x180045a80` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x18004c800` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x180053470` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x18005a060` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x180060ce0` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x180067900` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x18006e580` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x1800751a0` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x18007c120` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x180082d80` | 20 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x180200f00` | 2 | `DeviceSubscribeManager: auto create local channel succeeded, check cloud channel later.…` • `DeviceSubscribeManager: auto create local channel failed, won't retry. dev_id = {}` |
| `0x180203050` | 3 | `DeviceSubscribeManager: local or cloud channel missing, stop auto switch. dev_id = {}` • `DeviceSubscribeManager: status of local channel doesn't support closing cloud channel, …` • … |
| `0x180203c50` | 3 | `DeviceSubscribeManager: cloud channel status doesn't support switching to local(uptime …` • `DeviceSubscribeManager: there is an existing local channel. dev_id = {}` • … |
| `0x180204b20` | 1 | `DeviceSubscribeManager: local channel doesn't provide data for a long time, switch to c…` •  |
| `0x180205240` | 3 | `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • `DeviceSubscribeManager: request subscribe local device. dev_id = {}` • … |
| `0x180208ff0` | 1 | `DeviceSubscribeManager: can't create channel by {}` •  |
| `0x1802096c0` | 4 | `DeviceSubscribeManager: request subscribe cloud device. dev_id = {}` • `DeviceSubscribeManager: device {} is subscribing by channel {}, do nothing` • … |
| `0x18020b6b0` | 3 | `DeviceSubscribeManager: request unSubscribe device. dev_id = {}` • `DeviceSubscribeManager: stop a pending channel. channel = {}` • … |
| `0x1805b98b0` | 1 | `DeviceSubscribeManager::InternalHandleMessage parse json failed` •  |

## `FTJob failed` (21 distinct methods, 21 total log-string xrefs)

| Method VA | # logs | Sample log strings |
|---|---:|---|
| `0x180001000` | 1 | `FTJob failed, error = {}` •  |
| `0x1800082c0` | 1 | `FTJob failed, error = {}` •  |
| `0x18000ee80` | 1 | `FTJob failed, error = {}` •  |
| `0x180015ad0` | 1 | `FTJob failed, error = {}` •  |
| `0x18001c6b0` | 1 | `FTJob failed, error = {}` •  |
| `0x1800234d0` | 1 | `FTJob failed, error = {}` •  |
| `0x18002a310` | 1 | `FTJob failed, error = {}` •  |
| `0x180031190` | 1 | `FTJob failed, error = {}` •  |
| `0x180038170` | 1 | `FTJob failed, error = {}` •  |
| `0x18003ed90` | 1 | `FTJob failed, error = {}` •  |
| `0x180045a80` | 1 | `FTJob failed, error = {}` •  |
| `0x18004c800` | 1 | `FTJob failed, error = {}` •  |
| `0x180053470` | 1 | `FTJob failed, error = {}` •  |
| `0x18005a060` | 1 | `FTJob failed, error = {}` •  |
| `0x180060ce0` | 1 | `FTJob failed, error = {}` •  |
| `0x180067900` | 1 | `FTJob failed, error = {}` •  |
| `0x18006e580` | 1 | `FTJob failed, error = {}` •  |
| `0x1800751a0` | 1 | `FTJob failed, error = {}` •  |
| `0x18007c120` | 1 | `FTJob failed, error = {}` •  |
| `0x180082d80` | 1 | `FTJob failed, error = {}` •  |
| `0x18023c010` | 1 | `FTJob failed, error = {}` •  |

## `GenericSubscriptionManager:` (24 distinct methods, 126 total log-string xrefs)

| Method VA | # logs | Sample log strings |
|---|---:|---|
| `0x180001000` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x1800082c0` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x18000ee80` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x180015ad0` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x18001c6b0` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x1800234d0` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x18002a310` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x180031190` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x180038170` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x18003ed90` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x180045a80` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x18004c800` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x180053470` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x18005a060` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x180060ce0` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x180067900` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x18006e580` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x1800751a0` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x18007c120` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x180082d80` | 6 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x180207230` | 1 | `GenericSubscriptionManager: client closed, channel dead, topic = {}` •  |
| `0x18020a420` | 3 | `GenericSubscriptionManager: request subscribe topic {}` • `GenericSubscriptionManager: ISubscribeChannel.Subscribe return false, can't subscribe {}` • … |
| `0x18020b3c0` | 1 | `GenericSubscriptionManager: request unsubscribe all topics` •  |
| `0x18020be70` | 1 | `GenericSubscriptionManager: request unsubscribe topic {}` •  |

## `MqttClientKeeper:` (23 distinct methods, 84 total log-string xrefs)

| Method VA | # logs | Sample log strings |
|---|---:|---|
| `0x180001000` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x1800082c0` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x18000ee80` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x180015ad0` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x18001c6b0` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x1800234d0` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x18002a310` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x180031190` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x180038170` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x18003ed90` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x180045a80` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x18004c800` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x180053470` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x18005a060` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x180060ce0` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x180067900` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x18006e580` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x1800751a0` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x18007c120` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x180082d80` | 4 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: reconnect exception = {}` • … |
| `0x1801f5e80` | 2 | `MqttClientKeeper: reconnect a manager = {}` • `MqttClientKeeper: thread exiting` |
| `0x1805b4910` | 1 | `MqttClientKeeper: reconnect exception = {}` •  |
| `0x1805b4a90` | 1 | `MqttClientKeeper: reconnect exception` •  |

## `MqttConnectionManager create!` (21 distinct methods, 21 total log-string xrefs)

| Method VA | # logs | Sample log strings |
|---|---:|---|
| `0x180001000` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x1800082c0` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x18000ee80` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x180015ad0` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x18001c6b0` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x1800234d0` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x18002a310` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x180031190` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x180038170` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x18003ed90` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x180045a80` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x18004c800` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x180053470` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x18005a060` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x180060ce0` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x180067900` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x18006e580` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x1800751a0` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x18007c120` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x180082d80` | 1 | `MqttConnectionManager create! self = {}` •  |
| `0x1801eead0` | 1 | `MqttConnectionManager create! self = {}` •  |

## `MqttConnectionManager destroy!` (21 distinct methods, 21 total log-string xrefs)

| Method VA | # logs | Sample log strings |
|---|---:|---|
| `0x180001000` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x1800082c0` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x18000ee80` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x180015ad0` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x18001c6b0` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x1800234d0` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x18002a310` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x180031190` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x180038170` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x18003ed90` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x180045a80` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x18004c800` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x180053470` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x18005a060` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x180060ce0` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x180067900` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x18006e580` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x1800751a0` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x18007c120` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x180082d80` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |
| `0x1801f0170` | 1 | `MqttConnectionManager destroy! self = {}, alive channels count = {}` •  |

## `MqttConnectionManager:` (43 distinct methods, 486 total log-string xrefs)

| Method VA | # logs | Sample log strings |
|---|---:|---|
| `0x180001000` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x1800082c0` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x18000ee80` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x180015ad0` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x18001c6b0` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x1800234d0` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x18002a310` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x180031190` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x180038170` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x18003ed90` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x180045a80` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x18004c800` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x180053470` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x18005a060` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x180060ce0` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x180067900` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x18006e580` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x1800751a0` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x18007c120` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x180082d80` | 23 | `MqttConnectionManager: registerChannel channel = {}` • `MqttConnectionManager::RegisterChannel, exception={}` • … |
| `0x1801ed890` | 1 | `MqttConnectionManager::OnAutoReconnection!, self = {}` •  |
| `0x1801f3710` | 1 | `MqttConnectionManager::OnAutoReconnection!, self = {}` •  |
| `0x1801f3810` | 2 | `MqttConnectionManager::OnConnected! self = {}` • `MqttConnectionManager: start to resubscribe, channel = {}` |
| `0x1801f3ad0` | 1 | `MqttConnectionManager::OnConnectionLost!, self = {}, cause = {}` •  |
| `0x1801f3f30` | 1 | `MqttConnectionManager::OnMessageDeliveried!, self = {}` •  |
| `0x1801f4070` | 1 | `MqttConnectionManager::PerformSubscribe client disconnect, self = {}` •  |
| `0x1801f5060` | 1 | `MqttConnectionManager: registerChannel channel = {}` •  |
| `0x1801f6580` | 1 | `MqttConnectionManager: unregisterChannel channel = {}` •  |
| `0x1801f6cc0` | 3 | `MqttConnectionManager: Connection(mqtt) OK! self = {}, cli id = {}` • `MqttConnectionManager: Connection(mqtt) failed! self = {}, return code = {}, reason cod…` • … |
| `0x1801f8480` | 1 | `MqttConnectionManager::OnConnectionLost!, self = {}, cause = {}` •  |
| `0x1801f8880` | 1 | `MqttConnectionManager::OnMessageDeliveried!, self = {}` •  |
| `0x1805b0380` | 1 | `MqttConnectionManager: client disconnect exception, self = {}, exception = {}` •  |
| `0x1805b0520` | 1 | `MqttConnectionManager: client disconnect exception, self = {}, unknown exception` •  |
| `0x1805b3710` | 1 | `MqttConnectionManager: connect exception, self = {}, exception = {}` •  |
| `0x1805b38b0` | 1 | `MqttConnectionManager: connect exception, self = {}, unknown exception` •  |
| `0x1805b3bb0` | 1 | `MqttConnectionManager::PerformSubscribe, self = {}, exception = {}` •  |
| `0x1805b3dd0` | 1 | `MqttConnectionManager::PerformSubscribe, self = {}, exception` •  |
| `0x1805b3fe0` | 1 | `MqttConnectionManager::PerformUnsubscribe, self = {}, exception = {}` •  |
| `0x1805b4150` | 1 | `MqttConnectionManager::PerformUnsubscribe, self = {}, exception` •  |
| `0x1805b4300` | 1 | `MqttConnectionManager::Publish, self = {}, exception = {}` •  |
| `0x1805b4490` | 1 | `MqttConnectionManager::Publish, self = {}, exception` •  |
| `0x1805b46d0` | 1 | `MqttConnectionManager::RegisterChannel, exception={}` •  |
| `0x1805b4be0` | 1 | `MqttConnectionManager::UnregisterChannel, exception = {}` •  |

## `MqttLocalSubscribeChannel` (23 distinct methods, 125 total log-string xrefs)

| Method VA | # logs | Sample log strings |
|---|---:|---|
| `0x180001000` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x1800082c0` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x18000ee80` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x180015ad0` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x18001c6b0` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x1800234d0` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x18002a310` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x180031190` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x180038170` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x18003ed90` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x180045a80` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x18004c800` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x180053470` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x18005a060` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x180060ce0` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x180067900` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x18006e580` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x1800751a0` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x18007c120` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x180082d80` | 6 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` • … |
| `0x1801f0de0` | 2 | `MqttLocalSubscribeChannel: local mqtt client connected, start subscribe. channel = {}` • `MqttLocalSubscribeChannel: local mqtt client connection failed. channel = {}` |
| `0x1801f1950` | 2 | `MqttLocalSubscribeChannel: local mqtt channel RegisterChannel failed. channel = {}` • `MqttLocalSubscribeChannel: local mqtt channel GetDriver return nullptr. channel = {}` |
| `0x1801f5a00` | 1 | `MqttLocalSubscribeChannel: local mqtt client start connecting. channel = {}` •  |

## `MqttSubscribeChannel` (27 distinct methods, 189 total log-string xrefs)

| Method VA | # logs | Sample log strings |
|---|---:|---|
| `0x180001000` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x1800082c0` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x18000ee80` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x180015ad0` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x18001c6b0` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x1800234d0` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x18002a310` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x180031190` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x180038170` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x18003ed90` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x180045a80` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x18004c800` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x180053470` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x18005a060` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x180060ce0` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x180067900` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x18006e580` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x1800751a0` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x18007c120` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x180082d80` | 9 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` • … |
| `0x1801f0710` | 2 | `MqttSubscribeChannel: local channel closed, topic = {}` • `MqttSubscribeChannel: cloud channel closed, topic = {}` |
| `0x1801f20c0` | 2 | `MqttSubscribeChannel: subscribe failed! channel = {}, extra_info = {}, retrying, retry …` • `MqttSubscribeChannel: subscribe failed! channel = {}, extra_info = {}, stop retrying, r…` |
| `0x1801f4b20` | 1 | `MqttSubscribeChannel: GetDriver not ready, cant subscribe. channel = {}` •  |
| `0x1801f69f0` | 1 | `MqttSubscribeChannel: called unsubscribe. channel = {}` •  |
| `0x1801f8ff0` | 1 | `MqttSubscribeChannel: subscribe channel failure: channel = {}` •  |
| `0x1801f94c0` | 1 | `MqttSubscribeChannel: subscribe channel success: topic = {}` •  |
| `0x1805b3530` | 1 | `MqttSubscribeChannel::HandleMessage parse json failed` •  |

