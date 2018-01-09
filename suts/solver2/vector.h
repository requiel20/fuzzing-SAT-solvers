#ifndef _VECTOR_
#define _VECTOR_

#include <stdio.h>
#include <stdlib.h>

#define VECTOR_INIT_CAPACITY 100
#define VECTOR_CAPACITY_INCREASE_FACTOR 2
#define VECTOR_CAPACITY_DECREASE_FACTOR 2
#define VECTOR_CAPACITY_DECREASE_THRESHOLD 4

typedef struct vector *V;
typedef struct vector {
    void **items;
    unsigned int capacity;
    unsigned int total;
} vector;

V VECTORinit();

unsigned int VECTORtotal(V);

void VECTORresize(V, unsigned int);

void VECTORadd(V, void *);

void VECTORset(V, unsigned int, void *);

void *VECTORget(V, unsigned int);

void VECTORdelete(V, unsigned int);

void VECTORpop(V);

void VECTORshrink(V, unsigned int n);

void VECTORfree(V);

V VECTORshallowCopy(V);

void VECTORswitchPlace(V v, unsigned int a, unsigned int b);

#endif
