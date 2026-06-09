#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void initQueue(Queue *q)
{
    q->front = NULL;
    q->rear = NULL;
}

int isEmpty(Queue *q)
{
    return q->front == NULL;
}


void enqueue(Queue *q, int value)
{
    QueueNode *newNode =
        (QueueNode *)malloc(sizeof(QueueNode));

    if (newNode == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    newNode->data = value;
    newNode->next = NULL;

    
    if (isEmpty(q))
    {
        q->front = newNode;
        q->rear = newNode;
        return;
    }

    
    q->rear->next = newNode;
    q->rear = newNode;
}



int dequeue(Queue *q)
{
    if (isEmpty(q))
    {
        printf("Queue is empty!\n");
        return -1;
    }

    QueueNode *temp = q->front;

    int value = temp->data;

    q->front = q->front->next;

    
    if (q->front == NULL)
    {
        q->rear = NULL;
    }

    free(temp);

    return value;
}


int peek(Queue *q)
{
    if (isEmpty(q))
    {
        printf("Queue is empty!\n");
        return -1;
    }

    return q->front->data;
}


void displayQueue(Queue *q)
{
    QueueNode *current = q->front;

    printf("\n========== QUEUE ==========\n");

    while (current != NULL)
    {
        printf("%d -> ", current->data);
        current = current->next;
    }

    printf("NULL\n");
    printf("===========================\n");
}


void freeQueue(Queue *q)
{
    while (!isEmpty(q))
    {
        dequeue(q);
    }
}