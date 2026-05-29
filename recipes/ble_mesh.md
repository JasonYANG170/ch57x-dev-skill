# 创建 BLE Mesh 应用

> **适用摘要**: 创建 BLE Mesh 应用，基于阿里巴巴 Genie 智能灯模型，实现配网和控制。

## 触发意图

- "做一个 BLE Mesh 设备"
- "智能灯 Mesh"
- "阿里智能 Mesh"
- "BLE Mesh 配网"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/BLE/MESH/adv_ali_light/` |
| 库文件 | `resources/EXAM/BLE/MESH/MESH_LIB/` |
| 知识 | BLE Mesh 基础概念（配网、模型、组播） |

## 调用链

```
Step 1: 配置 config.h（Mesh 需要更多内存）
Step 2: 初始化 Mesh 协议栈
Step 3: 配置设备 composition
Step 4: 实现模型回调
Step 5: 处理配网事件
Step 6: 实现控制逻辑（开/关、亮度、色温）
```

## 分步说明

### Step 1: config.h

```c
#define BLE_MAC                 FALSE
#define DCDC_ENABLE             FALSE
#define HAL_SLEEP               FALSE
#define CLK_OSC32K              1
#define BLE_MEMHEAP_SIZE        (1024*12)  // Mesh 需要 12KB+
#define BLE_BUFF_MAX_LEN        27
#define BLE_BUFF_NUM            10
#define BLE_TX_NUM_EVENT        1
#define BLE_TX_POWER            LL_TX_POWEER_0_DBM
#define PERIPHERAL_MAX_CONNECTION   1
#define CENTRAL_MAX_CONNECTION      0
```

### Step 2: Mesh 初始化

参考 `adv_ali_light/APP/app_main.c`：

```c
#include "app_main.h"
#include "app.h"

static uint8_t appTaskId;

void app_main(void) {
    appTaskId = TMOS_ProcessEventRegister(app_ProcessEvent);

    // Mesh 初始化（在 BLE 初始化完成后调用）
    blemesh_on_sync();
}

static void blemesh_on_sync(void) {
    // 初始化 Mesh 协议栈
    bt_mesh_lib_init();

    // 配置设备参数
    bt_mesh_cfg_t cfg = {
        .relay = BT_MESH_RELAY_ENABLED,
        .beacon = BT_MESH_BEACON_ENABLED,
        .gatt_proxy = BT_MESH_GATT_PROXY_ENABLED,
        .frnd = BT_MESH_FRIEND_DISABLED,
        .default_ttl = 5,
    };
    bt_mesh_cfg_set(&cfg);

    // 注册 composition
    bt_mesh_comp_register(&comp);

    // 初始化 provisioning
    bt_mesh_prov_enable(BT_MESH_PROV_ADV | BT_MESH_PROV_GATT);

    PRINT("Mesh initialized, waiting for provisioning...\n");
}
```

### Step 3: 设备 Composition

```c
// 元素和模型定义
static struct bt_mesh_elem elements[] = {
    BT_MESH_ELEM(0,
        BT_MESH_MODEL_LIST(
            BT_MESH_MODEL_CFG_SRV,
            BT_MESH_MODEL_HEALTH_SRV,
            &vnd_model,           // 厂商自定义模型
            &gen_onoff_srv_model  // Generic OnOff Server
        ),
        BT_MESH_MODEL_NONE
    ),
};

static const struct bt_mesh_comp comp = {
    .cid = 0x01A8,     // 阿里巴巴 Company ID
    .pid = 0x0001,
    .vid = 0x0001,
    .elem = elements,
    .elem_count = ARRAY_SIZE(elements),
};
```

### Step 4: 模型回调

```c
// Generic OnOff Server 回调
static void gen_onoff_get(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx) {
    struct net_buf_simple *msg = NET_BUF_SIMPLE(2 + 1 + 4);
    bt_mesh_model_msg_init(msg, BT_MESH_MODEL_OP_GEN_ONOFF_STATUS);
    net_buf_simple_add_u8(msg, get_onoff_state());
    bt_mesh_model_send(model, ctx, msg, NULL, NULL);
}

static void gen_onoff_set(struct bt_mesh_model *model, struct bt_mesh_msg_ctx *ctx,
                           struct net_buf_simple *buf) {
    uint8_t onoff = net_buf_simple_pull_u8(buf);
    set_onoff_state(onoff);

    // 发送状态回复
    gen_onoff_get(model, ctx);

    // 发布状态变更
    bt_mesh_model_publish(model);
}

// 厂商模型回调（参考 app_vendor_model.c）
static void vnd_model_recv(struct bt_mesh_model *model,
                            struct bt_mesh_msg_ctx *ctx,
                            struct net_buf_simple *buf) {
    uint8_t opcode = net_buf_simple_pull_u8(buf);
    switch (opcode) {
        case VND_MSG_SET_ONOFF:
            // 处理开/关命令
            break;
        case VND_MSG_SET_LIGHTNESS:
            // 处理亮度命令
            break;
    }
}
```

### Step 5: 配网事件处理

```c
// 配网完成回调
static void prov_complete(uint16_t net_idx, uint16_t addr) {
    PRINT("Provisioned: net_idx=0x%04x, addr=0x%04x\n", net_idx, addr);

    // 配网完成后订阅组播地址
    bt_mesh_subnet_foreach(subscribe_group_addr);
}

static void subscribe_group_addr(struct bt_mesh_subnet *sub, void *user_data) {
    // 订阅 0xC000-0xCFFF 组播地址范围
    for (uint16_t addr = 0xC000; addr <= 0xCFFF; addr++) {
        bt_mesh_model_subscribe(&gen_onoff_srv_model, addr);
    }
}
```

### Step 6: 控制逻辑

```c
static uint8_t onoff_state = 0;

void set_onoff_state(uint8_t state) {
    onoff_state = state;
    if (state) {
        GPIOA_SetBits(GPIO_Pin_18);    // LED on
    } else {
        GPIOA_ResetBits(GPIO_Pin_18);  // LED off
    }
    PRINT("Light %s\n", state ? "ON" : "OFF");
}

uint8_t get_onoff_state(void) {
    return onoff_state;
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 无法配网 | Mesh 库未链接 | 确保 MESH_LIB 在项目中 |
| 配网后无响应 | 模型未绑定 AppKey | 配网完成后自动绑定 |
| 中继不工作 | relay 未启用 | 在 cfg 中设置 `relay = BT_MESH_RELAY_ENABLED` |
| 内存不足 | BLE_MEMHEAP_SIZE 太小 | 增大到 12KB 或更多 |

## 变体

- **添加亮度控制**: 参考 `adv_ali_light_add_lightness/`
- **添加风速控制**: 参考 `adv_ali_light_add_windspeed/`
- **多元素设备**: 参考 `adv_ali_light_multi_element/`
- **自定义厂商模型**: 参考 `adv_vendor/`
- **Provisioner**: 参考 `provisioner_vendor/`

## 参考项目

- `resources/EXAM/BLE/MESH/adv_ali_light/` — 基础阿里智能灯
- `resources/EXAM/BLE/MESH/adv_vendor/` — 自定义厂商模型
- `resources/EXAM/BLE/MESH/MESH_LIB/` — Mesh 协议栈库
- `resources/EXAM/BLE/MESH/蓝牙芯片的MESH开发参考手册.pdf` — 官方参考手册
