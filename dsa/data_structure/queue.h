#ifndef QUEUE_H
#define QUEUE_H

typedef struct QueueNode
{
    int data;
    struct QueueNode *next;
} QueueNode;

typedef struct
{
    QueueNode *front;
    QueueNode *rear;
} Queue;

void initQueue(Queue *q);

int isEmpty(Queue *q);

void enqueue(Queue *q, int value);

int dequeue(Queue *q);

int peek(Queue *q);

void displayQueue(Queue *q);

void freeQueue(Queue *q);

#endif