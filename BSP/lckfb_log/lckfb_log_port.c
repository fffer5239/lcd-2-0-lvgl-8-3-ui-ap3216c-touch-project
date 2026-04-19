#include "lckfb_log_port.h"
#include "main.h"

extern UART_HandleTypeDef huart1;
// 串口发送函数
static void send_char_to_uart(char ch) {
    // 实现串口发送逻辑
    HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);
}

// 获取时间戳函数（需用户实现）
uint32_t get_timestamp_ms(void) {
    // 返回从系统启动开始的毫秒数
    // return system_ticks * TICK_PER_MS;
    return HAL_GetTick(); // 使用HAL库获取系统滴答计数
}

// 初始化函数
void log_port_init(void) {
    log_init(send_char_to_uart);
    log_set_filter(LOG_LVL_DEBUG); // 设置默认日志级别为DEBUG
    LOG_INFO("Log system initialized");
}
