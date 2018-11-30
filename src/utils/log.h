#ifndef NESULATOR_LOG_H
#define NESULATOR_LOG_H

#include <stdio.h>
#include <stdarg.h>

enum ConsoleLogLevelEnum {
	ConsoleDisabled = -1, ConsoleError = 1, ConsoleInfo = 2, ConsoleDebug = 3
};

enum FileLogLevelEnum {
	FileDisabled = -1, FileError = 1, FileInfo = 2, FileDebug = 3
};

void vlog(const char *format, enum ConsoleLogLevelEnum cmin_level, enum FileLogLevelEnum fmin_level,va_list args);

void log_info(const char *log, ...);

void log_debug(const char *log, ...);

void log_error(const char *log, ...);

void log_to_file(const char *log, va_list args);

void set_console_log_level(enum ConsoleLogLevelEnum level);

void set_file_log_level(enum FileLogLevelEnum level);

void set_log_path(const char *path);

void set_clear_log_file();

#endif //NESULATOR_LOG_H
