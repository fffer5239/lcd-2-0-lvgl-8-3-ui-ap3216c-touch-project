#ifndef FT6336_DRV_H
#define FT6336_DRV_H

#include "soft_i2c.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* FT6336 I2C 从机地址 */
#define FT6336_I2C_SLAVE_ADDR 0x38

/* FT6336 最多支持的触摸点数 */
#define FT6336_MAX_TOUCH_POINTS 2

/**
 * @brief 触摸事件标志
 */
typedef enum {
    FT6336_EVENT_PRESS_DOWN = 0, /* 按下 */
    FT6336_EVENT_LIFT_UP    = 1, /* 抬起 */
    FT6336_EVENT_CONTACT    = 2, /* 接触/移动 */
    FT6336_EVENT_NONE       = 3  /* 无事件 */
} ft6336_touch_event_t;

/**
 * @brief 单个触摸点的信息结构体
 */
typedef struct {
    bool                   valid;      /* 此触摸点数据是否有效 */
    uint8_t                touch_id;   /* 触摸ID (0 或 1) */
    ft6336_touch_event_t   event;      /* 触摸事件 */
    uint16_t               x;          /* X 坐标 */
    uint16_t               y;          /* Y 坐标 */
} ft6336_touch_point_t;

/**
 * @brief FT6336 驱动实例结构体
 */
typedef struct {
    soft_i2c_bus_t *i2c_bus;            /* 关联的软件 I2C 总线 */
    uint8_t         touch_count;        /* 当前有效的触摸点数量 */
    ft6336_touch_point_t points[FT6336_MAX_TOUCH_POINTS]; /* 各触摸点信息 */
} ft6336_dev_t;


/**
 * @brief 初始化 FT6336 设备
 *
 * @param dev 指向 FT6336 设备实例的指针
 * @param i2c_bus 指向已初始化的软件 I2C 总线实例的指针
 * @return int 0 表示成功, -1 表示失败 (例如, 无法读取到正确的芯片ID)
 */
int ft6336_init(ft6336_dev_t *dev, soft_i2c_bus_t *i2c_bus);

/**
 * @brief 从 FT6336 读取当前的触摸数据
 *
 * @param dev 指向 FT6336 设备实例的指针
 * @return int 0 表示成功, -1 表示读取失败
 */
int ft6336_read_touch_data(ft6336_dev_t *dev);

#endif // FT6336_DRV_H