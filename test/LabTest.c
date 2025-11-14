#include <stdio.h>
#include "..\include\core.h"

/*fopen is based on cwd*/
#define INPUTE_FILE1 "cases/test1.mini"
#define INPUTE_FILE2 "cases/test2.mini"
#define OUTPUT_DIR "output/"


void test_lexer(){
    printf("Starting lexer test...\n");
    Lexer* lexer = lexer_init(INPUTE_FILE1, OUTPUT_DIR);
    if(lexer == NULL){
        printf("Lexer initialization failed\n");
        return;
    }
    printf("Lexer initialized successfully\n");
    lexical_analyze(lexer);
    printf("Lexical analysis completed\n");

    lexer_destroy(lexer);
    printf("Lexer destroyed successfully\n");
}

int main(){
    test_lexer();
    return 0;
}