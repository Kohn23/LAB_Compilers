#include "parser.h"
#include "common.h"

static const char* keywords[] = {
#define DEF(macro, str) str ,
#include "tok.def"
#undef DEF
};

static Parser* init_parser(TokenStream* , VarTable* , ProcTable* , ErrorLogger* ) {
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

static Token* advance_token(Parser* parser) {
    if (parser->token_index < parser->tokenstream->count) {
        parser->token_index++;
        if (parser->tokenstream->tokens[parser->token_index - 1].type == TOK_EOLN) {
            parser->current_line++;
            parser->token_index++;
        }
        return &parser->tokenstream->tokens[parser->token_index];
    }
}

// <程序>→<分程序>
static ParseStatus parse_program(Parser* parser) {
    return parse_subprogram(parser);
}

// <分程序>→begin <说明语句表>；<执行语句表> end
static ParseStatus parse_subprogram(Parser* parser) {
    Token* token;
    ParseStatus status;
    token = advance_token(parser);
    if (token->type != TOK_BEGIN) {
        log_error(parser->errlog, "Expecting", parser->current_line, "begin");
        return PARSE_STATUS_FAILED;
    }

    status = parse_declaration_list(parser);

    token = advance_token(parser); 
    if (token->type != TOK_PUNCT_SEMICOLON) {
        log_error(parser->errlog, "Expecting", parser->current_line, token->lexeme);
        return PARSE_STATUS_FAILED;
    }
}

// <说明语句表>→<说明语句><说明语句表递归>

// <说明语句表递归>→ ；<说明语句><说明语句表递归>|ε

// <说明语句>→<变量说明>│<函数说明>

// <变量说明>→integer <变量> 

// <变量>→<标识符>

// <函数说明>→integer function <标识符>（<参数>）；<函数体>

// <参数>→<变量> 

// <函数体>→begin <说明语句表>；<执行语句表> end

// <执行语句表>→<执行语句><执行语句表递归>

// <执行语句表递归>→ ；<执行语句><执行语句表递归>|ε

// <执行语句>→<读语句>│<写语句>│<赋值语句>│<条件语句>

// <读语句>→read(<变量>)

// <写语句>→write(<变量>)

// <赋值语句>→<变量>:=<算术表达式>

// <算术表达式>→<项><算术表达式递归>

// <算术表达式递归>→-<项><算术表达式递归>│ε

// <项>→<因子><项递归>

// <项递归>→*<因子><项递归>│ε

// <因子>→<变量>│<常数>│<函数调用>

// <函数调用>→<标识符>(<参数>)

// <条件语句>→if<条件表达式>then<执行语句>else <执行语句>

// <条件表达式>→<算术表达式><关系运算符><算术表达式>

// <关系运算符> →<│<=│>│>=│=│<>



CORE_API void recursive_descent_parse(TokenStream* tokenstream, VarTable* vartab, ProcTable* proctab, ErrorLogger* errlog) {
    Parser* parser = init_parser(tokenstream, vartab, proctab);

    
    
}
