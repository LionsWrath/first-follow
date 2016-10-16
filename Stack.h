#ifndef STACK_H
#define STACK_H

#define STACK_MAX 100

typedef struct Stack {
    char data[STACK_MAX];
    int size;
} Stack;

void Init(Stack);
int Top(Stack);
void Push(Stack, int);
void Pop(Stack);

#endif
