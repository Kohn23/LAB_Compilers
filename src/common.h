#ifndef COMMON_H
#define COMMON_H

#include "error.h"
#include "tok.h"
#include "sym.h"


// error
void log_error(ErrorLogger*, ErrorInfo);
// sym
// delete is not nessesary for current grammar
int lookup_var(VarTable* , const char* vname, int lev);
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
