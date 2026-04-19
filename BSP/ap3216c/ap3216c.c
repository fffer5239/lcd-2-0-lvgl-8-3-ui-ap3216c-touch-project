/**
 * @file ap3216c.c
 * @author LCKFB-YZH
 * @brief AP3216C Driver Implementation
 */

#include "ap3216c.h"

#define LOG_TAG "AP3216C"
#include "lckfb_log_port.h" // 假设你有这个日志头文件

/**
 * @brief 内部函数：向指定寄存器写数据
 * 利用 soft_i2c_write 已经封装好的逻辑
 */
static int ap3216c_write_reg(soft_i2c_bus_t *bus, uint8_t reg, uint8_t data)
{
    /* soft_i2c_write 协议：START -> ADDR(W) -> DATA... -> STOP */
    /* 我们需要发送: [RegAddr, Data] */
    uint8_t buf[2];
    buf[0] = reg;
    buf[1] = data;
    
    return soft_i2c_write(bus, AP3216C_ADDR, buf, 2);
}

/**
 * @brief 内部函数：从指定寄存器读数据
 * @note 由于 soft_i2c_read 不支持发送寄存器地址，我们需要手动构建时序
 * 时序: START -> ADDR(W) -> REG -> RESTART -> ADDR(R) -> DATA -> STOP
 */
static int ap3216c_read_regs(soft_i2c_bus_t *bus, uint8_t reg, uint8_t *buf, uint8_t len)
{
    if (!bus || !buf || len == 0) return -1;

    /* 1. START */
    if (soft_i2c_start(bus) != 0) return -1;

    /* 2. Send Device Address + Write (0) */
    if (soft_i2c_write_byte(bus, (AP3216C_ADDR << 1) | 0) != 0) goto err;

    /* 3. Send Register Address */
    if (soft_i2c_write_byte(bus, reg) != 0) goto err;

    /* 4. RESTART (Repeat Start) */
    /* 注意：soft_i2c_start 内部会判断 SCL 状态，直接调用即可产生 Restart 信号 */
    if (soft_i2c_start(bus) != 0) goto err;

    /* 5. Send Device Address + Read (1) */
    if (soft_i2c_write_byte(bus, (AP3216C_ADDR << 1) | 1) != 0) goto err;

    /* 6. Read Data Loop */
    for (uint8_t i = 0; i < len; i++) {
        /* 如果不是最后一个字节，发送 ACK(true)，否则 NACK(false) */
        bool send_ack = (i < len - 1);
        if (soft_i2c_read_byte(bus, &buf[i], send_ack) != 0) goto err;
    }

    /* 7. STOP */
    soft_i2c_stop(bus);
    return 0;

err:
    soft_i2c_stop(bus); // 发生错误必须释放总线
    return -1;
}

int ap3216c_device_init(soft_i2c_bus_t *bus)
{
    if (!bus) return -1;

    /* 1. 复位传感器 */
    /* Write 0x04 to Sys Config Register (0x00) -> SW Reset */
    if (ap3216c_write_reg(bus, AP3216C_SYS_CON, 0x04) != 0) {
        LOG_ERROR("AP3216C Reset Failed");
        return -1;
    }

    /* 2. 等待复位完成 (至少 10ms) */
    soft_port_udelay(20000); // 20ms safe

    /* 3. 开启 ALS 和 PS (Mode: ALS+PS+IR = 0x03) */
    if (ap3216c_write_reg(bus, AP3216C_SYS_CON, 0x03) != 0) {
        LOG_ERROR("AP3216C Enable Failed");
        return -1;
    }
    
    /* 稍作延时让传感器准备好 */
    soft_port_udelay(50000); 

    LOG_INFO("AP3216C Init Success");
    return 0;
}

int ap3216c_read_als(soft_i2c_bus_t *bus, uint16_t *als_val)
{
    uint8_t buf[2];

    /* ALS 数据在 0x0C (Low) 和 0x0D (High) */
    /* 我们一次性读取 2 个字节 */
    if (ap3216c_read_regs(bus, AP3216C_ALS_DATA_L, buf, 2) != 0) {
        return -1;
    }

    /* 组合数据: Low byte first */
    if (als_val) {
        *als_val = (uint16_t)((buf[1] << 8) | buf[0]);
    }

    return 0;
}

int ap3216c_read_ps(soft_i2c_bus_t *bus, uint16_t *ps_val)
{
    uint8_t buf[2];

    /* PS 数据在 0x0E (Low) 和 0x0F (High) */
    /* 注意：PS 数据只有低 10 位有效 (High Byte 的 bit0, bit1) */
    if (ap3216c_read_regs(bus, AP3216C_PS_DATA_L, buf, 2) != 0) {
        return -1;
    }

    if (ps_val) {
        uint16_t val = (uint16_t)((buf[1] << 8) | buf[0]);
        /* Mask out invalid bits if necessary, bit 15: IR_OF, bit 14: PS_OF */
        /* 这里保留原始数据，让上层应用处理溢出标志 */
        *ps_val = val;
    }

    return 0;
}