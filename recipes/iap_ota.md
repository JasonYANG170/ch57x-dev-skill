# IAP 引导程序与 BLE OTA 升级

> **适用摘要**: 实现 IAP (In-Application Programming) 引导程序和 BLE 空中固件升级。

## 触发意图

- "IAP 引导程序"
- "OTA 固件升级"
- "远程更新固件"
- "双镜像升级"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/IAP/`, `resources/EXAM/BLE/BackupUpgrade_OTA/` |
| 知识 | Flash 地址布局、引导加载程序概念 |

## 分步说明

### 内存布局

```
0x00000000 - 0x00000FFF  Bootloader (4KB)
0x00001000 - 0x00038FFF  Image A (216KB) — 运行中的应用
0x00039000 - 0x0006AFFF  Image B (216KB) — OTA 备份
0x0006D000 - 0x0006FFFF  IAP Image (12KB) — 跳转用
0x00077E00 - 0x00077FFF  DataFlash — 镜像标志位
```

### 镜像标志位

```c
#define FLAG_IMAGE_A    0x01
#define FLAG_IMAGE_B    0x02
#define FLAG_IAP        0x03
#define FLAG_ADDR       0x77E00

uint8_t ReadImageFlag(void) {
    uint8_t flag;
    EEPROM_READ(FLAG_ADDR, &flag, 1);
    return flag;
}

void WriteImageFlag(uint8_t flag) {
    EEPROM_ERASE(FLAG_ADDR, 256);
    EEPROM_WRITE(FLAG_ADDR, &flag, 1);
}
```

### IAP 引导程序（UART）

参考 `resources/EXAM/IAP/UART_IAP/`：

```c
// IAP 链接脚本
// FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 4K

// IAP 通信协议
// 命令: INFO(0x84), ERASE(0x81), PROM(0x80), VERIFY(0x82), END(0x83)
// 帧格式: CMD | LEN | ADDR/DATA | CHECKSUM

void IAP_ProcessCommand(uint8_t cmd, uint8_t *data, uint16_t len) {
    switch (cmd) {
        case 0x84:  // INFO — 返回设备信息
            SendDeviceInfo();
            break;
        case 0x81:  // ERASE — 擦除 Flash
            uint32_t addr = BUILD_UINT32(data[0], data[1], data[2], data[3]);
            uint16_t blocks = BUILD_UINT16(data[4], data[5]);
            FLASH_ROM_ERASE(addr, blocks * 256);
            break;
        case 0x80:  // PROM — 写入数据
            uint32_t addr = BUILD_UINT32(data[0], data[1], data[2], data[3]);
            FLASH_ROM_WRITE(addr, &data[4], len - 4);
            break;
        case 0x82:  // VERIFY — 校验数据
            uint32_t addr = BUILD_UINT32(data[0], data[1], data[2], data[3]);
            FLASH_ROM_VERIFY(addr, &data[4], len - 4);
            break;
        case 0x83:  // END — 完成，跳转到应用
            WriteImageFlag(FLAG_IMAGE_A);
            NVIC_SystemReset();
            break;
    }
}
```

### IAP 跳转到应用

```c
// 在 IAP 中判断是否需要跳转
void JumpToApp(void) {
    uint8_t flag = ReadImageFlag();

    uint32_t app_addr;
    switch (flag) {
        case FLAG_IMAGE_A: app_addr = 0x00001000; break;
        case FLAG_IMAGE_B: app_addr = 0x00039000; break;
        default: app_addr = 0x00001000; break;
    }

    // 检查应用是否有效（栈指针在 RAM 范围内）
    uint32_t sp = *((uint32_t *)app_addr);
    if (sp >= 0x20003800 && sp <= 0x20008000) {
        // 设置栈指针
        __set_MSP(sp);
        // 跳转到复位向量
        void (*app_entry)(void) = (void (*)(void))(*((uint32_t *)(app_addr + 4)));
        app_entry();
    }
    // 无效应用，留在 IAP
}
```

### BLE OTA 升级流程

参考 `resources/EXAM/BLE/BackupUpgrade_OTA/`：

```c
// OTA Profile UUID
// 服务: 9ECA-DC24-0EE5-A9E0-93F3-A3B5-0200-406E
// 写入特征: ...0201-406E (OTA 数据写入)
// 通知特征: ...0202-406E (OTA 状态通知)

// OTA 命令处理
void OTA_ProcessCommand(uint8_t cmd, uint8_t *data, uint16_t len) {
    switch (cmd) {
        case OTA_CMD_INFO:    // 0x84 — 返回设备信息
            SendOTAInfo();
            break;

        case OTA_CMD_ERASE:   // 0x81 — 擦除目标镜像
            uint32_t addr = GetTargetImageAddr();
            FLASH_ROM_ERASE(addr, target_size);
            break;

        case OTA_CMD_PROGRAM: // 0x80 — 写入固件数据
            uint32_t addr = current_write_addr;
            FLASH_ROM_WRITE(addr, data, 256);
            current_write_addr += 256;
            break;

        case OTA_CMD_VERIFY:  // 0x82 — 校验
            VerifyFirmware();
            break;

        case OTA_CMD_END:     // 0x83 — 完成，切换镜像
            SwitchImage();
            NVIC_SystemReset();
            break;
    }
}

// 切换活动镜像
void SwitchImage(void) {
    uint8_t current = ReadImageFlag();
    uint8_t next = (current == FLAG_IMAGE_A) ? FLAG_IMAGE_B : FLAG_IMAGE_A;
    WriteImageFlag(next);
}
```

### 双镜像 IAP 应用链接脚本

```
// Image A 应用
MEMORY {
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 216K
    RAM (xrw) : ORIGIN = 0x20003800, LENGTH = 18K
}

// Image B 应用
MEMORY {
    FLASH (rx) : ORIGIN = 0x00039000, LENGTH = 216K
    RAM (xrw) : ORIGIN = 0x20003800, LENGTH = 18K
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| OTA 后无法启动 | 镜像标志位未更新 | 确保 WriteImageFlag() 成功 |
| 写入数据错乱 | 地址未对齐 | OTA 数据按 256 字节对齐 |
| IAP 无法跳转 | 应用起始地址错误 | 检查链接脚本 ORIGIN 值 |
| 擦除超时 | Flash 擦除时间长 | 擦除大区域需等待 |

## 参考项目

- `resources/EXAM/IAP/UART_IAP/` — UART IAP 引导程序
- `resources/EXAM/IAP/USB_IAP/` — USB IAP 引导程序
- `resources/EXAM/BLE/BackupUpgrade_OTA/` — BLE OTA 双镜像升级
- `resources/EXAM/BLE/OnlyUpdateApp_Peripheral/` — 固定地址 OTA
