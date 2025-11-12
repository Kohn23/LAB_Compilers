#include <ctype.h>
#include <string.h>

#include "lexer.h"

static enum TokenType token_type;

static const char keywords[] =
#define DEF(macro, str) str "\0"
#include "tok.def"
#undef DEF
;

Lexer* lexer_init(const char *input_filename, const char *output_dir) {
    Lexer *lexer = (Lexer*)malloc(sizeof(Lexer));
    if (!lexer) return NULL;

    lexer->input_file = fopen(input_filename, "r");
    if (!lexer->input_file) {
        free(lexer);
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
        fclose(lexer->input_file);
        free(lexer);
        return NULL;
    }
    
    lexer->current_line = 1;
    lexer->current_char = 0;
    lexer->buffer_index = 0;
    lexer->token_buffer[0] = '\0';
    
    return lexer;
}

void lexer_destroy(Lexer *lexer) {
    if (lexer->input_file) fclose(lexer->input_file);
    if (lexer->dyd_file) fclose(lexer->dyd_file);
    if (lexer->err_file) fclose(lexer->err_file);
    free(lexer);
}

static void write_token(Lexer *lexer, const Token *token){
    fprintf(lexer->dyd_file, "<%s, %d>\n", token->lexeme, token->type);
}

static void write_error(Lexer *lexer, const char *message){
    fprintf(lexer->err_file, "Error at line %zu, char %zu: %s\n", 
            lexer->current_line, lexer->current_char, message);
}

static void write_eoln(Lexer *lexer){
    fprintf(lexer->dyd_file, "EOLN\n");
}

static void write_eof(Lexer *lexer){
    fprintf(lexer->dyd_file, "EOF\n");
}

void lexical_analyze(Lexer* lexer) {
    int ch;
    while ((ch = fgetc(lexer->input_file)) != EOF) {
        lexer->current_char++;
        
        if (ch == '\n') {
            write_eoln(lexer);
            lexer->current_line++;
            lexer->current_char = 0;
            continue;
        }
        
        // Simple whitespace skipping
        if (isspace(ch)) {
            continue;
        }
        
        // Example tokenization logic
        Token token;
        if (isalpha(ch)) {
            size_t len = 0;
            token.lexeme[len++] = ch;
            while ((ch = fgetc(lexer->input_file)) != EOF && isalnum(ch) && len < MAX_LEN_LEXME - 1) {
                token.lexeme[len++] = ch;
                lexer->current_char++;
            }
            ungetc(ch, lexer->input_file);
            token.lexeme[len] = '\0';
            token.type = TOK_IDENTIFIER; // Example token type
            write_token(lexer, &token);
        } else {
            write_error(lexer, "Unrecognized character");
        }
    }
    
    write_eof(lexer);
}
