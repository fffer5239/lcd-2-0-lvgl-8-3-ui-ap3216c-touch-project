/**
 * @file bsp_backlight.c
 * @author LCKFB-YZH
 * @brief STM32F407 TIM10 PWM Backlight Driver Implementation
 * @date 2025-12-06
 */

#include "bsp_backlight.h"

// 全局定时器句柄
extern TIM_HandleTypeDef htim10;

// 当前亮度记录
static uint8_t current_brightness = BACKLIGHT_DEFAULT_BRIGHTNESS;

/**
 * @brief 初始化背光 PWM
 *        Pin: PB8 (TIM10_CH1)
 *        Freq: 10kHz
 */
void back_light_init(void)
{
    TIM_OC_InitTypeDef sConfigOC = {0};
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    // 1. 开启时钟
    __HAL_RCC_TIM10_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    // 2. 配置 GPIO (PB8 -> AF3_TIM10)
    GPIO_InitStruct.Pin = GPIO_PIN_8;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;       // 复用推挽
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH; // 高速，利于PWM波形
    GPIO_InitStruct.Alternate = GPIO_AF3_TIM10;   // 映射到 TIM10
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    // 3. 配置定时器基础参数
    // Clock = 168MHz, Target = 10kHz
    // PSC = 167, ARR = 99
    htim10.Instance = TIM10;
    htim10.Init.Prescaler = 167;
    htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim10.Init.Period = 99; 
    htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE; // 使能ARR预装载，防止波形突变
    if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
    {
        // Error Handler
    }

    // 4. 配置 PWM 通道
    if (HAL_TIM_PWM_Init(&htim10) != HAL_OK)
    {
        // Error Handler
    }

    sConfigOC.OCMode = TIM_OCMODE_PWM1; // PWM模式1：CNT < CCR 时有效
    sConfigOC.Pulse = current_brightness; // 初始占空比
    sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW; // 有效电平为低电平
    sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
    if (HAL_TIM_PWM_ConfigChannel(&htim10, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
    {
        // Error Handler
    }

    // 5. 启动 PWM
    HAL_TIM_PWM_Start(&htim10, TIM_CHANNEL_1);
}

/**
 * @brief 设置屏幕亮度
 * @param brightness 0-100
 */
void back_light_set(uint8_t brightness)
{
    if (brightness > BACKLIGHT_MAX_BRIGHTNESS) brightness = BACKLIGHT_MAX_BRIGHTNESS;
    
    current_brightness = brightness;
    
    // 修改 CCR1 寄存器改变占空比
    __HAL_TIM_SET_COMPARE(&htim10, TIM_CHANNEL_1, brightness);
}

/**
 * @brief 获取当前亮度
 */
uint8_t back_light_get(void)
{
    return current_brightness;
}