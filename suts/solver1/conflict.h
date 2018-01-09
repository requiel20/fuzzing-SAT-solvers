#ifndef CONFLICT_H
#define CONFLICT_H

#include "clause.h"
#include "formula.h"

#define INITIAL_BT_LEVEL 1

//#define DEBUG_ANALYSE_CONFLICT 
// Quick dirty fix to get the DEBUG define
#include "solver.h"

typedef struct backtracking_info {
    // Backtracking level.
    int bt_level;
    // Literal assigned to TRUE, after backtracking.
    Literal literal;
    Clause *clause;
} BTInfo;


/**
 * Algorithm to find the learned confict clause given the initial confict.
 *
 * The initial conflict is denoted by the parameter @clause and it is
 * genetated by BCP. Given this starting point, algorithm tries to find the
 * learned clause using first - UIP scheme. The steps are the following:
 *     1) Find an implied literal assigned to the current decision level.
 *     2) Perform propositional resolution between the current clause and
 *        the antecedent (i.e. the clause which resulted in this specific
 *        assignment of the literal) of the literal we found at step 1).
 *     3) The clause given by the propositional resolution is the new
 *        current clause.
 *     4) Repeat step 1) until we find a clause which contains only literals
 *        assigned at previous decision levels or contains only one
 *        literal assigned at the current decision level.
 *
 * For more information:
 * http://satassociation.org/articles/FAIA185-0131.pdf
 */
Clause *
learn_clause(Formula *formula, BcpOutput *out);

BTInfo
get_backtracking_level(Formula *formula, Clause *learnt_clause);

void
backtrack(Formula *formula, int bt_level);

BTInfo
analyze_conflict(Formula *formula, BcpOutput *bcp_output);


#endif
