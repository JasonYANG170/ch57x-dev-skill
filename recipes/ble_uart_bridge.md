# 创建 BLE UART 透传桥接

> **适用摘要**: 实现 BLE 与 UART 之间的数据透传，用于无线串口通信。

## 触发意图

- "BLE 串口透传"
- "BLE UART 桥接"
- "蓝牙转串口"
- "无线串口"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/BLE_UART/` |

## 调用链

```
Step 1: 配置 config.h
Step 2: 定义透传服务 UUID 和特征值
Step 3: 初始化 UART（中断驱动）
Step 4: 实现 BLE→UART 数据转发
Step 5: 实现 UART→BLE 数据转发
Step 6: FIFO 缓冲管理
```

## 分步说明

### Step 1: 透传服务定义

```c
// 128-bit UUID（参考 BLE_UART/Profile/ble_uart_service.c）
// Base: 9FCA-DC24-0EE5-A9E0-93F3-A3B5-01xx-406E
static uint8_t serviceUUID[ATT_UUID_SIZE] = {
    0x6E, 0x40, 0x00, 0x01, 0xB5, 0xA3, 0xF3, 0x93,
    0xE0, 0xA9, 0xE5, 0x0E, 0x24, 0xDC, 0xCA, 0x9F
};
// RX 特征值（BLE→设备，写入）: ...01xx-406E → xx=02
// TX 特征值（设备→BLE，通知）: ...01xx-406E → xx=03
```

### Step 2: UART 初始化

```c
// 使用 UART3 作为透传串口
void UART_Trans_Init(uint32_t baudrate) {
    GPIOA_ModeCfg(GPIO_Pin_12, GPIO_ModeIN_PU);       // RX
    GPIOA_ModeCfg(GPIO_Pin_13, GPIO_ModeOut_PP_5mA);   // TX
    UART3_DefInit();
    UART3_BaudRateCfg(baudrate);

    // 使能接收中断
    UART3_INTCfg(ENABLE, RB_IER_RECV_RDY);
    PFIC_EnableIRQ(UART3_IRQn);
}
```

### Step 3: 双向 FIFO 缓冲

```c
#define BLE_TO_UART_FIFO_SIZE    2048
#define UART_TO_BLE_FIFO_SIZE    512

static uint8_t ble_to_uart_buf[BLE_TO_UART_FIFO_SIZE];
static uint16_t ble_uart_wr = 0, ble_uart_rd = 0;

static uint8_t uart_to_ble_buf[UART_TO_BLE_FIFO_SIZE];
static uint16_t uart_ble_wr = 0, uart_ble_rd = 0;

// FIFO 写入
static void fifo_write(uint8_t *fifo, uint16_t *wr, uint16_t *rd,
                        uint16_t size, uint8_t *data, uint16_t len) {
    for (uint16_t i = 0; i < len; i++) {
        uint16_t next = (*wr + 1) % size;
        if (next != *rd) {  // 非满
            fifo[*wr] = data[i];
            *wr = next;
        }
    }
}

// FIFO 读取
static uint16_t fifo_read(uint8_t *fifo, uint16_t *wr, uint16_t *rd,
                            uint16_t size, uint8_t *data, uint16_t maxLen) {
    uint16_t count = 0;
    while (*rd != *wr && count < maxLen) {
        data[count++] = fifo[*rd];
        *rd = (*rd + 1) % size;
    }
    return count;
}
```

### Step 4: UART 中断接收

```c
__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void UART3_IRQHandler(void) {
    uint8_t flag = UART3_GetITFlag();
    if ((flag & UART_II_MASK) == UART_II_RECV_RDY) {
        while (UART3_RecvByte(&data) == SUCCESS) {
            fifo_write(uart_to_ble_buf, &uart_ble_wr, &uart_ble_rd,
                       UART_TO_BLE_FIFO_SIZE, &data, 1);
        }
    }
}
```

### Step 5: 主循环数据转发

```c
void Main_Circulation(void) {
    while(1) {
        TMOS_SystemProcess();

        // BLE → UART 转发
        uint8_t tmp[64];
        uint16_t len = fifo_read(ble_to_uart_buf, &ble_uart_wr, &ble_uart_rd,
                                  BLE_TO_UART_FIFO_SIZE, tmp, sizeof(tmp));
        if (len > 0) {
            UART3_SendString(tmp, len);
        }

        // UART → BLE 转发
        len = fifo_read(uart_to_ble_buf, &uart_ble_wr, &uart_ble_rd,
                         UART_TO_BLE_FIFO_SIZE, tmp, mtu - 3);
        if (len > 0 && connHandle != INVALID_CONNHANDLE) {
            attHandleValueNoti_t noti;
            noti.handle = txCharHandle;
            noti.len = len;
            noti.pValue = tmp;
            GATT_Notification(connHandle, &noti, FALSE);
        }
    }
}
```

### Step 6: BLE 写入回调

```c
// 收到 BLE 数据时写入 FIFO
static uint8_t service_WriteAttrCB(uint16_t connHandle, gattAttribute_t *pAttr,
                                    uint8_t *pValue, uint16_t len, uint16_t offset) {
    if (osal_memcmp(pAttr->type.uuid, rxCharUUID, ATT_UUID_SIZE)) {
        fifo_write(ble_to_uart_buf, &ble_uart_wr, &ble_uart_rd,
                   BLE_TO_UART_FIFO_SIZE, pValue, len);
        return SUCCESS;
    }
    return ATT_ERR_ATTR_NOT_FOUND;
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 数据丢失 | FIFO 溢出 | 增大 FIFO 或提高 BLE 发送频率 |
| 乱码 | 波特率不匹配 | 确保 UART 波特率与对端一致 |
| 高延迟 | 连接间隔太大 | 设置较小连接间隔（7.5-15ms） |
| 通知失败 | MTU 太小 | 协商更大 MTU（最大 247） |

## 参考项目

- `resources/EXAM/BLE/BLE_UART/` — 完整 BLE UART 透传示例
