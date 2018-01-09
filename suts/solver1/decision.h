#ifndef DECISION_H
#define DECISION_H

#include "literal.h"

#define MAX_TREE_HEIGHT 10000
#define MAX(x, y) (((x) > (y)) ? (x) : (y))

/**
 * This data structure is used to store all unassigned literals.
 *
 * It is implemented by a binary search tree, and all literals are stored
 * based on their score.
 *
 * The score of each literal is computed based on VSIDS heuristic.
 */
typedef struct pq {
    double score;
    Literal literal;
    int height;
    struct pq *left;
    struct pq *right;
} PriorityQueue;

typedef PriorityQueue * LitTable;


LitTable
init_node(Literal, double score);

LitTable
insert_literal(LitTable table, Literal literal, double score);

LitTable
delete_literal(LitTable table, Literal literal, double score);

LitTable
find_min(LitTable table);

LitTable
extract_max(LitTable *table);

int
get_height(LitTable table);

void
rotate_right(LitTable *node);

void
rotate_left(LitTable *node);

#endif
