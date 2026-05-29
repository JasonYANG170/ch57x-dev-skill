**[中文](README.md)** | English

# ch57x-dev-skill

AI Skill for CH57x BLE microcontroller firmware development. Enables AI Agents to automatically generate embedded code conforming to the WCH SDK, including BLE peripheral/central/mesh applications, peripheral driver configuration, IAP/OTA updates, and more.

Supported chips: CH573, CH571 and other CH57x family members.

## Features

- API documentation based on the actual WCH CH57x EVT SDK
- Covers all BLE roles: Peripheral, Central, Broadcaster, Observer, Mesh
- Covers common peripherals: GPIO, UART, SPI, ADC, Timer, PWM, Flash, USB
- Provides IAP bootloader and BLE OTA firmware upgrade solutions
- Includes common errors and pitfalls to avoid

## Installation

### 1. Clone to your skill directory

Find or create the skill directory based on your AI Agent documentation:

```bash
git clone <repo-url> ch57x-dev-skill
```

Examples:

> **Claude Code**
> **Project scope**: `.claude/skills` in project root
> **User scope**: `~/.claude/skills` (applies to all projects)
> Navigate to the skills folder and run `git clone <repo-url> ch57x-dev-skill`

> **QwenCode**
> **Project scope**: `.qwen/skills` in project root
> **User scope**: `~/.qwen/skills` (applies to all projects)

> **OpenCode**
> **Project scope**: `.opencode/skills` in project root
> **User scope**: `~/.config/opencode/skills` (applies to all projects)

### 2. Use the skill

Confirm the skill is loaded in your AI Agent, then invoke it via command.

> **Claude Code**
> Run `claude` in terminal, then type `/ch57x-dev-skill` and describe your needs

> **QwenCode**
> Run `qwen` in terminal, type `/skills`, select ch57x-dev-skill

> **OpenCode**
> Run `opencode` in terminal, type `/skills`, select ch57x-dev-skill

## How It Works

The skill defines a workflow that the AI Agent follows when generating firmware code:

| Step | Name | Description |
|------|------|-------------|
| 1 | Plan | Understand requirements, identify BLE role and peripherals needed |
| 2 | Recipe | Match scenario in `recipes/`, get complete call chain |
| 3 | Query | Look up API signatures in `resources/` quick reference docs |
| 4 | Validate | Verify API signatures, header includes, pin assignments, config.h values |
| 5 | Confirm | Present implementation plan to user (includes, pins, init sequence, main loop) |
| 6 | Execute | Generate code following standard project structure |
| 7 | Check | Verify init order, GPIO config, interrupt handler sections |
| 8 | Flash | Build with MounRiver Studio, flash via WCH-LinkE or USB ISP |
| 9 | Debug | Verify via UART debug output (UART1 @ 115200), check BLE with nRF Connect |

## Covered Scenarios

### BLE Applications
- BLE Peripheral — advertising, GATT services, notifications
- BLE Central — scanning, connecting, service discovery, read/write
- BLE Broadcaster / Observer — Beacon, pure scanning
- BLE HID Device — keyboard, mouse, consumer control, touch
- BLE Standard Profiles — HeartRate, CyclingSensor, RunningSensor, SpeedTest
- BLE Multi-role / Multi-connection — CentPeri, MultiCentral, MultiCentPeri
- BLE Mesh — Alibaba light model, provisioning, vendor model
- BLE UART Transparent Bridge — bidirectional BLE-UART data
- RF Test & Non-standard PHY — DTM, RF_PHY, frequency hopping
- LWNS Networking — broadcast, unicast, mesh, IoCHub
- BLE + USB Combo — BLE-to-USB serial, WCHMcuIAP tool

### Peripheral Drivers
- GPIO — input, output, interrupt, pull-up/down
- UART — init, send, receive, interrupt-driven
- SPI — master/slave, FIFO, DMA transfer
- ADC — external channel, temperature, battery, touch key
- Timer / PWM — periodic interrupt, PWM output, input capture
- Flash / EEPROM — read, write, erase, data storage

### System Features
- Power Management — Idle, Halt, Sleep, Shutdown modes
- IAP Bootloader — UART/USB IAP, dual-image OTA
- USB Device — CDC serial, HID, vendor-defined
- USB Host — device enumeration, AOA, U-disk filesystem

## References

- EVT Example Projects → `resources/EXAM/` (all peripheral and BLE examples, browsable directly)
- BLE Development Guide → `resources/EXAM/BLE/沁恒低功耗蓝牙软件开发参考手册.pdf`
- BLE OTA Guide → `resources/EXAM/BLE/WCH蓝牙空中升级（BLE OTA）.PDF`
- MESH Guide → `resources/EXAM/BLE/MESH/蓝牙芯片的MESH开发参考手册.pdf`
- WCH Official Website → http://wch.cn
