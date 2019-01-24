/*
** Copyright (C) 2015 Wang Yaofu
** All rights reserved.
**
**Author:Wang Yaofu voipman@qq.com
**Description: The header file of class LogWriter.
*/
#pragma once

#include <string.h>
#include <errno.h>
#include "logthread.h"
#include "localdef.h"

#define LOG_FATAL(fmt, arg...) do { \
    LOGTHD()->writeLog(__FILE__, __LINE__, __FUNCTION__, LL_ERROR, fmt, ## arg); \
} while (0)

#define LOG_ERROR(fmt, arg...) do { \
    LOGTHD()->writeLog(__FILE__, __LINE__, __FUNCTION__, LL_ERROR, fmt, ## arg); \
} while (0)

#define LOG_WARN(fmt, arg...) do { \
    LOGTHD()->writeLog(__FILE__, __LINE__, __FUNCTION__, LL_WARN, fmt, ## arg); \
} while (0)

#define LOG_NOTICE(fmt, arg...) do { \
    LOGTHD()->writeLog(__FILE__, __LINE__, __FUNCTION__, LL_NOTICE, fmt, ## arg); \
} while (0)

#define LOG_INFO(fmt, arg...) do { \
    LOGTHD()->writeLog(__FILE__, __LINE__, __FUNCTION__, LL_INFO, fmt, ## arg); \
} while (0)

#define LOG_VARS(fmt, arg...) do { \
    LOGTHD()->writeLog(__FILE__, __LINE__, __FUNCTION__, LL_VARS, fmt, ## arg); \
} while (0)

#define LOG_DEBUG(fmt, arg...) do { \
    LOGTHD()->writeLog(__FILE__, __LINE__, __FUNCTION__, LL_DBG, fmt, ## arg); \
} while (0)

#define LOG_ALL(fmt, arg...) do { \
    LOGTHD()->writeLog(__FILE__, __LINE__, __FUNCTION__, LL_ALL, fmt, ## arg); \
} while (0)

#ifdef DEBUG_LOG

#define LOG_RAW(level, buf) do { \
    LOGTHD()->logRaw(level, buf); \
} while (0)

#define LOG_HEX_ASC(level, buf, len) do { \
    LOGTHD()->logHexAsc(__FILE__, __LINE__, __FUNCTION__, level, buf, len); \
} while (0)
#else
#define LOG_RAW //
#define LOG_HEX_ASC //
#endif

#ifdef _DEBUG
#define DEBUG(level, fmt, arg...) do { \
    LOGTHD()->writeLog(__FILE__, __LINE__, __FUNCTION__, level, fmt, ## arg); \
} while (0)
#define LOG_FUNC(level, fmt, arg...) do { \
    LOGTHD()->writeLog(__FILE__, __LINE__, __FUNCTION__, level, fmt, ## arg); \
} while (0)

#define DEBUG_RAW(level, buf) do { \
    LOGTHD()->logRaw(level, buf); \
} while (0)

#define DEBUG_HEX_ASC(level, buf, len) do { \
    LOGTHD()->logHexAsc(__FILE__, __LINE__, __FUNCTION__, level, buf, len); \
} while (0)
#else
#define DEBUG //
#define DEBUG_RAW //
#define DEBUG_HEX_ASC //
#define LOG_FUNC //
#endif

// for function log begin and end.
#define DEBUG_BEGIN DEBUG(LL_ALL, "Begin");
#define DEBUG_END   DEBUG(LL_ALL, "End");

#define LOG_COST(level) ScopeCostLog ScopeCostLog##level(__FUNCTION__, level);

// for c++ stream log
#define log_fatal(errno, content) do { \
    std::ostringstream o; \
    o << " errno:"<<errno<<" - "<< content; \
    LOG_FATAL("%s", o.str().c_str()); \
} while (0)

#define log_error(errno, content) do { \
    std::ostringstream o; \
    o << " errno:"<<errno<<" - "<< content; \
    LOG_ERROR("%s", o.str().c_str()); \
} while (0)

#define log_warn(errno, content) do { \
    std::ostringstream o; \
    o << " errno:"<<errno<<" - "<< content; \
    LOG_WARN("%s", o.str().c_str()); \
} while (0)

#define log_info(content) do { \
    std::ostringstream o; \
    o << content; \
    LOG_INFO("%s", o.str().c_str()); \
} while (0)

#define log_notice(content) do { \
    std::ostringstream o; \
    o << content; \
    LOG_NOTICE("%s", o.str().c_str()); \
} while (0)

#define log_debug(content) do { \
    std::ostringstream o; \
    o << content; \
    LOG_DEBUG("%s", o.str().c_str()); \
} while (0)

#define ERRNO_HTTP_ERR 500
#define ERRNO_OK       200