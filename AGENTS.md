# AGENTS.md — Supplementary Agent Guide

> Core rules, mapping table, pitfalls, recipes index, and execution workflow are all in `SKILL.md`.
> This file covers **only** conventions and tooling guidance not present in `SKILL.md`. Do not duplicate content.

## Project Context

**Language**: C · **Target**: WCH CH57x RISC-V BLE SoC（CH573/CH571） · **Toolchain**: MounRiver Studio (GCC RISC-V)

## Code Generation Conventions

### File Naming
- Source files: `*.c`, headers: `*.h`
- BLE application files: `<role>_main.c` (e.g., `peripheral_main.c`, `central_main.c`)
- BLE service profiles: `<service_name>.c` in `Profile/` directory
- Configuration: `config.h` in project `HAL/include/`

### Include Pattern
```c
// Non-BLE projects
#include "CH57x_common.h"  // pulls in all peripheral headers

// BLE projects
#include "CONFIG.h"
#include "HAL.h"
#include "CH57xBLE_LIB.h"
#include "peripheral.h"     // or central.h, etc.
#include "devinfoservice.h"
#include "gattprofile.h"
```

### Standard BLE Project Structure
```
MyBLEProject/
├── APP/
│   ├── include/
│   │   ├── config.h          # BLE stack configuration
│   │   └── peripheral.h      # Application header
│   ├── peripheral_main.c     # main() and init
│   └── peripheral.c          # Application logic, callbacks
├── Profile/
│   ├── include/
│   │   ├── devinfoservice.h
│   │   └── gattprofile.h
│   ├── devinfoservice.c      # Device Information Service
│   └── gattprofile.c         # Custom GATT profile
├── HAL/                      # Linked from resources/EXAM/BLE/HAL/
├── Ld/Link.ld                # Linked from resources/EXAM/SRC/Ld/
├── RVMSIS/                   # Linked from resources/EXAM/SRC/RVMSIS/
├── Startup/                  # Linked from resources/EXAM/SRC/Startup/
├── StdPeriphDriver/          # Linked from resources/EXAM/SRC/StdPeriphDriver/
├── .project                  # MounRiver Studio project file
└── *.wvproj                  # MounRiver Studio workspace project
```

### Standard Main Loop Pattern
```c
int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

#if (defined(DCDC_ENABLE)) && (DCDC_ENABLE == TRUE)
    PWR_DCDCCfg(ENABLE);
#endif

#ifdef DEBUG
    UART1_DefInit();
    PRINT("CH57x BLE init...\n");
#endif

    CH57X_BLEInit();
    HAL_Init();
    GAPRole_PeripheralInit();
    Peripheral_Init();

    Main_Circulation();  // never returns
    return 0;            // unreachable
}
```

### Interrupt Handler Template
```c
__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void UART1_IRQHandler(void) {
    uint8_t flag = UART1_GetITFlag();
    if ((flag & UART_II_MASK) == UART_II_RECV_RDY) {
        // Handle received data
        while (UART1_RecvByte(&data) == SUCCESS) {
            // process data
        }
    }
}
```

### Debug Output Convention
```c
#ifdef DEBUG
  #define PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
  #define PRINT(fmt, ...)
#endif
```
Debug UART: UART1 at 115200 baud, 8N1. PA8 (RX), PA9 (TX).

## Build Workflow

1. Open `.wvproj` in MounRiver Studio
2. Build: Project → Build Project (or Ctrl+B)
3. Flash: Run → Debug (WCH-LinkE) or use WCH ISP tool via USB
4. Debug: UART1 serial output at 115200 baud

## BLE App Code Generation Checklist

- [ ] `config.h` has correct `BLE_MEMHEAP_SIZE`, `BLE_BUFF_MAX_LEN`, connection limits
- [ ] `CH57X_BLEInit()` called with properly filled `bleConfig_t`
- [ ] GAP/GATT services added before custom services
- [ ] Application callbacks registered after service init
- [ ] `Main_Circulation()` called at end of main (never returns)
- [ ] Connection handle stored from `GAP_LINK_ESTABLISHED_EVENT`
- [ ] Advertising data ≤ 31 bytes (`B_MAX_ADV_LEN`)
- [ ] Custom UUIDs are 128-bit for custom services
- [ ] `config.h` `HAL_SLEEP` set correctly for power requirements
- [ ] Interrupt handlers use `.highcode` section

## Do Not Modify

- `resources/` — API documentation source
- `SKILL.md` front matter — Skill metadata
