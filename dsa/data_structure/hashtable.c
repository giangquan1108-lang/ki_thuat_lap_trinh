#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"

int hashFunction(int key) {
    return key % TABLE_SIZE;
}

void initHashTable(HashTable *ht) {
    for (int i = 0; i < TABLE_SIZE; i++) {
        ht->table[i] = NULL;
    }
}

void insert(HashTable *ht, int key, const char *value) {

    int index = hashFunction(key);

    Node *current = ht->table[index];

    while (current != NULL) {
        if (current->key == key) {
            strcpy(current->value, value);
            return;
        }
        current = current->next;
    }

    Node *newNode = (Node *)malloc(sizeof(Node));

    if (newNode == NULL) {
        printf("Khong du bo nho!\n");
        return;
    }

    newNode->key = key;
    strcpy(newNode->value, value);

    newNode->next = ht->table[index];
    ht->table[index] = newNode;
}

Node *search(HashTable *ht, int key) {

    int index = hashFunction(key);

    Node *current = ht->table[index];

    while (current != NULL) {

        if (current->key == key)
            return current;

        current = current->next;
    }

    return NULL;
}

void deleteKey(HashTable *ht, int key) {

    int index = hashFunction(key);

    Node *current = ht->table[index];
    Node *prev = NULL;

    while (current != NULL) {

        if (current->key == key) {

            if (prev == NULL)
                ht->table[index] = current->next;
            else
                prev->next = current->next;

            free(current);
            return;
        }

        prev = current;
        current = current->next;
    }
}

void display(HashTable *ht) {

    for (int i = 0; i < TABLE_SIZE; i++) {

        printf("[%d]: ", i);

        Node *current = ht->table[i];

        while (current != NULL) {

            printf("(%d,%s) -> ",
                   current->key,
                   current->value);

            current = current->next;
        }

        printf("NULL\n");
    }
}

void freeHashTable(HashTable *ht) {

    for (int i = 0; i < TABLE_SIZE; i++) {

        Node *current = ht->table[i];

        while (current != NULL) {

            Node *temp = current;
            current = current->next;

            free(temp);
        }

        ht->table[i] = NULL;
    }
}