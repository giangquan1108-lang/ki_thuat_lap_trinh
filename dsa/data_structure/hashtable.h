#ifndef HASHTABLE_H
#define HASHTABLE_H

#define TABLE_SIZE 101

typedef struct Node {
    int key;
    char value[100];
    struct Node *next;
} Node;

typedef struct {
    Node *table[TABLE_SIZE];
} HashTable;

void initHashTable(HashTable *ht);

int hashFunction(int key);

void insert(HashTable *ht, int key, const char *value);

Node *search(HashTable *ht, int key);

void deleteKey(HashTable *ht, int key);

void display(HashTable *ht);

void freeHashTable(HashTable *ht);

#endif