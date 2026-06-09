#ifndef STACK_H
#define STACK_H

typedef struct Node {
    int data;
    struct Node *next;
} Node;

typedef struct {
    Node *top;
} Stack;

void initStack(Stack *s);
int isEmpty(Stack *s);
void push(Stack *s, int x);
int pop(Stack *s);
int peek(Stack *s);
void freeStack(Stack *s);

#endif