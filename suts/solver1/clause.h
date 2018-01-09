#ifndef CLAUSE_H
#define CLAUSE_H

#include <stdbool.h>
#include <stdint.h>
#include "literal.h"

//#define DEBUG_GET

typedef struct chained_literal {
    Literal literal;
    struct chained_literal* next;
} ChainedLiteral;

typedef struct clause {
    ChainedLiteral *literals;
    int nliterals; // MUST BE updated when modifying @literals
} Clause;


ChainedLiteral *
init_chained_literal_from_int(int literal);

ChainedLiteral *
init_chained_literal(Literal literal);

ChainedLiteral *
init_chained_literal_next_from_int(int literal, Literal* next);

ChainedLiteral *
init_chained_literal_next(Literal literal, Literal* next);

Clause *
init_clause_empty();

Clause *
init_clause_from_int(int* literals, int nliterals);

Clause *
init_clause_from_literals(Literal * literals);

char *
ctoa(Clause this);

/*
 * Return a copy of a
 */
Clause *
copy_clause(Clause a);

/*
 * Perfom propositional resolution to the Clause @a and @b based on the
 * given @resolvent (i.e. variable id) and give a new clause.
 *
 * If propositional resolution is not applicable then just simply get
 * the union of literals.
 */
Clause *
merge(Clause a, Clause b, uint32_t resolvent);

/*
 * List of literals common to @a and @b
 */
Clause *
diff(Clause a, Clause b);

/*
 * Remove @literal from @this
 */
int
remove_literal(Clause *this, Literal literal);

/*
 * Add @literal to the end of @this without checking if it is already in the clause
 */
void
add(Clause* this, Literal literal);

/*
 * Add @literal to the end of @this without checking if it is already in the clause
 */
void
add_int(Clause* this, int literal);

/*
 * Get i-th literal of @this
 */
Literal
get(Clause this, int i);

/*
 * There are three possible scenarios:
 *     - Clause does not contain the given var. Then, return -1.
 *     - Clause contains the negation of this var. Then, return 0.
 *     - Clause contain the this var and it is not negated. Then return 1.
 */
int
check_var(Clause this, uint32_t var_id);

/*
 * True if clause is empty
 */
bool
is_empty(Clause this);

/*
 * True if clause is a literal
 */
bool
is_literal(Clause this);

void
delete_clause(Clause *this);

#endif
