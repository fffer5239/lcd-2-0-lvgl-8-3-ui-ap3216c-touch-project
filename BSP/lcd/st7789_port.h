#ifndef __ST7789_PORT_H__
#define __ST7789_PORT_H__

/*
 * 硬件端口适配层：
 *  - 只在这里放与具体 MCU / HAL / RT-Thread 相关的定义
 *  - 需用户根据实际硬件修改 GPIO/SPI 句柄
 */

#include <stdint.h>
#include <stddef.h>

/*================= 可选配置 ================*/
/* 定义此宏以启用 DMA 发送（需在 st7789_port.c 中实现对应 DMA 函数） */
#define ST7789_USING_DMA

/* 如果使用 RT-Thread 可自动使用 rt_thread_mdelay */
#if defined(RT_USING_RTTHREAD)
#include <rtthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/*================= 必须实现的端口接口（在 st7789_port.c） ================*/

/* SPI 普通发送 */
void st7789_port_spi_tx(const uint8_t *data, size_t len);

/* SPI DMA 发送（若未启用 DMA，可在实现里直接调用普通发送或留空） */
void st7789_port_spi_tx_dma(const uint8_t *data, size_t len);

/* 片选/数据命令/复位控制 */
void st7789_port_select(void);
void st7789_port_unselect(void);
void st7789_port_dc_set(void);
void st7789_port_dc_clr(void);
void st7789_port_reset_set(void);
void st7789_port_reset_clr(void);

/* 背光控制（高低电平视模块而定，此处采用宏配置逻辑） */
void st7789_port_backlight_on(void);
void st7789_port_backlight_off(void);

/* 毫秒延时 */
static inline void st7789_port_delay_ms(uint32_t ms)
{
#if defined(RT_USING_RTTHREAD)
    rt_thread_mdelay(ms);
#else
    extern void st7789_user_delay_ms(uint32_t ms); /* 如果没有 HAL，可让用户自行实现 */
    st7789_user_delay_ms(ms);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* __ST7789_PORT_H__ */