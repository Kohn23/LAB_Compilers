#include "common.h"

TokenStream* init_tokenstream() {
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