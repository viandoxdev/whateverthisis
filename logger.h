#ifndef __LOGGER_H
#define __LOGGER_H
#include "vector.h"
#include <stdio.h>

#define LOGGER_COMPACT

struct Logger {
	int verbosity;
	// Vector<FILE*>
	Vector outputs;
};

enum LogLevel {
	Debug =  0x1,
	Info  =  0x2,
	Warn  =  0x4,
	Error =  0x8,
	Fatal = 0x10
};

typedef struct Logger Logger;
typedef enum LogLevel LogLevel;


Logger logger_new();
void logger_toggle_level(Logger* lg, LogLevel level);
void logger_enable_all(Logger* lg);
void logger_disable_all(Logger* lg);
void logger_add_output(Logger* lg, FILE* output);
void logger_remove_output(Logger* lg, FILE* output);
void logger_log(Logger lg, LogLevel level, char* content, char* info);
void logger_debug(Logger lg, char* content, char* info);
void logger_info(Logger lg, char* content, char* info);
void logger_warn(Logger lg, char* content, char* info);
void logger_error(Logger lg, char* content, char* info);
void logger_fatal(Logger lg, char* content, char* info);
void logger_free(Logger lg);

#endif
