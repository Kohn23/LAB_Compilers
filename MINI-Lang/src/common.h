#ifndef COMMON_H
#define COMMON_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

/*Common data structure*/

// Stack was a nice try but not stupid actually
#define MAX_LEN_STACK 32

typedef struct Stack Stack;

Stack* init_stack();
bool stack_empty(Stack* );
bool stack_full(Stack* );
void stack_push(Stack* , size_t value);
size_t stack_pop(Stack* );

#include "error.h"
#include "tok.h"
#include "sym.h"


// error
void log_error(ErrorLogger*, ErrorInfo);
// sym
// delete is not nessesary for current grammar
int lookup_var(VarTable* , const char* vname, int lev);
int insert_param(VarTable* var_table, const char* vname, int lev);
int insert_var(VarTable* ,  
    const char* vname, 
    const char* vproc, 
    VarKind vkind, 
    VarType vtype, 
    int vlev
);

int lookup_proc(ProcTable* , const char* name, int lev);
int insert_proc(ProcTable* ,  
    const char* pname, 
    ProcType ptype, 
    int plev, 
    int fadr, 
    int ladr
);

#endif // COMMON_H
