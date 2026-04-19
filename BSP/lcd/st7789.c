#include "st7789.h"
#include "st7789_port.h"
#include "bsp_backlight.h"

#if defined(ST7789_USING_DMA)
#include "stm32f4xx_hal.h"
extern SPI_HandleTypeDef hspi1;

typedef struct
{
    st7789_dma_done_cb_t cb;
    void *user_data;
    bool auto_unselect;
} st7789_dma_context_t;

static volatile bool s_spi_dma_busy = false;
static st7789_dma_context_t s_spi_dma_ctx = {0};

static HAL_StatusTypeDef st7789_spi_dma_start(const uint8_t *data, size_t len,
                                              st7789_dma_done_cb_t cb,
                                              void *user_data,
                                              bool auto_unselect);
static void st7789_spi_dma_wait_idle(void);
static inline bool st7789_spi_dma_is_busy_internal(void)
{
    return s_spi_dma_busy;
}

static void st7789_dma_finish(bool success);
#endif

/*=================== 内部宏与静态变量 ===================*/
#define ST7789_CMD_COL_ADDR_SET            0x2A
#define ST7789_CMD_ROW_ADDR_SET            0x2B
#define ST7789_CMD_MEMORY_WRITE            0x2C
#define ST7789_CMD_MEMORY_DATA_ACCESS_CTRL 0x36
#define ST7789_CMD_INTERFACE_PIXEL_FORMAT  0x3A
#define ST7789_CMD_SLEEP_OUT               0x11
#define ST7789_CMD_DISPLAY_ON              0x29

/* 可扩展：垂直滚动定义 VSCRDEF(0x33), VSCAD(0x37) */

typedef struct
{
    uint16_t width;
    uint16_t height;
    st7789_dir_t dir;
} st7789_dev_t;

static st7789_dev_t s_lcd =
{
    .width  = ST7789_CFG_DEFAULT_WIDTH,
    .height = ST7789_CFG_DEFAULT_HEIGHT,
    .dir    = ST7789_DIR_PORTRAIT_0
};

/*=================== 辅助宏：颜色字节交换 ===================*/
/*
 * 核心修复点：统一颜色字节序处理
 * 如果 MCU 是小端模式，直接发送 uint16_t 会导致先发低字节后发高字节。
 * 而 ST7789 此时需要先收高字节。
 * 该宏将 0xRRGGBB (RGB565) 的高低字节翻转
 */
#define SWAP_BYTES(color) ((uint16_t)(((color) >> 8) | ((color) << 8)))

#if defined(ST7789_USING_DMA)
static HAL_StatusTypeDef st7789_spi_dma_start(const uint8_t *data, size_t len,
                                              st7789_dma_done_cb_t cb,
                                              void *user_data,
                                              bool auto_unselect)
{
    if (len == 0U)
    {
        if (auto_unselect)
        {
            st7789_port_unselect();
        }
        if (cb)
        {
            cb(true, user_data);
        }
        return HAL_OK;
    }

    if (s_spi_dma_busy)
    {
        return HAL_BUSY;
    }

    s_spi_dma_ctx.cb = cb;
    s_spi_dma_ctx.user_data = user_data;
    s_spi_dma_ctx.auto_unselect = auto_unselect;
    s_spi_dma_busy = true;

    HAL_StatusTypeDef status = HAL_SPI_Transmit_DMA(&hspi1, (uint8_t *)data, (uint16_t)len);
    if (status != HAL_OK)
    {
        s_spi_dma_ctx.cb = NULL;
        s_spi_dma_ctx.user_data = NULL;
        s_spi_dma_ctx.auto_unselect = false;
        s_spi_dma_busy = false;
    }
    return status;
}

static void st7789_spi_dma_wait_idle(void)
{
    while (s_spi_dma_busy)
    {
        __WFE();
    }
}

static void st7789_dma_finish(bool success)
{
    st7789_dma_done_cb_t cb = s_spi_dma_ctx.cb;
    void *user_data = s_spi_dma_ctx.user_data;
    bool auto_unselect = s_spi_dma_ctx.auto_unselect;

    s_spi_dma_ctx.cb = NULL;
    s_spi_dma_ctx.user_data = NULL;
    s_spi_dma_ctx.auto_unselect = false;
    s_spi_dma_busy = false;

    __SEV();

    if (auto_unselect)
    {
        st7789_port_unselect();
    }

    if (cb)
    {
        cb(success, user_data);
    }
}

void HAL_SPI_TxCpltCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1 && s_spi_dma_busy)
    {
        st7789_dma_finish(true);
    }
}

void HAL_SPI_ErrorCallback(SPI_HandleTypeDef *hspi)
{
    if (hspi->Instance == SPI1 && s_spi_dma_busy)
    {
        st7789_dma_finish(false);
    }
}

static void st7789_spi_tx_blocking(const uint8_t *data, size_t len)
{
    if (len == 0U)
    {
        return;
    }

    HAL_StatusTypeDef status;
    do
    {
        status = st7789_spi_dma_start(data, len, NULL, NULL, false);
        if (status == HAL_BUSY)
        {
            st7789_spi_dma_wait_idle();
        }
    }
    while (status == HAL_BUSY);

    if (status == HAL_OK)
    {
        st7789_spi_dma_wait_idle();
        return;
    }

    st7789_port_spi_tx(data, len);
}

static inline bool st7789_spi_dma_is_busy(void)
{
    return st7789_spi_dma_is_busy_internal();
}

void st7789_wait_for_dma(void)
{
    st7789_spi_dma_wait_idle();
}

bool st7789_is_dma_busy(void)
{
    return st7789_spi_dma_is_busy();
}
#else
static inline void st7789_spi_tx_blocking(const uint8_t *data, size_t len)
{
    if (len == 0U)
    {
        return;
    }
    st7789_port_spi_tx(data, len);
}
#endif

/*=================== 底层写命令/数据 ===================*/
static void st7789_write_cmd(uint8_t cmd)
{
    st7789_port_select();
    st7789_port_dc_clr();
    st7789_port_spi_tx(&cmd, 1);
    st7789_port_dc_set();
    st7789_port_unselect();
}

static void st7789_write_data8(uint8_t data)
{
    st7789_port_select();
    st7789_port_dc_set();
    st7789_port_spi_tx(&data, 1);
    st7789_port_unselect();
}

static void st7789_write_data16(uint16_t data)
{
    uint8_t buf[2] = { (uint8_t)(data >> 8), (uint8_t)(data & 0xFF) };
    st7789_port_select();
    st7789_port_dc_set();
    st7789_port_spi_tx(buf, 2);
    st7789_port_unselect();
}

static void st7789_write_datas(const uint8_t *data, size_t len)
{
    if (len == 0U)
    {
        return;
    }

    st7789_port_select();
    st7789_port_dc_set();
    st7789_spi_tx_blocking(data, len);
    st7789_port_unselect();
}

/*=================== 复位 ===================*/
static void st7789_reset(void)
{
    st7789_port_reset_clr();
    st7789_port_delay_ms(50);
    st7789_port_reset_set();
    st7789_port_delay_ms(120);
}

/*=================== 方向设置 ===================*/
void st7789_set_direction(st7789_dir_t dir)
{
    s_lcd.dir = dir;
    st7789_write_cmd(ST7789_CMD_MEMORY_DATA_ACCESS_CTRL);
    switch(dir)
    {
    case ST7789_DIR_PORTRAIT_0:
        s_lcd.width  = ST7789_CFG_DEFAULT_WIDTH;
        s_lcd.height = ST7789_CFG_DEFAULT_HEIGHT;
        st7789_write_data8(0x00);
        break;
    case ST7789_DIR_LANDSCAPE_90:
        s_lcd.width  = ST7789_CFG_DEFAULT_HEIGHT;
        s_lcd.height = ST7789_CFG_DEFAULT_WIDTH;
        st7789_write_data8(0xC0);
        break;
    case ST7789_DIR_PORTRAIT_180:
        s_lcd.width  = ST7789_CFG_DEFAULT_WIDTH;
        s_lcd.height = ST7789_CFG_DEFAULT_HEIGHT;
        st7789_write_data8(0x70);
        break;
    case ST7789_DIR_LANDSCAPE_270:
        s_lcd.width  = ST7789_CFG_DEFAULT_HEIGHT;
        s_lcd.height = ST7789_CFG_DEFAULT_WIDTH;
        st7789_write_data8(0xA0);
        break;
    default:
        break;
    }
}

/*=================== 设置窗口 ===================*/
static void st7789_set_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
    /* 根据方向可扩展偏移，如果特定屏需要列/行偏移 */
    uint16_t xs = x0 + ST7789_CFG_X_OFFSET;
    uint16_t xe = x1 + ST7789_CFG_X_OFFSET;
    uint16_t ys = y0 + ST7789_CFG_Y_OFFSET;
    uint16_t ye = y1 + ST7789_CFG_Y_OFFSET;

    st7789_write_cmd(ST7789_CMD_COL_ADDR_SET);
    st7789_write_data8(xs >> 8); st7789_write_data8(xs & 0xFF);
    st7789_write_data8(xe >> 8); st7789_write_data8(xe & 0xFF);

    st7789_write_cmd(ST7789_CMD_ROW_ADDR_SET);
    st7789_write_data8(ys >> 8); st7789_write_data8(ys & 0xFF);
    st7789_write_data8(ye >> 8); st7789_write_data8(ye & 0xFF);

    st7789_write_cmd(ST7789_CMD_MEMORY_WRITE);
}

uint16_t st7789_width(void)
{
    return s_lcd.width;
}

uint16_t st7789_height(void)
{
    return s_lcd.height;
}

/*=================== 填充矩形 & 清屏 ===================*/
void st7789_fill_rect(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
    if (x0 > x1 || y0 > y1) return;
    if (x1 >= s_lcd.width)  x1 = s_lcd.width -1;
    if (y1 >= s_lcd.height) y1 = s_lcd.height-1;

    st7789_set_window(x0, y0, x1, y1);

    uint32_t pixels = (uint32_t)(x1 - x0 + 1) * (uint32_t)(y1 - y0 + 1);
    uint16_t buffer[512]; // 1KB 缓冲区
    uint32_t block_size = 512;

    // 之前这里是正确的：手动交换了字节序
    uint16_t color_swapped = SWAP_BYTES(color);

    for (int i = 0; i < block_size; i++)
    {
        buffer[i] = color_swapped;
    }

    st7789_port_select();
    st7789_port_dc_set();

    while (pixels)
    {
        uint32_t batch = (pixels > 64U) ? 64U : pixels;
        st7789_spi_tx_blocking((const uint8_t *)buffer, batch * 2U);
        pixels -= batch;
    }

    st7789_port_unselect();
}

void st7789_clear(uint16_t color)
{
    st7789_fill_rect(0, 0, s_lcd.width-1, s_lcd.height-1, color);
}

/*=================== 像素/位图 ===================*/
void st7789_draw_pixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= s_lcd.width || y >= s_lcd.height) return;
    st7789_set_window(x, y, x, y);
    st7789_write_data16(color);
}

void st7789_draw_bitmap565(uint16_t x, uint16_t y, uint16_t w, uint16_t h, const uint8_t *data)
{
    if (x + w > s_lcd.width || y + h > s_lcd.height) return;
    st7789_set_window(x, y, x + w - 1, y + h - 1);
    st7789_write_datas(data, (size_t)w * h * 2);
}

#if defined(ST7789_USING_DMA)
bool st7789_draw_bitmap565_async(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                                 const uint8_t *data,
                                 st7789_dma_done_cb_t done_cb,
                                 void *user_data)
{
    if (x + w > s_lcd.width || y + h > s_lcd.height)
    {
        return false;
    }

    if (st7789_spi_dma_is_busy_internal())
    {
        return false;
    }

    st7789_set_window(x, y, x + w - 1U, y + h - 1U);
    st7789_port_select();
    st7789_port_dc_set();

    size_t len = (size_t)w * (size_t)h * 2U;
    HAL_StatusTypeDef status = st7789_spi_dma_start(data, len, done_cb, user_data, true);
    if (status != HAL_OK)
    {
        st7789_port_unselect();
        return false;
    }

    return true;
}
#endif

/*=================== 字符绘制 ===================*/
void st7789_draw_char(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg, uint8_t scale)
{
    if (c < 0x20 || c > 0x7E) c = '?';
    const uint8_t *glyph = st7789_font5x7[c - 0x20];

    uint16_t color_swapped = SWAP_BYTES(color);
    uint16_t bg_swapped    = SWAP_BYTES(bg);

    uint8_t col, row;
    for (col = 0; col < 5; col++)
    {
        uint8_t line = glyph[col];
        for (row = 0; row < 7; row++)
        {
            uint16_t draw_color = (line & 0x01) ? color_swapped : bg_swapped;
            if (draw_color != bg_swapped)
            {
                /* 放大缩放 */
                for (uint8_t dx=0; dx<scale; dx++)
                    for (uint8_t dy=0; dy<scale; dy++)
                        st7789_draw_pixel(x + col*scale + dx, y + row*scale + dy, draw_color);
            }
            else if (bg != 0xFFFF) /* 如果需要背景也填充（可以用特殊值表示透明） */
            {
                for (uint8_t dx=0; dx<scale; dx++)
                    for (uint8_t dy=0; dy<scale; dy++)
                        st7789_draw_pixel(x + col*scale + dx, y + row*scale + dy, draw_color);
            }
            line >>= 1;
        }
    }
    /* 1 列间隔 */
    if (bg != 0xFFFF)
    {
        for (uint8_t rowp=0; rowp<7*scale; rowp++)
            for (uint8_t dx=0; dx<scale; dx++)
                st7789_draw_pixel(x + 5*scale + dx, y + rowp, bg);
    }
}

void st7789_draw_string(uint16_t x, uint16_t y, const char *str, uint16_t color, uint16_t bg, uint8_t scale)
{
    uint16_t cursor_x = x;
    while (*str)
    {
        if (*str == '\n')
        {
            cursor_x = x;
            y += (7 * scale + 2);
            str++;
            continue;
        }
        st7789_draw_char(cursor_x, y, *str, color, bg, scale);
        cursor_x += (6 * scale); /* 5 列 + 1 列间隔 */
        str++;
    }
}

void st7789_draw_int(uint16_t x, uint16_t y, int32_t value, uint16_t color, uint16_t bg, uint8_t scale)
{
    char buf[16];
    int len = 0;
    if (value < 0)
    {
        buf[len++]='-';
        value = -value;
    }
    /* 转正数 */
    char tmp[12];
    int ti=0;
    if (value==0) tmp[ti++]='0';
    while (value>0 && ti<11)
    {
        tmp[ti++] = '0' + (value%10);
        value/=10;
    }
    for (int i=ti-1; i>=0; i--)
        buf[len++]=tmp[i];
    buf[len]='\0';
    st7789_draw_string(x,y,buf,color,bg,scale);
}

void st7789_draw_uint(uint16_t x, uint16_t y, uint32_t value, uint16_t color, uint16_t bg, uint8_t scale)
{
    char buf[16];
    int ti=0;
    if (value==0) buf[ti++]='0';
    while (value>0 && ti<15)
    {
        buf[ti++] = '0' + (value%10);
        value/=10;
    }
    buf[ti]='\0';
    /* 反转 */
    for (int i=0;i<ti/2;i++)
    {
        char c=buf[i]; buf[i]=buf[ti-1-i]; buf[ti-1-i]=c;
    }
    st7789_draw_string(x,y,buf,color,bg,scale);
}

void st7789_draw_hex(uint16_t x, uint16_t y, uint32_t value, uint8_t width, uint16_t color, uint16_t bg, uint8_t scale)
{
    char buf[16];
    const char *hex="0123456789ABCDEF";
    for (int i=width-1; i>=0; i--)
    {
        buf[width-1-i]=hex[(value>>(i*4)) & 0xF];
    }
    buf[width]='\0';
    st7789_draw_string(x,y,buf,color,bg,scale);
}

/*=================== 进度条 ===================*/
void st7789_draw_progress_bar(uint16_t x, uint16_t y, uint16_t w, uint16_t h,
                              uint8_t percent, uint16_t color_fg, uint16_t color_bg, uint16_t frame_color)
{
    if (w < 2 || h < 2) return;
    if (percent > 100) percent = 100;

    /* 边框 */
    st7789_fill_rect(x, y, x+w-1, y, frame_color);
    st7789_fill_rect(x, y+h-1, x+w-1, y+h-1, frame_color);
    st7789_fill_rect(x, y, x, y+h-1, frame_color);
    st7789_fill_rect(x+w-1, y, x+w-1, y+h-1, frame_color);

    uint16_t inner_w = w - 2;
    uint16_t inner_h = h - 2;
    uint16_t filled = (inner_w * percent) / 100;

    /* 背景区域 */
    st7789_fill_rect(x+1, y+1, x+1+inner_w-1, y+1+inner_h-1, color_bg);

    /* 填充进度 */
    if (filled > 0)
    {
        st7789_fill_rect(x+1, y+1, x+1+filled-1, y+1+inner_h-1, color_fg);
    }
}

/*=================== 初始化寄存器序列 ===================*/
static void st7789_reg_init(void)
{
    st7789_write_cmd(ST7789_CMD_INTERFACE_PIXEL_FORMAT);
    st7789_write_data8(0x05); /* 16bit */

    st7789_write_cmd(0xB2);
    {
        uint8_t seq[] = {0x0C,0x0C,0x00,0x33,0x33};
        st7789_write_datas(seq, sizeof(seq));
    }

    st7789_write_cmd(0xB7); st7789_write_data8(0x35);
    st7789_write_cmd(0xBB); st7789_write_data8(0x32);
    st7789_write_cmd(0xC2); st7789_write_data8(0x01);
    st7789_write_cmd(0xC3); st7789_write_data8(0x15);
    st7789_write_cmd(0xC4); st7789_write_data8(0x20);
    st7789_write_cmd(0xC6); st7789_write_data8(0x0F);

    st7789_write_cmd(0xD0);
    {
        uint8_t seq[] = {0xA4,0xA1};
        st7789_write_datas(seq, sizeof(seq));
    }

    st7789_write_cmd(0xE0);
    {
        uint8_t seq[] = {0xD0,0x08,0x0E,0x09,0x09,0x05,0x31,0x33,0x48,0x17,0x14,0x15,0x31,0x34};
        st7789_write_datas(seq, sizeof(seq));
    }
    st7789_write_cmd(0xE1);
    {
        uint8_t seq[] = {0xD0,0x08,0x0E,0x09,0x09,0x15,0x31,0x33,0x48,0x17,0x14,0x15,0x31,0x34};
        st7789_write_datas(seq, sizeof(seq));
    }

    st7789_write_cmd(0x21); /* inversion */
    st7789_write_cmd(ST7789_CMD_DISPLAY_ON);
    st7789_write_cmd(ST7789_CMD_MEMORY_WRITE);
}

/*=================== 外部初始化 ===================*/
void st7789_init(void)
{
    back_light_init(); // 初始化背光控制

    st7789_port_backlight_off();

    st7789_reset();

    st7789_write_cmd(ST7789_CMD_SLEEP_OUT);
    st7789_port_delay_ms(200);

    st7789_set_direction(ST7789_DIR_PORTRAIT_0);
    st7789_reg_init();

    st7789_clear(ST7789_COLOR_BLACK);
    st7789_port_backlight_on();
}

/*=================== demo ===================*/
void st7789_demo(void)
{
    st7789_clear(ST7789_COLOR_BLACK);
    st7789_draw_string(10, 10, "LCKFB ST7789 DEMO", ST7789_COLOR_YELLOW, ST7789_COLOR_BLACK, 2);
    st7789_draw_string(10, 40, "Number:", ST7789_COLOR_CYAN, ST7789_COLOR_BLACK, 1);
    st7789_draw_int(80, 40, -12345, ST7789_COLOR_GREEN, ST7789_COLOR_BLACK, 1);
    st7789_draw_string(10, 55, "Hex:", ST7789_COLOR_CYAN, ST7789_COLOR_BLACK, 1);
    st7789_draw_hex(50, 55, 0x1A2B3C, 6, ST7789_COLOR_MAGENTA, ST7789_COLOR_BLACK, 1);

    /* 进度条演示 */
    for (int p=0; p<=100; p+=5)
    {
        st7789_draw_progress_bar(10, 80, 200, 16, (uint8_t)p,
                                 ST7789_COLOR_GREEN, ST7789_COLOR_GRAY, ST7789_COLOR_WHITE);
        st7789_port_delay_ms(50);
    }

    /* 彩色渐变 LCKFB 字母演示 */
    const uint16_t colors[5] = {
        ST7789_COLOR_RED, ST7789_COLOR_GREEN, ST7789_COLOR_BLUE, ST7789_COLOR_YELLOW, ST7789_COLOR_CYAN
    };
    const char *letters = "LCKFB";
    for (int i = 0; i < 5; i++)
    {
        st7789_draw_char(20 + i*30, 120, letters[i], colors[i], ST7789_COLOR_BLACK, 4);
    }

    /* LCKFB 字母动态放大缩小演示 */
    // 统一清除整个演示区，避免任何残留
    const uint16_t demo_x0 = 10, demo_x1 = 220, demo_y0 = 170, demo_y1 = 210;
    for (uint8_t scale = 1; scale <= 4; scale++)
    {
        st7789_fill_rect(demo_x0, demo_y0, demo_x1, demo_y1, ST7789_COLOR_BLACK);
        st7789_draw_string(20, 175, "LCKFB", ST7789_COLOR_ORANGE, ST7789_COLOR_BLACK, scale);
        st7789_port_delay_ms(500);
    }
    for (uint8_t scale = 4; scale >= 1; scale--)
    {
        st7789_fill_rect(demo_x0, demo_y0, demo_x1, demo_y1, ST7789_COLOR_BLACK);
        st7789_draw_string(20, 175, "LCKFB", ST7789_COLOR_ORANGE, ST7789_COLOR_BLACK, scale);
        st7789_port_delay_ms(500);
        if (scale == 1) break;
    }

    /* LCKFB 进度条演示（字体居中显示） */
    for (int p = 0; p <= 100; p += 10)
    {
        st7789_draw_progress_bar(10, 270, 200, 16, (uint8_t)p,
                                 ST7789_COLOR_BLUE, ST7789_COLOR_GRAY, ST7789_COLOR_WHITE);
        // 居中计算
        uint16_t bar_x = 10, bar_w = 200, bar_y = 270, bar_h = 16;
        uint8_t font_scale = 2;
        uint16_t text_w = 6 * font_scale * 5; // "LCKFB" 5字母
        uint16_t text_x = bar_x + (bar_w - text_w) / 2;
        uint16_t text_y = bar_y + (bar_h - 7 * font_scale) / 2;
        st7789_draw_string(text_x, text_y, "LCKFB", ST7789_COLOR_RED, ST7789_COLOR_WHITE, font_scale);
        st7789_port_delay_ms(50);
    }
}
