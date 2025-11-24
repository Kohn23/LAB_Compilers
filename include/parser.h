#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "windll.h"
#include "tok.h"
#include "sym.h"
#include "error.h"

typedef enum {
    PARSE_STATUS_OK,
    PARSE_STATUS_FAILED
}ParseStatus;

typedef struct {
    TokenStream* tokenstream;
    VarTable* vartab;
    ProcTable* proctab;
    ErrorLogger* errlog;

    size_t token_index;
    size_t current_line;
    size_t current_level;
    char current_proc[MAX_LEN_PROC_NAME];
    char last_proc[MAX_LEN_PROC_NAME];
}Parser;

CORE_API void recursive_descent_parse(TokenStream* , VarTable* , ProcTable* , ErrorLogger* );


#endif // PARSER_H
