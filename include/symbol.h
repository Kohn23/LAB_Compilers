#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdbool.h>

// 变量种类
typedef enum {
    VAR_KIND_VARIABLE = 0,  // 变量
    VAR_KIND_PARAMETER = 1  // 形参
} VarKind;

// 符号类型
typedef enum {
    SYM_TYPE_INTEGER = 0,
    SYM_TYPE_FUNCTION = 1
} SymType;

// 变量符号
typedef struct {
    char name[16];          // 变量名
    char proc_name[16];     // 所属过程名
    VarKind kind;           // 分类 (0-变量, 1-形参)
    SymType type;           // 变量类型
    int level;              // 变量层次
    int address;            // 在变量表中的位置
} VarSymbol;

// 过程符号
typedef struct {
    char name[16];          // 过程名
    SymType type;           // 过程类型
    int level;              // 过程层次
    int first_var_addr;     // 第一个变量在变量表中的位置
    int last_var_addr;      // 最后一个变量在变量表中的位置
} ProcSymbol;

// 符号表
typedef struct {
    VarSymbol* var_table;   // 变量表
    int var_count;
    int var_capacity;
    
    ProcSymbol* proc_table; // 过程表  
    int proc_count;
    int proc_capacity;
    
    int current_level;      // 当前层次
} SymbolTable;

// 函数声明
SymbolTable* symbol_table_create(void);
void symbol_table_free(SymbolTable* table);
bool symbol_table_add_var(SymbolTable* table, const char* name, const char* proc_name, 
                         VarKind kind, SymType type, int level);
bool symbol_table_add_proc(SymbolTable* table, const char* name, SymType type, int level);

#endif