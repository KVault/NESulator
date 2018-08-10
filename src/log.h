#ifndef NESULATOR_LOG_H
#define NESULATOR_LOG_H

#include <stdio.h>
#include <zconf.h>

enum LogLevelEnum {
	Disabled = -1, Error = 1, Info = 2, Debug = 3
};

void vlog(const char *format, enum LogLevelEnum minLevel, va_list args);

void log_info(const char *log, ...);

void log_debug(const char *log, ...);

void log_error(const char *log, ...);

void log_to_file(const char *log, va_list args);

void set_log_level(enum LogLevelEnum level);

void set_log_path(const char *path);

void set_clear_log_file();

#endif //NESULATOR_LOG_H
