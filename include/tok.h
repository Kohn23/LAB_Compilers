#ifndef _TOK_H
#define _TOK_H

#define MAX_LEN_LEXME 32

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

#endif // _TOK_H