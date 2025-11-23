#ifndef COMMON_H
#define COMMON_H

#include "error.h"
#include "tok.h"
#include "sym.h"


// error
void log_error(ErrorLogger*, const char *message, size_t line, const char* character);
// sym
// int lookup_var(VarTable* , const char* , int );
// int insert_var(VarTable* , VarAttr* );
// int lookup_proc(ProcTable* , const char* );
// int insert_proc(ProcTable* , ProcAttr* );

#endif // COMMON_H