#include "parser.h"
#include "common.h"

static Parser* init_parser() {
    Parser* parser = (Parser*)malloc(sizeof(Parser));
    if (!parser) {
        return NULL;
    }
    parser->current_line = 1;
    parser->current_level = 0;
    parser->in_function = 0;
    parser->current_proc[0] = '\0';
    return parser;
}

static void free_parser(Parser* parser) {
    if (parser) {
        free(parser);
    }
}

//<程序>→<分程序>
//<分程序>→begin <说明语句表>；<执行语句表> end
//<说明语句表>→<说明语句><说明语句表递归>
//<说明语句表递归>→ ；<说明语句><说明语句表递归>|ε
//<说明语句>→<变量说明>│<函数说明>
//<变量说明>→integer <变量>
//<变量>→<标识符>
//<标识符>→<字母><标识符递归>
//<标识符递归>→<字母><标识符递归>|<数字><标识符递归>|ε
//<字母>→a│b│c│d│e│f│g│h│i│j│k│l│m│n│o │p│q │r│s│t│u│v│w│x│y│z
//<数字>→0│1│2│3│4│5│6│7│8│9
//<函数说明>→integer function <标识符>（<参数>）；<函数体>
//<参数>→<变量>:add 
//<函数体>→begin <说明语句表>；<执行语句表> end
//<执行语句表>→<执行语句><执行语句表递归>
//<执行语句表递归>→ ；<执行语句><执行语句表递归>|ε
//<执行语句>→<读语句>│<写语句>│<赋值语句>│<条件语句>
//<读语句>→read(<变量>)
//<写语句>→write(<变量>)
//<赋值语句>→<变量>:=<算术表达式>
//<算术表达式>→<项><算术表达式递归>
//<算术表达式递归>→-<项><算术表达式递归>│ε
//<项>→<因子><项递归>
//<项递归>→*<因子><项递归>│ε
//<因子>→<变量>│<常数>│<函数调用>
//<常数>→<无符号整数>
//<函数调用>→<标识符>(<参数>)
//<无符号整数>→<数字><无符号整数递归>
//<无符号整数递归>→<数字><无符号整数递归>│ε
//<条件语句>→if<条件表达式>then<执行语句>else <执行语句>
//<条件表达式>→<算术表达式><关系运算符><算术表达式>
//<关系运算符> →<│<=│>│>=│=│<>

CORE_API void recuersive_descent_parse(TokenStream* tokenstream, VarTable* vartab, ProcTable* proctab, ErrorLogger* errlog) {
    Parser* parser = init_parser(tokenstream, vartab, proctab);

    
    
}
