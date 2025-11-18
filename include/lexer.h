#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>

#include "windll.h" 
#include "tok.h"

// Lexical Analyzer
typedef struct {
    FILE *input_file;   
    FILE *dyd_file;    
    FILE *err_file;
    size_t current_line;  
    size_t current_char;
} Lexer;


CORE_API Lexer* lexer_init(const char *input_filename, const char *output_dir);
CORE_API void lexer_destroy(Lexer*);
CORE_API void lexical_analyze(Lexer*);

#endif // LEXER_H