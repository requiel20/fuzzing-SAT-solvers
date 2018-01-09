#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#include "conflict.h"
#include "solver.h"
#include "formula.h"
#include "literal.h"
#include "clause.h"
#include "watched_clause.h"


#define MAX_QUEUE_SIZE 20000


// ####### Custom queue for the bcp #########
AssignmentQueue queue[MAX_QUEUE_SIZE];
int head = 0;
int tail = -1;
int itemCount = 0;


void
reset()
{
    head = 0;
    tail = -1;
    itemCount = 0;
}

bool is_q_empty() {
   return itemCount == 0;
}

bool 
is_full() {
   return itemCount == MAX_QUEUE_SIZE;
}

void 
push(AssignmentQueue data) {
   if(!is_full()) {
      if(tail == MAX_QUEUE_SIZE-1) {
         tail = -1;            
      }       
      queue[++tail] = data;
      itemCount++;
   }
}

AssignmentQueue 
pop() {
   AssignmentQueue data = queue[head++];
   if(head == MAX_QUEUE_SIZE) {
      head = 0;
   }
   itemCount--;
   return data;  
}

Literal
decide(Formula *formula)
{
    // FIXME its hack.
    Literal invalid = {0, 1};
    if (formula->nassigned == formula->nvars) {
        return invalid;
    }

    LitTable node;
    Literal literal;
    while (formula->unassigned_lits){
        node = extract_max(&(formula->unassigned_lits));

        literal = node->literal;
        free(node);

        if (formula->assignments[literal.var_id - 1].value == UNASSIGNED)
            return literal;
    }

    return invalid;
}

/*
 * Pseudo code :
 *   - find a unit clause
 *     - naive : go through all the clauses
 *     - smart : only inspect the clauses when one of the watched 
 *               literral has only one unassigned literal
 *   - assign the value true
 *   - simplify all the other clauses
 *   - repeat until there is no more unit clause
 *   - 
 *
 * Return a struct containing : 
 * - the conflict clause
 * - an array of literal that were assigned (LIFO : from last to first)
 * - the number of literal assigned
 */
BcpOutput *
bcp(Formula *formula, Literal first_literal, Clause *antecedent) {
#if defined(DEBUG) || defined(DEBUG_BCP)
    printf("##### Beginning of BCP loop #####\nFormula: %s\n", ftoa(*formula));
#endif

    // Initialize the output object
    BcpOutput *output = malloc(sizeof(BcpOutput));
    int n_assigned_literals = 0;
    Literal assigned_literals[formula->nvars];

    //Initialize the queue of literals to assign
    AssignmentQueue first_assignment = {first_literal, antecedent};
    push(first_assignment);
    bool first_iteration = true;

    // Repeat until the queue of assignments to do is empty
    while (!is_q_empty()) {
        AssignmentQueue current_assignment = pop();
        Literal literal = current_assignment.literal;

        Assignment curr_assign = formula->assignments[literal.var_id - 1];

        if (curr_assign.value != UNASSIGNED){
            continue;
        }

        if (first_iteration){
#if defined(DEBUG) || defined(DEBUG_BCP)
            printf("  Assigning: (%s) true (Decision)\n", ltoa(first_literal));
#endif

            assign(formula, literal, antecedent);
            first_iteration = false;
        } else{
#if defined(DEBUG) || defined(DEBUG_BCP)
            printf("  Assigning: (%s) true (Consequence)\n", ltoa(literal));
#endif
            assign(formula, literal, current_assignment.antecedent);
        }

        // Add the literal to the list of assigned_literal for output
        assigned_literals[n_assigned_literals] = literal;
        n_assigned_literals++;

        /* Inspect them and re-assign the watched_literals and either:
         *   - return a conflict (-1)
         *   - add a new assignment to the queue
         *   - do nothing
         */
        WCList lits_clauses = formula->w_clauses[literal.var_id - 1];
        while (lits_clauses) {
            WatchedClause *w_clause = lits_clauses->clause;

            /* Get the clauses that matter (meaning: the ones that
             * have a watched literal that is being assigned false)
             */
            if (!check_watched_clauses(*w_clause, literal)) {
                lits_clauses = (WCList) lits_clauses->next;
                continue;
            }
 
            // Can either be 0 or 1, cf init_watched_clause
            int new_index = 0;
            Literal w_lit1 = get(*w_clause->clause, w_clause->literals[0]);
            Literal w_lit2 = get(*w_clause->clause, w_clause->literals[1]);

            if (!same_id(w_lit1, literal)) {
                new_index = 1;
            }
            
            bool found_new_w_literal = false;
            bool clause_is_true = false;
            int number_of_false_literals = 0;

            ChainedLiteral *c_literals = w_clause->clause->literals;
            int j = 0;
            while (c_literals) {
                Literal lit = c_literals->literal;
                Assignment lit_assign = formula->assignments[lit.var_id - 1];

                AssignedValue exp_value = lit.negated ?
                    ASSIGNED_FALSE : ASSIGNED_TRUE;
                AssignedValue op_value = lit.negated ?
                    ASSIGNED_TRUE : ASSIGNED_FALSE;

                // Inspect
                if (lit_assign.value == exp_value) {
                    // The clause evaluates to true, 
                    // so we shouldn't make a decision based on it
                    clause_is_true = true;

#if defined(DEBUG) || defined(DEBUG_BCP)
                        printf("      Clause evaluates to true (%s)\n",
                                ltoa(lit));
#endif
                    break;
                }

                if (lit_assign.value == UNASSIGNED && !equals(lit, w_lit1)
                        && !equals(lit, w_lit2)) {
                    found_new_w_literal = true;
                    w_clause->literals[new_index] = j;
                    add_w_clause(formula, lit, w_clause);

                    #if defined(DEBUG) || defined(DEBUG_BCP)
                        printf("    Updating w_literal: ( %s )\n",
                                wtoa(*w_clause));
                    #endif
                    
                } else if (lit_assign.value == op_value) {
                    number_of_false_literals++;
                }
                j++;
                c_literals = c_literals->next;

            }

            if(clause_is_true){
                lits_clauses = (WCList) lits_clauses->next;
                continue;
            }

            if (number_of_false_literals == w_clause->clause->nliterals){
#if defined(DEBUG) || defined(DEBUG_BCP)
                printf("      /!\\ Conflict found during BCP for clause: (%s)\n\
##### End of BCP loop ####\n\n", ctoa(*(w_clause->clause)));
#endif
                reset();

                // Reverse array of assigned literal for the output
                Literal *lits = malloc(n_assigned_literals * sizeof(Literal));
                Literal * assigned_lit_reverse[n_assigned_literals];
                for (int i = n_assigned_literals - 1; i >= 0; i--) {
                    lits[i] = assigned_literals[n_assigned_literals - i - 1];
                }
                output->nliterals = n_assigned_literals;
                output->clause = w_clause->clause;
                output->assigned_literals = lits;

                return output;
            }

            // This means that the clause is now a literal
            if (!found_new_w_literal) {
                Literal new_literal = get(*w_clause->clause, w_clause->literals[1 - new_index]);
#if defined(DEBUG) || defined(DEBUG_BCP)
                printf("      Pushing new literal in assignment queue: %s\n", ltoa(new_literal));
#endif

                /* 
                 * The remaining watched literal has to be assigned true 
                 */
                AssignmentQueue new_assignment = {new_literal, w_clause->clause};
                push(new_assignment);
            }
            lits_clauses = (WCList) lits_clauses->next;
        }
    }
#if defined(DEBUG) || defined(DEBUG_BCP)
    printf("##### End of BCP loop #####\n\n");
#endif
    reset();

    Literal *lits = malloc(n_assigned_literals * sizeof(Literal));
    Literal * assigned_lit_reverse[n_assigned_literals];
    for (int i = n_assigned_literals - 1; i >= 0; i--) {
        lits[i] = assigned_literals[n_assigned_literals - i - 1];
    }
    output->nliterals = n_assigned_literals;
    output->clause = NULL;
    output->assigned_literals = lits;
    return output;
}


/*
 * Look for literals that are only used positively or negatively
 */
void
plp(Formula *formula) {

#if defined(DEBUG) || defined(DEBUG_PLL)
    printf("\n##### Starting of PLP #####\nFormula: %s\n", ftoa(*formula));
#endif       
    
    /*
     * Map of literals
     * 0 means not seen yet
     * 1 means seen positively
     * -1 means seen negatively
     * 2 means seen positively and negatively
     */
    int literals[formula->nvars];

    for (int i = 0; i < formula->nvars; i++) {
        literals[i] = 0;
    }

    for (int clause_index = 0; clause_index < formula->nclauses; clause_index++) {
        Clause clause = *formula->watched_clauses[clause_index].clause;
        ChainedLiteral* clit = clause.literals;
        for (int lit_index = 0; lit_index < clause.nliterals; lit_index++) {
            Literal lit = clit->literal;   

            switch (literals[lit.var_id - 1]) {
                case 0:
                    literals[lit.var_id - 1] = lit.negated ? -1 : 1;
                    break;
                case 1:
                    literals[lit.var_id - 1] = lit.negated ? 2 : 1;
                    break;
                case -1:
                    literals[lit.var_id - 1] = lit.negated ? -1 : 2;
                    break;
                case 2:
                    break;
            }
            clit = clit->next;
        }    
    }
    for (int i = 0; i < formula->nvars; i++) {  
        if (literals[i] == 1 || literals[i] == -1) {
            Literal literal = {i + 1, literals[i] == 1 ? false : true};
            assign(formula, literal, NULL);

#if defined(DEBUG) || defined(DEBUG_PLL)
            printf("   Assigning: %s\n", ltoa(literal));
#endif            
        }
    } 

#if defined(DEBUG) || defined(DEBUG_PLL)
    printf("##### End of PLP #####\n\n");
#endif    

}

void
delete_bcp_output(BcpOutput *bcp_out)
{
    if (!bcp_out)
        return;
    free(bcp_out->assigned_literals);
    free(bcp_out);
}

SatModel
return_sat(Formula *formula)
{
    SatModel sat_model = {SAT, formula};
    return sat_model;
}

SatModel
return_unsat(Formula *formula)
{
    SatModel sat_model = {UNSAT, formula};
    return sat_model;
}

SatModel
solve(Formula *formula)
{
    plp(formula);
    for (;;) {

        bool conflict_at_top = false;
        Literal literal = decide(formula);

        // Literal assignment does not have an antecedent clause when comes
        // from decide().
        Clause *antecedent = NULL;

        if (!literal.var_id)
            return return_sat(formula);

        for (;;) {
            BcpOutput *bcp_out = bcp(formula, literal, antecedent);
            if (!bcp_out->clause) {
                formula->decision_level++;
                delete_bcp_output(bcp_out);
                break;
            }

            if (conflict_at_top) {
                return return_unsat(formula);
            }

            BTInfo bt_info = analyze_conflict(formula, bcp_out);
            delete_bcp_output(bcp_out);

            literal = bt_info.literal;
            antecedent = bt_info.clause;

            if (formula->decision_level == INITIAL_BT_LEVEL) {
                conflict_at_top = true;
                plp(formula);
            }

            if (!literal.var_id) {
                delete_bcp_output(bcp_out);
                return return_unsat(formula);
            }
        }
    }
    return return_sat(formula);
}


void
report(SatModel sat_model)
{
    printf("%s\n", sat_model.is_sat == SAT ? "SAT" : "UNSAT");
    if (sat_model.is_sat == UNSAT) {
        return;
    }
    Formula *formula = sat_model.formula;
    for (int i = 0; i < formula->nvars; i++) {
        Assignment assignment = formula->assignments[i];
        int value = assignment.value == ASSIGNED_TRUE ? 0 : 1;
        Literal lit = {i + 1, value};
        printf("%s ", ltoa(lit));
    }
    printf("\n");
}


void
sat(char *path)
{
    Formula *formula = init_formula_from_file(path);
    if (!formula) {
        fprintf(stderr, "Error while constructing formula. `NULL` received");
        exit(EXIT_FAILURE);
    }

    SatModel model = solve(formula);
    report(model);
    delete_formula(formula);
}
