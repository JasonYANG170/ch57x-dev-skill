---
name: ch57x-dev-skill
description: >-
  AI Skill for CH57x BLE microcontroller firmware development. Used when users need to
  create, modify, or debug CH57x embedded projects, including BLE peripheral/central/mesh
  applications, peripheral driver configuration, IAP/OTA updates, and power management.
  Supports CH573, CH571 and other chips in the CH57x family.
  Trigger words: "CH573", "CH571", "CH57x", "WCH", "沁恒", "BLE", "蓝牙", "RISC-V", "MounRiver", "OTA", "IAP"
tags:
  - embedded
  - BLE
  - bluetooth
  - RISC-V
  - WCH
  - CH57x
  - CH573
  - CH571
  - microcontroller
  - firmware
license: MIT
compatibility: Build requires MounRiver Studio IDE; target is CH57x RISC-V BLE SoC family
metadata:
  author: Community
  version: "1.2.0"
---

# ch57x-dev-skill

AI Skill for CH57x BLE microcontroller firmware development. Provides scenario-driven recipes, complete API reference, configuration guides, and common pitfalls. Supports the CH57x chip family including CH573, CH571.

## Core Principles

1. **Never guess APIs** — Check `resources/` first; no matching doc = does not exist
2. **Verify header inclusion** — Peripheral drivers need `CH57x_*.h`; BLE needs `CH57xBLE_LIB.h` + `HAL.h`
3. **BLE initialization order matters** — Always: SetSysClock → CH57X_BLEInit → HAL_Init → GAPRole_Init → App_Init → Main_Circulation
4. **config.h controls BLE stack** — Memory, buffers, TX power, connection limits, sleep behavior all configured here
5. **Interrupt handlers must use `.highcode` section** — Place in RAM for fast execution: `__attribute__((section(".highcode")))`
6. **Flash writes need safe access** — `FLASH_ROM_WRITE`/`FLASH_ROM_ERASE` handle this internally; EEPROM ops do too
7. **IAP applications start at 0x1000** — Bootloader occupies 0x0000-0x0FFF; application linker script must offset
8. **TMOS is the BLE event loop** — All BLE processing happens in `TMOS_SystemProcess()` inside `Main_Circulation()`

## When to Use

**Applicable:**
- Creating new CH57x firmware projects (BLE or non-BLE)
- Implementing BLE peripheral, central, broadcaster, observer, or mesh roles
- Configuring peripheral drivers (GPIO, UART, SPI, ADC, Timer, PWM, USB)
- Implementing IAP bootloader or BLE OTA firmware updates
- Power management and sleep mode configuration
- Flash/EEPROM data storage

**Not applicable:**
- General C programming questions unrelated to CH57x/WCH chips
- Other WCH chip families (CH569, CH582, CH32V, etc.) — different register maps
- PCB design or hardware schematics

---

## Scenario Quick Reference (Recipes)

When user intent matches a scenario below, **read the corresponding recipe first** — it contains the complete call chain, step-by-step instructions, common errors, and code examples.

### Project Setup

| recipe | scenario |
|---|---|
| `recipes/new_project.md` | Create a new CH57x project from scratch with proper project structure |

### BLE Applications

| recipe | scenario |
|---|---|
| `recipes/ble_peripheral.md` | BLE peripheral (slave) — advertising, GATT services, notifications |
| `recipes/ble_central.md` | BLE central (master) — scanning, connecting, service discovery, R/W |
| `recipes/ble_broadcaster_observer.md` | Broadcaster / Observer — beacon, scanning without connection |
| `recipes/ble_hid.md` | BLE HID device — keyboard, mouse, consumer control, touch |
| `recipes/ble_profiles.md` | Standard profiles — HeartRate, CyclingSensor, RunningSensor, SpeedTest |
| `recipes/ble_multi_role.md` | Multi-role / multi-connection — CentPeri, MultiCentral, MultiCentPeri |
| `recipes/ble_mesh.md` | BLE Mesh — Alibaba light model, provisioning, vendor model |
| `recipes/ble_uart_bridge.md` | BLE UART transparent bridge — bidirectional BLE-UART data |
| `recipes/ble_rf_dtm.md` | RF test — DTM, non-standard RF PHY, frequency hopping |
| `recipes/ble_lwns_mesh.md` | LWNS networking — broadcast, unicast, mesh, IoCHub |
| `recipes/ble_usb_combo.md` | BLE + USB combo — BLE-to-USB serial bridge, WCHMcuIAP tool |

### Peripheral Drivers

| recipe | scenario |
|---|---|
| `recipes/uart_comm.md` | UART communication — init, send, receive, interrupt-driven |
| `recipes/gpio_control.md` | GPIO — input, output, interrupt, pull-up/down |
| `recipes/adc_reading.md` | ADC — external channel, temperature, battery, touch key |
| `recipes/timer_pwm.md` | Timer and PWM — periodic interrupt, PWM output, capture |
| `recipes/spi_comm.md` | SPI — master/slave, FIFO, DMA transfer |
| `recipes/flash_storage.md` | Flash/EEPROM — read, write, erase, data storage |

### System Features

| recipe | scenario |
|---|---|
| `recipes/power_management.md` | Sleep modes — Idle, Halt, Sleep, Shutdown, GPIO/RTC wakeup |
| `recipes/iap_ota.md` | IAP bootloader and BLE OTA — dual-image update system |
| `recipes/usb_device.md` | USB device — CDC serial, HID, vendor-defined |
| `recipes/usb_host.md` | USB host — device enumeration, AOA, U-disk filesystem |

---

## Peripheral Pin Mapping

CH57x series has GPIOA (24 pins) and GPIOB (24 pins). Common peripheral pin assignments:

| Peripheral | Pin | Function |
|---|---|---|
| UART0 | PA4 (TX), PA5 (RX) | Default UART0 pins |
| UART1 | PA9 (TX), PA8 (RX) | Default UART1 pins (also debug UART) |
| UART2 | PA20 (TX), PA21 (RX) | Default UART2 pins |
| UART3 | PA13 (TX), PA12 (RX) | Default UART3 pins |
| SPI0 | PA12 (SCK), PA13 (MOSI), PA14 (MISO), PA15 (CS) | Default SPI0 pins |
| ADC_CH0 | PA4 | Analog input channel 0 |
| ADC_CH1 | PA5 | Analog input channel 1 |
| ADC_CH2 | PA12 | Analog input channel 2 |
| ADC_CH3 | PA13 | Analog input channel 3 |
| PWM4 | PA12 | PWM output channel 4 |
| PWM5 | PA13 | PWM output channel 5 |
| USB | PA11 (D-), PA12 (D+) | USB device/host |

> Pin mux is configured via `GPIOA_ModeCfg()` before peripheral initialization. Check CH57x datasheet for alternate function assignments.

---

## BLE Role State Machine

### Peripheral States
```
GAPROLE_INIT → GAPROLE_STARTED → GAPROLE_ADVERTISING ↔ GAPROLE_CONNECTED
                                                    ↓
                                            GAPROLE_WAITING (disconnected)
```

### Central States
```
GAPROLE_INIT → GAPROLE_STARTED → GAPROLE_STARTED (scanning) → GAPROLE_CONNECTED
```

### Key Callback Signatures
```c
// Peripheral state change
typedef void (*gapRolesStateNotify_t)(gapRole_States_t newState, gapRoleEvent_t *pEvent);

// Central event callback
typedef void (*pfnGapCentralRoleEventCB_t)(gapCentralRoleEvent_t *pEvent);

// GATT read/write callbacks
typedef uint8_t (*pfnGATTReadAttrCB_t)(uint16_t connHandle, gattAttribute_t *pAttr,
                                        uint8_t *pValue, uint16_t *pLen, uint16_t offset, uint16_t maxLen);
typedef uint8_t (*pfnGATTWriteAttrCB_t)(uint16_t connHandle, gattAttribute_t *pAttr,
                                         uint8_t *pValue, uint16_t len, uint16_t offset);
```

---

## Critical Pitfalls (Must Read)

These are the most common errors. Violating any of these will produce non-working firmware.

### 1. BLE Init Order Must Be Exact

```c
// ❌ WRONG — HAL_Init before BLE init
HAL_Init();
CH57X_BLEInit();

// ✅ CORRECT
SetSysClock(CLK_SOURCE_PLL_60MHz);
CH57X_BLEInit();
HAL_Init();
GAPRole_PeripheralInit();  // or CentralInit, etc.
Peripheral_Init();          // application init
Main_Circulation();         // never returns
```

### 2. Main_Circulation Never Returns

```c
// ❌ WRONG — main() returns
int main(void) {
    init_all();
    // main exits here, system halts
}

// ✅ CORRECT — main loop runs forever
int main(void) {
    init_all();
    Main_Circulation();  // contains TMOS_SystemProcess()
}
```

### 3. config.h BLE_MEMHEAP_SIZE Must Be Large Enough

```c
// ❌ WRONG — too small, BLE stack crashes
#define BLE_MEMHEAP_SIZE    (1024*2)

// ✅ CORRECT — minimum 6KB for most applications
#define BLE_MEMHEAP_SIZE    (1024*6)
// For MESH: may need 10KB+
```

### 4. ATT_MTU = BLE_BUFF_MAX_LEN - 4

```c
// ❌ WRONG — BUFF_MAX_LEN too small for desired MTU
#define BLE_BUFF_MAX_LEN    23  // actual MTU = 19

// ✅ CORRECT — for MTU 23 (default)
#define BLE_BUFF_MAX_LEN    27  // 23 + 4 = 27
// For MTU 247 (max):
#define BLE_BUFF_MAX_LEN    251 // 247 + 4 = 251
```

### 5. GATT Service Registration Order

```c
// ❌ WRONG — custom service before standard services
MyCustomService_AddService(GATT_ALL_SERVICES);
GGS_AddService(GATT_ALL_SERVICES);

// ✅ CORRECT — GAP and GATT services first
GGS_AddService(GATT_ALL_SERVICES);
GATTServApp_AddService(GATT_ALL_SERVICES);
DevInfo_AddService();
MyCustomService_AddService(GATT_ALL_SERVICES);  // custom last
```

### 6. Interrupt Handler Must Use .highcode Section

```c
// ❌ WRONG — interrupt in Flash, slow execution
void UART1_IRQHandler(void) {
    // ...
}

// ✅ CORRECT — interrupt in RAM for fast execution
__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void UART1_IRQHandler(void) {
    // ...
}
```

### 7. GPIO Mode Must Be Set Before Peripheral Use

```c
// ❌ WRONG — using UART without GPIO config
UART1_DefInit();
// TX/RX pins still in default (floating input) mode

// ✅ CORRECT — configure GPIO first
GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);       // UART1 RX pull-up
GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);   // UART1 TX output
UART1_DefInit();
```

### 8. Flash Write Erases Entire Sector

```c
// ❌ WRONG — thinking Flash can overwrite individual bytes
FLASH_ROM_WRITE(addr, &single_byte, 1);  // erases entire 256-byte sector!

// ✅ CORRECT — read-modify-write for partial updates
uint8_t buf[256];
FLASH_ROM_READ(sector_addr, buf, 256);
buf[offset] = new_value;
FLASH_ROM_ERASE(sector_addr, 256);
FLASH_ROM_WRITE(sector_addr, buf, 256);
```

### 9. IAP App Must Have Different Linker Script

```c
// ❌ WRONG — using default linker script for IAP application
// FLASH ORIGIN = 0x00000000 — overwrites bootloader!

// ✅ CORRECT — offset application to 0x1000
// In Link.ld:
// FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 444K
```

### 10. BLE Notification Requires Connection Handle

```c
// ❌ WRONG — sending notification without valid handle
GATT_Notification(0xFFFF, &noti, FALSE);  // INVALID_CONNHANDLE

// ✅ CORRECT — use handle from connection event
static uint16_t connHandle;
// Set in GAP_LINK_ESTABLISHED_EVENT:
connHandle = pEvent->linkCmpl.connectionHandle;
// Then send:
GATT_Notification(connHandle, &noti, FALSE);
```

### 11. TMOS Event Must Return Unhandled Bits

```c
// ❌ WRONG — returning 0 always
uint16_t ProcessEvent(uint8_t task_id, uint16_t events) {
    if (events & MY_EVT) {
        handle_my_evt();
        return 0;  // loses other pending events
    }
    return 0;
}

// ✅ CORRECT — XOR handled bits, return remainder
uint16_t ProcessEvent(uint8_t task_id, uint16_t events) {
    if (events & MY_EVT) {
        handle_my_evt();
        return (events ^ MY_EVT);
    }
    return 0;
}
```

### 12. BLE ConnectNumber Encoding

```c
// ❌ WRONG — separate variables
cfg.PeripheralNumber = 1;
cfg.CentralNumber = 3;

// ✅ CORRECT — packed into single uint8
cfg.ConnectNumber = (PERIPHERAL_MAX_CONNECTION & 3) | (CENTRAL_MAX_CONNECTION << 2);
// Max 3 peripheral (bits 0-1) + max 15 central (bits 4-7)
```

---

## Execution Workflow

| Step | Name | Description |
|------|------|-------------|
| 1 | Plan | Understand requirements, identify BLE role (if any), peripherals needed |
| 2 | Recipe | Check if a matching recipe exists in `recipes/`; if yes, follow its call chain |
| 3 | Query | For APIs not covered by recipes, query `resources/` using quick reference docs |
| 4 | Validate | Verify all API signatures, header includes, pin assignments, config.h values |
| 5 | Confirm | Present implementation plan to user: includes, pin config, init sequence, main loop |
| 6 | Execute | Generate code following the standard project structure |
| 7 | Check | Verify: init order, GPIO config before peripheral use, interrupt handler sections |
| 8 | Flash | Build with MounRiver Studio, flash via WCH-LinkE or USB ISP |
| 9 | Debug | Verify via UART debug output (UART1 @ 115200), check BLE with nRF Connect |

---

## Failure Strategies

| Situation | Action |
|---|---|
| API does not exist in resources | Stop immediately, inform user |
| BLE stack config unsure | Default to `BLE_MEMHEAP_SIZE=(1024*6)`, `BLE_BUFF_MAX_LEN=27` |
| Pin conflict detected | Check CH57x datasheet for alternate functions, suggest reassignment |
| IAP memory overlap | Verify bootloader at 0x0000, app at 0x1000, no overlap |
| MESH not working | Check provisioning state, verify mesh library linked |

## References

- Scenario recipes → `recipes/` directory
- Peripheral API quick ref → `resources/peripheral_api.md`
- BLE API quick ref → `resources/ble_api.md`
- Configuration options → `resources/config_reference.md`
- Memory layout → `resources/memory_layout.md`
- HAL reference → `resources/hal_reference.md`
- Common pitfalls → `resources/pitfalls.md`
- Example project index → `resources/example_list.md`
- Example projects → `resources/EXAM/` (all peripheral and BLE examples)
- BLE development guide → `resources/EXAM/BLE/沁恒低功耗蓝牙软件开发参考手册.pdf`
