#include "vector.h"
#include "var.h"

//TODO

#ifndef SAT_CLAUSE_H
#define SAT_CLAUSE_H

typedef struct clause *C;
typedef struct clause {
    bool learnt;
    float activity;
    V literals;
} clause;

bool CLAUSEclean(V literals);

bool CLAUSEnew(V literals, bool learnt, C *output);

void CLAUSEremoveDuplicates(V literals);

bool CLAUSEpropagate(C clause, Var p);

V CLAUSEreasonFor(C clause, Var p);

#endif //SAT_CLAUSE_H
