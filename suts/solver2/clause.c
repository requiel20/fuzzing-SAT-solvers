#include "clause.h"
#include "sat.h"

bool CLAUSEnew(V literals, bool learnt, C *output) {
    if (!learnt) {
        if (CLAUSEclean(literals)) {
            printDebug("clause is already satisfied");
            return true;
        }

        CLAUSEremoveDuplicates(literals);
    }

    if (VECTORtotal(literals) == 0) {
        printDebug("clause is empty");
        return false;
    }

    if (VECTORtotal(literals) == 1) {
        printDebug("enqueueing unit clause");
        enqueue((Var) VECTORget(literals, 0), NULL);
    } else {
        printDebug("allocating clause");
        *output = malloc(sizeof(struct clause));
        (*output)->literals = literals;
        (*output)->learnt = learnt;
        (*output)->activity = 0;

        if (learnt) {
            int maxLevelLiteralIndex = 0;
            int maxLevel = 0;
            for (unsigned int i = 0; i < VECTORtotal(literals); i++) {
                if (level[((Var) VECTORget(literals, i))->id] > maxLevel) {
                    maxLevel = level[((Var) VECTORget(literals, i))->id];
                    maxLevelLiteralIndex = i;
                }
            }
            VECTORswitchPlace(literals, 1, maxLevelLiteralIndex);
        }

        //adding to watchers of literals[0, 1]
        addToWatchersOf(*output, neg((Var) VECTORget(literals, 0)));
        addToWatchersOf(*output, neg((Var) VECTORget(literals, 1)));
    }

    return true;
}

bool CLAUSEclean(V literals) {
    for (unsigned int i = 0; i < VECTORtotal(literals); i++) {
        Var currentVar = VECTORget(literals, i);
        if (value(currentVar) == true) {
            printDebugVar("in CLAUSEclean, true for var ", currentVar);
            printDebugInt("in CLAUSEclean, for opposite ", value(neg(currentVar)));
            printDebugInt("assignment of ", currentVar->id);
            printDebugInt("is ", assignments[currentVar->id]);
            return true;
        } else if (value(currentVar) == false) {
            VECTORdelete(literals, i);
        }
    }

    //TODO if both p and -p appear, return true
    return false;
}

void CLAUSEremoveDuplicates(V literals) {
    for (unsigned int i = 0; i < VECTORtotal(literals); i++) {
        Var currentVar = VECTORget(literals, i);
        for (unsigned int j = i + 1; j < VECTORtotal(literals); j++) {
            Var comparisonVar = VECTORget(literals, j);
            if (currentVar->id == comparisonVar->id && currentVar->sign == comparisonVar->sign) {
                VECTORdelete(literals, j);
                j--;
            }
        }
    }
}

bool CLAUSEpropagate(C clause, Var p) {
    //invariant: at the moment of the call the clause is not in watchers[p->id] anymore

    printDebug("In CLAUSEpropagate of");
    printClause(clause);
    Var negP = neg(p);

    //make sure -p is in literals[1]
    if (((Var) VECTORget(clause->literals, 0))->id == p->id &&
        ((Var) VECTORget(clause->literals, 0))->sign == negP->sign) {

        VECTORset(clause->literals, 0, VECTORget(clause->literals, 1));
        VECTORset(clause->literals, 1, neg(p));
    }

    free(negP);

    if (value((Var) VECTORget(clause->literals, 0)) == true) {
        //already satisfied, reinsert into watchers of p
        addToWatchersOf(clause, p);
        return true;
    }

    //else look for another variable to watch
    for (unsigned int i = 2; i < VECTORtotal(clause->literals); i++) {
        Var currentVar = VECTORget(clause->literals, i);
        if (value(currentVar) != false) {

            VECTORset(clause->literals, 1, currentVar);
            VECTORset(clause->literals, i, neg(p));

            addToWatchersOf(clause, neg(currentVar));

            return true;
        }
    }

    //if no watchable variable is found, clause is unit

    addToWatchersOf(clause, p);
    return enqueue(VECTORget(clause->literals, 0), clause);
}


V CLAUSEreasonFor(C clause, Var p) {
    //ASSUMPTION: p is null or literals[0]
    unsigned int i;
    V reasonForP = VECTORinit();

    if (p == NULL) {
        i = 0;
    } else {
        i = 1;
    }
    V literals = clause->literals;
    for (; i < VECTORtotal(literals); i++) {
        VECTORadd(reasonForP, neg((Var) VECTORget(literals, i)));
    }
    return reasonForP;
}
