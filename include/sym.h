#ifndef _SYMBOL_H
#define _SYMBOL_H

#define MAX_LEN_VAR_NAME 16
#define MAX_LEN_PROC_NAME 16
#define MAX_TABLE_SIZE 100

// Symbol Table is splitted into Variable Table and Procedure Table as experiment required
// VarType only contains INT and FUNCT for now, can be extended later

typedef enum {
    VAR_KIND_NORMAL,
    VAR_KIND_PARAM
}VarKind;

typedef enum{
    VAR_TYPE_INT,
    VAR_TYPE_FUNCT
}VarType

typedef struct{
    char vname[MAX_LEN_VAR_NAME];
    char vproc[MAX_LEN_PROC_NAME];
    VarKind vkind;
    VarType vtype;
    int vlev;
    int vaddr; // offset in table
} VarAttr;

typedef struct {
    VarAttr vattr;
    int is_occupied;
} VarEntry;

// linear probing
typedef struct {
    VarEntry entries[MAX_TABLE_SIZE];  
    int count;                         
    int next_addr; 
} VarTable;

typedef enum {
    PROC_TYPE_MAIN,     
    PROC_TYPE_FUNCTION  
} ProcType;

typedef struct{
    char pname[MAX_LEN_PROC_NAME];  
    ProcType ptype;                 
    int plev;                       
    int fadr;                       
    int ladr;                       
} ProcAttr;

typedef struct {
    ProcAttr attr;      
    int is_occupied;    
} ProcEntry;

typedef struct {
    ProcEntry entries[MAX_TABLE_SIZE];  
    int count;
} ProcTable;

#endif /* _SYMBOL_H */