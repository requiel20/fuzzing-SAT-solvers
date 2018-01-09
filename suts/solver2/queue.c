#include "queue.h"

QUEUElink QUEUEnewElement(void *item, QUEUElink next)
{
    QUEUElink x = (QUEUElink)malloc(sizeof(struct QUEUEnode));
    x->item = item;
    x->next = next;
    return x;
}

Q QUEUEinit()
{
    Q q = (Q)malloc(sizeof(struct queue));
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
    return q;
}

int QUEUEempty(Q q)
{
    return q->head == NULL;
}

void QUEUEinsert(Q q, void* item)
{
    q->size += 1;
    if (q->head == NULL) {
        q->tail = QUEUEnewElement(item, q->head);
        q->head = q->tail;
        return;
    }
    q->tail->next = QUEUEnewElement(item, q->tail->next);
    q->tail = q->tail->next;
}

void* QUEUEdequeue(Q q)
{
    if (q->head == NULL) {
        return NULL;
    }
    q->size -= 1;
    void* item = q -> head -> item;
    QUEUElink t = q -> head -> next;
    free(q->head);
    q->head = t;
    return item;
}

void QUEUEclear(Q q)
{
    while (q->head != NULL) {
        q->size -= 1;
        QUEUElink t = q -> head -> next;
        free(q->head);
        q->head = t;
    }
}

int QUEUEsize(Q q)
{
    return q->size;
}
