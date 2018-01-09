#include <stdio.h>
#include <stdlib.h>
#include "solver.h"


#include "formula.h"
#include "clause.h"
#include "conflict.h"
#include "literal.h"
#include "solver.h"


//void
//problem1()
//{
//    Literal *lit1 = init_literal(-1);
//    Literal *lit2 = init_literal(2);
//
//    Clause *clause1 = init_clause_empty();
//    add(clause1, *lit1);
//    add(clause1, *lit2);
//
//    Literal *lit3 = init_literal(-2);
//    Literal *lit4 = init_literal(3);
//
//    Clause *clause2 = init_clause_empty();
//    add(clause2, *lit3);
//    add(clause2, *lit4);
//
//
//    Literal *lit5 = init_literal(4);
//
//    Clause *clause3 = init_clause_empty();
//    add(clause3, *lit3);
//    add(clause3, *lit5);
//
//
//    Literal *lit6 = init_literal(-4);
//    Literal *lit7 = init_literal(5);
//
//    Clause *clause4 = init_clause_empty();
//    add(clause4, *lit6);
//    add(clause4, *lit7);
//
//
//    Literal *lit8 = init_literal(6);
//
//    Clause *clause5 = init_clause_empty();
//    add(clause5, *lit6);
//    add(clause5, *lit8);
//
//
//    Literal *lit9 = init_literal(-5);
//    Literal *lit10 = init_literal(7);
//
//    Clause *clause6 = init_clause_empty();
//    add(clause6, *lit9);
//    add(clause6, *lit10);
//
//
//    Literal *lit11 = init_literal(-6);
//    Literal *lit12 = init_literal(-7);
//
//    Clause *clause7 = init_clause_empty();
//    add(clause7, *lit11);
//    add(clause7, *lit12);
//
//    Clause *clauses = malloc(7 * sizeof(Clause));
//    clauses[0] = *clause1;
//    clauses[1] = *clause2;
//    clauses[2] = *clause3;
//    clauses[3] = *clause4;
//    clauses[4] = *clause5;
//    clauses[5] = *clause6;
//    clauses[6] = *clause7;
//
//    Formula *formula = init_formula_from_clauses(clauses, 7, 8);
//    Assignment *assignments = malloc(8 * sizeof(Assignment));
//    Assignment ass = {0, NULL, 1};
//    assignments[1] = ass; 
//
//    Assignment ass1 = {0, clause1, 1};
//    assignments[2] = ass1; 
//
//
//    Assignment ass2 = {0, clause2, 1};
//    assignments[3] = ass2; 
//
//    
//    Assignment ass3 = {0, clause3, 1};
//    assignments[4] = ass3; 
//
//    Assignment ass4 = {0, clause4, 1};
//    assignments[5] = ass4; 
//
//    Assignment ass5 = {0, clause5, 1};
//    assignments[6] = ass5; 
//    
//
//    Assignment ass6 = {0, clause6, 1};
//    assignments[7] = ass6; 
//
//    Clause *con = init_clause_empty();
//    add(con, *lit11);
//    add(con, *lit12);
//    formula->nvars = 7;
//    formula->assignments = assignments;
//    formula->decision_level = 0;
//
//    Clause *learn = learn_clause(formula, con, 7);
//    if (!learn) {
//        printf("No learn clause\n");
//        exit(EXIT_SUCCESS);
//    }
//
//    ChainedLiteral *lits = learn->literals;
//    while(lits) {
//        printf("Literal: %d negated: %d\n", lits->literal.var_id,
//               lits->literal.negated);
//        lits = lits->next;
//    }
//    BTInfo bt_info = get_backtracking_level(formula, learn);
//    printf("Backtracking level: %d\n", bt_info.bt_level);
//    printf("Backtracking literal: (%d-%d)\n", bt_info.literal.var_id,
//           bt_info.literal.negated);
//    delete_clause(clause1);
//    delete_clause(clause2);
//    delete_clause(clause3);
//    delete_clause(clause4);
//    delete_clause(clause5);
//    delete_clause(clause6);
//    delete_clause(clause7);
//    delete_clause(learn);
//    free(lit1);
//    free(lit2);
//    free(lit3);
//    free(lit4);
//    free(lit5);
//    free(lit6);
//    free(lit7);
//    free(lit8);
//    free(lit9);
//    free(lit10);
//    free(lit11);
//    free(lit12);
//    free(clauses);
//    free(formula);
//    free(assignments);
//}
//
//
//void
//problem2()
//{
//    // C1 x31 x1 x2
//    Literal *lit1 = init_literal(1);
//    Literal *lit2 = init_literal(2);
//    Literal *lit3 = init_literal(-3);
//
//    Clause *clause1 = init_clause_empty();
//    add(clause1, *lit1);
//    add(clause1, *lit2);
//    add(clause1, *lit3);
//
//    // C2 x1 x3
//    Literal *lit4 = init_literal(-4);
//
//    Clause *clause2 = init_clause_empty();
//    add(clause2, *lit2);
//    add(clause2, *lit4);
//
//    // C3 X2 x3 x4
//    Literal *lit5 = init_literal(3);
//    Literal *lit6 = init_literal(4);
//    Literal *lit7 = init_literal(5);
//
//    Clause *clause3 = init_clause_empty();
//    add(clause3, *lit6);
//    add(clause3, *lit5);
//    add(clause3, *lit7);
//
//    // C4 x5 x4
//    Literal *lit8 = init_literal(-5);
//    Literal *lit9 = init_literal(-6);
//
//    Clause *clause4 = init_clause_empty();
//    add(clause4, *lit8);
//    add(clause4, *lit9);
//
//    // C5 x4 x6 x21
//    Literal *lit10 = init_literal(-7);
//    Literal *lit11 = init_literal(8);
//
//    Clause *clause5 = init_clause_empty();
//    add(clause5, *lit10);
//    add(clause5, *lit11);
//    add(clause5, *lit8);
//
//    Clause *clauses = malloc(5 * sizeof(Clause));
//    clauses[0] = *clause1;
//    clauses[1] = *clause2;
//    clauses[2] = *clause3;
//    clauses[3] = *clause4;
//    clauses[4] = *clause5;
//
//    Formula *formula = init_formula_from_clauses(clauses, 5, 8);
//    Assignment *assignments = malloc(8 * sizeof(Assignment));
//    Assignment ass = {3, NULL, 0, 0.0, 0.0};
//    assignments[0] = ass; 
//
//    Assignment ass1 = {5, NULL, 0, 0.0, 0.0};
//    assignments[1] = ass1; 
//
//
//    Assignment ass2 = {5, clause1, 0, 0.0, 0.0};
//    assignments[2] = ass2; 
//    
//    Assignment ass3 = {5, clause2, 0, 0.0, 0.0};
//    assignments[3] = ass3; 
//
//    Assignment ass4 = {5, clause3, 1, 0.0, 0.0};
//    assignments[4] = ass4; 
//
//    Assignment ass5 = {5, clause4, 0, 0.0, 0.0};
//    assignments[5] = ass5; 
//    
//
//    Assignment ass6 = {5, clause5, 0, 0.0, 0.0};
//    assignments[6] = ass6; 
//
//    Assignment ass7 = {2, NULL, 0, 0.0, 0.0};
//    assignments[7] = ass7; 
//
//    Clause *con = init_clause_empty();
//    Literal *lit12 = init_literal(6);
//    Literal *lit13 = init_literal(7);
//    add(con, *lit13);
//    add(con, *lit12);
//    formula->nvars = 8;
//    formula->assignments = assignments;
//
//    ATracker *assigns = malloc(5 * sizeof(ATracker));
//
//    ATracker assign6 = malloc(sizeof(AssignmentTracker));
//    assign6->var_id = 7;
//    assign6->next= NULL;
//
//    ATracker assign5 = malloc(sizeof(AssignmentTracker));
//    assign5->var_id = 6;
//    assign5->next = assign6;
//
//    ATracker assign4 = malloc(sizeof(AssignmentTracker));
//    assign4->var_id = 5;
//    assign4->next = assign5;
//
//    ATracker assign3 = malloc(sizeof(AssignmentTracker));
//    assign3->var_id = 4;
//    assign3->next = assign4;
//
//    ATracker assign2 = malloc(sizeof(AssignmentTracker));
//    assign2->var_id = 3;
//    assign2->next = assign3;
//
//    ATracker assign1 = malloc(sizeof(AssignmentTracker));
//    assign1->var_id = 2;
//    assign1->next = assign2;
//    assigns[4] = assign1;
//
//    ATracker assign32 = malloc(sizeof(AssignmentTracker));
//    assign32->var_id = 0;
//    assign32->next = NULL;
//    assigns[3] = assign32;
//
//    ATracker assign31 = malloc(sizeof(AssignmentTracker));
//    assign31->var_id = 1;
//    assign31->next = NULL;
//    assigns[2] = assign31;
//
//    ATracker assign21 = malloc(sizeof(AssignmentTracker));
//    assign21->var_id = 8;
//    assign21->next = NULL;
//    assigns[1] = assign21;
//    formula->assignment_trackers = assigns;
//    formula->decision_level = 5;
//
//    Literal literal = analyze_conflict(formula, con, 6);
//    for (int i = 0; i < formula->nvars; i++) {
//        printf("Assignment for variable %d\n", i + 1);
//        printf("Decision level: %d\n", formula->assignments[i].decision_level);
//        printf("Value: %d\n", formula->assignments[i].value);
//        if (formula->assignments[i].antecedent) {
//            printf("Has an antecedent\n\n");
//        }
//    }
//    printf("Backtracking level: %d\n", formula->decision_level);
//    printf("Backtracking literal: (%d-%d)\n", literal.var_id, literal.negated);
//    printf("Learnt clause db size: %d\n", formula->l_clauses->size);
//    printf("Learnt clause next item: %d\n", formula->l_clauses->n_add);
//    printf("Value: %d\n", formula->assignment_trackers[3]->var_id);
//    //for (int i = 1 i <= 4; i++) {
//    //    ATracker assign_t = formula->assigns_per_level[i];
//
//    //    if (assign_t.head == -1)
//    //        continue;
//
//    //    printf("Assignment")
//    //}
//    //if (!learn) {
//    //    printf("No learn clause\n");
//    //    exit(EXIT_SUCCESS);
//    //}
//
//    //ChainedLiteral *lits = learn->literals;
//    //while(lits) {
//    //    printf("Literal: %d negated: %d\n", lits->literal.var_id,
//    //           lits->literal.negated);
//    //    lits = lits->next;
//    //}
//    //int level = get_backtracking_level(formula, learn, 5);
//    //printf("Backtracking level: %d\n", level);
//    free(assignments);
//    free(clauses);
//
//}
//
//
//void
//problem3()
//{
//    Literal *lit1 = init_literal(1);
//    Literal *lit2 = init_literal(2);
//    Literal *lit3 = init_literal(-3);
//
//    Clause *clause1 = init_clause_empty();
//    add(clause1, *lit1);
//    add(clause1, *lit2);
//    add(clause1, *lit3);
//
//    // C2 x1 x3
//    Literal *lit4 = init_literal(-4);
//
//    Clause *clause2 = init_clause_empty();
//    add(clause2, *lit2);
//    add(clause2, *lit4);
//
//    // C3 X2 x3 x4
//    Literal *lit5 = init_literal(3);
//    Literal *lit6 = init_literal(4);
//    Literal *lit7 = init_literal(5);
//
//    Clause *clause3 = init_clause_empty();
//    add(clause3, *lit6);
//    add(clause3, *lit5);
//    add(clause3, *lit7);
//
//    // C4 x5 x4
//    Literal *lit8 = init_literal(-5);
//    Literal *lit9 = init_literal(-6);
//
//    Clause *clause4 = init_clause_empty();
//    add(clause4, *lit8);
//    add(clause4, *lit9);
//
//    // C5 x4 x6 x21
//    Literal *lit10 = init_literal(-7);
//    Literal *lit11 = init_literal(8);
//
//    Clause *clause5 = init_clause_empty();
//    add(clause5, *lit10);
//    add(clause5, *lit11);
//    add(clause5, *lit8);
//
//    Clause *clauses = malloc(5 * sizeof(Clause));
//    clauses[0] = *clause1;
//    clauses[1] = *clause2;
//    clauses[2] = *clause3;
//    clauses[3] = *clause4;
//    clauses[4] = *clause5;
//
//    Formula *formula = init_formula_from_clauses(clauses, 5, 8);
//    formula->assignments = init_assignments(8);
//    formula->assignment_trackers = init_assignment_tracker();
//
//    formula->decision_level = 3;
//    Literal l1 = {1, 1};
//    assign(formula, l1, NULL);
//
//
//    formula->decision_level = 5;
//
//    Literal l2 = {2, 1};
//    assign(formula, l2, NULL);
//
//
//    Literal l3 = {3, 1};
//    assign(formula, l3, clause1);
//
//    Literal l4 = {4, 0};
//    assign(formula, l4, clause2);
//    
//
//    Literal l5 = {5, 1};
//    assign(formula, l5, clause3);
//
//    Literal l6 = {6, 1};
//    assign(formula, l6, clause4);
//
//    Literal l7 = {7, 1};
//    assign(formula, l7, clause5);
//    
//    formula->decision_level = 2;
//
//    Literal l8 = {8, 1};
//    assign(formula, l8, NULL);
//    formula->decision_level = 5;
//
//
//    Clause *con = init_clause_empty();
//    Literal *lit12 = init_literal(6);
//    Literal *lit13 = init_literal(7);
//    add(con, *lit13);
//    add(con, *lit12);
//
//    Literal literal = analyze_conflict(formula, con, 6);
//    for (int i = 0; i < formula->nvars; i++) {
//        printf("Assignment for variable %d\n", i + 1);
//        printf("Decision level: %d\n", formula->assignments[i].decision_level);
//        printf("Value: %d\n", formula->assignments[i].value);
//        if (formula->assignments[i].antecedent) {
//            printf("Has an antecedent\n\n");
//        }
//    }
//    printf("Backtracking level: %d\n", formula->decision_level);
//    printf("Backtracking literal: (%d-%d)\n", literal.var_id, literal.negated);
//    printf("Learnt clause db size: %d\n", formula->l_clauses->size);
//    printf("Learnt clause next item: %d\n", formula->l_clauses->n_add);
//    printf("Value: %d\n", formula->assignment_trackers[3]->var_id);
//    //for (int i = 1 i <= 4; i++) {
//    //    ATracker assign_t = formula->assigns_per_level[i];
//
//    //    if (assign_t.head == -1)
//    //        continue;
//
//    //    printf("Assignment")
//    //}
//    //if (!learn) {
//    //    printf("No learn clause\n");
//    //    exit(EXIT_SUCCESS);
//    //}
//
//    //ChainedLiteral *lits = learn->literals;
//    //while(lits) {
//    //    printf("Literal: %d negated: %d\n", lits->literal.var_id,
//    //           lits->literal.negated);
//    //    lits = lits->next;
//    //}
//    //int level = get_backtracking_level(formula, learn, 5);
//    //printf("Backtracking level: %d\n", level);
//    free(clauses);
//}

void
proble4(char *path) {sat(path);}


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("sat accepts only 1 argument which is the filename of the formula.\n");
        exit(EXIT_FAILURE);
    }

    //problem3();
    proble4(argv[1]);

    exit(EXIT_SUCCESS);
}
