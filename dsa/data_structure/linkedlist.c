#include <stdio.h>
#include <stdlib.h>
#include "linkedlist_c.h"

void initList(CLinkedList* list) {
    list->head = NULL;
}

CNode* createNode(int x) {
    CNode* p = (CNode*)malloc(sizeof(CNode));

    if (p == NULL)
        return NULL;

    p->data = x;
    p->next = NULL;

    return p;
}

void insertFirst(CLinkedList* list, int x) {
    CNode* p = createNode(x);

    if (p == NULL)
        return;

    p->next = list->head;
    list->head = p;
}

void insertLast(CLinkedList* list, int x) {
    CNode* p = createNode(x);

    if (p == NULL)
        return;

    if (list->head == NULL) {
        list->head = p;
        return;
    }

    CNode* cur = list->head;

    while (cur->next != NULL)
        cur = cur->next;

    cur->next = p;
}

void deleteFirst(CLinkedList* list) {
    if (list->head == NULL)
        return;

    CNode* temp = list->head;
    list->head = temp->next;

    free(temp);
}

void deleteValue(CLinkedList* list, int x) {
    CNode* cur = list->head;
    CNode* prev = NULL;

    while (cur != NULL && cur->data != x) {
        prev = cur;
        cur = cur->next;
    }

    if (cur == NULL)
        return;

    if (prev == NULL)
        list->head = cur->next;
    else
        prev->next = cur->next;

    free(cur);
}

CNode* search(CLinkedList* list, int x) {
    CNode* cur = list->head;

    while (cur != NULL) {
        if (cur->data == x)
            return cur;

        cur = cur->next;
    }

    return NULL;
}

void display(CLinkedList* list) {
    CNode* cur = list->head;

    while (cur != NULL) {
        printf("%d ", cur->data);
        cur = cur->next;
    }

    printf("\n");
}

void freeList(CLinkedList* list) {
    CNode* cur = list->head;

    while (cur != NULL) {
        CNode* temp = cur;
        cur = cur->next;
        free(temp);
    }

    list->head = NULL;
}