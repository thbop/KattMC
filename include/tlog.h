#ifndef TLOG_H
#define TLOG_H
// A small logging library

#include "stdio.h"
#include "time.h"
#include "stdarg.h"
#include "stdbool.h"
#include "string.h"

#define TLOG_MAX_MSG_SIZE 64

// Example:
//     logs/2025-03-03.log
char _tlog_filename[20];
FILE *_tlog_file;

bool TLogInit() {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);

    strftime(_tlog_filename, sizeof(_tlog_filename), "logs/%Y-%m-%d.log", tm_info);

    _tlog_file = fopen(_tlog_filename, "a");
    if (_tlog_file == NULL) {
        printf("ERROR: Failed to open log file!\n");
        return false;
    }
    
    return true;
}


void TLog( const char *fmt, ... ) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);

    char message[TLOG_MAX_MSG_SIZE] = {0};
    vsnprintf(message, TLOG_MAX_MSG_SIZE, fmt, args);

    if ( _tlog_file != NULL )
        fwrite(message, sizeof(char), strlen(message), _tlog_file);

    va_end(args);
}

void TLogClose() {
    if ( _tlog_file != NULL )
        fclose(_tlog_file);
}

#endif