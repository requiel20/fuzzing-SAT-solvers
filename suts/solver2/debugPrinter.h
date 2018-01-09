#ifndef SAT_DEBUGPRINTER_H
#define SAT_DEBUGPRINTER_H

#include "vector.h"
#include "var.h"
#include "clause.h"
#include "sat.h"

#define IS_DEBUG 0

void printDebugInt(char *message, int value);

void printClause(C clause);

void printFormula(V formula);

void printDebugIntInt(char *message, int value1, int value2);

void printDebugChar(char *message, int value);

void printDebugVar(char *message, Var var);

void printClause(C clause);

void printDebug(char *message);

#endif
