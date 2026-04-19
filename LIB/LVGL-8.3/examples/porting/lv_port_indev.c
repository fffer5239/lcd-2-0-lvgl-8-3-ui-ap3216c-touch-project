/**
 * @file lv_port_indev_templ.c
 *
 */

/*Copy this file as "lv_port_indev.c" and set this value to "1" to enable content*/
#if 1

/*********************
 *      INCLUDES
 *********************/
#include "lv_port_indev.h"
#include "../../lvgl.h"

#define LOG_TAG "LVGL_INDEV"
#include "lckfb_log_port.h"

#include "soft_i2c.h"
#include "ft6336_drv.h"

/* 引入HAL库头文件，以便操作GPIO */
#include "main.h" 

/*********************
 *      DEFINES
 *********************/
/* 定义触摸中断引脚，方便后续维护 */
#define TOUCH_INT_GPIO_PORT   GPIOE
#define TOUCH_INT_GPIO_PIN    GPIO_PIN_2

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/

static void touchpad_init(void);
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool touchpad_is_pressed(void);
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y);

static void mouse_init(void);
static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static bool mouse_is_pressed(void);
static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y);

static void keypad_init(void);
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static uint32_t keypad_get_key(void);

static void encoder_init(void);
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static void encoder_handler(void);

static void button_init(void);
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static int8_t button_get_pressed_id(void);
static bool button_is_pressed(uint8_t id);

/**********************
 *  STATIC VARIABLES
 **********************/
lv_indev_t * indev_touchpad;

lv_indev_t * indev_mouse;
lv_indev_t * indev_keypad;
lv_indev_t * indev_encoder;
lv_indev_t * indev_button;

static int32_t encoder_diff;
static lv_indev_state_t encoder_state;
static lv_coord_t touch_last_x;
static lv_coord_t touch_last_y;
static bool touch_point_valid;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void lv_port_indev_init(void)
{
    static lv_indev_drv_t indev_drv;

    /*------------------
     * Touchpad
     * -----------------*/

    /*Initialize your touchpad if you have*/
    touchpad_init();

    /*Register a touchpad input device*/
    lv_indev_drv_init(&indev_drv);
    indev_drv.type = LV_INDEV_TYPE_POINTER;
    indev_drv.read_cb = touchpad_read;
    indev_touchpad = lv_indev_drv_register(&indev_drv);

    #if 0
       /* ... Mouse, Keypad, Encoder, Button init code ... */
    #endif
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

/*------------------
 * Touchpad
 * -----------------*/

/*Initialize your touchpad*/
ft6336_dev_t touch_dev;

static void touchpad_init(void)
{
    /* 
     * 注意：
     * 1. 请确保在 CubeMX 或 gpio.c 中已经初始化了 PE2。
     * 2. PE2 模式建议：GPIO_MODE_INPUT (如果不使用中断触发仅读取电平) 
     *    或者 GPIO_MODE_IT_FALLING (如果需要唤醒功能)。
     * 3. 上拉电阻：FT6336通常需要上拉，建议开启 MCU 内部上拉 (GPIO_PULLUP)。
     */

    /* I2C总线初始化及FT6336复位操作通常在 ft6336_init 内部或之前完成 */

    if (ft6336_init(&touch_dev, &soft_i2c_bus2) != 0) {
        LOG_ERROR("FT6336 初始化失败，程序终止\n");
        /* 
         * 注意：原代码此处 return -1; 是错误的，因为函数返回类型是 void。
         * 如果初始化失败，可以设置一个全局标志位禁用触摸，或者死循环报错。
         */
    }
    else {
        LOG_INFO("FT6336 触摸屏初始化成功\n");
    }
}

/*Will be called by the library to read the touchpad*/
static void touchpad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    /*Save the pressed coordinates and the state*/
    if(touchpad_is_pressed()) {
        touchpad_get_xy(&data->point.x, &data->point.y);
        data->state = LV_INDEV_STATE_PR;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }
}

/*Return true is the touchpad is pressed*/
static bool touchpad_is_pressed(void)
{
    touch_point_valid = false;

    /* 
     * FT6336 触摸时拉低 INT 引脚。
     * 如果检测到 PE2 为高电平（GPIO_PIN_SET），说明没有触摸，直接返回 false。
     * 这样避免了进行费时的 I2C 读取操作。
     */
    if(HAL_GPIO_ReadPin(TOUCH_INT_GPIO_PORT, TOUCH_INT_GPIO_PIN) != GPIO_PIN_RESET) {
        return false;
    }

    /* 只有当 PE2 被拉低时，才启动 I2C 通信 */
    if (ft6336_read_touch_data(&touch_dev) != 0) {
        return false;
    }

    if (touch_dev.touch_count == 0) {
        return false;
    }

    /* 遍历触摸点，取第一个有效点 */
    for (uint8_t i = 0; i < touch_dev.touch_count; ++i) {
        ft6336_touch_point_t *point = &touch_dev.points[i];
        /* 注意：部分驱动 valid 逻辑可能相反，请根据你的 ft6336_drv.h 确认 */
        if (!point->valid) { 
            continue;
        }

        touch_last_x = (lv_coord_t)point->x;
        touch_last_y = (lv_coord_t)point->y;
        touch_point_valid = true;
        return true;
    }

    return false;
}

/*Get the x and y coordinates if the touchpad is pressed*/
static void touchpad_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    if (!x || !y) {
        return;
    }

    if (touch_point_valid) {
        *x = touch_last_x;
        *y = touch_last_y;
    }
}

/*------------------
 * Mouse
 * -----------------*/

/*Initialize your mouse*/
static void mouse_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the mouse*/
static void mouse_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    /*Get the current x and y coordinates*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the mouse button is pressed or released*/
    if(mouse_is_pressed()) {
        data->state = LV_INDEV_STATE_PR;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }
}

/*Return true is the mouse button is pressed*/
static bool mouse_is_pressed(void)
{
    /*Your code comes here*/

    return false;
}

/*Get the x and y coordinates if the mouse is pressed*/
static void mouse_get_xy(lv_coord_t * x, lv_coord_t * y)
{
    /*Your code comes here*/

    (*x) = 0;
    (*y) = 0;
}

/*------------------
 * Keypad
 * -----------------*/

/*Initialize your keypad*/
static void keypad_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the mouse*/
static void keypad_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    static uint32_t last_key = 0;

    /*Get the current x and y coordinates*/
    mouse_get_xy(&data->point.x, &data->point.y);

    /*Get whether the a key is pressed and save the pressed key*/
    uint32_t act_key = keypad_get_key();
    if(act_key != 0) {
        data->state = LV_INDEV_STATE_PR;

        /*Translate the keys to LVGL control characters according to your key definitions*/
        switch(act_key) {
            case 1:
                act_key = LV_KEY_NEXT;
                break;
            case 2:
                act_key = LV_KEY_PREV;
                break;
            case 3:
                act_key = LV_KEY_LEFT;
                break;
            case 4:
                act_key = LV_KEY_RIGHT;
                break;
            case 5:
                act_key = LV_KEY_ENTER;
                break;
        }

        last_key = act_key;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    data->key = last_key;
}

/*Get the currently being pressed key.  0 if no key is pressed*/
static uint32_t keypad_get_key(void)
{
    /*Your code comes here*/

    return 0;
}

/*------------------
 * Encoder
 * -----------------*/

/*Initialize your keypad*/
static void encoder_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the encoder*/
static void encoder_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    data->enc_diff = encoder_diff;
    data->state = encoder_state;
}

/*Call this function in an interrupt to process encoder events (turn, press)*/
static void encoder_handler(void)
{
    /*Your code comes here*/

    encoder_diff += 0;
    encoder_state = LV_INDEV_STATE_REL;
}

/*------------------
 * Button
 * -----------------*/

/*Initialize your buttons*/
static void button_init(void)
{
    /*Your code comes here*/
}

/*Will be called by the library to read the button*/
static void button_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{

    static uint8_t last_btn = 0;

    /*Get the pressed button's ID*/
    int8_t btn_act = button_get_pressed_id();

    if(btn_act >= 0) {
        data->state = LV_INDEV_STATE_PR;
        last_btn = btn_act;
    }
    else {
        data->state = LV_INDEV_STATE_REL;
    }

    /*Save the last pressed button's ID*/
    data->btn_id = last_btn;
}

/*Get ID  (0, 1, 2 ..) of the pressed button*/
static int8_t button_get_pressed_id(void)
{
    uint8_t i;

    /*Check to buttons see which is being pressed (assume there are 2 buttons)*/
    for(i = 0; i < 2; i++) {
        /*Return the pressed button's ID*/
        if(button_is_pressed(i)) {
            return i;
        }
    }

    /*No button pressed*/
    return -1;
}

/*Test if `id` button is pressed or not*/
static bool button_is_pressed(uint8_t id)
{

    /*Your code comes here*/

    return false;
}

#else /*Enable this file at the top*/

/*This dummy typedef exists purely to silence -Wpedantic.*/
typedef int keep_pedantic_happy;
#endif
