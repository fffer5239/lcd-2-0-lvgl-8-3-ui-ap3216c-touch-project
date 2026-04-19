/**
 * @file ap3216c.h
 * @author LCKFB-YZH
 * @brief AP3216C Ambient Light & Proximity Sensor Driver
 * @date 2025-12-05
 */

#ifndef AP3216C_H
#define AP3216C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "soft_i2c.h" // 包含你的I2C库

/* AP3216C I2C Address (7-bit) */
#define AP3216C_ADDR            0x1E

/* Register Map */
#define AP3216C_SYS_CON         0x00 /* System Configuration */
#define AP3216C_ALS_DATA_L      0x0C /* ALS Data Low Byte */
#define AP3216C_ALS_DATA_H      0x0D /* ALS Data High Byte */
#define AP3216C_PS_DATA_L       0x0E /* PS Data Low Byte */
#define AP3216C_PS_DATA_H       0x0F /* PS Data High Byte */

/**
 * @brief 初始化 AP3216C
 * @param bus 指向已初始化的 soft_i2c_bus_t 结构体指针
 * @return 0: 成功, -1: 失败
 */
int ap3216c_device_init(soft_i2c_bus_t *bus);

/**
 * @brief 读取环境光强度 (ALS)
 * @param bus I2C总线指针
 * @param als_val 输出 ALS 数值指针 (单位: Lux 并不是直接对应，需查表，这里返回原始16位值)
 * @return 0: 成功, -1: 失败
 */
int ap3216c_read_als(soft_i2c_bus_t *bus, uint16_t *als_val);

/**
 * @brief 读取接近距离 (PS)
 * @param bus I2C总线指针
 * @param ps_val 输出 PS 数值指针
 * @return 0: 成功, -1: 失败
 */
int ap3216c_read_ps(soft_i2c_bus_t *bus, uint16_t *ps_val);

#ifdef __cplusplus
}
#endif

#endif // AP3216C_H