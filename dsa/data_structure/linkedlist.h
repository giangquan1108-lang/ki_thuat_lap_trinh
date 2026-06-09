#ifndef LINKEDLIST_H
#define LINKEDLIST_H

typedef struct Node {
    int data;
    struct Node* next;
} Node;

typedef struct {
    Node* head;
} LinkedList;

void initList(LinkedList* list);

Node* createNode(int x);

void insertFirst(LinkedList* list, int x);

void insertLast(LinkedList* list, int x);

void deleteFirst(LinkedList* list);

void deleteValue(LinkedList* list, int x);

Node* search(LinkedList* list, int x);

void display(LinkedList* list);

void freeList(LinkedList* list);

#endif