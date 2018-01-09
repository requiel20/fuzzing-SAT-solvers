#include "sat.h"

bool value(Var p) {
    if (p->id <= numberOfLiterals && assignments[p->id] != unassigned) {
        if (p->sign == true) {
            return assignments[p->id];
        } else if (p->sign == false)
            return !(assignments[p->id]);
    }
    return unassigned;
}

void addToWatchersOf(C clause, Var p) {
    int id;
    if (p->sign)
        id = p->id;
    else
        id = -p->id;
    VECTORadd(watchers[id], clause);
}

V watchersOf(Var p) {
    int id;
    if (p->sign)
        id = p->id;
    else
        id = -p->id;
    return watchers[id];
}

int currentDecisionLevel() {
    return trail_lim_size;
}

bool allVarsAssigned() {
    for (unsigned int i = 1; i <= numberOfLiterals; i++) {
        if (assignments[i] == unassigned) return false;
    }
    return true;
}

void staticVarOrder() {

    // Clear activity
    for (unsigned int i = 1; i <= numberOfLiterals; i++) activity[i] = 0;

    // Simple variable activity heuristic
    for (unsigned int i = 0; i < VECTORtotal(cnf); i++) {

        C c = VECTORget(cnf, i);
        
        for (unsigned int j = 0; j < VECTORtotal(c->literals); j++) {
            Var v = VECTORget(c->literals, j);
            activity[v->id] += 1;
        }
    }
}

unsigned int selectVar() {

    //int maxActivity = 0;
    unsigned int maxId = 0;

    for (unsigned int id = 1; id < numberOfLiterals; id++) {
        if (assignments[id] == unassigned) {
            //if (maxActivity <= activity[id]) {
                //maxActivity = activity[id];
                return id;
            //}
        }
    }
    return maxId;
}

bool decide(unsigned int id) {
    trail_lim_size++;
    trail_lim = realloc(trail_lim, sizeof(unsigned int) * trail_lim_size);

    printDebugInt("Trail lim size: ", trail_lim_size);
    trail_lim[trail_lim_size - 1] = VECTORtotal(trail);

    printDebugInt("--------- LEVEL ", currentDecisionLevel());

    // Dummy variable to store the id
    Var decidingVar = VARinit(id, true);

    printDebugVar("Assigned true to ", decidingVar);
    return enqueue(decidingVar, NULL);
}

//returns false on conflict, true on succesfull enqueueing
bool enqueue(Var p, C from) {
    if (value(p) == false) {
        printDebugVar("Conflicting, not enqueueing: ", p);
        return false;
    } else if (value(p) == true) {
        printDebugVar("Already true, not enqueueing: ", p);
        return true;
    } else {
        printDebugVar("Enqueueing for propagation: ", p);
        assignments[p->id] = p->sign;

        level[p->id] = currentDecisionLevel();
        reason[p->id] = from;
        VECTORadd(trail, p);

        QUEUEinsert(propagationQ, p);
        return true;
    }
}

C propagate() {
    printDebugInt("Propagation starting, queue size: ", propagationQ->size);
    while (propagationQ->size > 0) {

        printFormula(cnf);

        Var propagatingVar = QUEUEdequeue(propagationQ);
        unsigned int numberOfWatchers = watchersOf(propagatingVar)->total;

        printDebugVar("Propagating of ", propagatingVar);

        C propagationWatchers[numberOfWatchers];

        //moving the watching clauses to a temporary vector
        for (unsigned int i = 0; i < numberOfWatchers; i++) {
            propagationWatchers[i] = VECTORget(watchersOf(propagatingVar), i);
        }

        VECTORfree(watchersOf(propagatingVar));

        int propagatingVarId;
        if (propagatingVar->sign)
            propagatingVarId = propagatingVar->id;
        else
            propagatingVarId = -propagatingVar->id;

        watchers[propagatingVarId] = VECTORinit();

        for (unsigned int i = 0; i < numberOfWatchers; i++) {
            printDebug("Clause before propagation: ");
            printClause(propagationWatchers[i]);
            if (!CLAUSEpropagate(propagationWatchers[i], propagatingVar)) {
                //clause was unit and conflicting, aborting

                //adding back the watchers
                for (unsigned int j = i + 1; j < numberOfWatchers; j++) {
                    VECTORadd(watchersOf(propagatingVar), propagationWatchers[j]);
                }

                //flushing propagationQ
                QUEUEclear(propagationQ);

                printDebug("Clause after propagation: ");
                printClause(propagationWatchers[i]);

                //and returning the conflicting clause
                return propagationWatchers[i];
            }

            printDebug("Clause after propagation: ");
            printClause(propagationWatchers[i]);
        }
    }

    return NULL;
}

void undoOne() {
    // Get the last element from the trail vector (last assignment)
    Var p = VECTORget(trail, VECTORtotal(trail) - 1);
    int id = p->id;

    assignments[id] = unassigned;
    reason[id] = NULL;
    level[id] = -1;

    VECTORpop(trail);
}

void cancel() {

    // c is the difference between the total number of assignments and and first assignment of the 
    // current level, i.e., the number of assignments to cancel
    printDebugInt("--------- Deleting level: ", currentDecisionLevel());
    printDebugInt("Trail size: ", VECTORtotal(trail));
    printDebugInt("Level to be deleted first trail address: ", trail_lim[trail_lim_size - 1]);

    unsigned int c = VECTORtotal(trail) - trail_lim[--trail_lim_size];
    printDebugInt("Reducing trail of: ", c);
    for (; c != 0; c--) {
        undoOne();
    }

}

void cancelUntil(int level) {
    while (currentDecisionLevel() > level)
        cancel();
    printDebugInt("trail_lim_size: ", trail_lim_size);
}

void varBumpActivity(Var v) {

    if (activity[v->id] += var_inc >= 1e100) varRescaleActivity();
    activity[v->id] *= var_inc;

}

void varRescaleActivity() {

    for (unsigned int i = 1; i <= numberOfLiterals; i++) activity[i] *= VARINC;
    var_inc *= VARINC;

}

void varDecayActivity() {

    var_inc *= var_decay;

}

void printAssignments() {

    for (unsigned int i = 1; i <= numberOfLiterals; i++) {
        if (assignments[i]) printf("%d", i);
        else printf("-%d", i);
        if (i != numberOfLiterals) printf(" ");
    }
    printf("\n");
}

void learn(V learntClauseVars) {
    C clause = NULL;
    CLAUSEnew(learntClauseVars, true, &clause);
    if (clause != NULL) {
        enqueue(VECTORget(clause->literals, 0), clause);
        VECTORadd(learnts, clause);
        printClause(clause);
    }
}

int analyze(C conflictClause, V learntClauseLits) {
    Var p = NULL;
    V reasonForP;
    int backtrackLevel = 0;
    int count = 0;
    bool seen[numberOfLiterals + 1];

    //first position will be set last
    VECTORadd(learntClauseLits, NULL);

    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        seen[i] = false;
    }

    printDebugInt("Trail size: ", VECTORtotal(trail));

    do {
        // VECTORfree(reasonForP);
        printDebug("Main learning cycle started");
        reasonForP = CLAUSEreasonFor(conflictClause, p);

        printDebug("Expanding reasoning");
        //expand reasoning for p
        for (unsigned int i = 0; i < VECTORtotal(reasonForP); i++) {
            Var q = VECTORget(reasonForP, i);
            if (!seen[q->id]) {
                seen[q->id] = true;
                printDebugVar("\tFound new variable: ", q);

                if (level[q->id] == currentDecisionLevel()) {
                    count++;
                } else if (level[q->id] > 0) {
                    VECTORadd(learntClauseLits, neg(q));
                    backtrackLevel = max(backtrackLevel, level[q->id]);
                }
            }
        }
        VECTORfree(reasonForP);
        
        

        printDebug("Selecting next p");
        //select next p
        do {
            p = VECTORget(trail, VECTORtotal(trail) - 1);
            if (VECTORtotal(trail) <= trail_lim[trail_lim_size - 1])
                break;
            conflictClause = reason[p->id];
            undoOne();

        } while (!seen[p->id]);

        printDebugVar("Next p is: ", p);
        printDebugInt("Trail size: ", VECTORtotal(trail));
        printDebugInt("Counter: ", count);
        count--;

    } while (count > 0 && VECTORtotal(trail) > trail_lim[trail_lim_size - 1]);

    VECTORset(learntClauseLits, 0, neg(p));

    return backtrackLevel;
}

int solve(V formula) {

    // Currently last assigned variable
    unsigned int varToDecide = 0;
    var_decay = VARDECAY;
    // staticVarOrder();

    while (true) {
        printFormula(formula);

        C conflictingClause = propagate();

        if (conflictingClause == NULL) {
            //no conflict
            if (allVarsAssigned()) {
                return true;
            } else {
                //select a new var
                printDebug("Selecting new var");

                varToDecide = selectVar();

                if (varToDecide > 0) {
                    printDebugInt("Selected new var: ", varToDecide);
                    //assigne true to varToDecide
                    decide(varToDecide);
                } else {
                    printDebug("No more variables to assign ");
                    return false;
                }
            }
        } else {
            //conflict
            printDebugInt("Conflict at level", currentDecisionLevel());
            //conflict
            if (currentDecisionLevel() == 0) {
                //top level conflict
                return false;
            }

            V learntClauseVars = VECTORinit();

            int backtrackTo = analyze(conflictingClause, learntClauseVars);

            printDebug("Analyzed conflict");
            
            printDebugInt("Backtracking until: ", max(backtrackTo, rootLevel));

            cancelUntil(max(backtrackTo, rootLevel));

            learn(learntClauseVars);

            printDebug("Learnt conflict clause");
        }
    }
}

void initializeAssigments() {
    assignments = (bool *) malloc(sizeof(bool) * numberOfLiterals + sizeof(bool));
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        assignments[i] = unassigned;
    }
}

void initializeLevel() {
    level = (int *) malloc(sizeof(int) * numberOfLiterals + sizeof(int));
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        level[i] = -1;
    }
}

void initializeReason() {
    reason = (C *) malloc(sizeof(C) * numberOfLiterals + sizeof(C));
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        reason[i] = NULL;
    }
}

void initializeTrail() {
    // Trail vector will keep track of each assignment, to backtrack
    trail = VECTORinit();
    trail_lim_size = 0;
}

void initializeWatchers() {
    watchers = (V *) malloc(sizeof(V) * numberOfLiterals * 2 + sizeof(V));
    for (unsigned int i = 0; i <= numberOfLiterals * 2; i++) {
        watchers[i] = VECTORinit();
    }

    watchers = watchers + numberOfLiterals;
}

void initializeActivities() {
    activity = (int *) malloc(sizeof(double) * numberOfLiterals + sizeof(int));
    for (unsigned int i = 0; i <= numberOfLiterals; i++) {
        activity[i] = 0;
    }
}

V initialize(char *path) {

    FILE *inputFile = PARSERinit(path);
    PARSEheader(inputFile);


    initializeAssigments();
    initializeTrail();
    initializeLevel();
    initializeReason();

    learnts = VECTORinit();

    propagationQ = QUEUEinit();

    initializeWatchers();
    initializeActivities();

    rootLevel = currentDecisionLevel();

    lastAssignedVar = NULL;

    return PARSEformula(inputFile);
}


void destroy() {
    
    free(activity);
    free(assignments);
    free(level);
 
    VECTORfree(learnts);
   
    watchers -= numberOfLiterals;
    for (unsigned int i = 0; i <= numberOfLiterals * 2; i++) {
        VECTORfree(watchers[i]);
    }

    VECTORfree(cnf);
}


int main(int argc, char **argv) {

    if (argc != 2) {
        printf("sat accepts only 1 argument which is the filename of the formula.\n");
        exit(EXIT_FAILURE);
    }

    cnf = initialize(argv[1]);
    if (cnf == NULL) {
        printf("UNSAT\n");
        exit(EXIT_SUCCESS);
    }

    printDebug("-----RUNNING DPLL-----");
    int result = solve(cnf);

    if (result == true) {
        printf("SAT\n");
        printAssignments();
    } else {
        printf("UNSAT\n");
    }

    destroy();

    exit(EXIT_SUCCESS);
}
