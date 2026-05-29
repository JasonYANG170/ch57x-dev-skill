# 创建 BLE 主机（Central）应用

> **适用摘要**: 创建 BLE 主机角色，实现扫描、连接、服务发现和特征值读写。

## 触发意图

- "创建 BLE 主机"
- "BLE central 应用"
- "扫描并连接 BLE 设备"
- "读写 BLE 特征值"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/Central/` |
| 知识 | BLE GAP Central 角色概念 |
| 测试 | 需要一个 BLE Peripheral 设备作为目标 |

## 调用链

```
Step 1: 配置 config.h（Central 连接数）
Step 2: 初始化 GAP Central 角色
Step 3: 实现扫描回调
Step 4: 建立连接
Step 5: 服务发现
Step 6: 特征值读写
Step 7: 接收通知
```

## 分步说明

### Step 1: config.h

```c
#define BLE_MAC                 FALSE
#define DCDC_ENABLE             FALSE
#define HAL_SLEEP               FALSE
#define CLK_OSC32K              1
#define BLE_MEMHEAP_SIZE        (1024*8)  // Central 需要更多内存
#define BLE_BUFF_MAX_LEN        27
#define BLE_BUFF_NUM            15        // 多连接需要更多 buffer
#define BLE_TX_NUM_EVENT        1
#define BLE_TX_POWER            LL_TX_POWEER_0_DBM
#define PERIPHERAL_MAX_CONNECTION   0     // 纯 Central
#define CENTRAL_MAX_CONNECTION      3     // 支持 3 个连接
```

### Step 2: 初始化 Central 角色

```c
static uint8_t centralTaskId;

void Central_Init(void) {
    centralTaskId = TMOS_ProcessEventRegister(Central_ProcessEvent);

    GAPRole_CentralInit();

    // 设置扫描参数
    GAP_SetParamValue(TGAP_GEN_DISC_SCAN, 10000);   // 扫描 10 秒
    GAP_SetParamValue(TGAP_LIM_DISC_SCAN, 10000);

    // 设置连接参数
    uint16_t intervalMin = 80;
    uint16_t intervalMax = 160;
    uint16_t latency = 0;
    uint16_t timeout = 600;
    GAP_SetParamValue(TGAP_CONN_EST_INT_MIN, intervalMin);
    GAP_SetParamValue(TGAP_CONN_EST_INT_MAX, intervalMax);
    GAP_SetParamValue(TGAP_CONN_EST_LATENCY, latency);
    GAP_SetParamValue(TGAP_CONN_EST_SUPERV_TIMEOUT, timeout);
}
```

### Step 3: 启动扫描

```c
// 目标设备地址（修改为实际地址）
static uint8_t targetAddr[B_ADDR_LEN] = {0x02, 0x02, 0x03, 0xE4, 0xC2, 0x84};

void Central_StartScan(void) {
    // 启动发现（扫描）
    GAPRole_CentralStartDiscovery(DEVDISC_MODE_ALL,    // 发现所有设备
                                   TRUE,                 // 主动扫描
                                   FALSE);               // 不使用白名单
}

// 扫描结果回调
static void centralEventCB(gapCentralRoleEvent_t *pEvent) {
    switch (pEvent->gap.opcode) {
        case GAP_DEVICE_DISCOVERY_EVENT:
            // 扫描完成，检查发现的设备列表
            PRINT("Found %d devices\n", pEvent->discCmpl.numDevs);
            // 尝试连接目标设备
            GAPRole_CentralEstablishLink(FALSE, FALSE,
                                          ADDRTYPE_PUBLIC, targetAddr);
            break;

        case GAP_LINK_ESTABLISHED_EVENT:
            if (pEvent->linkCmpl.hdr.status == SUCCESS) {
                connHandle = pEvent->linkCmpl.connectionHandle;
                PRINT("Connected to ");
                PRINT_ADDR(pEvent->linkCmpl.devAddr);
                // 开始服务发现
                GATT_DiscAllPrimaryServices(connHandle, centralTaskId);
            } else {
                PRINT("Connect failed, status=%d\n", pEvent->linkCmpl.hdr.status);
            }
            break;

        case GAP_LINK_TERMINATED_EVENT:
            connHandle = INVALID_CONNHANDLE;
            PRINT("Disconnected\n");
            break;
    }
}
```

### Step 4: 服务发现

```c
// GATT 发现回调
static void centralGATTDiscoveryCB(gattMsgEvent_t *pMsg) {
    switch (pMsg->method) {
        case ATT_FIND_BY_TYPE_VALUE_RSP:
            // 找到服务
            for (uint8_t i = 0; i < pMsg->msg.findByTypeValueRsp.numInfo; i++) {
                PRINT("Service found: handle 0x%04X-0x%04X\n",
                      pMsg->msg.findByTypeValueRsp.pHandlesInfo[i].handle,
                      pMsg->msg.findByTypeValueRsp.pHandlesInfo[i].grpEndHandle);
            }
            break;

        case ATT_READ_BY_TYPE_RSP:
            // 找到特征值
            for (uint8_t i = 0; i < pMsg->msg.readByTypeRsp.numPairs; i++) {
                uint16_t charHandle = BUILD_UINT16(
                    pMsg->msg.readByTypeRsp.pDataList[i * 21 + 18],
                    pMsg->msg.readByTypeRsp.pDataList[i * 21 + 19]);
                PRINT("Characteristic: handle 0x%04X\n", charHandle);
            }
            break;

        case ATT_ERROR_RSP:
            PRINT("GATT Error: opcode=%d, handle=0x%04X, error=%d\n",
                  pMsg->msg.errorRsp.reqOpcode,
                  pMsg->msg.errorRsp.handle,
                  pMsg->msg.errorRsp.err);
            break;
    }
}
```

### Step 5: 读写特征值

```c
// 读取特征值
void Central_ReadChar(uint16_t charHandle) {
    attReadReq_t req;
    req.handle = charHandle;
    GATT_ReadCharValue(connHandle, &req, centralTaskId);
}

// 写入特征值
void Central_WriteChar(uint16_t charHandle, uint8_t *data, uint8_t len) {
    attWriteReq_t req;
    req.handle = charHandle;
    req.len = len;
    req.pValue = data;
    req.cmd = FALSE;   // FALSE=write request (with response), TRUE=write command (no response)
    req.sig = FALSE;
    GATT_WriteCharValue(connHandle, &req, centralTaskId);
}

// 写入无响应
void Central_WriteCharNoRsp(uint16_t charHandle, uint8_t *data, uint8_t len) {
    attWriteReq_t req;
    req.handle = charHandle;
    req.len = len;
    req.pValue = data;
    req.cmd = TRUE;
    req.sig = FALSE;
    GATT_WriteNoRsp(connHandle, &req);
}
```

### Step 6: 接收通知

```c
// 启用通知（写入 CCCD）
void Central_EnableNotification(uint16_t charHandle) {
    uint8_t value[2] = {LO_UINT16(GATT_CLIENT_CFG_NOTIFY),
                         HI_UINT16(GATT_CLIENT_CFG_NOTIFY)};
    // CCCD handle 通常是 charHandle + 1
    Central_WriteChar(charHandle + 1, value, 2);
}

// 处理收到的通知
static void centralProcessGATTMsg(gattMsgEvent_t *pMsg) {
    if (pMsg->method == ATT_HANDLE_VALUE_NOTI) {
        PRINT("Notification from handle 0x%04X: ", pMsg->msg.handleValueNoti.handle);
        for (uint8_t i = 0; i < pMsg->msg.handleValueNoti.len; i++) {
            PRINT("%02X ", pMsg->msg.handleValueNoti.pValue[i]);
        }
        PRINT("\n");
    }
    GATT_bm_free(&pMsg->msg, pMsg->method);
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 扫描不到设备 | 目标设备未广播 | 确认目标设备正在 advertising |
| 连接失败 | 地址类型不匹配 | 尝试 `ADDRTYPE_PUBLIC` 或 `ADDRTYPE_RANDOM` |
| 服务发现失败 | 连接未建立就开始发现 | 等待 `GAP_LINK_ESTABLISHED_EVENT` |
| 读写超时 | 特征值 handle 错误 | 先执行服务发现获取正确 handle |

## 参考项目

- `resources/EXAM/BLE/Central/` — 完整主机示例
- `resources/EXAM/BLE/MultiCentral/` — 多连接主机示例
- `resources/ble_api.md` — BLE API 速查
