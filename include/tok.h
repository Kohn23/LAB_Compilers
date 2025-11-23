#ifndef _TOK_H
#define _TOK_H

#define MAX_LEN_LEXME 32
#define MAX_LEN_FILEPATH 256
#define MAX_LEN_TOKENSTREAM 1024

#include <stdio.h>
#include <stdlib.h>

#include "windll.h"

typedef enum{
    TOK_NULL
#define DEF(macro, str) ,macro
#include "tok.def"
#undef DEF
}TokenType;

typedef struct Token {
    TokenType type;
    char lexeme[MAX_LEN_LEXME]; 
    // int line_num;          
}Token;

typedef struct {
    Token* tokens;
    size_t count;
    size_t capacity;
}TokenStream;

CORE_API TokenStream* init_tokenstream();
CORE_API void free_tokenstream(TokenStream* );
CORE_API void fprint_tokenstream(const char* filepath, TokenStream* );
CORE_API void fload_tokenstream(const char* filepath, TokenStream* );


#endif // _TOK_H