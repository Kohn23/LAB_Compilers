#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "windll.h"
#include "tok.h"
#include "sym.h"
#include "error.h"


typedef struct {
    size_t current_line;
    size_t current_level;
    char current_proc[MAX_LEN_PROC_NAME];
    int in_function;
}Parser;

CORE_API void recuersive_descent_parse(TokenStream* , VarTable* , ProcTable* , ErrorLogger* );


#endif // PARSER_H
