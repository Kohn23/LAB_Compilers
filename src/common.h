#ifndef COMMON_H
#define COMMON_H

#include "error.h"
#include "tok.h"

// error
void log_error(ErrorLogger* error_logger, const char *message, size_t line, const char* character);
// tok
TokenStream* init_tokenstream();

#endif // COMMON_H