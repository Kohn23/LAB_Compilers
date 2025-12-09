#ifndef ERROR_H
#define ERROR_H

#define MAX_LEN_ERROR_SPEC 64
#define MAX_LEN_ERROR_INFO 256
#define MAX_ERRORS 128

#include<stdio.h>
#include<stdlib.h>

#include"windll.h"

typedef const char* ErrorInfo;

typedef struct {
    ErrorInfo* errors;
    size_t count;
    size_t capacity;
} ErrorLogger;

CORE_API ErrorLogger* init_errorlogger();
CORE_API void free_errorlogger(ErrorLogger* );
CORE_API void fprint_errors(const char* filepath, ErrorLogger* );

#endif // ERROR_H