#include "logger.h"
#include "vector.h"

Logger logger_new() {
	Logger res = {};
	res.outputs = vector_new(3, sizeof(FILE*));
	res.verbosity = 0;
	return res;
}

void logger_toggle_level(Logger* lg, LogLevel level) {
	lg->verbosity ^= level;
}

void logger_enable_all(Logger* lg) {
	lg->verbosity = Debug | Info | Warn | Error | Fatal;
}

void logger_disable_all(Logger* lg) {
	lg->verbosity = 0;
}

void logger_add_output(Logger* lg, FILE* output) {
	vector_push(&lg->outputs, &output);
}

void logger_remove_output(Logger* lg, FILE* output) {
	signed int index = -1;
	for(int i = 0; i < lg->outputs.used; i++) {
		FILE** fptr = vector_get_ptr(&lg->outputs, i);
		if(*fptr == output) {
			index = i;
			break;
		}
	}
	if(index != -1) {
		vector_remove(&lg->outputs, index, NULL);
	}
}

char* logger_get_level_string(LogLevel level) {
#ifdef LOGGER_COMPACT
	switch(level) {
		case Debug:
			return "\033[32m[DBG]";
		case Info:
			return "\033[36m[INF]";
		case Warn:
			return "\033[33m[WRN]";
		case Error:
			return "\033[1m\033[31m[ERR]";
		case Fatal:
			return "\033[101m\033[97m[FTL]";
	}
	return "[UKN]";
#else
	switch(level) {
		case Debug:
			return "\033[32m[DEBUG]";
		case Info:
			return "\033[36m[INFO]";
		case Warn:
			return "\033[33m[WARN]";
		case Error:
			return "\033[1m\033[31m[ERROR]";
		case Fatal:
			return "\033[101m\033[97m[FATAL]";
	}
	return "[UNKNOWN]";
#endif
}

void logger_log(Logger lg, LogLevel level, char* content, char* info) {
	if(!(lg.outputs.used)) return; // if no output, skip
	if(!(lg.verbosity & level)) return; // if logger is not verbose enough, skip
	if(info != NULL) {
		printf("[LOGGER] %s\033[0m\033[2m\033[38;5;238m (%s)\033[0m: %s\n", logger_get_level_string(level), info, content);
	} else {
		printf("[LOGGER] %s\033[0m: %s\n", logger_get_level_string(level), content);
	}
}

void logger_debug(Logger lg, char* content, char* info) {
	logger_log(lg, Debug, content, info);
}
void logger_info(Logger lg, char* content, char* info) {
	logger_log(lg, Info, content, info);
}
void logger_warn(Logger lg, char* content, char* info) {
	logger_log(lg, Warn, content, info);
}
void logger_error(Logger lg, char* content, char* info) {
	logger_log(lg, Error, content, info);
}
void logger_fatal(Logger lg, char* content, char* info) {
	logger_log(lg, Fatal, content, info);
}

void logger_free(Logger lg) {
	vector_free(&lg.outputs);
}
