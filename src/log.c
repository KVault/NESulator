
#include "log.h"

enum ConsoleLogLevelEnum console_log_level = ConsoleDisabled;
enum FileLogLevelEnum file_log_level = FileInfo;

char *log_file_path;
int clean_file = 0;
FILE *file;


void vlog(const char *format, enum ConsoleLogLevelEnum cmin_level, enum FileLogLevelEnum fmin_level,va_list args) {
	if (console_log_level >= cmin_level) {
		vfprintf(stdout, format, args);
	}

	if(file_log_level >= fmin_level){
		log_to_file(format, args);
	}
}

void log_info(const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vlog(format, ConsoleInfo, FileInfo, arg);
	va_end (arg);
}

void log_debug(const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vlog(format, ConsoleDebug, FileDebug, arg);
	va_end (arg);
}

void log_error(const char *format, ...) {
	va_list arg;
	va_start (arg, format);
	vlog(format, ConsoleError, FileError, arg);
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

void set_console_log_level(enum ConsoleLogLevelEnum level) {
	console_log_level = level;
}

void set_file_log_level(enum FileLogLevelEnum level){
	file_log_level = level;
}

void set_clear_log_file() {
	clean_file = 1;
}
