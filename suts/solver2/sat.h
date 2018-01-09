#ifndef SAT_SAT_H
#define SAT_SAT_H

#include <stdio.h>
#include <stdlib.h>
#include "vector.h"
#include "var.h"
#include "parser.h"
#include "debugPrinter.h"
#include "queue.h"
#include "clause.h"

// --- BECAUSE C
#define max(X, Y)  ((X) > (Y) ? (X) : (Y))

#define VARDECAY 1
#define VARINC 1

V cnf;

// --- Propagation
V *watchers;
V *undos;
Q propagationQ;

// --- Ordering

int *activity;
double var_inc;
double var_decay;

// --- Assignments
unsigned int numberOfLiterals;
unsigned int numberOfClauses;
bool *assignments;
V trail;
unsigned int *trail_lim;
unsigned int trail_lim_size;
Var lastAssignedVar;
bool lastAssignedValue;

// --- Backtrack and learning
C *reason;
int rootLevel;
V learnts;
Var lastDecisionUndone;

// --- Decision level
int *level;

// --- Solver Functions
bool value(Var p);

bool conflict(V formula);

bool allVarsAssigned();

unsigned int selectVar();

bool decide(unsigned int id);

void addToWatchersOf(C clause, Var p);

void change_decision(unsigned int assigned);

void simplifyClause(V clause, V unitVars);

V watchersOf(Var p);

void undoOne();

C propagate();

void printAssignments();

int solve(V formula);

bool enqueue(Var p, C from);

int currentDecisionLevel();

void initializeAssigments();

void varBumpActivity(Var v);

void varDecayActivity();

void varRescaleActivity();

#endif //SAT_SAT_H
