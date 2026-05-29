# BLE 主从一体与多连接

> **适用摘要**: 实现同时运行 Peripheral + Central 角色，或多连接 Central/MultiPeripheral。

## 触发意图

- "主从一体"
- "同时做主机和从机"
- "多连接 BLE"
- "连接多个设备"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/CentPeri/`, `MultiCentPeri/`, `MultiCentral/` |

## 调用链

```
Step 1: 配置 config.h（增大内存和 buffer）
Step 2: 同时初始化 Peripheral 和 Central 角色
Step 3: 分别处理各角色的事件回调
Step 4: 管理多个连接句柄
```

## 分步说明

### 主从一体（CentPeri）

参考 `resources/EXAM/BLE/CentPeri/`：

```c
// config.h
#define BLE_MEMHEAP_SIZE        (1024*8)   // 需要更多内存
#define BLE_BUFF_NUM            15
#define PERIPHERAL_MAX_CONNECTION   1
#define CENTRAL_MAX_CONNECTION      1

// 同时初始化两个角色
void CentPeri_Init(void) {
    peripheralTaskId = TMOS_ProcessEventRegister(Peripheral_ProcessEvent);
    centralTaskId = TMOS_ProcessEventRegister(Central_ProcessEvent);

    // 初始化 Peripheral
    GAPRole_PeripheralInit();
    // ... 配置广播参数、GATT 服务 ...

    // 初始化 Central
    GAPRole_CentralInit();
    // ... 配置扫描参数 ...

    // 同时启动
    GAPRole_PeripheralStartDevice(peripheralTaskId, ...);
    // Central 在需要时启动扫描
}
```

### 多主机连接（MultiCentral）

参考 `resources/EXAM/BLE/MultiCentral/`：

```c
// config.h
#define BLE_MEMHEAP_SIZE        (1024*10)
#define BLE_BUFF_NUM            20
#define PERIPHERAL_MAX_CONNECTION   0
#define CENTRAL_MAX_CONNECTION      3   // 支持 3 个从机连接

// 多目标地址
static uint8_t targetAddr1[B_ADDR_LEN] = {0x02, 0x02, 0x03, 0xE4, 0xC2, 0x84};
static uint8_t targetAddr2[B_ADDR_LEN] = {0x03, 0x02, 0x03, 0xE4, 0xC2, 0x84};
static uint8_t targetAddr3[B_ADDR_LEN] = {0x04, 0x02, 0x03, 0xE4, 0xC2, 0x84};

// 连接管理
static uint16_t connHandles[3] = {INVALID_CONNHANDLE, INVALID_CONNHANDLE, INVALID_CONNHANDLE};
static uint8_t connCount = 0;

// 依次连接多个设备
void MultiCentral_ConnectNext(void) {
    if (connCount < 3) {
        uint8_t *addr = (connCount == 0) ? targetAddr1 :
                        (connCount == 1) ? targetAddr2 : targetAddr3;
        GAPRole_CentralEstablishLink(FALSE, FALSE, ADDRTYPE_PUBLIC, addr);
    }
}
```

### 多主多从（MultiCentPeri）

参考 `resources/EXAM/BLE/MultiCentPeri/`：

```c
// config.h
#define BLE_MEMHEAP_SIZE        (1024*12)
#define BLE_BUFF_NUM            25
#define PERIPHERAL_MAX_CONNECTION   3   // 3 个主机可连入
#define CENTRAL_MAX_CONNECTION      3   // 连接 3 个从机

// 连接句柄管理
typedef struct {
    uint16_t handle;
    uint8_t role;     // 0=peripheral, 1=central
    uint8_t addr[B_ADDR_LEN];
} conn_info_t;

static conn_info_t connections[6];  // 最多 6 个连接

// 事件处理需要区分角色
void ProcessEvent(uint8_t task_id, uint16_t events) {
    if (task_id == peripheralTaskId) {
        // 处理 Peripheral 事件
    } else if (task_id == centralTaskId) {
        // 处理 Central 事件
    }
}
```

## 连接句柄管理

```c
// 存储连接句柄
void StoreConnHandle(uint16_t handle, uint8_t role, uint8_t *addr) {
    for (int i = 0; i < 6; i++) {
        if (connections[i].handle == INVALID_CONNHANDLE) {
            connections[i].handle = handle;
            connections[i].role = role;
            memcpy(connections[i].addr, addr, B_ADDR_LEN);
            break;
        }
    }
}

// 移除连接句柄
void RemoveConnHandle(uint16_t handle) {
    for (int i = 0; i < 6; i++) {
        if (connections[i].handle == handle) {
            connections[i].handle = INVALID_CONNHANDLE;
            break;
        }
    }
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 内存不足 | BLE_MEMHEAP_SIZE 太小 | 增大到 10KB+ |
| Buffer 耗尽 | 连接数多但 buffer 少 | 增加 BLE_BUFF_NUM |
| 事件混乱 | 未区分 task_id | 在回调中检查 task_id |
| 连接失败 | 超出最大连接数 | 检查 ConnectNumber 编码 |

## 参考项目

- `resources/EXAM/BLE/CentPeri/` — 主从一体
- `resources/EXAM/BLE/MultiCentPeri/` — 多主多从（3+3）
- `resources/EXAM/BLE/MultiCentral/` — 多主机（3 连接）
