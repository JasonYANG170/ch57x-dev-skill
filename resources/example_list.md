# EXAM 目录结构

```
CH573: 低功耗蓝牙 - 32位RISC-V内核微控制器
└── EXAM:
    ├── SRC                          共享源码（链接到每个项目）
    │   ├── Ld                       Linker 链接脚本
    │   ├── RVMSIS                   内核系统头文件
    │   ├── Startup                  CH57x 系列启动文件
    │   └── StdPeriphDriver          基本外设驱动源文件及头文件
    │
    ├── ADC                          ADC 采样例程
    │                                （温度检测、单通道检测、差分通道检测、TouchKey 检测、中断方式采样）
    │
    ├── FLASH                        片上 Flash 例程
    │                                （Code 区、DataFlash 区的擦/读/写）
    │
    ├── IAP                          在线编程
    │   ├── APP                      与 IAP 配套使用的 APP 程序例程
    │   ├── USB_IAP                  通过 USB 更新片上程序的例程
    │   ├── UART_IAP                 通过串口更新片上程序的例程
    │   └── WCHMcuIAP_WinAPP         IAP 上位机工具及源码
    │
    ├── PM                           系统睡眠模式并唤醒例程
    │                                （GPIOA_5 作为唤醒源，共 4 种功耗等级）
    │
    ├── PWMX                         PWM4-11 输出功能例程
    │
    ├── SPI0                         SPI0 例程（Master/Slave 模式数据收发）
    │
    ├── TMR                          定时器功能例程
    │
    ├── UART1                        串口 1 收发例程
    │
    ├── USB
    │   ├── Device
    │   │   ├── COM                  USB 模拟 CDC 设备例程
    │   │   ├── VendorDefinedDev     模拟自定义 USB 设备（CH372 设备）例程
    │   │   │                        （8 个非 0 通道，数据下传后取反上传）
    │   │   ├── CompoundDev          USB 模拟键鼠例程
    │   │   │                        （支持数据上传、USB 唤醒、USB HID 类命令）
    │   │   └── HID_CompliantDev     USB 模拟 HID 兼容设备例程
    │   │
    │   └── Host
    │       ├── HostEnum             USB 常见设备（hid 键鼠、hub）枚举数据上下传演示
    │       ├── HostAOA              USB 主机应用例程（支持连接安卓设备与 APP 通讯）
    │       └── U_DISK               U 盘文件系统示例
    │           ├── EXAM1.C          以字节为单位读写文件（创建、删除、修改属性、改名）
    │           ├── EXAM10.C         文件创建、删除、修改属性、改名
    │           ├── EXAM11.C         枚举根目录或指定目录下的文件
    │           ├── EXAM13.C         创建长文件名文件
    │           └── USB_LIB          U 盘文件系统库文件
    │
    └── BLE                          蓝牙例程
        ├── Broadcaster              广播者角色例程（一直处于广播态）
        ├── Central                  主机例程（扫描、连接从机、读写特征值）
        ├── Peripheral               外设从机角色例程（自定义五种属性的服务）
        ├── CentPeri                 主从一体例程（主机+从机同时运行）
        ├── MultiCentral             主机多连接例程（同时连接三个从机）
        ├── MultiCentPeri            多主机多从机例程（3 主机 + 3 从机）
        ├── Observer                 观察者角色例程（定时扫描，打印广播地址）
        ├── HeartRate                心率计例程（连接后定时上传心率）
        ├── CyclingSensor            骑行传感器例程（上传速度和踏频）
        ├── RunningSensor            跑步传感器例程（上传速度）
        ├── HID_Keyboard             蓝牙键盘例程
        ├── HID_Mouse                蓝牙鼠标例程
        ├── HID_Consumer             蓝牙拍照器例程（音量键下键）
        ├── HID_Touch                蓝牙触摸例程（触摸笔设备）
        ├── Direct_Test_Mode         DTM 测试例程（结合 RF 测试工具使用）
        ├── RF_PHY                   非标准无线收发例程
        ├── RF_PHY_Hop               非标准无线跳频收发例程
        ├── IoCHub_Net               BLE + IoChub 简单灯控开关例程
        ├── BLE_UART                 蓝牙串口透传例程
        ├── BLE_USB                  蓝牙与 USB 合用例程（USB 模拟 340 设备转发蓝牙数据）
        ├── SpeedTest_Central        蓝牙测速主机例程
        ├── SpeedTest_Peripheral     蓝牙测速从机例程
        │
        ├── MESH                     BLE Mesh 例程
        │   ├── adv_ali_light                          天猫精灵灯例程（开关控制）
        │   ├── adv_ali_light_add_lightness            添加亮度属性示例
        │   ├── adv_ali_light_add_windspeed            添加风速属性示例
        │   ├── adv_ali_light_multi_element            多元素风扇灯例程（风扇+灯）
        │   ├── adv_ali_light_with_peripheral          天猫精灵灯 + BLE 调试助手控制
        │   ├── adv_proxy                              代理节点例程（PB_GATT 配网）
        │   ├── adv_vendor                             厂商自定义模型例程
        │   ├── adv_vendor_friend                      厂商模型 + 朋友节点功能
        │   ├── adv_vendor_low_power                   厂商模型 + 低功耗节点功能
        │   ├── adv_vendor_self_provision              厂商模型 + 本地自配网
        │   ├── adv_vendor_self_provision_with_peripheral  厂商模型 + BLE 调试助手 + 自配网
        │   ├── provisioner_vendor                     配网发起者例程（配合 adv_vendor）
        │   ├── provisioner_vendor_with_peripheral     配网发起者 + BLE 调试助手控制
        │   └── MESH_LIB                               MESH 协议栈库文件及头文件
        │
        ├── LWNS                     LWNS 无线组网例程
        │                            （broadcast、unicast、netflood、mesh）
        │
        ├── BackupUpgrade_IAP        备份无线升级 IAP 例程
        ├── BackupUpgrade_JumpIAP    备份无线升级跳转 IAP 例程
        ├── BackupUpgrade_OTA        备份无线升级用户例程（外设从机 + OTA）
        ├── OnlyUpdateApp_IAP        固定库无线升级 IAP 例程
        ├── OnlyUpdateApp_JumpIAP    固定库无线升级跳转 IAP 例程
        ├── OnlyUpdateApp_Peripheral 固定库无线升级用户例程
        │
        ├── HAL                      例程共用的硬件相关文件
        └── LIB                      BLE 协议栈库文件及头文件
```


