#include"common.h"

/*Stack*/
struct Stack{
    size_t data[MAX_LEN_STACK];
    int top;
};

Stack* init_stack(){
    Stack* s = (Stack*)malloc(sizeof(Stack));
    s->top = -1;
    return s;
}

bool stack_empty(Stack *s) {
    return s->top == -1;
}

bool stack_full(Stack *s) {
    return s->top == MAX_LEN_STACK - 1;
}

void stack_push(Stack *s, size_t value) {
    if (stack_full(s)) {
        printf("Stack overflow!\n");
        return;
    }
    s->data[++(s->top)] = value;
}

size_t stack_pop(Stack *s) {
    if (stack_empty(s)) {
        printf("Stack underflow!\n");
        exit(1);
    }
    return s->data[(s->top)--];
}