#include "lckfb_log.h"
#include "lckfb_log_port.h"
#include <string.h>
#include <stdio.h>

/* 静态全局变量 */
static send_char_func_t g_send_char = NULL;
static log_level_t g_min_level = LOG_LVL_DEBUG;
static uint8_t g_use_timestamp = LOG_SHOW_TIMESTAMP;

void log_init(send_char_func_t send_func)
{
    g_send_char = send_func;
}

void log_set_filter(log_level_t min_level)
{
    g_min_level = min_level;
}

/* 内部辅助函数：发送字符串 */
static void send_string(const char *str)
{
    if (g_send_char != NULL && str != NULL)
    {
        while (*str)
        {
            g_send_char(*str++);
        }
    }
}

/* 将日志级别转换为字符串和颜色 */
static void get_level_info(log_level_t level, const char **str, const char **color)
{
    static const char *strings[] = 
    {
        "DEBUG", "INFO", "WARN", "ERROR", "FATAL"
    };
    
    static const char *colors[] = 
    {
        LOG_COLOR_DEBUG, 
        LOG_COLOR_INFO, 
        LOG_COLOR_WARN, 
        LOG_COLOR_ERROR, 
        LOG_COLOR_FATAL
    };
    
    *str = strings[level];
    *color = colors[level];
}

/* 优化实现：文件名提取 */
static const char *get_basename(const char *path)
{
    const char *name = strrchr(path, '/');
    const char *alt_name = strrchr(path, '\\');
    
    if (alt_name && (!name || alt_name > name)) 
        name = alt_name;
    
    return name ? name + 1 : path;
}

void log_output(log_level_t level, const char *tag, const char *file, int line, const char *fmt, ...)
{
    if ((g_send_char == NULL) || (level < g_min_level)) 
    {
        return;
    }
    
    const char *level_str, *level_color;
    get_level_info(level, &level_str, &level_color);
    
    char header[128];
    char *p = header;
    int remain = sizeof(header);
    int ret;
    
    /* 添加颜色 */
    ret = snprintf(p, remain, "%s", level_color);
    if (ret > 0) { p += ret; remain -= ret; }
    
    /* 添加时间戳 */
    if (g_use_timestamp) {
        uint32_t timestamp = get_timestamp_ms();
        ret = snprintf(p, remain, "[%u.%03u] ", timestamp/1000, timestamp%1000);
        if (ret > 0 && ret < remain) { p += ret; remain -= ret; }
    }
    
    /* 添加模块名 */
    if (tag != NULL) {
        ret = snprintf(p, remain, "[%s] ", tag);
        if (ret > 0 && ret < remain) { p += ret; remain -= ret; }
    }
    
    /* 添加文件名和行号 */
#if LOG_SHOW_FILENAME_LINE
    const char *base_name = get_basename(file);
    ret = snprintf(p, remain, "[%s:%d] ", base_name, line);
    if (ret > 0 && ret < remain) { p += ret; remain -= ret; }
#endif
    
#if LOG_USE_LOG_LEVEL
    /* 添加日志级别 */
    ret = snprintf(p, remain, "[%s] ", level_str);
    if (ret > 0 && ret < remain) { p += ret; remain -= ret; }
#endif

    /* 发送头部信息 */
    send_string(header);
    
    /* 格式化用户消息 */
    char msg_buf[256];
    va_list args;
    va_start(args, fmt);
    vsnprintf(msg_buf, sizeof(msg_buf), fmt, args);
    va_end(args);
    
    /* 发送消息和换行 */
    send_string(msg_buf);
    send_string(LOG_COLOR_RESET); // 重置颜色
    send_string("\r\n");
}
