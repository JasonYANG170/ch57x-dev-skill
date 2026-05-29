# BLE 广播者（Broadcaster）与观察者（Observer）

> **适用摘要**: 实现纯广播或纯扫描角色，无需建立连接。

## 触发意图

- "BLE 广播"
- "Beacon 信标"
- "BLE 扫描"
- "观察者角色"
- "iBeacon"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/Broadcaster/`, `resources/EXAM/BLE/Observer/` |

## 调用链

```
Step 1: 配置 config.h（无需 Central/Peripheral 连接）
Step 2: 初始化 GAP Broadcaster 或 Observer 角色
Step 3: 配置广播数据 / 扫描参数
Step 4: 启动广播 / 扫描
```

## 分步说明

### Broadcaster（广播者）

```c
// config.h
#define PERIPHERAL_MAX_CONNECTION   0
#define CENTRAL_MAX_CONNECTION      0

// 初始化
void Broadcaster_Init(void) {
    uint8_t taskId = TMOS_ProcessEventRegister(Broadcaster_ProcessEvent);
    GAPRole_BroadcasterInit();

    // 设置广播数据（≤31 字节）
    uint8_t advertData[] = {
        0x02, GAP_ADTYPE_FLAGS, GAP_ADTYPE_BREDR_NOT_SUPPORTED,
        0x07, GAP_ADTYPE_LOCAL_NAME_COMPLETE, 'C', 'H', '5', '7', '3',
    };
    GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(advertData), advertData);
    GAPRole_SetParameter(GAPROLE_SCAN_RSP_DATA, sizeof(scanRspData), scanRspData);

    // 启动广播
    GAPRole_BroadcasterStartDevice(taskId);
}
```

### Observer（观察者）

```c
// 初始化
void Observer_Init(void) {
    uint8_t taskId = TMOS_ProcessEventRegister(Observer_ProcessEvent);
    GAPRole_ObserverInit();

    // 启动扫描
    GAPRole_ObserverStartDiscovery(DEVDISC_MODE_ALL, TRUE);
}

// 扫描结果回调
static void Observer_EventCB(gapCentralRoleEvent_t *pEvent) {
    switch (pEvent->gap.opcode) {
        case GAP_DEVICE_DISCOVERY_EVENT:
            PRINT("Found %d devices\n", pEvent->discCmpl.numDevs);
            for (uint8_t i = 0; i < pEvent->discCmpl.numDevs; i++) {
                PRINT_ADDR(pEvent->discCmpl.pDevList[i].addr);
            }
            // 重新扫描
            GAPRole_ObserverStartDiscovery(DEVDISC_MODE_ALL, TRUE);
            break;

        case GAP_ADV_DEV_DISCOVERY_EVENT:
            // 收到单个广播包
            PRINT("Device: ");
            PRINT_ADDR(pEvent->deviceDisc.addr);
            break;
    }
}
```

### iBeacon 广播示例

```c
static uint8_t iBeaconData[] = {
    0x02, 0x01, 0x06,                         // Flags
    0x1A, 0xFF,                                // Length + Manufacturer Specific
    0x4C, 0x00,                                // Apple Company ID
    0x02, 0x15,                                // iBeacon type + length
    // UUID (16 bytes)
    0xE2, 0xC5, 0x6D, 0xB5, 0xDF, 0xFB, 0x48, 0xD2,
    0xB0, 0x60, 0xD0, 0xF5, 0xA7, 0x10, 0x96, 0xE0,
    0x00, 0x01,                                // Major
    0x00, 0x02,                                // Minor
    0xC5,                                      // TX Power
};

// 设置为广播数据
GAPRole_SetParameter(GAPROLE_ADVERT_DATA, sizeof(iBeaconData), iBeaconData);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 广播无法被发现 | 广播数据超过 31 字节 | 精简广播数据 |
| 扫描不到设备 | 扫描参数错误 | 使用 DEVDISC_MODE_ALL |
| 扫描结果为空 | 设备不在广播范围 | 确认目标设备正在广播 |

## 参考项目

- `resources/EXAM/BLE/Broadcaster/` — 广播者角色完整示例
- `resources/EXAM/BLE/Observer/` — 观察者角色完整示例
