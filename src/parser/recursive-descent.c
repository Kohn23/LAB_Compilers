#include "parser.h"
#include "common.h"
#include <string.h>

static const char* keywords[] = {
#define DEF(macro, str) str ,
#include "tok.def"
#undef DEF
};

static Parser* init_parser(TokenStream* tokenstream, VarTable* vartab, ProcTable* proctab, ErrorLogger* errlog) {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) {
        return NULL;
    }
    parser->tokenstream = tokenstream;
    parser->vartab = vartab;
    parser->proctab = proctab;
    parser->errlog = errlog;

    parser->token_index = -1;
    parser->current_line = 1;
    parser->current_level = 0;
    parser->current_proc[0] = '\0';
    return parser;
}

static void free_parser(Parser* parser) {
    if (parser) {
        free(parser);
    }
}

static ErrorInfo parse_error_format(size_t line, const char* spec1, const char* spec2) {
    static char buffer[MAX_LEN_ERROR_INFO];
    snprintf(buffer, MAX_LEN_ERROR_INFO, "Line:%zu Expecting '%s' before '%s' ", line, spec1, spec2);
    return buffer;
}

// static Token* advance_token(Parser* parser) {
//     if (parser->token_index < parser->tokenstream->count) {
//         parser->token_index++;
//         if (parser->tokenstream->tokens[parser->token_index].type == TOK_EOLN) {
//             parser->current_line++;
//             parser->token_index++;
//         }
//         return &parser->tokenstream->tokens[parser->token_index];
//     }
// }

// static void retreat_token(Parser* parser) {
//     if (parser->token_index > 0) {
//         parser->token_index--;
//     }
// }

static Token* advance_token(Parser* parser) {
    if (!parser || !parser->tokenstream) return NULL;
    if ((size_t)parser->token_index + 1 < parser->tokenstream->count) {
        parser->token_index++;
        Token* t = &parser->tokenstream->tokens[parser->token_index];
        if (t->type == TOK_EOLN) {
            parser->current_line++;
            parser->token_index++;
            if ((size_t)parser->token_index < parser->tokenstream->count)
                return &parser->tokenstream->tokens[parser->token_index];
            else
                return NULL;
        }
        return t;
    }
    return NULL;
}

static Token* peek_token(Parser* parser, int offset) {
    if (!parser || !parser->tokenstream) return NULL;
    int idx = parser->token_index + offset;
    if (idx >= 0 && (size_t)idx < parser->tokenstream->count) {
        return &parser->tokenstream->tokens[idx];
    }
    return NULL;
}

static void retreat_token(Parser* parser) {
    if (parser && parser->token_index > 0) {
        parser->token_index--;
    }
}

// forward declarations
static ParseStatus parse_program(Parser*);
static ParseStatus parse_subprogram(Parser*);
static ParseStatus parse_decl_list(Parser*);
static ParseStatus parse_decl_list_rec(Parser*);
static ParseStatus parse_decl(Parser*);
static ParseStatus parse_decl_var(Parser*);
static ParseStatus parse_decl_func(Parser*);
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

// // <程序>→<分程序>
// static ParseStatus parse_program(Parser* parser) {
//     return parse_subprogram(parser);
// }

// // <分程序>→begin <说明语句表>;<执行语句表> end
// static ParseStatus parse_subprogram(Parser* parser) {
//     // begin
//     if (advance_token(parser)->type != TOK_BEGIN) {
//         log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_BEGIN], token->lexeme));
//         return PARSE_STATUS_FAILED;
//     }

//     parser->current_level++;
//     parrser->current_proc = "main";

//     if(parse_decl_list(parser) == PARSE_STATUS_FAILED) {
//         return PARSE_STATUS_FAILED;
//     }

//     // ;
//     if (advance_token(parser)->type != TOK_PUNCT_SEMICOLON) {
//         log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_SEMICOLON], token->lexeme));
//         return PARSE_STATUS_FAILED;
//     }

//     if(parse_stmt_list(parser) == PARSE_STATUS_FAILED) {
//         return PARSE_STATUS_FAILED;
//     }

//     // end
//     if (advance_token(parser)->type != TOK_END) {
//         log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_END], token->lexeme));
//         return PARSE_STATUS_FAILED;
//     }

//     parser->current_level--;
//     parser->current_proc[0] = '\0';

//     return PARSE_STATUS_OK;
// }

// // <说明语句表>→<说明语句><说明语句表递归>
// static ParseStatus parse_decl_list(Parser* parser) {
//     if (parse_decl(parser) == PARSE_STATUS_FAILED) {
//         return PARSE_STATUS_FAILED;
//     }

//     // the following if-stmt will always be False because ε-production
//     if(parse_decl_list_rec(parser) == PARSE_STATUS_FAILED) {
//         return PARSE_STATUS_FAILED;
//     }

//     return PARSE_STATUS_OK;
// }

// // <说明语句表递归>→;<说明语句><说明语句表递归>|ε
// static ParseStatus parse_decl_list_rec(Parser* parser) {
//     if(advance_token(parser)->type != TOK_PUNCT_SEMICOLON){
//         retreat_token(parser);
//     }   
//     else{
//         parse_decl(parser);
//     }
//     return PARSE_STATUS_OK;
// }

// // <说明语句>→<变量说明>│<函数说明>
// static ParseStatus parse_decl(Parser* parser) {
//     if(advance_token(parser)->type != TOK_INTEGER){
//         retreat_token(parser);
//         return PARSE_STATUS_FAILED;
//     }
//     else{
//         if(parse_decl_func(parser) == PARSE_STATUS_FAILED){
//             return parse_decl_var(parser);
//         }
//         return PARSE_STATUS_OK;
//     }
// }

// // <变量说明>→(integer) <变量> 
// static ParseStatus parse_decl_var(Parser* parser){
    
// }

// // <变量>→<标识符>

// // <函数说明>→(integer) function <标识符>（<参数>）；<函数体>
// static ParseStatus parse_decl_func(Parser* parser){
//     if(advance_token(parser)->type != TOK_FUNCTION){
//         retreat_token(parser);
//         return PARSE_STATUS_FAILED;
//     }
//     if(advance_token(parser)->type != TOK_IDENTIFIER){
//         log_error(parser->errlog, parse_error_format(parser->current_line, "identifier", token->lexeme));
//         return PARSE_STATUS_FAILED;
//     }
//     if(advance_token(parser)->type != TOK_PUNCT_LPAR){
//         log_error();
//         return PARSE_STATUS_FAILED;
//     }

//     if(parse_param(parser) == PARSE_STATUS_FAILED){
//         return PARSE_STATUS_FAILED;
//     }

//     if(advance_token(parser)->type != TOK_PUNCT_RPAR){
//         log_error();
//         return PARSE_STATUS_FAILED;
//     }
//     if(advance_token(parser)->type !=
//     if(advance_token(parser)->type !=

// }

// // <参数>→<变量> 

// // <函数体>→begin <说明语句表>；<执行语句表> end

// // <执行语句表>→<执行语句><执行语句表递归>
// static ParseStatus parse_stmt_list(Parser* parser) {
// }
// // <执行语句表递归>→ ；<执行语句><执行语句表递归>|ε

// // <执行语句>→<读语句>│<写语句>│<赋值语句>│<条件语句>

// // <读语句>→read(<变量>)

// // <写语句>→write(<变量>)

// // <赋值语句>→<变量>:=<算术表达式>

// // <算术表达式>→<项><算术表达式递归>

// // <算术表达式递归>→-<项><算术表达式递归>│ε

// // <项>→<因子><项递归>

// // <项递归>→*<因子><项递归>│ε

// // <因子>→<变量>│<常数>│<函数调用>

// // <函数调用>→<标识符>(<参数>)

// // <条件语句>→if<条件表达式>then<执行语句>else <执行语句>

// // <条件表达式>→<算术表达式><关系运算符><算术表达式>



// CORE_API void recursive_descent_parse(TokenStream* tokenstream, VarTable* vartab, ProcTable* proctab, ErrorLogger* errlog) {
//     Parser* parser = init_parser(tokenstream, vartab, proctab);

    
    
// }


static ParseStatus parse_program(Parser* parser) {
    return parse_subprogram(parser);
}

static ParseStatus parse_subprogram(Parser* parser) {
    Token* token = advance_token(parser);
    if (!token || token->type != TOK_BEGIN) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_BEGIN], token ? token->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }

    parser->current_level++;
    strncpy(parser->current_proc, "main", MAX_LEN_PROC_NAME);

    if (parse_decl_list(parser) == PARSE_STATUS_FAILED) {
        return PARSE_STATUS_FAILED;
    }

    token = advance_token(parser);
    if (!token || token->type != TOK_PUNCT_SEMICOLON) {
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

    parser->current_level--;
    parser->current_proc[0] = '\0';

    return PARSE_STATUS_OK;
}

static ParseStatus parse_decl_list(Parser* parser) {
    if (parse_decl(parser) == PARSE_STATUS_FAILED) {
        return PARSE_STATUS_FAILED;
    }

    if (parse_decl_list_rec(parser) == PARSE_STATUS_FAILED) {
        return PARSE_STATUS_FAILED;
    }

    return PARSE_STATUS_OK;
}

static ParseStatus parse_decl_list_rec(Parser* parser) {
    Token* token = advance_token(parser);
    if (!token) return PARSE_STATUS_OK;
    if (token->type != TOK_PUNCT_SEMICOLON) {
        retreat_token(parser);
    } else {
        if (parse_decl(parser) == PARSE_STATUS_FAILED) {
            return PARSE_STATUS_FAILED;
        }
        if (parse_decl_list_rec(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    }
    return PARSE_STATUS_OK;
}

static ParseStatus parse_decl(Parser* parser) {
    Token* token = advance_token(parser);
    if (!token || token->type != TOK_INTEGER) {
        retreat_token(parser);
        return PARSE_STATUS_FAILED;
    } else {
        // try function first
        if (parse_decl_func(parser) == PARSE_STATUS_FAILED) {
            // not a function -> treat as var declaration
            // rewind: the function attempt may have consumed tokens; best-effort: set index back to after the integer
            // For simplicity we assume parse_decl_func will retreat on failure; otherwise handle var here:
            return parse_decl_var(parser);
        }
        return PARSE_STATUS_OK;
    }
}

static ParseStatus parse_decl_var(Parser* parser){
    Token* token = advance_token(parser);
    if (!token || token->type != TOK_IDENTIFIER) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "identifier", token ? token->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    // insert variable into var table
    if (lookup_var(parser->vartab, token->lexeme) >= 0) {
        // duplicate -> log but continue
        log_error(parser->errlog, parse_error_format(parser->current_line, "new variable", token->lexeme));
    } else {
        insert_var(parser->vartab, token->lexeme, parser->current_proc, VAR_KIND_NORMAL, VAR_TYPE_INT, parser->current_level);
    }
    return PARSE_STATUS_OK;
}

static ParseStatus parse_decl_func(Parser* parser){
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

    // insert proc
    if (lookup_proc(parser->proctab, ident->lexeme) >= 0) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "new procedure", ident->lexeme));
    } else {
        insert_proc(parser->proctab, ident->lexeme, PROC_TYPE_INT, parser->current_level, 0, 0);
    }

    Token* t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_LPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_LPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }

    // parse optional parameter
    if (peek_token(parser, 1) && peek_token(parser, 1)->type == TOK_IDENTIFIER) {
        if (parse_param(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    }

    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_RPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_RPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }

    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_SEMICOLON) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_SEMICOLON], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }

    // parse function body
    // enter new proc scope
    parser->current_level++;
    strncpy(parser->current_proc, ident->lexeme, MAX_LEN_PROC_NAME);

    if (parse_decl_list(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;

    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_SEMICOLON) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_SEMICOLON], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }

    if (parse_stmt_list(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;

    t = advance_token(parser);
    if (!t || t->type != TOK_END) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_END], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }

    // leave proc scope
    parser->current_level--;
    parser->current_proc[0] = '\0';

    return PARSE_STATUS_OK;
}

static ParseStatus parse_param(Parser* parser){
    Token* t = advance_token(parser);
    if (!t || t->type != TOK_IDENTIFIER) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "parameter identifier", t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    // insert parameter as variable with VAR_KIND_PARAM
    if (lookup_var(parser->vartab, t->lexeme) >= 0) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "new parameter", t->lexeme));
    } else {
        insert_var(parser->vartab, t->lexeme, parser->current_proc, VAR_KIND_PARAM, VAR_TYPE_INT, parser->current_level + 1);
    }
    return PARSE_STATUS_OK;
}

static ParseStatus parse_stmt_list(Parser* parser) {
    if (parse_stmt(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    if (parse_stmt_list_rec(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return PARSE_STATUS_OK;
}

static ParseStatus parse_stmt_list_rec(Parser* parser) {
    Token* t = advance_token(parser);
    if (!t) return PARSE_STATUS_OK;
    if (t->type != TOK_PUNCT_SEMICOLON) {
        retreat_token(parser);
        return PARSE_STATUS_OK;
    }
    if (parse_stmt(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return parse_stmt_list_rec(parser);
}

static ParseStatus parse_stmt(Parser* parser) {
    Token* t = peek_token(parser, 1);
    if (!t) return PARSE_STATUS_FAILED;

    if (t->type == TOK_READ) {
        advance_token(parser);
        return parse_read(parser);
    } else if (t->type == TOK_WRITE) {
        advance_token(parser);
        return parse_write(parser);
    } else if (t->type == TOK_IF) {
        advance_token(parser);
        return parse_condition(parser);
    } else if (t->type == TOK_IDENTIFIER) {
        // could be assignment or call
        advance_token(parser);
        Token* next = peek_token(parser, 1);
        if (!next) {
            log_error(parser->errlog, parse_error_format(parser->current_line, ":=", "<eof>"));
            return PARSE_STATUS_FAILED;
        }
        if (strcmp(next->lexeme, ":=") == 0) {
            // assignment
            return parse_assignment(parser);
        } else if (next->type == TOK_PUNCT_LPAR) {
            // function call
            return parse_call(parser);
        } else {
            log_error(parser->errlog, parse_error_format(parser->current_line, "statement", next->lexeme));
            return PARSE_STATUS_FAILED;
        }
    } else {
        log_error(parser->errlog, parse_error_format(parser->current_line, "statement", t->lexeme));
        return PARSE_STATUS_FAILED;
    }
}

static ParseStatus parse_read(Parser* parser) {
    Token* t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_LPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_LPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    Token* id = advance_token(parser);
    if (!id || id->type != TOK_IDENTIFIER) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "identifier", id ? id->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    if (lookup_var(parser->vartab, id->lexeme) < 0) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "defined variable", id->lexeme));
        return PARSE_STATUS_FAILED;
    }
    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_RPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_RPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    return PARSE_STATUS_OK;
}

static ParseStatus parse_write(Parser* parser) {
    Token* t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_LPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_LPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    Token* id = advance_token(parser);
    if (!id || id->type != TOK_IDENTIFIER) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "identifier", id ? id->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    if (lookup_var(parser->vartab, id->lexeme) < 0) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "defined variable", id->lexeme));
        return PARSE_STATUS_FAILED;
    }
    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_RPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_RPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    return PARSE_STATUS_OK;
}

static ParseStatus parse_assignment(Parser* parser) {
    // current token is identifier (already consumed by caller)
    Token* assignee = peek_token(parser, 0);
    if (!assignee) return PARSE_STATUS_FAILED;
    Token* t = advance_token(parser); // this was identifier earlier
    Token* op = advance_token(parser);
    if (!op || strcmp(op->lexeme, ":=") != 0) {
        log_error(parser->errlog, parse_error_format(parser->current_line, ":=", op ? op->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    if (lookup_var(parser->vartab, t->lexeme) < 0) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "defined variable", t->lexeme));
        return PARSE_STATUS_FAILED;
    }
    if (parse_expression(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return PARSE_STATUS_OK;
}

static ParseStatus parse_call(Parser* parser) {
    // identifier already consumed by caller
    Token* func = peek_token(parser, 0);
    Token* t = advance_token(parser); // identifier
    t = advance_token(parser); // '('
    if (!t || t->type != TOK_PUNCT_LPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_LPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    // optional parameter list (only single variable allowed in grammar)
    Token* next = peek_token(parser, 1);
    if (next && next->type == TOK_IDENTIFIER) {
        Token* id = advance_token(parser);
        if (lookup_var(parser->vartab, id->lexeme) < 0) {
            log_error(parser->errlog, parse_error_format(parser->current_line, "defined variable", id->lexeme));
            return PARSE_STATUS_FAILED;
        }
    }
    t = advance_token(parser);
    if (!t || t->type != TOK_PUNCT_RPAR) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_PUNCT_RPAR], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    return PARSE_STATUS_OK;
}

static ParseStatus parse_expression(Parser* parser) {
    if (parse_term(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return parse_expression_rec(parser);
}

static ParseStatus parse_expression_rec(Parser* parser) {
    Token* next = peek_token(parser, 1);
    if (next && next->lexeme[0] == '-') {
        advance_token(parser);
        if (parse_term(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
        return parse_expression_rec(parser);
    }
    return PARSE_STATUS_OK;
}

static ParseStatus parse_term(Parser* parser) {
    if (parse_factor(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return parse_term_rec(parser);
}

static ParseStatus parse_term_rec(Parser* parser) {
    Token* next = peek_token(parser, 1);
    if (next && next->lexeme[0] == '*') {
        advance_token(parser);
        if (parse_factor(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
        return parse_term_rec(parser);
    }
    return PARSE_STATUS_OK;
}

static ParseStatus parse_factor(Parser* parser) {
    Token* t = peek_token(parser, 1);
    if (!t) return PARSE_STATUS_FAILED;
    if (t->type == TOK_IDENTIFIER) {
        advance_token(parser);
        Token* next = peek_token(parser, 1);
        if (next && next->type == TOK_PUNCT_LPAR) {
            // function call
            return parse_call(parser);
        } else {
            // variable usage -> check defined
            if (lookup_var(parser->vartab, t->lexeme) < 0) {
                log_error(parser->errlog, parse_error_format(parser->current_line, "defined variable", t->lexeme));
                return PARSE_STATUS_FAILED;
            }
            return PARSE_STATUS_OK;
        }
    } else if (t->type == TOK_NUM_LITERAL) {
        advance_token(parser);
        return PARSE_STATUS_OK;
    } else {
        log_error(parser->errlog, parse_error_format(parser->current_line, "factor", t->lexeme));
        return PARSE_STATUS_FAILED;
    }
}

static ParseStatus parse_condition(Parser* parser) {
    if (parse_condition_expr(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    Token* t = advance_token(parser);
    if (!t || t->type != TOK_THEN) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_THEN], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    if (parse_stmt(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    t = advance_token(parser);
    if (!t || t->type != TOK_ELSE) {
        log_error(parser->errlog, parse_error_format(parser->current_line, keywords[TOK_ELSE], t ? t->lexeme : "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    if (parse_stmt(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return PARSE_STATUS_OK;
}

static ParseStatus parse_condition_expr(Parser* parser) {
    if (parse_expression(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    Token* op = advance_token(parser);
    if (!op) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "relop", "<eof>"));
        return PARSE_STATUS_FAILED;
    }
    // accept common relational lexemes
    if (!(strcmp(op->lexeme, "<") == 0 || strcmp(op->lexeme, "<=") == 0 ||
          strcmp(op->lexeme, ">") == 0 || strcmp(op->lexeme, ">=") == 0 ||
          strcmp(op->lexeme, "=") == 0)) {
        log_error(parser->errlog, parse_error_format(parser->current_line, "relational operator", op->lexeme));
        return PARSE_STATUS_FAILED;
    }
    if (parse_expression(parser) == PARSE_STATUS_FAILED) return PARSE_STATUS_FAILED;
    return PARSE_STATUS_OK;
}

CORE_API void recursive_descent_parse(TokenStream* tokenstream, VarTable* vartab, ProcTable* proctab, ErrorLogger* errlog) {
    Parser* parser = init_parser(tokenstream, vartab, proctab, errlog);
    if (!parser) return;

    parse_program(parser);

    free_parser(parser);
}