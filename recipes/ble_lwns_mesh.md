# BLE 轻量组网（LWNS）与 IoT 应用

> **适用摘要**: 使用 LWNS 轻量网络栈实现广播、单播、网状组网，以及 BLE+IoT 应用。

## 触发意图

- "LWNS 组网"
- "无线组网"
- "BLE IoT"
- "IoCHub"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/LWNS/`, `resources/EXAM/BLE/IoCHub_NET/` |

## 分步说明

### LWNS 功能概述

参考 `resources/EXAM/BLE/LWNS/`：

LWNS（Lightweight Network Stack）是 WCH 提供的轻量级无线组网协议，支持：
- **broadcast** — 广播通信
- **unicast** — 单播通信
- **netflood** — 网络泛洪
- **mesh** — 网状网络
- **multicast** — 组播通信

```c
// LWNS 初始化
#include "lwns_config.h"

void LWNS_Init(void) {
    // LWNS 在 BLE 初始化后自动配置
    // 无需手动调用初始化函数
}

// 广播发送
void LWNS_BroadcastSend(uint8_t *data, uint16_t len) {
    lwns_broadcast_send(&broadcast_conn, data, len);
}

// 单播发送
void LWNS_UnicastSend(uint8_t *data, uint16_t len, lwns_addr_t *dest) {
    lwns_unicast_send(&unicast_conn, data, len, dest);
}

// 网状网络
void LWNS_MeshSend(uint8_t *data, uint16_t len, lwns_addr_t *dest) {
    lwns_mesh_send(&mesh_conn, data, len, dest);
}
```

### LWNS 示例功能

| 示例 | 功能 |
|------|------|
| `lwns_broadcast_example.c` | 广播通信示例 |
| `lwns_unicast_example.c` | 单播通信示例 |
| `lwns_netflood_example.c` | 网络泛洪示例 |
| `lwns_mesh_example.c` | 网状网络示例 |
| `lwns_multicast_example.c` | 组播通信示例 |
| `lwns_multinetflood_example.c` | 多跳泛洪示例 |

### IoCHub_Net（BLE + IoT 灯控）

参考 `resources/EXAM/BLE/IoCHub_NET/`：

```c
// BLE + IoCHub 简单灯控开关例程
// 包含 Android 应用 (IoCHubNetApp_Android)

// 功能：
// 1. BLE 从机接收手机命令
// 2. 通过 IoCHub 协议转发到 IoT 网络
// 3. 控制灯的开关状态

void IoCHub_Init(void) {
    // BLE 从机初始化
    GAPRole_PeripheralInit();
    // ... GATT 服务配置 ...
}

// 收到 BLE 命令后处理
void IoCHub_ProcessCommand(uint8_t cmd) {
    switch (cmd) {
        case CMD_LIGHT_ON:
            // 控制灯开
            break;
        case CMD_LIGHT_OFF:
            // 控制灯关
            break;
    }
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 组网不通 | 节点地址冲突 | 确保每个节点有唯一地址 |
| 广播收不到 | 距离太远或功率不足 | 增加 TX 功率或缩短距离 |
| MESH 不稳定 | 跳数太多 | 减少中继跳数 |

## 参考项目

- `resources/EXAM/BLE/LWNS/` — 轻量组网协议示例
- `resources/EXAM/BLE/IoCHub_NET/` — BLE + IoT 灯控应用
