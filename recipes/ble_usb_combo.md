# BLE + USB 合用与 IAP 上位机

> **适用摘要**: BLE 与 USB 同时使用（蓝牙转 USB 串口），以及 IAP 上位机工具。

## 触发意图

- "蓝牙转 USB"
- "BLE USB 合用"
- "IAP 上位机"
- "WCHMcuIAP"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/BLE_USB/`, `resources/EXAM/IAP/WCHMcuIAP_WinAPP/` |

## 分步说明

### BLE_USB（蓝牙转 USB 串口）

参考 `resources/EXAM/BLE/BLE_USB/`：

```c
// USB 模拟 CDC 串口设备，转发 BLE 数据
// 数据流：手机 BLE ↔ CH57x ↔ USB CDC ↔ PC

// 初始化
void BLE_USB_Init(void) {
    // 1. BLE 从机初始化
    SetSysClock(CLK_SOURCE_PLL_48MHz);  // USB 需要 48MHz
    CH57X_BLEInit();
    HAL_Init();
    GAPRole_PeripheralInit();
    // ... GATT 服务配置 ...

    // 2. USB CDC 设备初始化
    USB_DeviceInit();
    CDC_ACM_Init();
}

// 数据转发
void BLE_USB_Forward(void) {
    // BLE → USB
    uint8_t buf[64];
    uint16_t len = BLE_RecvData(buf, sizeof(buf));
    if (len > 0) {
        CDC_ACM_Send(buf, len);
    }

    // USB → BLE
    len = CDC_ACM_Recv(buf, sizeof(buf));
    if (len > 0) {
        BLE_SendNotification(buf, len);
    }
}
```

### WCHMcuIAP 上位机工具

参考 `resources/EXAM/IAP/WCHMcuIAP_WinAPP/`：

```c
// WCHMcuIAP 是 WCH 提供的 IAP 上位机工具
// 支持通过 UART 或 USB 对 CH57x 进行固件升级

// 功能：
// 1. 连接 CH57x IAP 引导程序
// 2. 读取设备信息
// 3. 擦除 Flash
// 4. 写入固件数据
// 5. 校验写入结果
// 6. 跳转到应用

// 使用方法：
// 1. 编译 IAP 引导程序 (UART_IAP 或 USB_IAP) 并烧录
// 2. 编译 WCHMcuIAP_WinAPP 上位机工具
// 3. 连接 UART 或 USB
// 4. 选择固件文件 (.bin)
// 5. 点击升级

// IAP 通信协议：
// 命令码 | 长度 | 数据 | 校验和
// 0x84: INFO (获取设备信息)
// 0x81: ERASE (擦除 Flash)
// 0x80: PROM (写入固件)
// 0x82: VERIFY (校验数据)
// 0x83: END (完成升级)
```

## 时钟配置注意

BLE + USB 合用时，时钟需要配置为 48MHz（USB 要求）：

```c
// USB 必须使用 48MHz
SetSysClock(CLK_SOURCE_PLL_48MHz);

// BLE 在 48MHz 下也能正常工作
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| USB 无法枚举 | 时钟不是 48MHz | 使用 PLL_48MHz |
| IAP 升级失败 | 波特率不匹配 | 上位机和 IAP 使用相同波特率 |
| 数据丢失 | BLE 和 USB 速度不匹配 | 添加 FIFO 缓冲 |

## 参考项目

- `resources/EXAM/BLE/BLE_USB/` — BLE + USB 合用例程
- `resources/EXAM/IAP/WCHMcuIAP_WinAPP/` — IAP 上位机工具源码
- `resources/EXAM/IAP/UART_IAP/` — UART IAP 引导程序
- `resources/EXAM/IAP/USB_IAP/` — USB IAP 引导程序
