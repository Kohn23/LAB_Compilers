#include "error.h"

// CORE_API ErrorLogger* init_errorlogger() {
//     ErrorLogger* error_logger = (ErrorLogger*)malloc(sizeof(ErrorLogger));
//     if (!error_logger) {
//         return NULL;
//     }
//     error_logger->errors = (ErrorInfo*)malloc(MAX_ERRORS * sizeof(ErrorInfo));
//     if (!error_logger->errors) {
//         free(error_logger);
//         return NULL;
//     }
//     error_logger->count = 0;
//     error_logger->capacity = MAX_ERRORS;
//     return error_logger;
// }

// CORE_API void free_errorlogger(ErrorLogger* error_logger) {
//     if (error_logger->errors) {
//         free(error_logger->errors);
//         error_logger->errors = NULL;
//     }
//     error_logger->count = 0;
//     error_logger->capacity = 0;
// }
CORE_API ErrorLogger* init_errorlogger() {
    ErrorLogger* error_logger = (ErrorLogger*)malloc(sizeof(ErrorLogger));
    if (!error_logger) {
        return NULL;
    }
    
    error_logger->errors = (ErrorInfo*)calloc(MAX_ERRORS, sizeof(ErrorInfo));
    if (!error_logger->errors) {
        free(error_logger);
        return NULL;
    }
    
    error_logger->count = 0;
    error_logger->capacity = MAX_ERRORS;
    return error_logger;
}

CORE_API void free_errorlogger(ErrorLogger* error_logger) {
    if (!error_logger) return;
    
    if (error_logger->errors) {
    
        for (size_t i = 0; i < error_logger->count; i++) {
            free((void*)error_logger->errors[i]);  
        }
        free(error_logger->errors); 
        error_logger->errors = NULL;
    }
    
    error_logger->count = 0;
    error_logger->capacity = 0;
    free(error_logger); 
}

CORE_API void fprint_errors(const char* filepath, ErrorLogger* error_logger) {
    FILE* file = fopen(filepath, "w");
    if (!file) {
        perror("Error opening error log file");
        return;
    }
    
    for (size_t i = 0; i < error_logger->count; i++) {
        fprintf(file, "%s\n", error_logger->errors[i]);
    }
    
    fclose(file);
}
