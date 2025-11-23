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


void test_lexer(){
    ErrorLogger* error_logger = init_errorlogger();
    TokenStream* token_stream = init_tokenstream();
    lex_analyze(INPUTE_FILE2, token_stream, error_logger);

    // Generate output files
    char *base_filename = get_base_filename(INPUTE_FILE2);
    char dyd_filename[256];
    char err_filename[256];
    snprintf(dyd_filename, sizeof(dyd_filename), "%s%s.dyd", OUTPUT_DIR, base_filename);
    snprintf(err_filename, sizeof(err_filename), "%s%s.err", OUTPUT_DIR, base_filename);
    // Print tokenstream to files
    fprint_tokenstream(dyd_filename, token_stream);
    fprint_errors(err_filename, error_logger);

    free_errorlogger(error_logger);
    free_tokenstream(token_stream);
}

int main(){
    test_lexer();
    return 0;
}