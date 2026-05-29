# CH57x Memory Layout Reference

## Chip Memory Map

### Flash (Code Area)
| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| Boot/System | 0x00000000 | 0x00000FFF | 4KB | System bootloader (ROM) |
| Application | 0x00001000 | 0x0006FFFF | 444KB | User application code |
| DataFlash | 0x00077E00 | 0x00077FFF | 512B | NV storage, calibration data |

### RAM
| Region | Start Address | End Address | Size | Description |
|--------|---------------|-------------|------|-------------|
| RAM | 0x20003800 | 0x20007FFF | 18KB | Main RAM (data, bss, stack) |

## Standard Application Linker Script

From `resources/EXAM/SRC/Ld/Link.ld`:

```
ENTRY( _start )
__stack_size = 512;

MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 448K
    RAM (xrw) : ORIGIN = 0x20003800, LENGTH = 18K
}

SECTIONS
{
    .highcode : { *(.vector); *(.highcode); } >RAM AT>FLASH
    .text : { *(.text); *(.rodata); } >FLASH
    .data : { *(.data); *(.sdata); } >RAM AT>FLASH
    .bss : { *(.bss); *(.sbss); } >RAM
    .stack : { . = ORIGIN(RAM) + LENGTH(RAM) - __stack_size; } >RAM
}
```

Key points:
- `.highcode` section is loaded from Flash but executed from RAM (for fast interrupt handlers)
- Stack grows downward from top of RAM (512 bytes default)
- `.data` is initialized from Flash at startup

## IAP Memory Layout

### Bootloader (UART_IAP / USB_IAP)
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 4K    // Bootloader at start
    RAM (xrw) : ORIGIN = 0x20003800, LENGTH = 18K
}
```

### Application (with IAP)
```
MEMORY
{
    FLASH (rx) : ORIGIN = 0x00001000, LENGTH = 444K  // App starts after bootloader
    RAM (xrw) : ORIGIN = 0x20003800, LENGTH = 18K
}
```

### OTA Dual-Image Layout

```
Address     Size      Region
0x00000000  12KB      IAP Bootloader
0x00003000  ---       (reserved)
0x00001000  216KB     Image A (running application)
0x00039000  216KB     Image B (backup / OTA target)
0x0006D000  12KB      IAP image (for jump-to-IAP)
0x00070000  ---       Application code end
0x00077E00  512B      DataFlash (image flags, NV)
```

Image Flag values in DataFlash:
- `0x01`: Image A is active
- `0x02`: Image B is active
- `0x03`: IAP image is active

## Section Placement Guidelines

### `.highcode` Section
Use for time-critical code that must execute from RAM:
- Interrupt handlers (`__attribute__((section(".highcode")))`)
- Flash erase/write routines
- BLE stack critical paths

### `.text` Section
Normal code execution from Flash:
- Application logic
- Peripheral drivers
- Utility functions

### `.data` Section
Initialized global/static variables:
- Stored in Flash, copied to RAM at startup
- Keep small to save Flash space

### `.bss` Section
Zero-initialized global/static variables:
- No Flash storage needed
- Zeroed at startup

## BLE Stack Memory

The BLE stack requires a dedicated heap (`MEM_BUF`):
```c
// In config.h
#define BLE_MEMHEAP_SIZE    (1024*6)   // 6KB BLE heap

// In application
static uint8_t MEM_BUF[BLE_MEMHEAP_SIZE * 4];  // Allocated in .bss
```

BLE heap usage depends on:
- Number of connections
- ATT MTU size
- Number of GATT services/characteristics
- Buffer count

## Flash Sector Operations

Flash operates on 256-byte sectors:
- **Read**: Any address, any length
- **Write**: Must be 256-byte aligned, writes full sector
- **Erase**: Must be 256-byte aligned, erases full sector

DataFlash (EEPROM) operates similarly but in a protected region.

## Unique ID

Each CH57x chip has a unique 8-byte ID readable via:
```c
uint8_t uid[8];
GET_UNIQUE_ID(uid);
```
