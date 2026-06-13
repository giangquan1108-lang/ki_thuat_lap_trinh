#ifndef LINKEDLIST_C_H
#define LINKEDLIST_C_H

typedef struct CNode {
    int data;
    struct CNode *next;
} CNode;

typedef struct {
    CNode *head;
} CLinkedList;

void initList(CLinkedList* list);
CNode* createNode(int x);
void insertFirst(CLinkedList* list, int x);
void insertLast(CLinkedList* list, int x);
void deleteFirst(CLinkedList* list);
void deleteValue(CLinkedList* list, int x);
CNode* search(CLinkedList* list, int x);
void display(CLinkedList* list);
void freeList(CLinkedList* list);

#endif