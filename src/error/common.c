#include "common.h"
#include <string.h>

void log_error(ErrorLogger* error_logger, ErrorInfo error_info) {
    if (error_logger->count >= error_logger->capacity) {
        size_t new_capacity = error_logger->capacity * 2;
        ErrorInfo* new_errors = (ErrorInfo*)realloc(error_logger->errors, new_capacity * sizeof(ErrorInfo));
        if (!new_errors) {
            perror("Failed to allocate memory for error logger");
            return;
        }
        error_logger->errors = new_errors;
        error_logger->capacity = new_capacity;
    }
    
    // 复制字符串内容，而不是存储指针
    error_logger->errors[error_logger->count] = strdup(error_info);
    if (error_logger->errors[error_logger->count] == NULL) {
        perror("Failed to duplicate error string");
        return;
    }
    
    error_logger->count++;
}