/*
 * soft_i2c - 端口适配层（头文件）
 *
 * 职责：
 *  - 定义 GPIO 端口类型与快速读写的内联函数
 *  - 提供端口时钟使能与开漏配置函数
 *  - 提供微秒级延时函数
 *
 * 缺省实现面向 STM32F4 HAL，亦可很方便地适配 GD32/AT32 等平台：
 *   - 修改/实现本文件与 soft_i2c_port.c 中对应的端口相关函数
 */
#ifndef SOFT_I2C_PORT_H__
#define SOFT_I2C_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif

/* 平台选择：默认使用 STM32 HAL。若移植到其他平台，请修改为 0 并按模板实现 */
#ifndef SOFT_I2C_PLATFORM_STM32_HAL
#define SOFT_I2C_PLATFORM_STM32_HAL 1
#endif

#include <stdint.h>

#if SOFT_I2C_PLATFORM_STM32_HAL

/* -------- STM32F4 HAL 相关 -------- */
#include "stm32f4xx_hal.h"

/* 将平台 GPIO 端口类型别名为 soft_gpio_port_t */
typedef GPIO_TypeDef soft_gpio_port_t;

/* 以内联方式提供快速置位/复位与读入（使用 BSRR/IDR 寄存器） */
static inline void soft_port_gpio_set(soft_gpio_port_t *port, uint32_t pin_mask)
{
    port->BSRR = (uint32_t)pin_mask;
}
static inline void soft_port_gpio_clr(soft_gpio_port_t *port, uint32_t pin_mask)
{
    port->BSRR = (uint32_t)(pin_mask << 16u);
}
static inline int soft_port_gpio_read(soft_gpio_port_t *port, uint32_t pin_mask)
{
    return ((port->IDR & pin_mask) != 0) ? 1 : 0;
}

/* 根据端口指针使能 GPIO 时钟（STM32F4 位于 AHB1） */
void soft_port_gpio_clock_enable(soft_gpio_port_t *port);

/* 将引脚配置为开漏输出（可选上拉）并设定高速 */
void soft_port_gpio_config_od(soft_gpio_port_t *port, uint32_t pin_mask, int pullup);

/* 微秒延时（忙等方式；优先使用 DWT CYCCNT；失败则回退到粗略循环） */
void soft_port_udelay(uint32_t us);

/* -------- 默认 I2C 总线实例（由 .c 中给出定义） -------- */
struct soft_i2c_bus; /* 前置声明，避免循环包含 */
extern struct soft_i2c_bus soft_i2c_bus1; /* 默认：PB6/PB7 */
extern struct soft_i2c_bus soft_i2c_bus2; /* 默认：PD10/PE13 */

#else
/* -------- 其他平台适配模板（GD32/AT32/…） --------
 * 需提供：
 *  - soft_gpio_port_t
 *  - soft_port_gpio_set/soft_port_gpio_clr/soft_port_gpio_read（内联）
 *  - soft_port_gpio_clock_enable
 *  - soft_port_gpio_config_od
 *  - soft_port_udelay
 */

typedef void soft_gpio_port_t; /* 按平台替换 */

static inline void soft_port_gpio_set(soft_gpio_port_t *port, uint32_t pin_mask) { (void)port; (void)pin_mask; }
static inline void soft_port_gpio_clr(soft_gpio_port_t *port, uint32_t pin_mask) { (void)port; (void)pin_mask; }
static inline int  soft_port_gpio_read(soft_gpio_port_t *port, uint32_t pin_mask) { (void)port; (void)pin_mask; return 1; }

void soft_port_gpio_clock_enable(soft_gpio_port_t *port);
void soft_port_gpio_config_od(soft_gpio_port_t *port, uint32_t pin_mask, int pullup);
void soft_port_udelay(uint32_t us);

extern struct soft_i2c_bus soft_i2c_bus1;
extern struct soft_i2c_bus soft_i2c_bus2;

#endif /* SOFT_I2C_PLATFORM_STM32_HAL */

#ifdef __cplusplus
}
#endif

#endif /* SOFT_I2C_PORT_H__ */