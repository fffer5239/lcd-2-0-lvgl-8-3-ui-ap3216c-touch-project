/*
 * soft_i2c - 可移植软件 I2C（位带/比特翻转）核心层
 *
 * 设计目标：
 * - 借鉴 RT-Thread 风格但不完全相同
 * - 裸机友好：不使用设备查找/注册框架，只通过结构体实例管理多总线
 * - 内联 GPIO 翻转函数以提高速度（由端口层提供）
 * - 模块化、便于移植（STM32/GD32/AT32 等）
 * - 支持任意 GPIO 定义和多 I2C 总线
 * - 初始化时可设置 I2C 频率
 *
 * 提供的 API：
 *   - soft_i2c_init, soft_i2c_start, soft_i2c_stop
 *   - soft_i2c_write_byte, soft_i2c_read_byte
 *   - soft_i2c_write, soft_i2c_read
 *   - soft_i2c_scan（以表格打印扫描结果）
 *
 * 使用方式：
 *   1) 定义 soft_i2c_bus_t 变量，填入 SCL/SDA 端口与引脚，设置 speed_hz（可选）
 *   2) 调用 soft_i2c_init()
 *   3) 调用 soft_i2c_read()/soft_i2c_write() 或底层字节/起止条件接口进行访问
 */

#ifndef SOFT_I2C_H__
#define SOFT_I2C_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

#include "soft_i2c_port.h"

/* 可选编译期配置 */
#ifndef SOFT_I2C_DEFAULT_SPEED_HZ
#define SOFT_I2C_DEFAULT_SPEED_HZ 100000u  /* 缺省 100 kHz */
#endif

#ifndef SOFT_I2C_STRETCH_TIMEOUT_US
#define SOFT_I2C_STRETCH_TIMEOUT_US 2000u  /* SCL 拉伸等待超时（微秒） */
#endif

/* 软件 I2C 总线实例结构体 */
typedef struct soft_i2c_bus
{
    soft_gpio_port_t *scl_port;
    uint32_t          scl_pin;       /* 平台相关引脚掩码（如 STM32 的 GPIO_PIN_x） */
    soft_gpio_port_t *sda_port;
    uint32_t          sda_pin;

    uint32_t          speed_hz;      /* 目标 I2C 速率（Hz） */
    uint16_t          t_half_us;     /* 半周期延时（微秒，内部根据速率推导） */
    uint16_t          stretch_timeout_us; /* SCL 高电平等待（拉伸）超时（微秒） */

    /* 预留字段 */
    void             *user_data;
} soft_i2c_bus_t;

/* ---------------- 核心 API ---------------- */

void soft_i2c_init(soft_i2c_bus_t *bus);

int  soft_i2c_start(soft_i2c_bus_t *bus);
void soft_i2c_stop(soft_i2c_bus_t *bus);

int  soft_i2c_write_byte(soft_i2c_bus_t *bus, uint8_t byte);
int  soft_i2c_read_byte(soft_i2c_bus_t *bus, uint8_t *byte, bool ack);

int  soft_i2c_write(soft_i2c_bus_t *bus, uint8_t addr7, const uint8_t *buf, size_t len);
int  soft_i2c_read(soft_i2c_bus_t *bus, uint8_t addr7, uint8_t *buf, size_t len);

/* 扫描 I2C 总线，使用 printf 以“表格”方式打印 0x00~0x7F 的地址分布 */
void soft_i2c_scan(soft_i2c_bus_t *bus);

/* ---------------- 内联帮助函数：GPIO 控制与延时 ---------------- */

/* 半周期延时（微秒） */
static inline void soft_i2c_delay_half(const soft_i2c_bus_t *bus)
{
    soft_port_udelay(bus->t_half_us);
}

/* 开漏语义：
 * - 拉低：输出 0
 * - 释放：输出 1（由上拉电阻拉高）
 */
static inline void soft_i2c_scl_low(soft_i2c_bus_t *bus)
{
    soft_port_gpio_clr(bus->scl_port, bus->scl_pin);
}

static inline void soft_i2c_scl_release(soft_i2c_bus_t *bus)
{
    soft_port_gpio_set(bus->scl_port, bus->scl_pin);
}

static inline void soft_i2c_sda_low(soft_i2c_bus_t *bus)
{
    soft_port_gpio_clr(bus->sda_port, bus->sda_pin);
}

static inline void soft_i2c_sda_release(soft_i2c_bus_t *bus)
{
    soft_port_gpio_set(bus->sda_port, bus->sda_pin);
}

static inline int soft_i2c_read_scl(soft_i2c_bus_t *bus)
{
    return soft_port_gpio_read(bus->scl_port, bus->scl_pin);
}

static inline int soft_i2c_read_sda(soft_i2c_bus_t *bus)
{
    return soft_port_gpio_read(bus->sda_port, bus->sda_pin);
}

#ifdef __cplusplus
}
#endif

#endif /* SOFT_I2C_H__ */