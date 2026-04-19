/**
 * @file ui_test.h
 * @author LCKFB-YZH
 * @brief UI Test Interface for 2.0-Screen-test Board
 */

#ifndef _UI_TEST_H
#define _UI_TEST_H

#include "..\..\LIB\LVGL-8.3\lvgl.h"
#include <stdbool.h>
#include <stdint.h>

// 初始化 UI
void ui_test_init(void);

/**
 * @brief 更新 UI 数据，需在主循环中周期性调用
 * 
 * @param als_val 光照传感器数值 (0-65535)
 * @param ps_val  接近传感器数值 (0-1023)
 * @param touch_x 当前触摸 X 坐标 (如果是 -1 表示未触摸)
 * @param touch_y 当前触摸 Y 坐标
 * @param key1_pressed 物理按键 1 状态 (PA0)
 * @param key2_pressed 物理按键 2 状态 (PE8)
 * @param key3_pressed 物理按键 3 状态 (PC13)
 */
void ui_test_update(uint16_t als_val, uint16_t ps_val, int16_t touch_x, int16_t touch_y, 
                    bool key1_pressed, bool key2_pressed, bool key3_pressed);

void ui_update_touch_info(lv_coord_t x, lv_coord_t y, bool is_pressed);
void ui_update_sensors(uint16_t als_val, uint16_t ps_val);
void ui_update_physical_btn_state(uint8_t btn_index, bool is_pressed);

#endif