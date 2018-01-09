#include <stdio.h>
#include "debugPrinter.h"

void printFormula(V formula) {
    if (IS_DEBUG) {
        printf("Printing %d clauses\n", formula->total);
        for (unsigned int i = 0; i < VECTORtotal(formula); i++) {
            C clause = VECTORget(formula, i);
            printClause(clause);
        }
    }
}

void printClause(C clause) {
    if (IS_DEBUG) {
        if (clause->learnt == true)
            printf("[L]  ");
        else if (clause->learnt == false)
            printf("[N] ");
        for (unsigned int j = 0; j < VECTORtotal(clause->literals); j++) {
            Var var = VECTORget(clause->literals, j);
            if (!var->sign)
                printf("-");
            printf("%d:", var->id);
            if (value(var) != unassigned)
                printf("%d, ", value(var));
            else
                printf("_");
            printf(" ");
        }
        printf("\n");
        printf("\n");
    }
}

void printDebugInt(char *message, int value) {
    if (IS_DEBUG)
        printf("%s%d\n", message, value);
}

void printDebugVar(char *message, Var var) {
    if (IS_DEBUG) {
        printf("%s", message);
        if (!var->sign)
            printf("-");
        printf("%d\n", var->id);
    }
}

void printDebugIntInt(char *message, int value1, int value2) {
    if (IS_DEBUG)
        printf("%s%d : %d\n", message, value1, value2);
}

void printDebugChar(char *message, int value) {
    if (IS_DEBUG)
        printf("%s%c\n", message, value);
}

void printDebug(char *message) {
    if (IS_DEBUG)
        printf("%s\n", message);

}
