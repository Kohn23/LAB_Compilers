#ifndef TOKENS_H
#define TOKENS_H

#include <stdbool.h>

// Token类型定义（按照实验要求）
typedef enum {
    // 关键字 (1-9)
    TOK_BEGIN = 1,
    TOK_END = 2,
    TOK_INTEGER = 3,
    TOK_FUNCTION = 4,
    TOK_READ = 5,
    TOK_WRITE = 6,
    TOK_IF = 7,
    TOK_THEN = 8,
    TOK_ELSE = 9,
    
    // 运算符 (10-20)
    TOK_ASSIGN = 10,      // :=
    TOK_LE = 11,          // <=
    TOK_LT = 12,          // <
    TOK_GE = 13,          // >=  
    TOK_GT = 14,          // >
    TOK_NE = 15,          // <>
    TOK_EQ = 16,          // =
    TOK_PLUS = 17,        // +
    TOK_MINUS = 18,       // -
    TOK_MULTIPLY = 19,    // *
    TOK_DIVIDE = 20,      // /
    
    // 分隔符 (21-24)
    TOK_LPAREN = 21,      // (
    TOK_RPAREN = 22,      // )
    TOK_SEMICOLON = 23,   // ;
    TOK_COMMA = 24,       // ,
    
    // 特殊标记 (25-26)
    TOK_EOLN = 25,        // 行结束
    TOK_EOF = 26,         // 文件结束
    
    // 标识符和常量 (27-28)
    TOK_IDENTIFIER = 27,
    TOK_NUMBER = 28
} TokenType;

// Token结构
typedef struct {
    TokenType type;
    char* value;        // 对于标识符和数字，存储实际值
    int line;           // 行号
    int column;         // 列号
} Token;

// Token列表
typedef struct {
    Token* tokens;
    int count;
    int capacity;
} TokenList;

// 函数声明
TokenList* token_list_create(void);
void token_list_add(TokenList* list, Token token);
void token_list_free(TokenList* list);
const char* token_type_to_string(TokenType type);

#endif