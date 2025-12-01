#include"common.h"
#include<string.h>

int lookup_var(VarTable* var_table, const char* name, int lev) {
    for (size_t i = 0; i < var_table->count; ++i) {
        if (strcmp(var_table->entries[i].vname, name) == 0 && var_table->entries[i].vlev == lev) {
            return i; // Found, return index
        }
    }
    return -1; // Not found
}

int insert_param(
    VarTable* var_table,  
    const char* vname,
    int vlev
){
    if (var_table->count >= MAX_TABLE_SIZE) {
        // Table full, handle error as needed
        return -1;
    }

    VarAttr* entry = &var_table->entries[var_table->count];
    strncpy(entry->vname, vname, MAX_LEN_VAR_NAME);
    entry->vkind = VAR_KIND_PARAM;
    entry->vaddr = var_table->count;
    entry->vlev = vlev + 1;
    var_table->count++;
    return var_table->count -1;
}


int insert_var(
    VarTable* var_table,  
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
    // if (lookup_var(var_table, vname, vlev) != -1) {
    //     // Variable already exists, handle error as needed
    //     return -1;
    // }

    int param_check = lookup_var(var_table, vname, vlev);
    if (param_check != -1){
        VarAttr* entry = &var_table->entries[param_check];
        // Not a param
        if(entry->vkind != VAR_KIND_PARAM) return -1;
        else{
            // param
            strncpy(entry->vproc, vproc, MAX_LEN_PROC_NAME);
            entry->vtype = vtype;
            entry->vlev = vlev;
            return param_check;
        }
    }
    else{
        // normal
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
}

int lookup_proc(ProcTable* proc_table, const char* name, int lev) {
    for (size_t i = 0; i < proc_table->count; ++i) {
        if (strcmp(proc_table->entries[i].pname, name) == 0 && proc_table->entries[i].plev == lev) {
            return i; // Found, return index
        }
    }
    return -1; // Not found
}

int insert_proc(ProcTable* proc_table,  
    const char* pname, 
    ProcType ptype, 
    int plev, 
    size_t fadr, 
    size_t ladr
) {
    if (proc_table->count >= MAX_TABLE_SIZE) {
        // Table full, handle error as needed
        return -1;
    }

    // check for existing entry
    if (lookup_proc(proc_table, pname, plev) != -1) {
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

