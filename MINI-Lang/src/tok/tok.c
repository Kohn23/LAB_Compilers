#include "tok.h"
#include <string.h>

CORE_API TokenStream* init_tokenstream() {
    TokenStream* ts = (TokenStream*)malloc(sizeof(TokenStream));
    if (!ts) {
        return NULL;
    }
    ts->tokens = (Token*)malloc(MAX_LEN_TOKENSTREAM * sizeof(Token));
    if (!ts->tokens) {
        free(ts);
        return NULL;
    }
    ts->count = 0;
    ts->capacity = MAX_LEN_TOKENSTREAM;
    return ts;
} 

CORE_API void free_tokenstream(TokenStream* tokenstream) {
    if (tokenstream) {
        if (tokenstream->tokens) {
            free(tokenstream->tokens);
        }
        free(tokenstream);
    }
}

CORE_API void fprint_tokenstream(const char* filepath, TokenStream* tokenstream){
    FILE* file = fopen(filepath, "w");
    if(file == NULL){
        perror("Error");
        return;
    }

    for(size_t i = 0; i < tokenstream->count; i++){
        Token token = tokenstream->tokens[i];
        fprintf(file, "%s %d\n", token.lexeme, token.type);
    }

    fclose(file);
}


// Currently not used
CORE_API void fload_tokenstream(const char* filepath, TokenStream* tokenstream){
    FILE* file = fopen(filepath, "r");
    if(file == NULL){
        perror("Error");
        return;
    }

    char lexeme[MAX_LEN_LEXME];
    int type;
    while(fscanf(file, "%s %d\n", lexeme, &type) != EOF){
        tokenstream->tokens[tokenstream->count].type = (TokenType)type;
        strcpy(tokenstream->tokens[tokenstream->count].lexeme, lexeme);
        tokenstream->count++;
    }

    fclose(file);
}

