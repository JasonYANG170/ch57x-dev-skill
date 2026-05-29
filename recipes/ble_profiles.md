# BLE 标准 Profile 例程

> **适用摘要**: 使用标准 BLE GATT Profile 的例程，包括心率、骑行传感器、跑步传感器、测速等。

## 触发意图

- "心率服务"
- "传感器 Profile"
- "BLE 测速"
- "标准 GATT 服务"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/HeartRate/`, `CyclingSensor/`, `RunningSensor/`, `SpeedTest_Central/Peripheral/` |

## 分步说明

### HeartRate（心率服务）

参考 `resources/EXAM/BLE/HeartRate/`：

```c
// 标准心率服务 UUID: 0x180D
// 心率测量特征 UUID: 0x2A37

// Profile 文件
Profile/heartRateService.c    // 心率服务实现
Profile/battService.c         // 电池服务
Profile/devInfoService.c      // 设备信息服务

// 发送心率数据
void HeartRate_MeasureNotify(uint8_t heartRate) {
    attHandleValueNoti_t noti;
    noti.handle = heartRateCharHandle;
    noti.len = 2;
    uint8_t data[2] = {0x00, heartRate};  // flags=0, heart rate
    noti.pValue = data;
    GATT_Notification(connHandle, &noti, FALSE);
}
```

### CyclingSensor（骑行传感器）

参考 `resources/EXAM/BLE/CyclingSensor/`：

```c
// 标准骑行功率服务 UUID: 0x1818
// 骑行速度/踏频特征

// 定时上报速度和踏频
void CyclingSensor_Notify(void) {
    uint8_t data[11];
    data[0] = 0x02;  // flags: speed + cadence present
    // ... 填充速度和踏频数据 ...
    attHandleValueNoti_t noti;
    noti.handle = cyclingCharHandle;
    noti.len = sizeof(data);
    noti.pValue = data;
    GATT_Notification(connHandle, &noti, FALSE);
}
```

### RunningSensor（跑步传感器）

参考 `resources/EXAM/BLE/RunningSensor/`：

```c
// 标准跑步速度服务 UUID: 0x1814
// 跑步速度和踏频测量特征 UUID: 0x2A53

void RunningSensor_Notify(uint16_t speed) {
    uint8_t data[4];
    data[0] = 0x00;  // flags
    data[1] = LO_UINT16(speed);
    data[2] = HI_UINT16(speed);
    data[3] = 0;     // cadence
    attHandleValueNoti_t noti;
    noti.handle = runningCharHandle;
    noti.len = sizeof(data);
    noti.pValue = data;
    GATT_Notification(connHandle, &noti, FALSE);
}
```

### SpeedTest（测速）

参考 `resources/EXAM/BLE/SpeedTest_Central/` 和 `SpeedTest_Peripheral/`：

```c
// 用于测试 BLE 吞吐量
// 从机持续发送通知，主机统计接收速率

// 从机端：高速发送
void SpeedTest_Send(void) {
    uint8_t data[244];  // MTU-3 字节
    memset(data, 0xAA, sizeof(data));
    attHandleValueNoti_t noti;
    noti.handle = speedCharHandle;
    noti.len = sizeof(data);
    noti.pValue = data;
    GATT_Notification(connHandle, &noti, FALSE);
}

// 主机端：统计接收速率
static uint32_t rxCount = 0;
static uint32_t rxBytesPerSec = 0;

void SpeedTest_Receive(attHandleValueNoti_t *noti) {
    rxCount += noti->len;
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 数据格式错误 | 未按标准格式填充 | 参考蓝牙 SIG 规范 |
| 通知不发送 | CCCD 未使能 | 等待主机使能通知 |
| 测速丢包 | 发送速度超过连接间隔 | 调整连接参数或降低发送频率 |

## 参考项目

- `resources/EXAM/BLE/HeartRate/` — 心率服务
- `resources/EXAM/BLE/CyclingSensor/` — 骑行传感器
- `resources/EXAM/BLE/RunningSensor/` — 跑步传感器
- `resources/EXAM/BLE/SpeedTest_Central/` — 测速主机
- `resources/EXAM/BLE/SpeedTest_Peripheral/` — 测速从机
