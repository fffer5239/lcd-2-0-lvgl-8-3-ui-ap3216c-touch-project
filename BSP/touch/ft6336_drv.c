#include "ft6336_drv.h"

// 假设 lckfb_log_port.h 和 soft_i2c.h 在包含路径中
#define LOG_TAG "FT6336"
#include "lckfb_log_port.h"

/**
 * @brief FT6336 内部寄存器地址定义
 */
#define FT6336_REG_TD_STATUS       0x02  /* 触摸状态和点数 */
#define FT6336_REG_P1_XH           0x03  /* 触摸点1数据起始地址 */
#define FT6336_REG_P2_XH           0x09  /* 触摸点2数据起始地址 */
#define FT6336_REG_CHIP_VENDOR_ID  0xA3  /* 芯片厂商ID寄存器 */
#define FT6336_REG_FIRMWARE_ID     0xA6  /* 固件版本号寄存器 */

/* 预期的芯片厂商ID */
#define FT6336_EXPECTED_VENDOR_ID  0x64

/**
 * @brief FT6336 组合读事务：先写寄存器地址，然后读取数据
 *
 * @param dev FT6336 设备实例
 * @param reg_addr 要读取的寄存器地址
 * @param buf 存储读取数据的缓冲区
 * @param len 要读取的数据长度
 * @return int 0 表示成功, -1 表示失败
 */
static int ft6336_write_read(ft6336_dev_t *dev, uint8_t reg_addr, uint8_t *buf, size_t len)
{
    if (!dev || !dev->i2c_bus) return -1;
    if (len > 0 && !buf) return -1;

    soft_i2c_bus_t *bus = dev->i2c_bus;

    /* 1. 发送 START 和从机地址（写模式） */
    if (soft_i2c_start(bus) != 0) {
        soft_i2c_stop(bus);
        return -1;
    }
    if (soft_i2c_write_byte(bus, (FT6336_I2C_SLAVE_ADDR << 1) | 0) != 0) {
        soft_i2c_stop(bus);
        return -1;
    }

    /* 2. 发送要读取的寄存器地址 */
    if (soft_i2c_write_byte(bus, reg_addr) != 0) {
        soft_i2c_stop(bus);
        return -1;
    }

    /* 如果不需要读取数据，在此结束 */
    if (len == 0) {
        soft_i2c_stop(bus);
        return 0;
    }

    /* 3. 发送 REPEATED START 和从机地址（读模式） */
    if (soft_i2c_start(bus) != 0) { /* 重复起始条件 */
        soft_i2c_stop(bus);
        return -1;
    }
    if (soft_i2c_write_byte(bus, (FT6336_I2C_SLAVE_ADDR << 1) | 1) != 0) {
        soft_i2c_stop(bus);
        return -1;
    }

    /* 4. 连续读取数据 */
    for (size_t i = 0; i < len; ++i) {
        bool send_ack = (i + 1) < len; /* 最后一个字节发送 NACK */
        if (soft_i2c_read_byte(bus, &buf[i], send_ack) != 0) {
            soft_i2c_stop(bus);
            return -1;
        }
    }

    /* 5. 发送 STOP */
    soft_i2c_stop(bus);
    return 0;
}

/**
 * @brief 初始化 FT6336 设备
 */
int ft6336_init(ft6336_dev_t *dev, soft_i2c_bus_t *i2c_bus)
{
    if (!dev || !i2c_bus) return -1;

    dev->i2c_bus = i2c_bus;
    dev->touch_count = 0;
    for (int i = 0; i < FT6336_MAX_TOUCH_POINTS; ++i) {
        dev->points[i].valid = false;
    }

    uint8_t vendor_id = 0;
    if (ft6336_write_read(dev, FT6336_REG_CHIP_VENDOR_ID, &vendor_id, 1) != 0) {
        LOG_ERROR("读取厂商ID失败");
        return -1;
    }

    if (vendor_id != FT6336_EXPECTED_VENDOR_ID) {
        LOG_ERROR("厂商ID不匹配! 期望: 0x%02X, 实际: 0x%02X",
                  FT6336_EXPECTED_VENDOR_ID, vendor_id);
        return -1;
    }

    uint8_t fw_id = 0;
    if (ft6336_write_read(dev, FT6336_REG_FIRMWARE_ID, &fw_id, 1) != 0) {
        LOG_ERROR("读取固件ID失败");
        return -1;
    }

    LOG_INFO("FT6336 初始化成功. 厂商ID: 0x%02X, 固件ID: 0x%02X", vendor_id, fw_id);
    return 0;
}

/**
 * @brief 从 FT6336 读取当前的触摸数据
 */
int ft6336_read_touch_data(ft6336_dev_t *dev)
{
    if (!dev) return -1;

    /* 从 TD_STATUS 寄存器开始，一次性读取所有触摸点数据，提高效率 */
    uint8_t data_buf[1 + FT6336_MAX_TOUCH_POINTS * 6]; // 1 for status, 6 per point
    if (ft6336_write_read(dev, FT6336_REG_TD_STATUS, data_buf, sizeof(data_buf)) != 0) {
        LOG_WARN("读取触摸数据失败");
        return -1;
    }

    /* 解析 TD_STATUS */
    dev->touch_count = data_buf[0] & 0x0F;
    if (dev->touch_count > FT6336_MAX_TOUCH_POINTS) {
        dev->touch_count = FT6336_MAX_TOUCH_POINTS;
    }

    /* 清空旧数据 */
    for (int i = 0; i < FT6336_MAX_TOUCH_POINTS; ++i) {
        dev->points[i].valid = false;
    }

    /* 解析每个触摸点的数据 */
    for (uint8_t i = 0; i < dev->touch_count; ++i) {
        uint8_t *point_data = &data_buf[1 + i * 6]; /* 定位到当前点的数据块 */
        
        ft6336_touch_point_t *point = &dev->points[i];
        point->valid = true;

        /* 解析事件标志 (XH[7:6]) */
        point->event = (point_data[0] >> 6) & 0x03;

        /* 解析X坐标 (XH[3:0] << 8 | XL) */
        point->x = ((uint16_t)(point_data[0] & 0x0F) << 8) | point_data[1];

        /* 解析触摸ID (YH[7:4]) */
        point->touch_id = (point_data[2] >> 4) & 0x0F;

        /* 解析Y坐标 (YH[3:0] << 8 | YL) */
        point->y = ((uint16_t)(point_data[2] & 0x0F) << 8) | point_data[3];
        
        // point_data[4] 是触摸权重，这里忽略
        // point_data[5] 是触摸面积，这里也忽略
    }

    return 0;
}