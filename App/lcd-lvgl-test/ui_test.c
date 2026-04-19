/**
 * @file ui_test.c
 * @author LCKFB-YZH
 * @brief LVGL 8.3 Screen Test UI with Brightness Control
 * @date 2025-12-30
 */

#include "ui_test.h"
#include "bsp_backlight.h"
#include <stdio.h>

// ------------------------------------------------------------
// 全局对象句柄
// ------------------------------------------------------------
static lv_obj_t * bar_als;
static lv_obj_t * label_als_val;
static lv_obj_t * bar_ps;
static lv_obj_t * label_ps_val;
static lv_obj_t * label_touch;
static lv_obj_t * screen_btns[3];
static lv_obj_t * slider_brightness; 
static lv_obj_t * label_brightness_val; 

// 定义按键样式
static lv_style_t style_btn_default;
static lv_style_t style_btn_pressed_touch;
static lv_style_t style_btn_pressed_phys;

// 量程定义
#define MAX_ALS_VALUE 1000 
#define MAX_PS_VALUE  1000 

// ------------------------------------------------------------
// 亮度滑块回调函数
// ------------------------------------------------------------
static void brightness_slider_event_cb(lv_event_t * e) {
    lv_obj_t * slider = lv_event_get_target(e);
    int32_t val = lv_slider_get_value(slider);

    // 限制最小值为 1%
    if(val < 1) val = 1;

    // 1. 更新 UI 显示
    if(label_brightness_val) {
        lv_label_set_text_fmt(label_brightness_val, "%d%%", val);
    }

    // 2. 调用硬件驱动调整 PWM
    // 注意：val 范围是 1-100
    back_light_set((uint8_t)val);
}

// ------------------------------------------------------------
// 辅助函数：创建垂直布局的传感器面板
// ------------------------------------------------------------
static void create_sensor_panel(lv_obj_t * parent, const char * title, lv_color_t color, lv_obj_t ** bar_out, lv_obj_t ** label_out) {
    lv_obj_t * cont = lv_obj_create(parent);
    lv_obj_set_size(cont, lv_pct(100), LV_SIZE_CONTENT); 
    lv_obj_set_style_bg_opa(cont, LV_OPA_TRANSP, 0);     
    lv_obj_set_style_border_width(cont, 0, 0);           
    lv_obj_clear_flag(cont, LV_OBJ_FLAG_SCROLLABLE);     
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_COLUMN);     
    lv_obj_set_style_pad_all(cont, 5, 0);                
    lv_obj_set_style_pad_gap(cont, 5, 0);                

    lv_obj_t * top_row = lv_obj_create(cont);
    lv_obj_set_size(top_row, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(top_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(top_row, 0, 0);
    lv_obj_set_style_pad_all(top_row, 0, 0);
    lv_obj_clear_flag(top_row, LV_OBJ_FLAG_SCROLLABLE);
    
    lv_obj_t * lbl_title = lv_label_create(top_row);
    lv_label_set_text(lbl_title, title);
    lv_obj_set_style_text_font(lbl_title, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(lbl_title, color, 0);
    lv_obj_align(lbl_title, LV_ALIGN_LEFT_MID, 0, 0);

    *label_out = lv_label_create(top_row);
    lv_label_set_text(*label_out, "0");
    lv_obj_set_style_text_font(*label_out, &lv_font_montserrat_14, 0);
    lv_obj_set_style_text_color(*label_out, lv_color_make(0x33, 0x33, 0x33), 0);
    lv_obj_align(*label_out, LV_ALIGN_RIGHT_MID, 0, 0);

    *bar_out = lv_bar_create(cont);
    lv_obj_set_size(*bar_out, lv_pct(100), 12);
    lv_bar_set_range(*bar_out, 0, 100);
    lv_obj_set_style_bg_color(*bar_out, color, LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(*bar_out, lv_color_hex(0xE0E0E0), LV_PART_MAIN);
}

// ------------------------------------------------------------
// 初始化
// ------------------------------------------------------------
void ui_test_init(void) {
    lv_obj_t * scr = lv_scr_act();
    lv_obj_set_style_bg_color(scr, lv_color_white(), 0);

    // 0. 初始化样式
    lv_style_init(&style_btn_default);
    lv_style_set_bg_color(&style_btn_default, lv_color_white());
    lv_style_set_border_color(&style_btn_default, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_border_width(&style_btn_default, 1);
    lv_style_set_shadow_width(&style_btn_default, 4);
    lv_style_set_shadow_ofs_y(&style_btn_default, 2);
    lv_style_set_shadow_opa(&style_btn_default, LV_OPA_20);
    lv_style_set_text_color(&style_btn_default, lv_color_make(0x33, 0x33, 0x33));

    lv_style_init(&style_btn_pressed_touch);
    lv_style_set_bg_color(&style_btn_pressed_touch, lv_palette_main(LV_PALETTE_BLUE)); 
    lv_style_set_bg_grad_color(&style_btn_pressed_touch, lv_palette_darken(LV_PALETTE_BLUE, 2)); 
    lv_style_set_transform_width(&style_btn_pressed_touch, -8); 
    lv_style_set_transform_height(&style_btn_pressed_touch, -8);
    lv_style_set_text_color(&style_btn_pressed_touch, lv_color_white());
    lv_style_set_shadow_width(&style_btn_pressed_touch, 0);

    lv_style_init(&style_btn_pressed_phys);
    lv_style_set_bg_color(&style_btn_pressed_phys, lv_palette_main(LV_PALETTE_RED)); 
    lv_style_set_bg_grad_color(&style_btn_pressed_phys, lv_palette_darken(LV_PALETTE_RED, 2)); 
    lv_style_set_transform_width(&style_btn_pressed_phys, -8); 
    lv_style_set_transform_height(&style_btn_pressed_phys, -8);
    lv_style_set_text_color(&style_btn_pressed_phys, lv_color_white());
    lv_style_set_shadow_width(&style_btn_pressed_phys, 0);

    // 1. 顶部标题
    lv_obj_t * header = lv_label_create(scr);
    lv_label_set_text(header, "2.0-Screen-test");
    lv_obj_set_style_text_font(header, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(header, lv_color_black(), 0);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 5);

    // 2. 主容器
    lv_obj_t * main_col = lv_obj_create(scr);
    lv_obj_set_size(main_col, lv_pct(100), 260); 
    lv_obj_align(main_col, LV_ALIGN_TOP_MID, 0, 30);
    lv_obj_set_flex_flow(main_col, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_bg_opa(main_col, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(main_col, 0, 0);
    lv_obj_set_style_pad_all(main_col, 10, 0);
    lv_obj_set_style_pad_gap(main_col, 15, 0);
    lv_obj_clear_flag(main_col, LV_OBJ_FLAG_SCROLLABLE);

    // 3. 传感器区域
    create_sensor_panel(main_col, "ALS (Lux)", lv_palette_main(LV_PALETTE_ORANGE), &bar_als, &label_als_val);
    create_sensor_panel(main_col, "PS (Dist)", lv_palette_main(LV_PALETTE_TEAL),   &bar_ps,  &label_ps_val);

    // -------------------- 新增：亮度调节区域 --------------------
    lv_obj_t * brightness_cont = lv_obj_create(main_col);
    lv_obj_set_size(brightness_cont, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(brightness_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(brightness_cont, 0, 0);
    lv_obj_set_flex_flow(brightness_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(brightness_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_all(brightness_cont, 0, 0);

    // 左侧图标/文字
    lv_obj_t * lbl_icon = lv_label_create(brightness_cont);
    lv_label_set_text(lbl_icon, "BL:"); // Backlight
    lv_obj_set_style_text_color(lbl_icon, lv_palette_main(LV_PALETTE_GREY), 0);

    // 滑块
    slider_brightness = lv_slider_create(brightness_cont);
    lv_obj_set_size(slider_brightness, 140, 20); // 调整宽度适配屏幕
    lv_slider_set_range(slider_brightness, 1, 100);
    lv_slider_set_value(slider_brightness, 50, LV_ANIM_ON); // 默认 50%
    lv_obj_set_style_bg_color(slider_brightness, lv_palette_main(LV_PALETTE_BLUE), LV_PART_INDICATOR);
    lv_obj_set_style_bg_color(slider_brightness, lv_palette_main(LV_PALETTE_BLUE), LV_PART_KNOB); 
    lv_obj_add_event_cb(slider_brightness, brightness_slider_event_cb, LV_EVENT_VALUE_CHANGED, NULL);

    // 右侧数值
    label_brightness_val = lv_label_create(brightness_cont);
    lv_label_set_text(label_brightness_val, "50%");
    // 1. 显式设定标签对象的宽度，防止数字变化时（如 9% -> 10%）造成抖动
    lv_obj_set_width(label_brightness_val, 45); 
    // 2. 设置文字在标签内部靠右对齐
    lv_obj_set_style_text_align(label_brightness_val, LV_TEXT_ALIGN_RIGHT, 0);
    // 初始化硬件亮度

    back_light_set(50);
    // ----------------------------------------------------------

    // 4. 分隔线
    lv_obj_t * line = lv_obj_create(main_col);
    lv_obj_set_size(line, lv_pct(100), 1);
    lv_obj_set_style_bg_color(line, lv_palette_lighten(LV_PALETTE_GREY, 2), 0);
    lv_obj_set_style_border_width(line, 0, 0);

    // 5. 按键区域
    lv_obj_t * btn_cont = lv_obj_create(main_col);
    lv_obj_set_size(btn_cont, lv_pct(100), 60);
    lv_obj_set_flex_flow(btn_cont, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(btn_cont, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_bg_opa(btn_cont, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(btn_cont, 0, 0);
    lv_obj_clear_flag(btn_cont, LV_OBJ_FLAG_SCROLLABLE);

    const char * btn_labels[] = {"K1", "K2", "K3"};
    for(int i=0; i<3; i++) {
        screen_btns[i] = lv_btn_create(btn_cont);
        lv_obj_set_size(screen_btns[i], 60, 40); 
        lv_obj_add_style(screen_btns[i], &style_btn_default, LV_PART_MAIN);
        lv_obj_add_style(screen_btns[i], &style_btn_pressed_touch, LV_STATE_PRESSED);
        lv_obj_add_style(screen_btns[i], &style_btn_pressed_phys, LV_STATE_CHECKED);
        lv_obj_add_flag(screen_btns[i], LV_OBJ_FLAG_CHECKABLE);

        lv_obj_t * lbl = lv_label_create(screen_btns[i]);
        lv_label_set_text(lbl, btn_labels[i]);
        lv_obj_center(lbl);
    }

    // 6. 底部触摸坐标
    label_touch = lv_label_create(scr);
    lv_label_set_text(label_touch, "Touch: -");
    lv_obj_set_style_text_font(label_touch, &lv_font_montserrat_30, 0);
    lv_obj_set_style_text_color(label_touch, lv_palette_main(LV_PALETTE_GREY), 0);
    lv_obj_align(label_touch, LV_ALIGN_BOTTOM_LEFT, 0, -5);
}

void ui_update_sensors(uint16_t als_val, uint16_t ps_val) {
    if(!bar_als) return;

    int32_t als_pct = (als_val * 100) / MAX_ALS_VALUE;
    if(als_pct > 100) als_pct = 100;
    lv_bar_set_value(bar_als, als_pct, LV_ANIM_OFF); 
    lv_label_set_text_fmt(label_als_val, "%d", als_val);

    int32_t ps_pct = (ps_val * 100) / MAX_PS_VALUE;
    if(ps_pct > 100) ps_pct = 100;
    lv_bar_set_value(bar_ps, ps_pct, LV_ANIM_OFF);
    lv_label_set_text_fmt(label_ps_val, "%d", ps_val);
}

void ui_update_touch_info(lv_coord_t x, lv_coord_t y, bool is_pressed) {
    if(!label_touch) return;
    if(is_pressed) {
        lv_label_set_text_fmt(label_touch, "X:%d Y:%d", x, y);
        lv_obj_set_style_text_color(label_touch, lv_palette_main(LV_PALETTE_BLUE), 0);
    } else {
        lv_label_set_text_fmt(label_touch, "X:%d Y:%d", x, y);
        lv_obj_set_style_text_color(label_touch, lv_palette_main(LV_PALETTE_GREY), 0);
    }
}

void ui_update_physical_btn_state(uint8_t btn_index, bool is_pressed) {
    if(btn_index >= 3 || !screen_btns[btn_index]) return;
    if(is_pressed) {
        lv_obj_add_state(screen_btns[btn_index], LV_STATE_CHECKED);
    } else {
        lv_obj_clear_state(screen_btns[btn_index], LV_STATE_CHECKED);
    }
}