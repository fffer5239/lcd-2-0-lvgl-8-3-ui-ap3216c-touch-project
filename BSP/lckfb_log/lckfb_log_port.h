#ifndef LOG_PORT_H
#define LOG_PORT_H

#include "lckfb_log.h"

// 初始化日志系统（必须在其他日志函数前调用）
void log_port_init(void);

// 获取毫秒时间戳（需要用户实现）
uint32_t get_timestamp_ms(void);

#endif /* LOG_PORT_H */
