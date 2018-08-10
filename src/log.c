
#include "log.h"

enum LogLevelEnum log_level = Disabled;

char *log_file_path;
int clean_file = 0;
FILE *file;


void vlog(const char *format, enum LogLevelEnum minLevel, va_list args) {
	if (log_level >= minLevel) {
		vfprintf(stdout, format, args);
		log_to_file(format, args);
	}
}

void log_info(const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vlog(format, Info, arg);
	va_end (arg);
}

void log_debug(const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vlog(format, Debug, arg);
	va_end (arg);
}

void log_error(const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vlog(format, Error, arg);
	va_end (arg);
}


void log_to_file(const char *log, va_list args) {
	if (file == NULL) {
		if (clean_file) {//This is the way to clear a file. Open it in read mode, not update and it'll do the job
			file = fopen(log_file_path, "w");
		} else {
			file = fopen(log_file_path, "w+");
		}
	}
	vfprintf(file, log, args);
}

void set_log_path(const char *path) {
	log_file_path = (char *) path;
}

void set_log_level(enum LogLevelEnum level) {
	log_level = level;
}

void set_clear_log_file() {
	clean_file = 1;
}
