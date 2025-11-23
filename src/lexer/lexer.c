#include <ctype.h>
#include <string.h>

#include "lexer.h"
#include "common.h"

static const char keywords[] =
#define DEF(macro, str) str "\0"
#include "tok.def"
#undef DEF
;

static Lexer* init_lexer() {
    printf("Initializing lexer\n");
    Lexer *lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) {
        printf("Memory allocation for Lexer failed\n");
        return NULL;
    }
    
    // should be initialized in lex_analyze again
    lexer->current_line = 1;
    lexer->current_char = 0;
    
    return lexer;
}

static void free_lexer(Lexer *lexer) {
    free(lexer);
}

CORE_API void lex_analyze(const char *input_filename, TokenStream* token_stream, ErrorLogger* errorlogger) {
    Lexer* lexer = init_lexer();
    
    FILE* input_file = fopen(input_filename, "r");
    if(input_file == NULL){
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }

    // analyze
    int ch;
    while ((ch = fgetc(input_file)) != EOF) {
        lexer->current_char++;

        if (ch == '\n') {
            token_stream->tokens[token_stream->count].type = TOK_EOLN; 
            strcpy(token_stream->tokens[token_stream->count].lexeme, "EOLN");
            token_stream->count++;
            lexer->current_line++;
            lexer->current_char = 0;
            continue;
        }
        
        // include '\n', thus check '\n' first
        if (isspace(ch)) {
            continue;
        }
        
        // number literal
        if (isdigit(ch)) {
            size_t len = 0;
            token_stream->tokens[token_stream->count].lexeme[len++] = ch;
            while ((ch = fgetc(input_file)) != EOF && isdigit(ch) && len < MAX_LEN_LEXME - 1) {
                token_stream->tokens[token_stream->count].lexeme[len++] = ch;
                lexer->current_char++;
            }
            ungetc(ch, input_file);
            token_stream->tokens[token_stream->count].lexeme[len] = '\0';
            token_stream->tokens[token_stream->count].type = TOK_NUM_LITERAL;
            token_stream->count++;
            continue;
        }

        // operators
        if (ch == '-')
        {
            token_stream->tokens[token_stream->count].type = TOK_OP_SUB; 
            strcpy(token_stream->tokens[token_stream->count].lexeme, "-");
            token_stream->count++;
            continue;
        }

        if (ch == '*')
        {
            token_stream->tokens[token_stream->count].type = TOK_OP_MUL; 
            strcpy(token_stream->tokens[token_stream->count].lexeme, "*");
            token_stream->count++;
            continue;
        }

        // operator: <, <=, <>
        if (ch == '<')
        {
            int next_ch = fgetc(input_file);
            lexer->current_char++;
            if (next_ch == '=')
            {
                token_stream->tokens[token_stream->count].type = TOK_OP_LEQ; 
                strcpy(token_stream->tokens[token_stream->count].lexeme, "<=");
            }
            else if (next_ch == '>')
            {
                token_stream->tokens[token_stream->count].type = TOK_OP_NE; 
                strcpy(token_stream->tokens[token_stream->count].lexeme, "<>");
            }
            else
            {
                ungetc(next_ch, input_file);
                lexer->current_char--;
                token_stream->tokens[token_stream->count].type = TOK_OP_LT; 
                strcpy(token_stream->tokens[token_stream->count].lexeme, "<");
            }
            token_stream->count++;
            continue;
        }

        if (ch == '=')
        {
            token_stream->tokens[token_stream->count].type = TOK_OP_EQ; 
            strcpy(token_stream->tokens[token_stream->count].lexeme, "=");
            token_stream->count++;
            continue;
        }

        // operator: > and >=
        if (ch == '>')
        {
            int next_ch = fgetc(input_file);
            lexer->current_char++;
            if (next_ch == '=')
            {
                token_stream->tokens[token_stream->count].type = TOK_OP_GEQ; 
                strcpy(token_stream->tokens[token_stream->count].lexeme, ">=");
            }
            else
            {
                ungetc(next_ch, input_file);
                lexer->current_char--;
                token_stream->tokens[token_stream->count].type = TOK_OP_GT; 
                strcpy(token_stream->tokens[token_stream->count].lexeme, ">");
            }
            continue;
        }

        if (ch == ':')
        {
            int next_ch = fgetc(input_file);
            lexer->current_char++;
            if (next_ch == '=')
            {
                token_stream->tokens[token_stream->count].type = TOK_OP_ASSIGN; 
                strcpy(token_stream->tokens[token_stream->count].lexeme, ":=");
                token_stream->count++;
            }
            else
            {
                ungetc(next_ch, input_file);
                lexer->current_char--;
                log_error(errorlogger, "Unrecognized token", lexer->current_line, (char[]){(char)ch, '\0'});
            }
            continue;
        }

        if (ch == ';')
        {
            token_stream->tokens[token_stream->count].type = TOK_PUNCT_SEMICOLON; 
            strcpy(token_stream->tokens[token_stream->count].lexeme, ";");
            token_stream->count++;
            continue;
        }

        if (ch == '(')
        {
            token_stream->tokens[token_stream->count].type = TOK_PUNCT_LPAR; 
            strcpy(token_stream->tokens[token_stream->count].lexeme, "(");
            token_stream->count++;
            continue;
        }

        if (ch == ')')
        {
            token_stream->tokens[token_stream->count].type = TOK_PUNCT_RPAR; 
            strcpy(token_stream->tokens[token_stream->count].lexeme, ")");
            token_stream->count++;
            continue;
        }

        if (isalpha(ch)) {
            size_t len = 0;
            token_stream->tokens[token_stream->count].lexeme[len++] = ch;
            while ((ch = fgetc(input_file)) != EOF && isalnum(ch) && len < MAX_LEN_LEXME - 1) {
                token_stream->tokens[token_stream->count].lexeme[len++] = ch;
                lexer->current_char++;
            }
            ungetc(ch, input_file);
            token_stream->tokens[token_stream->count].lexeme[len] = '\0';
            // check if keyword
            const char *kw_ptr = keywords;
            TokenType found_type = TOK_IDENTIFIER;
            int keyword_index = 0;
            while (*kw_ptr) {
                if (strcmp(token_stream->tokens[token_stream->count].lexeme, kw_ptr) == 0) {
                    found_type = (TokenType)(TOK_NULL + keyword_index + 1);
                    break;
                }
                kw_ptr += strlen(kw_ptr) + 1;
                keyword_index++;
            }
            token_stream->tokens[token_stream->count].type = found_type;
            token_stream->count++;
            continue;
        } else {
            log_error(errorlogger, "Unrecognized token", lexer->current_line, (char[]){(char)ch, '\0'});
        }
    }
    // Write EOF token at the end
    token_stream->tokens[token_stream->count].type = TOK_EOF;
    strcpy(token_stream->tokens[token_stream->count].lexeme, "EOF");
    token_stream->count++;
    
    free_lexer(lexer);
    fclose(input_file);

    printf("Lexical analysis completed. Total tokens: %zu\n", token_stream->count);
}
