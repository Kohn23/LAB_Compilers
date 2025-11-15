#include <ctype.h>
#include <string.h>

#include "lexer.h"

static const char keywords[] =
#define DEF(macro, str) str "\0"
#include "tok.def"
#undef DEF
;

CORE_API Lexer* lexer_init(const char *input_filename, const char *output_dir) {
    printf("Initializing lexer with input file: %s and output dir: %s\n", input_filename, output_dir);
    Lexer *lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) {
        printf("Memory allocation for Lexer failed\n");
        return NULL;
    }

    lexer->input_file = fopen(input_filename, "r");
    if (!lexer->input_file) {
        free(lexer);
        printf("Failed to open input file: %s\n", input_filename);
        return NULL;
    }
    
    char dyd_filename[256];
    char err_filename[256];
    snprintf(dyd_filename, sizeof(dyd_filename), "%s.dyd", output_dir);
    snprintf(err_filename, sizeof(err_filename), "%s.err", output_dir);
    
    lexer->dyd_file = fopen(dyd_filename, "w");
    lexer->err_file = fopen(err_filename, "w");
    
    if (!lexer->dyd_file || !lexer->err_file) {
        if (lexer->dyd_file) fclose(lexer->dyd_file);
        if (lexer->err_file) fclose(lexer->err_file);
        printf("Failed to open output files in directory: %s\n", output_dir);
        fclose(lexer->input_file);
        free(lexer);
        return NULL;
    }
    
    lexer->current_line = 1;
    lexer->current_char = 0;
    
    return lexer;
}

CORE_API void lexer_destroy(Lexer *lexer) {
    if (lexer->input_file) fclose(lexer->input_file);
    if (lexer->dyd_file) fclose(lexer->dyd_file);
    if (lexer->err_file) fclose(lexer->err_file);
    free(lexer);
}

static void write_token(Lexer *lexer, const Token *token){
    fprintf(lexer->dyd_file, "%s %d\n", token->lexeme, token->type);
}

static void write_error(Lexer *lexer, const char *message, const char *token_lexeme){
    fprintf(lexer->err_file, "LINE:%zu %s:'%s'\n", 
            lexer->current_line, message, token_lexeme);
}

CORE_API void lexical_analyze(Lexer* lexer) {
    int ch;
    while ((ch = fgetc(lexer->input_file)) != EOF) {
        lexer->current_char++;

        if (ch == '\n') {
            Token token;
            token.type = TOK_EOLN; 
            strcpy(token.lexeme, "EOLN");
            write_token(lexer, &token);
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
            Token token;
            size_t len = 0;
            token.lexeme[len++] = ch;
            while ((ch = fgetc(lexer->input_file)) != EOF && isdigit(ch) && len < MAX_LEN_LEXME - 1) {
                token.lexeme[len++] = ch;
                lexer->current_char++;
            }
            ungetc(ch, lexer->input_file);
            token.lexeme[len] = '\0';
            token.type = TOK_NUM_LITERAL; 
            write_token(lexer, &token);
            continue;
        }

        // operators

        if (ch == '-')
        {
            Token token;
            token.type = TOK_OP_SUB; 
            strcpy(token.lexeme, "-");
            write_token(lexer, &token);
            continue;
        }

        if (ch == '*')
        {
            Token token;
            token.type = TOK_OP_MUL; 
            strcpy(token.lexeme, "*");
            write_token(lexer, &token);
            continue;
        }

        // operator: <, <=, <>
        if (ch == '<')
        {
            Token token;
            int next_ch = fgetc(lexer->input_file);
            lexer->current_char++;
            if (next_ch == '=')
            {
                token.type = TOK_OP_LEQ; 
                strcpy(token.lexeme, "<=");
            }
            else if (next_ch == '>')
            {
                token.type = TOK_OP_NE; 
                strcpy(token.lexeme, "<>");
            }
            else
            {
                ungetc(next_ch, lexer->input_file);
                lexer->current_char--;
                token.type = TOK_OP_LT; 
                strcpy(token.lexeme, "<");
            }
            write_token(lexer, &token);
            continue;
        }

        if (ch == '=')
        {
            Token token;
            token.type = TOK_OP_EQ; 
            strcpy(token.lexeme, "=");
            write_token(lexer, &token);
            continue;
        }

        // operator: > and >=
        if (ch == '>')
        {
            Token token;
            int next_ch = fgetc(lexer->input_file);
            lexer->current_char++;
            if (next_ch == '=')
            {
                token.type = TOK_OP_GEQ; 
                strcpy(token.lexeme, ">=");
            }
            else
            {
                ungetc(next_ch, lexer->input_file);
                lexer->current_char--;
                token.type = TOK_OP_GT; 
                strcpy(token.lexeme, ">");
            }
            write_token(lexer, &token);
            continue;
        }

        if (ch == ':')
        {
            Token token;
            int next_ch = fgetc(lexer->input_file);
            lexer->current_char++;
            if (next_ch == '=')
            {
                token.type = TOK_OP_ASSIGN; 
                strcpy(token.lexeme, ":=");
                write_token(lexer, &token);
            }
            else
            {
                ungetc(next_ch, lexer->input_file);
                lexer->current_char--;
                write_error(lexer, "Unrecognized token", ":");
            }
            continue;
        }

        if (ch == ';')
        {
            Token token;
            token.type = TOK_PUNCT_SEMICOLON; 
            strcpy(token.lexeme, ";");
            write_token(lexer, &token);
            continue;
        }

        if (ch == '(')
        {
            Token token;
            token.type = TOK_PUNCT_LPAR; 
            strcpy(token.lexeme, "(");
            write_token(lexer, &token);
            continue;
        }

        if (ch == ')')
        {
            Token token;
            token.type = TOK_PUNCT_RPAR; 
            strcpy(token.lexeme, ")");
            write_token(lexer, &token);
            continue;
        }

        if (isalpha(ch)) {
            Token token;
            size_t len = 0;
            token.lexeme[len++] = ch;
            while ((ch = fgetc(lexer->input_file)) != EOF && isalnum(ch) && len < MAX_LEN_LEXME - 1) {
                token.lexeme[len++] = ch;
                lexer->current_char++;
            }
            ungetc(ch, lexer->input_file);
            token.lexeme[len] = '\0';
            // check if keyword
            const char *kw_ptr = keywords;
            enum TokenType found_type = TOK_IDENTIFIER;
            int keyword_index = 0;
            while (*kw_ptr) {
                if (strcmp(token.lexeme, kw_ptr) == 0) {
                    found_type = (enum TokenType)(TOK_NULL + keyword_index + 1);
                    break;
                }
                kw_ptr += strlen(kw_ptr) + 1;
                keyword_index++;
            }
            token.type = found_type;
            write_token(lexer, &token);
            continue;
        } else {
            write_error(lexer, "Unrecognized token", (char[]){(char)ch, '\0'});
        }
    }
    
    // Write EOF token at the end
    Token token;
    token.type = TOK_EOF;
    strcpy(token.lexeme, "EOF");
    write_token(lexer, &token);
}
