#include "parser.h"
#include "common.h"
#include <string.h>

static const char* keywords[] = {
#define DEF(macro, str) str ,
#include "tok.def"
#undef DEF
};

typedef enum {
    PARSE_STATUS_OK,
    PARSE_STATUS_FAILED
}ParseStatus;

typedef struct {
    TokenStream* tokenstream;
    VarTable* vartab;
    ProcTable* proctab;
    ErrorLogger* errlog;

    size_t token_index;
    size_t current_line;
    size_t current_level;
    Stack* vartab_idx_stamp;
    char current_proc[MAX_LEN_PROC_NAME];
    char last_proc[MAX_LEN_PROC_NAME];
}Parser;

static Parser* init_parser(TokenStream* tokenstream, VarTable* vartab, ProcTable* proctab, ErrorLogger* errlog) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) {
        return NULL;
    }
    parser->tokenstream = tokenstream;
    parser->vartab = vartab;
    parser->proctab = proctab;
    parser->errlog = errlog;

    parser->token_index = 0;
    parser->current_line = 1;
    parser->current_level = 0;
    parser->vartab_idx_stamp = init_stack();
    parser->current_proc[0] = '\0';
    parser->last_proc[0] = '\0';

    return parser;
}

static void free_parser(Parser* parser) {
    if (parser) {
        free(parser);
    }
}

static ErrorInfo parse_error_format(size_t line, const char* spec1, const char* spec2) {
    static char buffer[MAX_LEN_ERROR_INFO];
    snprintf(buffer, MAX_LEN_ERROR_INFO, "Line:%zu Expecting '%s' before '%s'", line, spec1, spec2);
    return buffer;
}

static Token* advance_token(Parser* parser) {
    if (!parser || !parser->tokenstream) return NULL;
    while(parser->token_index < parser->tokenstream->count && parser->tokenstream->tokens[parser->token_index].type == TOK_EOLN){
        parser->token_index++;
        parser->current_line++;
    }
    if(parser->token_index < parser->tokenstream->count){
        Token* t = &parser->tokenstream->tokens[parser->token_index];
        parser->token_index++;

        // // debug
        // printf("%s", t->lexeme);
        
        return t;
    }
    return NULL;
}

static Token* peek_token(Parser* parser, size_t offset) {
    if (!parser || !parser->tokenstream) return NULL;
    size_t idx = parser->token_index + offset;
    while (parser->tokenstream->tokens[idx].type == TOK_EOLN) idx++;
    
    if (idx >= 0 && (size_t)idx < parser->tokenstream->count) {
        return &parser->tokenstream->tokens[idx];
    }
    return NULL;
}

static void retreat_token(Parser* parser) {
    int flag = parser->token_index > 0;
    while (flag) {
        parser->token_index--;
        if(parser->tokenstream->tokens[parser->token_index].type == TOK_EOLN)
        {
            parser->current_line--;
            flag = parser->token_index > 0;
            continue;
        }
        break;
    }
}

static ParseStatus panic_mode_recovery(Parser* parser, TokenType tokentype){
    // // debug
    // printf("recovery: ");
    Token* token = peek_token(parser, 0);
    while(token->type != tokentype){
        token = advance_token(parser);
        if(token->type == TOK_EOF){
            break;
        }
    }
    retreat_token(parser);
    return PARSE_STATUS_OK;
}

// forward declarations
static ParseStatus parse_program(Parser*);
static ParseStatus parse_subprogram(Parser*);
static ParseStatus parse_decl_list(Parser*);
static ParseStatus parse_decl_list_rec(Parser*);
static ParseStatus parse_decl(Parser*);
static ParseStatus parse_decl_var(Parser*);
static ParseStatus parse_decl_func(Parser*);
static ParseStatus parse_func_block(Parser*);
static ParseStatus parse_param(Parser*);
static ParseStatus parse_stmt_list(Parser*);
static ParseStatus parse_stmt(Parser*);
static ParseStatus parse_stmt_list_rec(Parser*);
static ParseStatus parse_read(Parser*);
static ParseStatus parse_write(Parser*);
static ParseStatus parse_assignment(Parser*);
static ParseStatus parse_expression(Parser*);
static ParseStatus parse_expression_rec(Parser*);
static ParseStatus parse_term(Parser*);
static ParseStatus parse_term_rec(Parser*);
static ParseStatus parse_factor(Parser*);
static ParseStatus parse_call(Parser*);
static ParseStatus parse_condition(Parser*);
static ParseStatus parse_condition_expr(Parser*);


// <程序>→<分程序>
static ParseStatus parse_program(Parser* parser) {
    // debug
    // printf("<程序> ");
    return parse_subprogram(parser);
}

// <分程序>→begin <说明语句表>;<执行语句表> end
static ParseStatus parse_subprogram(Parser* parser) {
    // debug
    // printf("<分程序> ");

    Token* token = advance_token(parser);
    if (!token || token->type != TOK_BEGIN) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_BEGIN], token ? token->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    
    parser->current_level++;
    strncpy(parser->current_proc, "main", MAX_LEN_PROC_NAME);
    stack_push(parser->vartab_idx_stamp, parser->vartab->count);

    
    if (parse_decl_list(parser) == PARSE_STATUS_FAILED) {
        return PARSE_STATUS_FAILED;
    }
    
    token = advance_token(parser);
    if (!token || token->type != TOK_PUNCT_SEMICOLON) {
        // // debug
        // printf("error 1");
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_SEMICOLON], token ? token->lexeme : "<eof>"));
        // return PARSE_STATUS_FAILED;
        panic_mode_recovery(parser, peek_token(parser,0)->type);
    }
    
    if (parse_stmt_list(parser) == PARSE_STATUS_FAILED) {
        panic_mode_recovery(parser, TOK_END);
    }
    
    token = advance_token(parser);
    if (!token || token->type != TOK_END) {
        if(token->type == TOK_EOF){
            char spec2[MAX_LEN_ERROR_SPEC];
            retreat_token(parser); //EOF
            retreat_token(parser); //LAST
            Token* last_tok = peek_token(parser, 0);
            snprintf(spec2, MAX_LEN_ERROR_SPEC, "%s', found '%s' instead", token->lexeme, last_tok->lexeme);
            log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_END], spec2));
        }
        else{
            log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_END], token ? token->lexeme : "<eof>"));
        }
        return PARSE_STATUS_FAILED;
    }
    
    parser->current_level--;

    // this could cause a glitch if no variable ever exists in the program since (size_t)0 - 1 will underflow
    insert_proc(parser->proctab, parser->current_proc, PROC_TYPE_VOID, parser->current_level, stack_pop(parser->vartab_idx_stamp), parser->vartab->count - 1);
    
    parser->current_proc[0] = '\0';

    
    
    return PARSE_STATUS_OK;
}

// <说明语句表>→<说明语句><说明语句表递归>
static ParseStatus parse_decl_list(Parser* parser) {
    // // debug
    // printf("<说明语句表> ");
    if (parse_decl(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    if (parse_decl_list_rec(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED; // Not possible
    return PARSE_STATUS_OK;
}

// <说明语句表递归>→;<说明语句><说明语句表递归>|ε
static ParseStatus parse_decl_list_rec(Parser* parser) {
    // // debug
    // printf("<说明语句表递归> ");
    Token* token = advance_token(parser);
    if (token->type != TOK_PUNCT_SEMICOLON) {
        retreat_token(parser);
    } else {
        if (parse_decl(parser) == PARSE_STATUS_FAILED) {
            retreat_token(parser);
            // // debug
            // printf(" catch ret: %s ", parser->tokenstream->tokens[parser->token_index].lexeme);
            return PARSE_STATUS_OK;
        }
        parse_decl_list_rec(parser);
    }
    return PARSE_STATUS_OK;
}

// <说明语句>→<变量说明>│<函数说明>
static ParseStatus parse_decl(Parser* parser) {
    // debug
    // printf("<说明语句> ");
    Token* token = advance_token(parser);
    if (!token || token->type != TOK_INTEGER) {
        retreat_token(parser);
        // debug
        // printf(" ret: %s ", parser->tokenstream->tokens[parser->token_index].lexeme);
        return PARSE_STATUS_FAILED;
    } else {
        if (parse_decl_func(parser) == PARSE_STATUS_FAILED) {
            return parse_decl_var(parser);
        }
        return PARSE_STATUS_OK;
    }
}

// <变量说明>→(integer) <变量> 
// <变量>→<标识符>
static ParseStatus parse_decl_var(Parser* parser){
    // debug
    // printf("<变量说明> ");
    Token* token = advance_token(parser);
    if (!token || token->type != TOK_IDENTIFIER) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "identifier", token ? token->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    // insert variable into var table
    // if (lookup_var(parser->vartab, token->lexeme, parser->current_level) >= 0) {
    //     // duplicate -> log but continue
    //     log_error(parser->errlog, parse_error_format(parser->current_line, "new variable", token->lexeme));
    // } else {
    //     insert_var(parser->vartab, token->lexeme, parser->current_proc, VAR_KIND_NORMAL, VAR_TYPE_INT, parser->current_level);
    // }
    insert_var(parser->vartab, token->lexeme, parser->current_proc, VAR_KIND_NORMAL, VAR_TYPE_INT, parser->current_level);
    return PARSE_STATUS_OK;
}

// <函数说明>→(integer) function <标识符>(<参数>);<函数体>
static ParseStatus parse_decl_func(Parser* parser){
    // debug
    // printf("<函数说明> ");
    Token* token = advance_token(parser);
    if (!token || token->type != TOK_FUNCTION) {
        retreat_token(parser);
        return PARSE_STATUS_FAILED;
    }

    Token* ident = advance_token(parser);
    if (!ident || ident->type != TOK_IDENTIFIER) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "identifier", ident ? ident->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }

    Token* t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_LPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_LPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }

    t = advance_token(parser);
    if (!t || t->type != TOK_IDENTIFIER){
        log_error(parser->errlog, parse_error_format(parser->current_line, "parameter", t ? t->lexeme : "<eof>"));
    }
    else{
        // count variables from now
        stack_push(parser->vartab_idx_stamp, parser->vartab->count);
        insert_param(parser->vartab, t->lexeme, parser->current_level);
    }

    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_RPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_RPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }

    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_SEMICOLON) {
        // debug
        // printf("error 2");
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_SEMICOLON], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }

    // parse function body
    // enter new proc scope
    parser->current_level++;
    strncpy(parser->last_proc, parser->current_proc, MAX_LEN_PROC_NAME);
    strncpy(parser->current_proc, ident->lexeme, MAX_LEN_PROC_NAME);

    if(parse_func_block(parser) == PARSE_STATUS_FAILED){
        return PARSE_STATUS_FAILED;
    }

    // leave proc scope
    parser->current_level--;
    strncpy(parser->current_proc, parser->last_proc, MAX_LEN_PROC_NAME);

    // insert proc
    if (lookup_proc(parser->proctab, ident->lexeme, parser->current_level) >= 0) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "new procedure", ident->lexeme));
    } else {
        insert_proc(parser->proctab, ident->lexeme, PROC_TYPE_INT, parser->current_level, stack_pop(parser->vartab_idx_stamp), parser->vartab->count - 1);
    }

    return PARSE_STATUS_OK;
}

// <函数体>→begin <说明语句表>；<执行语句表> end
static ParseStatus parse_func_block(Parser* parser){
    // debug
    // printf("<函数体> ");
    Token* token = advance_token(parser);
    if (!token || token->type != TOK_BEGIN) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_BEGIN], token ? token->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
        
    if (parse_decl_list(parser) == PARSE_STATUS_FAILED) {
        return PARSE_STATUS_FAILED;
    }
    
    token = advance_token(parser);
    if (!token || token->type != TOK_PUNCT_SEMICOLON) {
        // debug
        // printf("error 3");
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_SEMICOLON], token ? token->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    
    if (parse_stmt_list(parser) == PARSE_STATUS_FAILED) {
        return PARSE_STATUS_FAILED;
    }
    
    token = advance_token(parser);
    if (!token || token->type != TOK_END) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_END], token ? token->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    
    return PARSE_STATUS_OK;
}

// <参数>→<变量> 
// <变量>→<标识符>
static ParseStatus parse_param(Parser* parser){
    // debug
    // printf("<参数> ");
    Token* t = advance_token(parser);
    if (!t || t->type != TOK_IDENTIFIER) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "parameter", t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    // insert parameter as variable with VAR_KIND_PARAM
    if (lookup_var(parser->vartab, t->lexeme, parser->current_level) >= 0) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "new parameter", t->lexeme));
    } else {
        insert_var(parser->vartab, t->lexeme, parser->current_proc, VAR_KIND_PARAM, VAR_TYPE_INT, parser->current_level + 1);
    }
    return PARSE_STATUS_OK;
}

// <执行语句表>→<执行语句><执行语句表递归>
static ParseStatus parse_stmt_list(Parser* parser) {
    // debug
    // printf("<执行语句表> ");
    if (parse_stmt(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    if (parse_stmt_list_rec(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return PARSE_STATUS_OK;
}

// <执行语句表递归>→;<执行语句><执行语句表递归>|ε
static ParseStatus parse_stmt_list_rec(Parser* parser) {
    // debug
    // printf("<执行语句表递归> ");
    Token* t = advance_token(parser);
    if (!t) return PARSE_STATUS_OK;
    if (t->type != TOK_PUNCT_SEMICOLON) {
        retreat_token(parser);
        return PARSE_STATUS_OK;
    }
    if (parse_stmt(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return parse_stmt_list_rec(parser);
}


// <执行语句>→<读语句>│<写语句>│<赋值语句>│<条件语句>
static ParseStatus parse_stmt(Parser* parser) {
    // debug
    // printf("<执行语句> ");
    Token* t = peek_token(parser, 0);
    if (!t) return PARSE_STATUS_FAILED;

    if (t->type == TOK_READ) {
        return parse_read(parser);
    } else if (t->type == TOK_WRITE) {
        return parse_write(parser);
    } else if (t->type == TOK_IF) {
        return parse_condition(parser);
    } else if (t->type == TOK_IDENTIFIER) {
        // assignment or call
        advance_token(parser);
        Token* next = peek_token(parser, 0);
        if (!next) {
            log_error(parser->errlog, parse_error_format(parser->current_line, ":=", "<eof>"));
            return PARSE_STATUS_FAILED;
        }
        if (next->type == TOK_OP_ASSIGN) {
            // assignment
            return parse_assignment(parser);
        } else if (next->type == TOK_PUNCT_LPAR) {
            // function call
            return parse_call(parser);
        } else {
            log_error(parser->errlog, parse_error_format(parser->current_line, "<执行语句>", next->lexeme));
            return PARSE_STATUS_FAILED;
        }
    } else {
        log_error(parser->errlog, parse_error_format(parser->current_line, "<执行语句>", t->lexeme));
        return PARSE_STATUS_FAILED;
    }
}


// <读语句>→read(<变量>)
static ParseStatus parse_read(Parser* parser) {
    // debug
    // printf("<读语句> ");
    Token* t = advance_token(parser);
    if (!t || t->type != TOK_READ) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_READ], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_LPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_LPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    Token* id = advance_token(parser);
    if (!id || id->type != TOK_IDENTIFIER) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "identifier", id ? id->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    if (lookup_var(parser->vartab, id->lexeme, parser->current_level) < 0) {
        char spec2[MAX_LEN_ERROR_SPEC];
        snprintf(spec2, MAX_LEN_ERROR_SPEC, "变量 %s 被引用", id->lexeme);
        log_error(parser->errlog, parse_error_format(parser->current_line, "<变量说明>", spec2));
        return panic_mode_recovery(parser, TOK_PUNCT_SEMICOLON);
    }
    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_RPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_RPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    return PARSE_STATUS_OK;
}


// <写语句>→write(<变量>)
static ParseStatus parse_write(Parser* parser) {
    // debug
    // printf("<写语句> ");
    Token* t = advance_token(parser);
    if (!t || t->type != TOK_WRITE) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_WRITE], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_LPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_LPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    Token* id = advance_token(parser);
    if (!id || id->type != TOK_IDENTIFIER) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "identifier", id ? id->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    if (lookup_var(parser->vartab, id->lexeme, parser->current_level) < 0) {
        char spec2[MAX_LEN_ERROR_SPEC];
        snprintf(spec2, MAX_LEN_ERROR_SPEC, "变量 %s 被引用", id->lexeme);
        log_error(parser->errlog, parse_error_format(parser->current_line, "<变量说明>", spec2));
        return panic_mode_recovery(parser, TOK_PUNCT_SEMICOLON);
    }
    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_RPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_RPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    return PARSE_STATUS_OK;
}


// <赋值语句>→<变量>:=<算术表达式>
static ParseStatus parse_assignment(Parser* parser) {
    // debug
    // printf("<赋值语句> ");
    Token* op = advance_token(parser);
    if (!op || op->type != TOK_OP_ASSIGN) {
        log_error(parser->errlog, parse_error_format(parser->current_line, ":=", op ? op->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    if (parse_expression(parser) == PARSE_STATUS_FAILED) return panic_mode_recovery(parser, TOK_PUNCT_SEMICOLON);
    return PARSE_STATUS_OK;
}

// <函数调用>→<标识符>(<算术表达式>)
static ParseStatus parse_call(Parser* parser) {
    // debug
    // printf("<函数调用> ");
    Token* t = advance_token(parser); 
    if (!t || t->type != TOK_PUNCT_LPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_LPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    if(parse_expression(parser) == PARSE_STATUS_FAILED){
        return PARSE_STATUS_FAILED;
    }
    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_RPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_RPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    return PARSE_STATUS_OK;
}

// <算术表达式>→<项><算术表达式递归>
static ParseStatus parse_expression(Parser* parser) {
    // debug
    // printf("<算术表达式> ");
    if (parse_term(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return parse_expression_rec(parser);
}


// <算术表达式递归>→-<项><算术表达式递归>│ε
static ParseStatus parse_expression_rec(Parser* parser) {
    // debug
    // printf("<算术表达式递归> ");
    Token* next = peek_token(parser, 0);
    if (next && next->type == TOK_OP_SUB) {
        advance_token(parser);
        if (parse_term(parser) == PARSE_STATUS_FAILED){
            retreat_token(parser);
            return PARSE_STATUS_OK;
        } 
        return parse_expression_rec(parser);
    }
    return PARSE_STATUS_OK;
}


// <项>→<因子><项递归>
static ParseStatus parse_term(Parser* parser) {
    // debug
    // printf("<项> ");
    if (parse_factor(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return parse_term_rec(parser);
}

// <项递归>→*<因子><项递归>│ε
static ParseStatus parse_term_rec(Parser* parser) {
    // debug
    // printf("<项递归> ");
    Token* next = peek_token(parser, 0);
    if (next && next->type == TOK_OP_MUL) {
        advance_token(parser);
        if (parse_factor(parser) == PARSE_STATUS_FAILED){
            retreat_token(parser);
            return PARSE_STATUS_OK;
        } 
        return parse_term_rec(parser);
    }
    return PARSE_STATUS_OK;
}


// <因子>→<变量>│<常数>│<函数调用>
static ParseStatus parse_factor(Parser* parser) {
    // debug
    // printf("<因子> ");
    Token* t = advance_token(parser);
    if (!t) return PARSE_STATUS_FAILED;
    if (t->type == TOK_IDENTIFIER) {
        Token* next = peek_token(parser, 0);
        if (next && next->type == TOK_PUNCT_LPAR) {
            return parse_call(parser);
        } else {
            if (lookup_var(parser->vartab, t->lexeme, parser->current_level) < 0) {
                char spec2[MAX_LEN_ERROR_SPEC];
                snprintf(spec2, MAX_LEN_ERROR_SPEC, "变量 %s 被引用", t->lexeme);
                log_error(parser->errlog, parse_error_format(parser->current_line, "<变量说明>", spec2));
                return PARSE_STATUS_FAILED;
            }
            return PARSE_STATUS_OK;
        }
    } else if (t->type == TOK_NUM_LITERAL) {
        return PARSE_STATUS_OK;
    } else {
        log_error(parser->errlog, parse_error_format(parser->current_line, "factor", t->lexeme));
        return PARSE_STATUS_FAILED;
    }
}


// <条件语句>→if<条件表达式>then<执行语句>else <执行语句>
static ParseStatus parse_condition(Parser* parser) {
    // debug
    // printf("<条件语句> ");
    Token* t = advance_token(parser);
    if (!t || t->type != TOK_IF) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_IF], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    if (parse_condition_expr(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    t = advance_token(parser);
    if (!t || t->type != TOK_THEN) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_THEN], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    if (parse_stmt(parser) == PARSE_STATUS_FAILED) {
        panic_mode_recovery(parser, TOK_ELSE);
    }
    t = advance_token(parser);
    if (!t || t->type != TOK_ELSE) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_ELSE], t ? t->lexeme : "<eof>"));
        // panic_mode_recovery: not sufficient when else is missing
        panic_mode_recovery(parser, TOK_IDENTIFIER);
        advance_token(parser);
    }
    if (parse_stmt(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return PARSE_STATUS_OK;
}


// <条件表达式>→<算术表达式><关系运算符><算术表达式>
static ParseStatus parse_condition_expr(Parser* parser) {
    // debug
    // printf("<条件表达式> ");
    if (parse_expression(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    Token* op = advance_token(parser);
    if (!op) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "relop", "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    if (parse_expression(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return PARSE_STATUS_OK;
}


CORE_API void recursive_descent_parse(TokenStream* tokenstream, VarTable* vartab, ProcTable* proctab, ErrorLogger* errlog) {
    Parser* parser = init_parser(tokenstream, vartab, proctab, errlog);
    if (!parser) return;

    if(parse_program(parser)==PARSE_STATUS_OK) printf("parse done");
    else printf("parse failed");

    free_parser(parser);
}