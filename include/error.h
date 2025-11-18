#ifndef ERROR_H
#define ERROR_H

#include<stdio.h>
#include<stdlib.h>
#include"windll.h"

// Error handling
#define MAX_ERROR_MSG_LEN 256
#define MAX_ERRORS 128

typedef struct {
    char message[MAX_ERROR_MSG_LEN];
    size_t line;
    char character;
} ErrorInfo;

typedef struct {
    ErrorInfo* errors;
    size_t count;
    size_t capacity;
} ErrorLogger;

CORE_API ErrorLogger* init_errorlogger();
CORE_API void free_errorlogger(ErrorLogger* error_logger);
CORE_API void fprint_errors(const char* filepath, ErrorLogger* error_logger);

#endif // ERROR_H