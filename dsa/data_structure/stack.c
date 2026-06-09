#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

void initStack(Stack *s) {
    s->top = NULL;
}

int isEmpty(Stack *s) {
    return s->top == NULL;
}

void push(Stack *s, int x) {
    Node *p = (Node*)malloc(sizeof(Node));

    if (p == NULL) {
        printf("Khong du bo nho!\n");
        return;
    }

    p->data = x;
    p->next = s->top;
    s->top = p;
}

int pop(Stack *s) {
    if (isEmpty(s)) {
        printf("Stack rong!\n");
        return -1;
    }

    Node *p = s->top;
    int x = p->data;

    s->top = p->next;
    free(p);

    return x;
}

int peek(Stack *s) {
    if (isEmpty(s)) {
        printf("Stack rong!\n");
        return -1;
    }

    return s->top->data;
}

void freeStack(Stack *s) {
    while (!isEmpty(s))
        pop(s);
}