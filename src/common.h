#ifndef COMMON_H
#define COMMON_H

#include "error.h"
#include "tok.h"
// #include "sym.h"


// error
void log_error(ErrorLogger*, const char *message, size_t line, const char* spec);
// sym
// delete is not nessesary for current grammar
// int lookup_var(VarTable* , const char* vname);
// void insert_var(VarTable* ,  
//     const char* vname, 
//     const char* vproc, 
//     VarKind vkind, 
//     VarType vtype, 
//     int vlev
// );

// int lookup_proc(ProcTable* , const char* );
// void insert_proc(ProcTable* ,  
//     const char* pname, 
//     ProcType ptype, 
//     int plev, 
//     int fadr, 
//     int ladr
// );

#endif // COMMON_H
