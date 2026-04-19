/*
 * soft_i2c - 端口适配层（实现）
 *
 * 缺省：STM32F4 HAL。其它国产 MCU（GD32/AT32 等）亦可参考下方模板快速适配。
 */
#include "soft_i2c_port.h"
#include "soft_i2c.h"   /* 引入 soft_i2c_bus_t 以定义默认实例 */
#include <string.h>

/* ---------------- STM32 HAL 实现 ---------------- */
#if SOFT_I2C_PLATFORM_STM32_HAL

/* 根据端口指针开启 GPIO 时钟（针对 STM32F4 AHB1 总线） */
void soft_port_gpio_clock_enable(soft_gpio_port_t *port)
{
#ifdef GPIOA
    if (port == GPIOA) { __HAL_RCC_GPIOA_CLK_ENABLE(); return; }
#endif
#ifdef GPIOB
    if (port == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); return; }
#endif
#ifdef GPIOC
    if (port == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); return; }
#endif
#ifdef GPIOD
    if (port == GPIOD) { __HAL_RCC_GPIOD_CLK_ENABLE(); return; }
#endif
#ifdef GPIOE
    if (port == GPIOE) { __HAL_RCC_GPIOE_CLK_ENABLE(); return; }
#endif
#ifdef GPIOF
    if (port == GPIOF) { __HAL_RCC_GPIOF_CLK_ENABLE(); return; }
#endif
#ifdef GPIOG
    if (port == GPIOG) { __HAL_RCC_GPIOG_CLK_ENABLE(); return; }
#endif
#ifdef GPIOH
    if (port == GPIOH) { __HAL_RCC_GPIOH_CLK_ENABLE(); return; }
#endif
#ifdef GPIOI
    if (port == GPIOI) { __HAL_RCC_GPIOI_CLK_ENABLE(); return; }
#endif
}

/* 将 GPIO 配置为开漏输出（可选上拉），并设置为最高速 */
void soft_port_gpio_config_od(soft_gpio_port_t *port, uint32_t pin_mask, int pullup)
{
    GPIO_InitTypeDef init = {0};
    init.Pin   = (uint16_t)pin_mask;
    init.Mode  = GPIO_MODE_OUTPUT_OD;
    init.Pull  = pullup ? GPIO_PULLUP : GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    HAL_GPIO_Init(port, &init);

    /* 释放为高（由外部上拉电阻拉高） */
    soft_port_gpio_set(port, pin_mask);
}

/* 微秒级延时：优先使用 DWT CYCCNT，若不可用则使用粗略忙等 */
void soft_port_udelay(uint32_t us)
{
    /* DWT 方式（精确） */
    static uint8_t dwt_ok = 0;
    if (dwt_ok == 0)
    {
        /* 使能 TRC */
        CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
#if defined(DWT_LAR)
        /* 解锁 DWT（某些内核需要） */
        DWT->LAR = 0xC5ACCE55;
#endif
        DWT->CYCCNT = 0;
        DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
        dwt_ok = (DWT->CTRL & DWT_CTRL_CYCCNTENA_Msk) ? 1 : 2; /* 1=OK, 2=Fail */
    }
    if (dwt_ok == 1)
    {
        uint32_t cycles = (SystemCoreClock / 1000000u) * us;
        uint32_t start = DWT->CYCCNT;
        while ((DWT->CYCCNT - start) < cycles) { __NOP(); }
        return;
    }

    /* 回退：粗略忙等循环（频率变化时精度会受影响） */
    uint32_t loops_per_us = (SystemCoreClock / 6000000u);
    if (loops_per_us == 0) loops_per_us = 1;
    while (us--)
    {
        for (volatile uint32_t i = 0; i < loops_per_us; ++i)
        {
            __NOP(); __NOP(); __NOP();
        }
    }
}

/* ---------------- 默认 I2C 总线实例 ----------------
 * 缺省引脚：
 *  - I2C1: SCL PB6, SDA PB7
 *  - I2C2: SCL PD10, SDA PE13
 * 缺省速率 100 kHz；如需 400 kHz，请在 soft_i2c_init() 之前修改 speed_hz 字段。
 */
soft_i2c_bus_t soft_i2c_bus1 = {
    .scl_port = GPIOB, .scl_pin = GPIO_PIN_6,
    .sda_port = GPIOB, .sda_pin = GPIO_PIN_7,
    .speed_hz = SOFT_I2C_DEFAULT_SPEED_HZ,
    .t_half_us = 0,
    .stretch_timeout_us = SOFT_I2C_STRETCH_TIMEOUT_US,
    .user_data = NULL,
};

soft_i2c_bus_t soft_i2c_bus2 = {
    .scl_port = GPIOD, .scl_pin = GPIO_PIN_10,
    .sda_port = GPIOE, .sda_pin = GPIO_PIN_13,
    .speed_hz = SOFT_I2C_DEFAULT_SPEED_HZ,
    .t_half_us = 0,
    .stretch_timeout_us = SOFT_I2C_STRETCH_TIMEOUT_US,
    .user_data = NULL,
};

#else
/* ---------------- 其他平台模板 ---------------- */

void soft_port_gpio_clock_enable(soft_gpio_port_t *port) { (void)port; }
void soft_port_gpio_config_od(soft_gpio_port_t *port, uint32_t pin_mask, int pullup) { (void)port; (void)pin_mask; (void)pullup; }
void soft_port_udelay(uint32_t us) { (void)us; }

/* 其他平台可视需要给出默认实例或留空由用户定义 */
struct soft_i2c_bus soft_i2c_bus1 = {0};
struct soft_i2c_bus soft_i2c_bus2 = {0};

#endif /* SOFT_I2C_PLATFORM_STM32_HAL */