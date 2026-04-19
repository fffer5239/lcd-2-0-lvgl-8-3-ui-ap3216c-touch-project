#ifndef __ST7789_FONT_H__
#define __ST7789_FONT_H__

#include <stdint.h>

/*
 * 简单 5x7 ASCII 字体 (0x20—0x7E)
 * 每字符 5 列，bit0 在顶或底取决于实现，这里采用：最低位是顶像素(行0)。
 * 取模方式：纵向 8 点，宽 5 列。
 */

#ifdef __cplusplus
extern "C" {
#endif

extern const uint8_t st7789_font5x7[][5];

#ifdef __cplusplus
}
#endif

#endif /* __ST7789_FONT_H__ */