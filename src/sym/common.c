#include"common.h"
#include<string.h>

int lookup_var(VarTable* var_table, const char* name) {
    for (size_t i = 0; i < var_table->count; ++i) {
        if (strcmp(var_table->entries[i].vname, name) == 0) {
            return i; // Found, return index
        }
    }
    return -1; // Not found
}

int insert_var(VarTable* var_table,  
    const char* vname, 
    const char* vproc, 
    VarKind vkind, 
    VarType vtype, 
    int vlev
) {
    if (var_table->count >= MAX_TABLE_SIZE) {
        // Table full, handle error as needed
        return -1;
    }

    // check for existing entry
    if (lookup_var(var_table, vname) != -1) {
        // Variable already exists, handle error as needed
        return -1;
    }

    VarAttr* entry = &var_table->entries[var_table->count];
    strncpy(entry->vname, vname, MAX_LEN_VAR_NAME);
    strncpy(entry->vproc, vproc, MAX_LEN_PROC_NAME);
    entry->vkind = vkind;
    entry->vtype = vtype;
    entry->vlev = vlev;
    entry->vaddr = var_table->count;
    var_table->count++;

    return var_table->count -1;
}

int lookup_proc(ProcTable* proc_table, const char* name) {
    for (size_t i = 0; i < proc_table->count; ++i) {
        if (strcmp(proc_table->entries[i].pname, name) == 0) {
            return i; // Found, return index
        }
    }
    return -1; // Not found
}

int insert_proc(ProcTable* proc_table,  
    const char* pname, 
    ProcType ptype, 
    int plev, 
    int fadr, 
    int ladr
) {
    if (proc_table->count >= MAX_TABLE_SIZE) {
        // Table full, handle error as needed
        return -1;
    }

    // check for existing entry
    if (lookup_proc(proc_table, pname) != -1) {
        // Procedure already exists, handle error as needed
        return -1;
    }

    ProcAttr* entry = &proc_table->entries[proc_table->count++];
    strncpy(entry->pname, pname, MAX_LEN_PROC_NAME);
    entry->ptype = ptype;
    entry->plev = plev;
    entry->fadr = fadr;
    entry->ladr = ladr;

    return proc_table->count - 1;
}

