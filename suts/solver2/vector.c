#include "vector.h"

V VECTORinit() {
    V v = (V) malloc(sizeof(struct vector));
    v->capacity = VECTOR_INIT_CAPACITY;
    v->total = 0;
    v->items = malloc(sizeof(void *) * v->capacity);
    return v;
}

unsigned int VECTORtotal(V v) {
    return v->total;
}

void VECTORresize(V v, unsigned int capacity) {
    void **items = realloc(v->items, sizeof(void *) * capacity);
    if (items) {
        v->items = items;
        v->capacity = capacity;
    }
}

void VECTORadd(V v, void *item) {
    if (v->capacity == v->total)
        VECTORresize(v, v->capacity * VECTOR_CAPACITY_INCREASE_FACTOR);
    v->items[v->total++] = item;
}

void VECTORset(V v, unsigned int index, void *item) {
    if (index < v->total)
        v->items[index] = item;
}

void *VECTORget(V v, unsigned int index) {
    if (index < v->total)
        return v->items[index];
    return NULL;
}

void VECTORdelete(V v, unsigned int index) {
    if (index >= v->total)
        return;

    v->items[index] = NULL;

    for (unsigned int i = index; i < v->total - 1; i++) {
        v->items[i] = v->items[i + 1];
        v->items[i + 1] = NULL;
    }

    v->total--;

    if (v->total > 0 && v->total == v->capacity / VECTOR_CAPACITY_DECREASE_THRESHOLD)
        VECTORresize(v, v->capacity / VECTOR_CAPACITY_DECREASE_THRESHOLD);
}

void VECTORpop(V v) {
    VECTORdelete(v, v->total - 1);
}

void VECTORshrink(V v, unsigned int n) {
    for (unsigned int i = 0; i < n; i++) {
        VECTORdelete(v, v->total - 1);
    }
}

void VECTORfree(V v) {
    free(v->items);
}


void VECTORswitchPlace(V v, unsigned int a, unsigned int b) {
    void *temp = VECTORget(v, a);
    VECTORset(v, b, temp);
    VECTORset(v, b, temp);
}

V VECTORshallowCopy(V v) {
    V copy = VECTORinit();

    for (unsigned int i = 0; i < VECTORtotal(v); ++i) {
        VECTORadd(copy, VECTORget(v, i));
    }

    return copy;
}
