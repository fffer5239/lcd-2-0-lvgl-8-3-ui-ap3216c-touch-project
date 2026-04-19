#ifndef __LCKFB_LOG_H__
#define __LCKFB_LOG_H__

#include <stdint.h>
#include <stdarg.h>
#include "lckfb_log_config.h"

/* 日志级别定义 */
typedef enum
{
    LOG_LVL_DEBUG,   /* 调试信息 */
    LOG_LVL_INFO,    /* 常规信息 */
    LOG_LVL_WARN,    /* 警告 */
    LOG_LVL_ERROR,   /* 错误 */
    LOG_LVL_FATAL    /* 致命错误 */
} log_level_t;

/* 字符发送函数的指针类型 */
typedef void (*send_char_func_t)(char ch);

/* 初始化日志系统 */
void log_init(send_char_func_t send_func);

/* 设置日志过滤级别 */
void log_set_filter(log_level_t min_level);

/* 核心日志输出函数 */
void log_output(log_level_t level, const char *tag, const char *file, int line, const char *fmt, ...);

/* 自动选择模块名的日志宏 */
#if LOG_USE_CUSTOM_TAG
    #ifdef LOG_TAG
        #define LOG_DEBUG(fmt, ...) log_output(LOG_LVL_DEBUG, LOG_TAG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
        #define LOG_INFO(fmt, ...)  log_output(LOG_LVL_INFO,  LOG_TAG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
        #define LOG_WARN(fmt, ...)  log_output(LOG_LVL_WARN,  LOG_TAG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
        #define LOG_ERROR(fmt, ...) log_output(LOG_LVL_ERROR, LOG_TAG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
        #define LOG_FATAL(fmt, ...) log_output(LOG_LVL_FATAL, LOG_TAG, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #else
        #define LOG_DEBUG(fmt, ...) log_output(LOG_LVL_DEBUG, "NO_TAG", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
        #define LOG_INFO(fmt, ...)  log_output(LOG_LVL_INFO,  "NO_TAG", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
        #define LOG_WARN(fmt, ...)  log_output(LOG_LVL_WARN,  "NO_TAG", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
        #define LOG_ERROR(fmt, ...) log_output(LOG_LVL_ERROR, "NO_TAG", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
        #define LOG_FATAL(fmt, ...) log_output(LOG_LVL_FATAL, "NO_TAG", __FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #endif
#else
    #define LOG_DEBUG(fmt, ...) log_output(LOG_LVL_DEBUG, NULL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define LOG_INFO(fmt, ...)  log_output(LOG_LVL_INFO,  NULL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define LOG_WARN(fmt, ...)  log_output(LOG_LVL_WARN,  NULL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define LOG_ERROR(fmt, ...) log_output(LOG_LVL_ERROR, NULL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
    #define LOG_FATAL(fmt, ...) log_output(LOG_LVL_FATAL, NULL, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#endif

#endif /* __LCKFB_LOG_H__ */
