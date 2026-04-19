#ifndef __ST7789_H__
#define __ST7789_H__

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "st7789_port.h"
#include "st7789_font.h"

/*================ 配置宏（可在编译器或外部配置覆盖） ================*/
#ifndef ST7789_CFG_DEFAULT_WIDTH
#define ST7789_CFG_DEFAULT_WIDTH   240
#endif

#ifndef ST7789_CFG_DEFAULT_HEIGHT
#define ST7789_CFG_DEFAULT_HEIGHT  320
#endif

/* 针对某些屏幕在横屏时需要的偏移（若无可设为 0） */
#ifndef ST7789_CFG_X_OFFSET
#define ST7789_CFG_X_OFFSET        0
#endif

#ifndef ST7789_CFG_Y_OFFSET
#define ST7789_CFG_Y_OFFSET        0
#endif

/*================ 方向枚举 ================*/
typedef enum
{
    ST7789_DIR_PORTRAIT_0   = 0,  /* 竖屏 0° */
    ST7789_DIR_LANDSCAPE_90 = 1,  /* 横屏 90° */
    ST7789_DIR_PORTRAIT_180 = 2,  /* 竖屏 180° */
    ST7789_DIR_LANDSCAPE_270= 3   /* 横屏 270° */
} st7789_dir_t;

/*================ 颜色定义 (RGB565) 加前缀 ST7789_COLOR_ ================*/
#define ST7789_COLOR_WHITE     0xFFFF
#define ST7789_COLOR_BLACK     0x0000
#define ST7789_COLOR_BLUE      0x001F
#define ST7789_COLOR_BRED      0xF81F      // 蓝红（洋红/品红），与MAGENTA相同
#define ST7789_COLOR_GRED      0xFFE0      // 黄（Green+Red），与YELLOW相同
#define ST7789_COLOR_GBLUE     0x07FF      // 绿蓝（青色），与CYAN相同
#define ST7789_COLOR_RED       0xF800
#define ST7789_COLOR_MAGENTA   0xF81F      // 品红，与BRED相同
#define ST7789_COLOR_GREEN     0x07E0
#define ST7789_COLOR_CYAN      0x7FFF      // 标准青色
#define ST7789_COLOR_YELLOW    0xFFE0      // 标准黄色，与GRED相同
#define ST7789_COLOR_GRAY      0x8430
#define ST7789_COLOR_ORANGE    0xFD20
#define ST7789_COLOR_PINK      0xF8B2
#define ST7789_COLOR_PURPLE    0x8010
#define ST7789_COLOR_BROWN     0xA145
#define ST7789_COLOR_DARKBLUE  0x01CF
#define ST7789_COLOR_LIGHTBLUE 0x7D7C

#ifdef __cplusplus
extern "C" {
#endif

/*================ 对外 API ================*/
void st7789_init(void);
void st7789_set_direction(st7789_dir_t dir);

uint16_t st7789_width(void);
uint16_t st7789_height(void);

void st7789_clear(uint16_t color);
void st7789_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color);
void st7789_draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void st7789_draw_bitmap565(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *data);

/* 字符/字符串/数字显示（5x7 基础字体，可放大倍数） */
void st7789_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t scale);
void st7789_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t scale);
void st7789_draw_int(uint16_t x, uint16_t y, int32_t value, uint16_t color, uint16_t bg, uint8_t scale);
void st7789_draw_uint(uint16_t x, uint16_t y, uint32_t value, uint16_t color, uint16_t bg, uint8_t scale);
void st7789_draw_hex(uint16_t x, uint16_t y, uint32_t value, uint8_t width, uint16_t color, uint16_t bg, uint8_t scale);

/* 进度条 */
void st7789_draw_progress_bar(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                              uint8_t percent, uint16_t color_fg, uint16_t color_bg, uint16_t frame_color);

/* 简单水平滚动文本（软件重绘方式） */
void st7789_scroll_text_h(const char *str, uint16_t area_x, uint16_t area_y,
                          uint16_t area_w, uint16_t area_h,
                          uint16_t text_color, uint16_t bg_color, uint8_t scale,
                          uint16_t step_delay_ms);

/* 演示函数 */
void st7789_demo(void);

#if defined(ST7789_USING_DMA)
typedef void (*st7789_dma_done_cb_t)(bool success, void *user_data);

bool st7789_draw_bitmap565_async(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                                 const uint8_t *data,
                                 st7789_dma_done_cb_t done_cb,
                                 void *user_data);

bool st7789_is_dma_busy(void);
void st7789_wait_for_dma(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __ST7789_H__ */