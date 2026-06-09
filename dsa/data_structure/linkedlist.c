#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"

void initList(LinkedList* list) {
    list->head = NULL;
}

Node* createNode(int x) {
    Node* p = (Node*)malloc(sizeof(Node));

    if (p == NULL)
        return NULL;

    p->data = x;
    p->next = NULL;

    return p;
}

void insertFirst(LinkedList* list, int x) {
    Node* p = createNode(x);

    if (p == NULL)
        return;

    p->next = list->head;
    list->head = p;
}

void insertLast(LinkedList* list, int x) {
    Node* p = createNode(x);

    if (p == NULL)
        return;

    if (list->head == NULL) {
        list->head = p;
        return;
    }

    Node* cur = list->head;

    while (cur->next != NULL)
        cur = cur->next;

    cur->next = p;
}

void deleteFirst(LinkedList* list) {
    if (list->head == NULL)
        return;

    Node* temp = list->head;
    list->head = temp->next;

    free(temp);
}

void deleteValue(LinkedList* list, int x) {
    Node* cur = list->head;
    Node* prev = NULL;

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

Node* search(LinkedList* list, int x) {
    Node* cur = list->head;

    while (cur != NULL) {
        if (cur->data == x)
            return cur;

        cur = cur->next;
    }

    return NULL;
}

void display(LinkedList* list) {
    Node* cur = list->head;

    while (cur != NULL) {
        printf("%d ", cur->data);
        cur = cur->next;
    }

    printf("\n");
}

void freeList(LinkedList* list) {
    Node* cur = list->head;

    while (cur != NULL) {
        Node* temp = cur;
        cur = cur->next;
        free(temp);
    }

    list->head = NULL;
}