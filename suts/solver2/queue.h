#ifndef _QUEUE_
#define _QUEUE_

#include <stdlib.h>

typedef struct queue *Q;
typedef struct QUEUEnode* QUEUElink;
struct QUEUEnode
{
	void* item;
	QUEUElink next;
};
struct queue { QUEUElink head; QUEUElink tail; int size; };

Q    QUEUEinit();
int QUEUEempty(Q);
void QUEUEinsert(Q, void*);
void* QUEUEdequeue(Q);
void QUEUEclear(Q);
int QUEUEsize(Q);

#endif
