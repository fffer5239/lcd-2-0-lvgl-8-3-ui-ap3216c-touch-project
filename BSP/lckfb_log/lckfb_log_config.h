#ifndef LOG_CONFIG_H
#define LOG_CONFIG_H

// ============= 用户配置区域 =============
// 是否开启颜色支持
#define LOG_ENABLE_COLOR        1

// 是否显示文件名和行号
#define LOG_SHOW_FILENAME_LINE  0

// 是否使用自定义模块名
#define LOG_USE_CUSTOM_TAG      1

// 是否显示时间戳
#define LOG_SHOW_TIMESTAMP      1

// 是否显示日志级别
#define LOG_USE_LOG_LEVEL       0

// 颜色定义
#if LOG_ENABLE_COLOR
    #define LOG_COLOR_DEBUG     "\033[0m"     // 无颜色
    #define LOG_COLOR_INFO      "\033[0;32m"  // 绿色
    #define LOG_COLOR_WARN      "\033[0;33m"  // 黄色
    #define LOG_COLOR_ERROR     "\033[0;31m"  // 红色
    #define LOG_COLOR_FATAL     "\033[1;31m"  // 粗体红色
    #define LOG_COLOR_RESET     "\033[0m"
#else
    #define LOG_COLOR_DEBUG     ""
    #define LOG_COLOR_INFO      ""
    #define LOG_COLOR_WARN      ""
    #define LOG_COLOR_ERROR     ""
    #define LOG_COLOR_FATAL     ""
    #define LOG_COLOR_RESET     ""
#endif

#endif /* LOG_CONFIG_H */
