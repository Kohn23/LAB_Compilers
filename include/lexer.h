#include <stdio.h>
#include <stdlib.h>

#define MAX_LEN_LEXME 32
#define MAX_LEN_TOK_BUF 256

// Lexical Analyzer
typedef struct {
    FILE *input_file;   
    FILE *dyd_file;    
    FILE *err_file;
    
    size_t current_line;  
    size_t current_char;
    char token_buffer[MAX_LEN_TOK_BUF];
    size_t buffer_index;
} Lexer;

enum TokenType {
    TOK_NULL
#define DEF(macro, str) ,macro
#include "tok.def"
#undef DEF
};

typedef struct {
    TokenType type;
    char lexeme[MAX_LEN_LEXME];           
} Token;



Lexer* lexer_init(const char *input_filename, const char *output_dir);
void lexer_destroy(Lexer*);
void lexical_analyze(Lexer*);
