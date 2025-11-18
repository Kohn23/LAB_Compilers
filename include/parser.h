#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <stdlib.h>

#include "windll.h"
#include "sym.h"
#include "tok.h"

typedef struct {
    Token* tokens;
    int token_count;
    int current_index;
    int error_count;
    
    int source_line;            
    
    VarTable* var_table;
    ProcTable* proc_table;
    
    char current_proc[MAX_LEN_PROC_NAME];
    int current_level;
    int in_function;
    
    FILE* err_file;
    FILE* var_file;
    FILE* pro_file;
} ParserState;



#endif // PARSER_H
