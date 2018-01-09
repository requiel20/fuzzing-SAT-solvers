#ifndef WATCHED_CLAUSE_H
#define WATCHED_CLAUSE_H

#define NWATCHED_LITERALS 2

#include <stdbool.h>

#include "literal.h"
#include "clause.h"

//#define DEBUG_INIT_W_CLAUSE

typedef struct watched_clause {
    Clause *clause;
    int literals[NWATCHED_LITERALS];
} WatchedClause;


WatchedClause
init_watched_clause(Clause *clause);

char *
wtoa(WatchedClause this);

void
print_debug_w_clause(WatchedClause this);

WatchedClause
copy_watched_clause(WatchedClause a);

/*
 * @this's watched literals
 */
Literal *
get_literals(WatchedClause this);

/*
 * Decidable (0 <= @this.clause.size() <= 2)
 * 
 * if size == 1 return true
 * check if x_1 and 
 */
bool
is_decidable(WatchedClause this);


/*
 * Remove @literal from @this and update watched literals
 */
int
remove_watched(WatchedClause *this, Literal literal);

/*
 * There are three possible scenarios:
 *     - WatchedClause does not contain the given var. Then, return -1.
 *     - WatchedClause contains the negation of this var. Then, return 0.
 *     - WatchedClause contain the this var and it is not negated. Then return 1.
 */
bool
check_watched_clauses(WatchedClause this, Literal literal);

void
delete_watched_clause(WatchedClause *this);

#endif
