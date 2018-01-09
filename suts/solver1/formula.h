#ifndef FORMULA_H
#define FORMULA_H

#include <stddef.h>
#include <stdbool.h>
#include "clause.h"
#include "decision.h"
#include "watched_clause.h"

#define NLEARNT_CLAUSES 100
#define MAX_DECISION_LEVEL 1000000


typedef enum assigned_value {
	UNASSIGNED = -1,
	ASSIGNED_TRUE = 1,
	ASSIGNED_FALSE = 0
} AssignedValue;

typedef struct assignment_queue {
    Literal literal;
    Clause* antecedent;
} AssignmentQueue;

typedef struct wc_item {
    WatchedClause *clause;
    struct wc_item *next;
} WCItem;

typedef WCItem * WCList;

typedef struct assignment {
    int decision_level;
    Clause *antecedent;
    AssignedValue value;
    double neg_score;
    double score;
} Assignment;

typedef struct learnt_clauses {
    WatchedClause *clauses;
    int n_add;
    int size;
} LearntClauses;

typedef struct assignment_tracker {
    uint32_t var_id;
    struct assignment_tracker *next;
} AssignmentTracker;

typedef AssignmentTracker* ATracker;

typedef struct formula {
    LearntClauses *l_clauses;
    WatchedClause *watched_clauses;
    Assignment *assignments;
    int nclauses;
    int nvars;
    int decision_level;
    int nassigned;
    WCList *w_clauses;
    LitTable unassigned_lits;
    ATracker *assignment_trackers;
} Formula;

LitTable
init_unassigned_table(int nvars, Assignment *assignments);

LearntClauses *
init_learnt_clauses();

Formula *
init_formula_empty();

Formula *
init_formula_from_strings(char* cnf, int nclauses, int nvars);

Formula *
init_formula_from_file(char* path);

Formula *
init_formula_from_clauses(Clause* cnf, int nclauses, int nvars);

Assignment *
init_assignments(int nvars);

ATracker *
init_assignment_tracker();

char *
ftoa(Formula this);

/*
 * True if formula is satisfied
 * TODO: optimisaiton idea -> when formula satisfied, possible to replace
 * it with an empty array of clauses
 */
bool
is_sat();

/*
 * All clauses from @this containing @literal
 */
Formula *
get_all_clauses(Formula this, Literal literal);

/*
 * Assign @literal to true in @this
 */
int
assign(Formula* this, Literal literal, Clause *antecedent);

/*
 * Cancel @var_id assignment at @this only if it has been assigned at the
 * current decision level.
 */
int
cancel(Formula* this, uint32_t var_id);

void
add_learnt_clause(Formula *this, Clause *learnt_clause);

void
update_scores(Formula *this, Clause *learnt_clasue);

void
delete_learnt_clauses(LearntClauses *l_clauses);

void
delete_formula(Formula *this);

void
add_w_clause(Formula *formula, Literal literal, WatchedClause *clause);

void
remove_w_clause(Formula *formula, Literal literal, WatchedClause *clause);

#endif
