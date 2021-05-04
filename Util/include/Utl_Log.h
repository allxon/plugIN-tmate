//
//  Copyright 2010 iGware Inc.
//  All Rights Reserved.
//
//  THIS SOFTWARE CONTAINS CONFIDENTIAL INFORMATION AND
//  TRADE SECRETS OF IGWARE INC.
//  USE, DISCLOSURE OR REPRODUCTION IS PROHIBITED WITHOUT
//  THE PRIOR EXPRESS WRITTEN PERMISSION OF IGWARE INC.
//
#ifndef __UTL_LOG_H__
#define __UTL_LOG_H__

#include "Comm.h"
#include <stdarg.h>

typedef enum {
	UTL_LOG_LEVEL_TRACE = 0,
	UTL_LOG_LEVEL_DEBUG,
	UTL_LOG_LEVEL_INFO,
	UTL_LOG_LEVEL_WARN,
	UTL_LOG_LEVEL_ERROR,
	UTL_LOG_LEVEL_CRITICAL,
	UTL_LOG_LEVEL_ALWAYS,
	UTL_LOG_NUM_LEVELS,
} UTL_LOGLevel;

#define UTL_LOG_OK              0
#define UTL_LOG_ERR_PARAMETER   -1
#define UTL_LOG_ERR_OPEN        -2
#define UTL_LOG_ERR_SHM         -3

#define UTL_KMSG_FILENO 3

#define UTL_STDIN_FILENO    0
#define UTL_STDOUT_FILENO   1
#define UTL_STDERR_FILENO   2

#define UTLFILE_INVALID_HANDLE -1
/// Any and all processes must call this function first.
/// @param root Root directory for app data. "/logs/<process name>/" will be appended to this.
///     If NULL, we will not write logs to the filesystem.
void UTL_LOGInit(const char* processName, const char* root);

void UTL_LOGSetLevel(UTL_LOGLevel level, unsigned char enable);

void UTL_LOGStartSpecialLog(const char *logName, unsigned int max);

void UTL_LOGStopSpecialLogs();      // Free up resources used by special logs

/// @param maxSize Maximum log size. Use default if 0.
void UTL_LOGSetMax(unsigned int maxSize);

/// Pass VPL_FALSE to disable logging to stdout/stderr.
/// This can be useful if the output will be discarded (for GUI apps for example)
/// or to help detect messages that aren't being sent to the log file.
/// @return The previous setting.
bool UTL_LOGSetWriteToStdout(bool write);

/// Pass VPL_FALSE to disable logging to a file.
/// @return The previous setting.
bool UTL_LOGSetWriteToFile(bool write);

/// Pass VPL_FALSE to disable logging to the system log (currently only applies to Android).
/// @return The previous setting.
bool UTL_LOGSetWriteToSystemLog(bool write);

/// Pass VPL_TRUE to enable in-memory logging.
/// In-memory logging is only expected to be used by the Orbe CCD.
/// If writeToFile is also true, then in-memory logging will cache the logs into
/// a shared memory region first.
int UTL_LOGSetEnableInMemoryLogging(bool enable, int id);

/// Flush the in-memory logs.
void UTL_LOGFlushInMemoryLogs();

void UTL_LOGPrint(UTL_LOGLevel level, const char* file, int line, const char* function,
        const char* format, ...) ATTRIBUTE_PRINTF(5, 6);

void UTL_LOGVPrint(UTL_LOGLevel level, const char* file, int line, const char* function, const char* format, va_list ap);

#define UTL___LOG(level, fmt, ...) do {                             \
	UTL_LOGPrint(level, __FILE__, __LINE__, __func__, fmt, ##__VA_ARGS__);    \
	} while (0)

#define UTL___LOGV(level, fmt, ap) do {                             \
	UTL_LOGVPrint(level, __FILE__, __LINE__, __func__, fmt, ap);    \
	} while (0)

#define UTIL_LOG_SENSITIVE_STRING_INFO(prefix_, msg_, suffix_) \
	Util_LogSensitiveString((prefix_), (msg_), (suffix_), __FILE__, __LINE__, __func__)

void Util_LogSensitiveString(const char* prefix, const char* msg, const char* suffix, const char* file, int line, const char* function);
char* UTLString_strnstr(const char* haystack, const char* needle, size_t n);

#define UTL_LOG_ENABLE_LEVEL(level)     UTL_LOGSetLevel(level, 1)
#define UTL_LOG_DISABLE_LEVEL(level)    UTL_LOGSetLevel(level, 0)

// For things that are expected to be logged repeatedly during normal use,
// even when nothing is interacting with the machine.
#define UTL_LOG_TRACE(...)				UTL___LOG(UTL_LOG_LEVEL_TRACE, ##__VA_ARGS__)

#define UTL_LOG_DEBUG(...)			UTL___LOG(UTL_LOG_LEVEL_DEBUG, ##__VA_ARGS__)
#define UTL_LOG_INFO(...)				UTL___LOG(UTL_LOG_LEVEL_INFO, ##__VA_ARGS__)
#define UTL_LOG_WARN(...)				UTL___LOG(UTL_LOG_LEVEL_WARN, ##__VA_ARGS__)
#define UTL_LOG_ERROR(...)			UTL___LOG(UTL_LOG_LEVEL_ERROR, ##__VA_ARGS__)
#define UTL_LOG_CRITICAL(...)			UTL___LOG(UTL_LOG_LEVEL_CRITICAL, ##__VA_ARGS__)

// For important info that doesn't indicate a problem.
#define UTL_LOG_ALWAYS(...)			UTL___LOG(UTL_LOG_LEVEL_ALWAYS, ##__VA_ARGS__)

#define UTL_LOG_FUNC_ENTRY(level)		UTL___LOG(level, "%s", __func__)

#endif // __LOG_H__

