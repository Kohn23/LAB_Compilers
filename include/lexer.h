#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>

#include "windll.h" 
#include "tok.h"
#include "error.h"

// Lexical Analyzer
typedef struct {
    size_t current_line;  
    size_t current_char;
} Lexer;


CORE_API TokenStream* lex_analyze(const char *input_filename, ErrorLogger*);

#endif // LEXER_H