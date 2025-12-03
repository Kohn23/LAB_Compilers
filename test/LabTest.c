#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "..\include\minicomp.h"

/*fopen is based on cwd*/
#define INPUTE_FILE1 "./test/cases/test1.mini"
#define INPUTE_FILE2 "./test/cases/test2.mini"
#define OUTPUT_DIR "./test/output/"

#define PATH_SEPARATOR '/'

char* get_base_filename(const char *full_path) {
    static char base_name[256];
    
    const char *last_slash = strrchr(full_path, PATH_SEPARATOR);
#ifdef _WIN32
    const char *last_backslash = strrchr(full_path, PATH_SEPARATOR);
    if (last_backslash && (!last_slash || last_backslash > last_slash)) {
        last_slash = last_backslash;
    }
#endif
    
    const char *filename = last_slash ? last_slash + 1 : full_path;
    
    strncpy(base_name, filename, sizeof(base_name) - 1);
    base_name[sizeof(base_name) - 1] = '\0';
    
    char *dot = strrchr(base_name, '.');
    if (dot) {
        *dot = '\0';
    }
    
    return base_name;
}


void test(){
    char *base_filename = get_base_filename(INPUTE_FILE1);
    char dyd_filename[256];
    char err_filename[256];
    char var_filename[256];
    char proc_filename[256];
    snprintf(dyd_filename, sizeof(dyd_filename), "%s%s.dyd", OUTPUT_DIR, base_filename);
    snprintf(err_filename, sizeof(err_filename), "%s%s.err", OUTPUT_DIR, base_filename);
    snprintf(var_filename, sizeof(var_filename), "%s%s.var", OUTPUT_DIR, base_filename);
    snprintf(proc_filename, sizeof(proc_filename), "%s%s.proc", OUTPUT_DIR, base_filename);


    ErrorLogger* error_logger = init_errorlogger();
    TokenStream* token_stream = init_tokenstream();
    VarTable* var_table = init_var_table();
    ProcTable* proc_table = init_proc_table();

    // lex
    lex_analyze(INPUTE_FILE1, token_stream, error_logger);
    // parse
    recursive_descent_parse(token_stream, var_table, proc_table, error_logger);
    
    // Print tokenstream to files
    fprint_tokenstream(dyd_filename, token_stream);
    fprint_var_table(var_filename, var_table);
    fprint_proc_table(proc_filename, proc_table);
    fprint_errors(err_filename, error_logger);

    free_errorlogger(error_logger);
    free_tokenstream(token_stream);
    free_var_table(var_table);
    free_proc_table(proc_table);
}

int main(){
    test();
    return 0;
}