#include "common.h"
#include <string.h>

void log_error(ErrorLogger* error_logger, const char *message, size_t line, const char* character) {
    if (error_logger->count >= error_logger->capacity) {
        size_t new_capacity = error_logger->capacity * 2;
        ErrorInfo* new_errors = (ErrorInfo*)realloc(error_logger->errors, new_capacity * sizeof(ErrorInfo));
        if (!new_errors) {
            // Handle memory allocation failure
            return;
        }
        error_logger->errors = new_errors;
        error_logger->capacity = new_capacity;
    }
    
    ErrorInfo* error = &error_logger->errors[error_logger->count++];
    strncpy(error->message, message, MAX_ERROR_MSG_LEN - 1);
    error->message[MAX_ERROR_MSG_LEN - 1] = '\0';
    error->line = line;
    error->character = *character;
}
