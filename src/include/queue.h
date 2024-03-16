#ifndef QUEUE_H
#define QUEUE_H

typedef struct _Node_S {
    void           *data;
    struct _Node_S *next;
} Q_Node;

typedef struct _Queue {
    Q_Node *_head;
    Q_Node *_last;
    int   size;
} Queue;

Queue* q_new_queue();
void  q_push(Queue *q, void *data);
void *q_pop(Queue* q);

#endif // !QUEUE_
