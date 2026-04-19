/**
 * @file bsp_backlight.h
 * @author LCKFB-YZH
 * @brief STM32F407 TIM10 PWM Backlight Driver
 * @date 2025-12-06
 */

#ifndef __BSP_BACKLIGHT_H__
#define __BSP_BACKLIGHT_H__

#include "stm32f4xx_hal.h"

// 亮度范围 0-100
#define BACKLIGHT_MAX_BRIGHTNESS 100
#define BACKLIGHT_DEFAULT_BRIGHTNESS 50

/**
 * @brief 初始化背光 PWM (TIM10_CH1 / PB8)
 */
void back_light_init(void);

/**
 * @brief 设置屏幕亮度
 * @param brightness 0-100
 */
void back_light_set(uint8_t brightness);

/**
 * @brief 获取当前亮度值
 * @return uint8_t 0-100
 */
uint8_t back_light_get(void);

#endif /* __BSP_BACKLIGHT_H__ */