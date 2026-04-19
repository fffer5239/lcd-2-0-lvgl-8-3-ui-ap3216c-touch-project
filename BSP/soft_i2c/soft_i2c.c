/*
 * soft_i2c - 可移植软件 I2C（位带/比特翻转）核心实现
 */
#include "soft_i2c.h"
#include <stdio.h>

#define LOG_TAG "SOFT_I2C"
#include "lckfb_log_port.h"

/* 等待 SCL 实际变为高电平（处理从机时钟拉伸），带超时（微秒） */
static int soft_i2c_wait_scl_high(soft_i2c_bus_t *bus)
{
    uint32_t waited = 0;
    while (!soft_i2c_read_scl(bus))
    {
        soft_port_udelay(1);
        if (++waited >= bus->stretch_timeout_us)
            return -1; /* 超时 */
    }
    return 0;
}

/* 产生 START 条件：SCL 为高时，SDA 由高变低 */
int soft_i2c_start(soft_i2c_bus_t *bus)
{
    /* 空闲态：SCL/SDA 释放为高 */
    soft_i2c_sda_release(bus);
    soft_i2c_scl_release(bus);
    if (soft_i2c_wait_scl_high(bus) != 0) return -1;
    soft_i2c_delay_half(bus);

    /* START：SDA 拉低 */
    soft_i2c_sda_low(bus);
    soft_i2c_delay_half(bus);

    /* 拉低 SCL 开始传输 */
    soft_i2c_scl_low(bus);
    soft_i2c_delay_half(bus);
    return 0;
}

/* 产生 STOP 条件：SCL 为高时，SDA 由低变高 */
void soft_i2c_stop(soft_i2c_bus_t *bus)
{
    soft_i2c_sda_low(bus);
    soft_i2c_delay_half(bus);

    soft_i2c_scl_release(bus);
    (void)soft_i2c_wait_scl_high(bus);
    soft_i2c_delay_half(bus);

    soft_i2c_sda_release(bus);
    soft_i2c_delay_half(bus);
}

/* 写 1 位（MSB 优先序列中的单比特） */
static int soft_i2c_write_bit(soft_i2c_bus_t *bus, int bit)
{
    if (bit) soft_i2c_sda_release(bus);
    else     soft_i2c_sda_low(bus);

    soft_i2c_delay_half(bus);

    soft_i2c_scl_release(bus);
    if (soft_i2c_wait_scl_high(bus) != 0) return -1;
    soft_i2c_delay_half(bus);

    soft_i2c_scl_low(bus);
    soft_i2c_delay_half(bus);
    return 0;
}

/* 读 1 位 */
static int soft_i2c_read_bit(soft_i2c_bus_t *bus)
{
    /* 释放 SDA 交给从机驱动 */
    soft_i2c_sda_release(bus);
    soft_i2c_delay_half(bus);

    soft_i2c_scl_release(bus);
    if (soft_i2c_wait_scl_high(bus) != 0) return -1;
    soft_i2c_delay_half(bus);

    int bit = soft_i2c_read_sda(bus) ? 1 : 0;

    soft_i2c_scl_low(bus);
    soft_i2c_delay_half(bus);

    return bit;
}

/* 写 1 字节，返回 0 表示收到 ACK，-1 表示 NACK 或错误 */
int soft_i2c_write_byte(soft_i2c_bus_t *bus, uint8_t byte)
{
    for (int i = 7; i >= 0; --i)
    {
        if (soft_i2c_write_bit(bus, (byte >> i) & 0x1) != 0)
            return -1;
    }
    /* 读取 ACK 位：0=ACK，1=NACK */
    int ack = soft_i2c_read_bit(bus);
    if (ack < 0) return -1;
    return (ack == 0) ? 0 : -1;
}

/* 读 1 字节；ack=true 则发送 ACK（表示后续还有数据），ack=false 发送 NACK（最后 1 字节） */
int soft_i2c_read_byte(soft_i2c_bus_t *bus, uint8_t *byte, bool ack)
{
    if (!byte) return -1;

    uint8_t val = 0;
    for (int i = 7; i >= 0; --i)
    {
        int bit = soft_i2c_read_bit(bus);
        if (bit < 0) return -1;
        val |= (uint8_t)(bit << i);
    }
    /* 发送 ACK/NACK */
    if (soft_i2c_write_bit(bus, ack ? 0 : 1) != 0)
        return -1;

    *byte = val;
    return 0;
}

/* 根据速率（Hz）推导半周期延时（微秒） */
static uint16_t soft_i2c_calc_half_us(uint32_t speed_hz)
{
    if (speed_hz == 0) speed_hz = SOFT_I2C_DEFAULT_SPEED_HZ;
    /* half_us ≈ 1e6 / (2*speed) 的四舍五入实现，至少为 1us */
    uint32_t half_us = (500000u + (speed_hz / 2u)) / speed_hz;
    if (half_us == 0) half_us = 1;
    if (half_us > 1000u) half_us = 1000u; /* 保护 */
    return (uint16_t)half_us;
}

/* 初始化：配置 GPIO 时钟与模式为开漏输出，上拉，高速；并计算半周期 */
void soft_i2c_init(soft_i2c_bus_t *bus)
{
    if (!bus) return;

    if (bus->speed_hz == 0)
        bus->speed_hz = SOFT_I2C_DEFAULT_SPEED_HZ;

    bus->t_half_us = soft_i2c_calc_half_us(bus->speed_hz);
    if (bus->stretch_timeout_us == 0)
        bus->stretch_timeout_us = SOFT_I2C_STRETCH_TIMEOUT_US;

    /* 使能端口时钟并配置为开漏输出（便于快速翻转） */
    soft_port_gpio_clock_enable(bus->scl_port);
    soft_port_gpio_clock_enable(bus->sda_port);

    soft_port_gpio_config_od(bus->scl_port, bus->scl_pin, /*pullup=*/1);
    soft_port_gpio_config_od(bus->sda_port, bus->sda_pin, /*pullup=*/1);

    /* 空闲态拉高 */
    soft_i2c_scl_release(bus);
    soft_i2c_sda_release(bus);

    /* 稍作稳定 */
    soft_port_udelay(5);
}

/* 简单写事务：START, SLA+W, data..., STOP；成功返回 0 */
int soft_i2c_write(soft_i2c_bus_t *bus, uint8_t addr7, const uint8_t *buf, size_t len)
{
    if (!bus) return -1;
    if (soft_i2c_start(bus) != 0) goto err;

    uint8_t addr_w = (uint8_t)((addr7 << 1) | 0u);
    if (soft_i2c_write_byte(bus, addr_w) != 0) goto err;

    for (size_t i = 0; i < len; ++i)
    {
        if (soft_i2c_write_byte(bus, buf[i]) != 0) goto err;
    }
    soft_i2c_stop(bus);
    return 0;
err:
    soft_i2c_stop(bus);
    return -1;
}

/* 简单读事务：START, SLA+R, 读 N 字节, STOP；成功返回 0 */
int soft_i2c_read(soft_i2c_bus_t *bus, uint8_t addr7, uint8_t *buf, size_t len)
{
    if (!bus || (!buf && len)) return -1;
    if (soft_i2c_start(bus) != 0) goto err;

    uint8_t addr_r = (uint8_t)((addr7 << 1) | 1u);
    if (soft_i2c_write_byte(bus, addr_r) != 0) goto err;

    for (size_t i = 0; i < len; ++i)
    {
        bool ack = (i + 1u) < len; /* 非最后一字节发送 ACK，最后一字节发送 NACK */
        if (soft_i2c_read_byte(bus, &buf[i], ack) != 0) goto err;
    }
    soft_i2c_stop(bus);
    return 0;
err:
    soft_i2c_stop(bus);
    return -1;
}

/* 扫描 I2C 设备，以“表格”打印（0x00~0x7F，实际上仅探测 0x03~0x77） */
void soft_i2c_scan(soft_i2c_bus_t *bus)
{
    if (!bus) return;

    LOG_INFO("=== 软件 I2C 扫描开始 ===");
    LOG_INFO("速率: %lu Hz (t_half=%u us)",
             (unsigned long)bus->speed_hz, (unsigned)bus->t_half_us);
    LOG_INFO("    0  1  2  3  4  5  6  7  8  9  a  b  c  d  e  f");

    char line_buf[64];
    for (uint8_t row = 0x00; row <= 0x70; row += 0x10)
    {
        int remain = (int)sizeof(line_buf);
        int written = snprintf(line_buf, sizeof(line_buf), "%02x: ", row);
        if (written < 0) written = 0;
        if (written < remain) remain -= written;
        char *cursor = line_buf + written;

        for (uint8_t col = 0; col < 0x10; ++col)
        {
            int used = 0;
            uint8_t addr = (uint8_t)(row | col);
            char cell_str[4] = "-- ";

            if (addr >= 0x03 && addr <= 0x77)
            {
                int acked = -1;
                if (soft_i2c_start(bus) == 0)
                {
                    uint8_t addr_w = (uint8_t)(addr << 1);
                    acked = soft_i2c_write_byte(bus, addr_w);
                    soft_i2c_stop(bus);
                }

                if (acked == 0)
                {
                    snprintf(cell_str, sizeof(cell_str), "%02x ", addr);
                }

                /* 适当间隔，避免过快 */
                soft_port_udelay(120);
            }

            if (remain > 0)
            {
                used = snprintf(cursor, (size_t)remain, "%s", cell_str);
                if (used < 0) used = 0;
                if (used < remain)
                {
                    cursor += used;
                    remain -= used;
                }
                else
                {
                    /* 缓冲区不足，提前结束该行 */
                    cursor = line_buf + sizeof(line_buf) - 1;
                    *cursor = '\0';
                    remain = 0;
                }
            }
        }

        LOG_INFO("%s", line_buf);
    }

    LOG_INFO("=== 软件 I2C 扫描结束 ===");
}