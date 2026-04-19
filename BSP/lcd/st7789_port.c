#include "st7789_port.h"
#include "stm32f4xx_hal.h"
#include "bsp_backlight.h"

// 引用外部定义的 SPI 和 TIM 句柄
// 请确保 main.c 或其他地方定义了这些句柄
extern SPI_HandleTypeDef hspi1;
extern TIM_HandleTypeDef htim10; 

/*---------- 引脚定义 ----------*/
#define ST7789_DC_GPIO_Port    GPIOD
#define ST7789_DC_Pin          GPIO_PIN_14
#define ST7789_CS_GPIO_Port    GPIOE
#define ST7789_CS_Pin          GPIO_PIN_14
#define ST7789_RST_GPIO_Port   GPIOE
#define ST7789_RST_Pin         GPIO_PIN_1

/* PWM 配置参数 */
#define PWM_PERIOD_VALUE       1000 

/*================= 基础 GPIO 操作封装 =================*/
void st7789_port_select(void)
{
    HAL_GPIO_WritePin(ST7789_CS_GPIO_Port, ST7789_CS_Pin, GPIO_PIN_RESET);
}

void st7789_port_unselect(void)
{
    HAL_GPIO_WritePin(ST7789_CS_GPIO_Port, ST7789_CS_Pin, GPIO_PIN_SET);
}

void st7789_port_dc_set(void)
{
    HAL_GPIO_WritePin(ST7789_DC_GPIO_Port, ST7789_DC_Pin, GPIO_PIN_SET);
}

void st7789_port_dc_clr(void)
{
    HAL_GPIO_WritePin(ST7789_DC_GPIO_Port, ST7789_DC_Pin, GPIO_PIN_RESET);
}

void st7789_port_reset_set(void)
{
    HAL_GPIO_WritePin(ST7789_RST_GPIO_Port, ST7789_RST_Pin, GPIO_PIN_SET);
}

void st7789_port_reset_clr(void)
{
    HAL_GPIO_WritePin(ST7789_RST_GPIO_Port, ST7789_RST_Pin, GPIO_PIN_RESET);
}

void st7789_port_backlight_on(void)
{
    back_light_set(100); // 全亮
}

void st7789_port_backlight_off(void)
{
    back_light_set(0);
}

void st7789_user_delay_ms(uint32_t ms)
{
    HAL_Delay(ms);
}

void st7789_port_spi_tx(const uint8_t *data, size_t len)
{
    HAL_SPI_Transmit(&hspi1, (uint8_t*)data, (uint16_t)len, HAL_MAX_DELAY);
}

void st7789_port_spi_tx_dma(const uint8_t *data, size_t len)
{
#ifdef ST7789_USING_DMA
    if (len == 0U) return;
    HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)data, (uint16_t)len);
    if (status != HAL_OK) {
        HAL_SPI_Transmit(&hspi1, (uint8_t*)data, (uint16_t)len, HAL_MAX_DELAY);
    }
#else
    st7789_port_spi_tx(data, len);
#endif
}