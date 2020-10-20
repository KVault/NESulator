#include <stdlib.h>
#include <memory.h>
#include "log.h"

enum ConsoleLogLevelEnum console_log_level = ConsoleDisabled;
enum FileLogLevelEnum file_log_level = FileInfo;

char *log_file_path;
int clean_file = 0;
FILE *file;

// Total size in bytes the file buffer has
#define LOG_BUFFER_SIZE 4096
/// Used to fill in before dumping to file, avoiding dumping every single log
char *file_log_buffer;
/// Next position to be filled by the log
int buffer_position = 0;
int buffer_created = 0;
int chars_written = 0;

void setup_buffer(){
	buffer_created = 1;
	file_log_buffer = malloc(LOG_BUFFER_SIZE * sizeof(char));
}

void dump_to_file(){
	buffer_position = chars_written = 0;
	fwrite(file_log_buffer, sizeof(char), LOG_BUFFER_SIZE, file);
}

/**
 * Logs to a buffer until it fills, then it dumps the buffer into a file and releases the buffer
 */
void buffer_log(const char *format, va_list args){
	// The buffer is full. Empty it by dumping to file
	if(buffer_position >=  LOG_BUFFER_SIZE){
		dump_to_file();
	}

	//Log to the buffer. Keep track of the position with buffer_position
	chars_written = vsprintf(&file_log_buffer[buffer_position], format, args);

	/** This means success. sprintf returns characters written on success
	 *  and there's still more room in the buffer
	*/
	if(chars_written >= 0) {
		buffer_position += chars_written;
	}else{// Has failed, probably not enough space so dump it to file
		dump_to_file();
	}
}


void vlog(const char *format, enum ConsoleLogLevelEnum cmin_level, enum FileLogLevelEnum fmin_level,va_list args) {
	if(buffer_created == 0){
		setup_buffer();
	}

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
	buffer_log(log, args);
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
