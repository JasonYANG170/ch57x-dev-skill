**[English](README_EN.md)** | 中文

# ch57x-dev-skill

用于 CH57x 系列 BLE 微控制器固件开发的 AI Skill。让 AI Agent 自动生成符合 WCH SDK 规范的嵌入式代码，包括 BLE 外设/主机/Mesh 应用、外设驱动配置、IAP/OTA 升级等。

支持芯片：CH573、CH571 等 CH57x 系列。

## 功能特性

- 基于 WCH CH57x EVT SDK 的实际 API 文档
- 覆盖 BLE 所有角色：Peripheral、Central、Broadcaster、Observer、Mesh
- 覆盖常用外设：GPIO、UART、SPI、ADC、Timer、PWM、Flash、USB
- 提供 IAP 引导程序和 BLE OTA 固件升级方案
- 包含常见错误和陷阱，避免踩坑

## 安装说明

### 1. 拉取仓库到 skill 目录

根据你使用的 AI Agent 文档，找到或创建存放 Skill 的目录：

```bash
git clone <repo-url> ch57x-dev-skill
```

例如：

> **Claude Code**
> **项目作用域**：位于项目根目录下的 `.claude/skills`
> **用户作用域**：位于 `~/.claude/skills`，对本机所有项目生效
> 进入到对应的 skills 文件夹下
> 在终端执行 `git clone <repo-url> ch57x-dev-skill` 即可

> **QwenCode**
> **项目作用域**：位于项目根目录下的 `.qwen/skills`
> **用户作用域**：位于 `~/.qwen/skills`，对本机所有项目生效

> **OpenCode**
> **项目作用域**：位于项目根目录下的 `.opencode/skills`
> **用户作用域**：位于 `~/.config/opencode/skills`，对本机所有项目生效

### 2. 使用指定 skill

在你的 AI Agent 中确认 Skill 已加载，可通过命令指定 skill。

例如：

> **Claude Code**
> 在终端中输入 `claude` 后回车，然后输入 `/ch57x-dev-skill` 并描述你的需求

> **QwenCode**
> 在终端中输入 `qwen` 后回车，输入 `/skills` 回车，选择 ch57x-dev-skill

> **OpenCode**
> 在终端中输入 `opencode` 后回车，输入 `/skills` 回车，选择 ch57x-dev-skill

## 工作原理

Skill 定义了一套工作流，AI Agent 在生成固件代码时会遵循：

| 步骤 | 名称 | 说明 |
|------|------|------|
| 1 | 计划 | 理解需求，确定 BLE 角色和所需外设 |
| 2 | 方案 | 匹配 `recipes/` 中的场景，获取完整调用链 |
| 3 | 查询 | 查阅 `resources/` 中的 API 速查文档 |
| 4 | 验证 | 确认 API 签名、头文件包含、引脚分配、config.h 配置 |
| 5 | 确认 | 向用户展示实现方案（包含、引脚、初始化序列、主循环） |
| 6 | 执行 | 生成代码，遵循标准项目结构 |
| 7 | 检查 | 验证初始化顺序、GPIO 配置、中断处理程序段 |
| 8 | 烧录 | 通过 MounRiver Studio 构建，WCH-LinkE 或 USB ISP 烧录 |
| 9 | 调试 | 通过 UART 调试输出验证（UART1 @ 115200），nRF Connect 检查 BLE |

## 覆盖场景

### BLE 应用
- BLE 从机（Peripheral）— 广播、GATT 服务、通知
- BLE 主机（Central）— 扫描、连接、服务发现、读写
- BLE 广播者/观察者（Broadcaster/Observer）— Beacon 信标、纯扫描
- BLE HID 设备 — 键盘、鼠标、消费者控制、触摸
- BLE 标准 Profile — 心率、骑行传感器、跑步传感器、测速
- BLE 主从一体/多连接 — CentPeri、MultiCentral、MultiCentPeri
- BLE Mesh — 阿里智能灯模型、配网、厂商模型
- BLE UART 透传 — 双向 BLE-UART 数据桥接
- RF 测试与非标射频 — DTM、RF_PHY、跳频
- LWNS 轻量组网 — 广播、单播、网状网络、IoCHub
- BLE + USB 合用 — 蓝牙转 USB 串口、WCHMcuIAP 上位机

### 外设驱动
- GPIO — 输入、输出、中断、上下拉
- UART — 初始化、发送、接收、中断驱动
- SPI — 主/从模式、FIFO、DMA 传输
- ADC — 外部通道、温度、电池、触摸按键
- Timer / PWM — 定时中断、PWM 输出、输入捕获
- Flash / EEPROM — 读、写、擦除、数据存储

### 系统功能
- 电源管理 — Idle、Halt、Sleep、Shutdown 模式
- IAP 引导程序 — UART/USB IAP，双镜像 OTA
- USB 设备 — CDC 串口、HID、自定义设备
- USB 主机 — 设备枚举、AOA、U 盘文件系统

## 参考资源

- EVT 例程代码 → `resources/EXAM/`（包含所有外设和 BLE 例程，可直接查阅）
- BLE 开发参考手册 → `resources/EXAM/BLE/沁恒低功耗蓝牙软件开发参考手册.pdf`
- BLE OTA 说明 → `resources/EXAM/BLE/WCH蓝牙空中升级（BLE OTA）.PDF`
- MESH 参考手册 → `resources/EXAM/BLE/MESH/蓝牙芯片的MESH开发参考手册.pdf`
- WCH 官网 → http://wch.cn
