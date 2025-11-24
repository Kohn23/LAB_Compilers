#include"sym.h"

CORE_API VarTable* init_var_table(){
    VarTable* table = (VarTable*)malloc(sizeof(VarTable));
    table->count = 0;
    return table;
}

CORE_API ProcTable* init_proc_table(){
    ProcTable* table = (ProcTable*)malloc(sizeof(ProcTable));
    table->count = 0;
    return table;
}

CORE_API void free_var_table(VarTable* table){
    if(table){
        free(table);
    }
}

CORE_API void free_proc_table(ProcTable* table){
    if(table){
        free(table);
    }
}

CORE_API void fprint_var_table(const char* filename, VarTable* table){
    FILE* fp = fopen(filename, "w");
    if(fp == NULL){
        perror("Error");
        return;
    }
    for(int i = 0; i < table->count; i++){
        VarAttr* attr = &table->entries[i];
        fprintf(fp, "vname: %s, vproc: %s, vkind: %d, vtype: %d, vlev: %d, vaddr: %d\n",
                attr->vname, attr->vproc, attr->vkind, attr->vtype, attr->vlev, attr->vaddr);
    }
    fclose(fp);
}

CORE_API void fprint_proc_table(const char* filename, ProcTable* table){
    FILE* fp = fopen(filename, "w");
    if(fp == NULL){
        perror("Error");
        return;
    }
    for(int i = 0; i < table->count; i++){
        ProcAttr* attr = &table->entries[i];
        fprintf(fp, "pname: %s, ptype: %d, plev: %d, faddr: %d, laddr: %d\n",
                attr->pname, attr->ptype, attr->plev, attr->fadr, attr->ladr);
    }
    fclose(fp);
}
