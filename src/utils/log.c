#include <malloc.h>
#include "log.h"

enum ConsoleLogLevelEnum console_log_level = ConsoleDisabled;
enum FileLogLevelEnum file_log_level = FileInfo;

char *log_file_path;
int clean_file = 0;
FILE *file;

/// Number of logs to be stored before triggering a dump to file
#define FILE_BUFFER_SIZE 100
/// Max Size in chars that a log can have
const int log_buffer_size = 1000;
/// Used to fill in before dumping to file, avoiding dumping every single log
char *file_log_buffer[FILE_BUFFER_SIZE];
/// Next position to be filled by the log
int buffer_possition = 0;
int buffer_created = 0;

void setup_buffer(){
	buffer_created = 1;
	for (int i = 0; i < FILE_BUFFER_SIZE; ++i) {
		file_log_buffer[i] = malloc(log_buffer_size * sizeof(char));
	}
}

//TODO this isn't really working. change it to use a single buffer and just one write to file
void buffer_log(const char *format, va_list args){
	if(buffer_possition < FILE_BUFFER_SIZE){
		sprintf(file_log_buffer[buffer_possition], format, args);
		++buffer_possition;
	}else{
		for (int i = 0; i < FILE_BUFFER_SIZE; ++i) {
			fprintf(file, file_log_buffer[i]);
		}
		buffer_possition = 0;

		//Make a recursive call now to buffer the log again. At this point the log hasn't been buffered
		buffer_log(format, args);
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
