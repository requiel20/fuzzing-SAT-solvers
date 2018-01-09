#include "conflict.h"

#include <limits.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "clause.h"
#include "formula.h"
#include "literal.h"


Literal *
get_implied_lit(Formula *formula, Clause *clause, Literal *literals, int nlits)
{
    // Number of implied literals in the current clause.
    int nimplied_lits = 0;
    ChainedLiteral *lits = clause->literals;

    int index = INT_MAX;

    while (lits) {
        Literal *literal = &(lits->literal);
        if (!literal->var_id || literal->var_id > (uint32_t) formula->nvars) {
            fprintf(stderr,  "Error: Literal '%s' without any assignment"
                    " information found.", ltoa(*literal));
            exit(EXIT_FAILURE);
        }
        Assignment assignment = formula->assignments[literal->var_id - 1];

        if (assignment.decision_level != formula->decision_level) {
            lits = lits->next;
            continue;
        }
        nimplied_lits++;
        // We pick the most recently assigned variable.
        for (int i = 0; i < nlits; i++) {
            if (same_id(lits->literal, literals[i]) && i < index) {
                index = i;
                break;
            }
        }

        lits = lits->next;
    }

    /*
     * If the number of literals included in the given clause is 1, then we
     * presume that we find the first UIP, so we do not need to continue
     * execution of conflict analyzer. We found our learnt clause (i.e.
     * the clause given as parameter).
     */
    return (nimplied_lits == 1 || !nimplied_lits || index == INT_MAX) ?
        NULL : &literals[index];
}


Clause *
learn_clause(Formula *formula, BcpOutput *out)
{
    if (!formula || !out || !out->clause) {
        fprintf(stderr, "Cannot proceed in `learn_clause()` function\n");
        exit(EXIT_FAILURE);
    }

    Assignment assignment;
    Clause *antecedent_clause;
    bool *visited = malloc(formula->nvars * sizeof(bool));
    memset(visited, 0, formula->nvars * sizeof(bool));
    Clause *clause = out->clause;

    for (int i = 0; i < out->nliterals; i++) {
        Clause *tmp_clause = clause;
        Literal *literal = get_implied_lit(formula, tmp_clause,
            out->assigned_literals, out->nliterals);
        if (!literal) {
            // Either the clause does not contained any implied literals to
            // the current decision level, or it contains first UIP.
            return clause;
        }
        if (visited[literal->var_id - 1]) {
            continue;
        }
        visited[literal->var_id - 1] = true;

        assignment = formula->assignments[literal->var_id - 1];
        antecedent_clause = assignment.antecedent;

        // We reached to a literal which is not implied, but it is assigned
        // explicitly at the current or previous decision level.
        if (!antecedent_clause) {
            return clause;
        }
        clause = merge(*antecedent_clause, *tmp_clause, literal->var_id);
        // If we cannot perform propositional resolution, then we have
        // already found our learnt clause.
        if (!clause) {
            return tmp_clause;
        }
    }
    free(visited);
    return clause;
}


BTInfo
get_backtracking_level(Formula *formula, Clause *learnt_clause)
{
    if (!learnt_clause) {
        fprintf(stderr, "Error: Backtracking cannot be computed."
                         " An 'NULL' learnt_clause is given.");
        exit(EXIT_FAILURE);
    }

    if (!formula) {
        fprintf(stderr, "Error: Backtracking cannot be computed."
                         " An 'NULL' formula is given.");
        exit(EXIT_FAILURE);
    }
    if (learnt_clause->nliterals == 1) {
        BTInfo bt_info = {
            INITIAL_BT_LEVEL, learnt_clause->literals->literal, learnt_clause};
        return bt_info;
    }

    ChainedLiteral *literals = learnt_clause->literals;
    int backtracking_level = 1;
    Literal backtrack_lit = {0, -1};
    while (literals) {
        Literal literal = literals->literal;
        if (!literal.var_id || literal.var_id > (uint32_t) formula->nvars) {
            fprintf(stderr,  "Error: Unknown literal '%s' found during "
                    "backtracking level computation", ltoa(literal));
            exit(EXIT_FAILURE);
        }
        Assignment assignment = formula->assignments[literal.var_id - 1];
        
        // We care for literals other than the implied ones on the current
        // backtracking level.
        if (assignment.decision_level != formula->decision_level &&
                assignment.decision_level > backtracking_level) {
            backtracking_level = assignment.decision_level;
        } else if (assignment.decision_level == formula->decision_level){
            backtrack_lit.var_id = literal.var_id;
            backtrack_lit.negated = literal.negated;
        }
        literals = literals->next;
    }
    BTInfo bt_info = {backtracking_level, backtrack_lit, learnt_clause};
    return bt_info;
}


void
backtrack(Formula *formula, int bt_level)
{
    if (bt_level <= 0 || bt_level > formula->decision_level) {
        fprintf(stderr, "Error: Cannot backtrack. Invalid backtracking level"
                " given (%d).", bt_level);
        exit(EXIT_FAILURE);
    }

    if (bt_level == INITIAL_BT_LEVEL)
        bt_level--;

    int dec_lvl = formula->decision_level;
    for (int i = dec_lvl - 1; i > bt_level - 1; i--) {
        ATracker assignments = (formula->assignment_trackers[i]);
        // Add new decision level for the formula.
        formula->decision_level = i + 1;
        while (assignments) {
            ATracker tmp = assignments;
            assignments = assignments->next;
            cancel(formula, tmp->var_id);
        }
    }
    if (formula->decision_level != 1)
        formula->decision_level = bt_level;
}


BTInfo
analyze_conflict(Formula *formula, BcpOutput *out)
{
    int dec_lvl = formula->decision_level;

    Clause *learnt_clause = learn_clause(formula, out);
    BTInfo bt_info = get_backtracking_level(formula, learnt_clause);

    if (bt_info.bt_level > dec_lvl) {
        fprintf(stderr, "Error: Computed backtracking level is greater"
                " than current decision level (%d > %d)",
                bt_info.bt_level, dec_lvl);
        exit(EXIT_FAILURE);
    }
    backtrack(formula, bt_info.bt_level);
    add_learnt_clause(formula, learnt_clause);

    return bt_info;
}
