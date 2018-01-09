#ifndef SOLVER_H
#define SOLVER_H

#define UNSAT 0
#define SAT 1

//#define DEBUG_BCP
//#define DEBUG

#include "literal.h"
#include "formula.h"


typedef struct sat_model {
    int is_sat; // 1 if formula is satisfiable; 0 otherwise.
    Formula *formula;
} SatModel;

typedef struct bcp_output {
	Clause *clause;
	Literal *assigned_literals;
	int nliterals;
} BcpOutput;


/*
 * TODO: Read the file of CNF formula and construct a native
 * representation of it.
 */
Formula *
parse_cnf(char *path);


Literal
decide(Formula *formula);


BcpOutput *
bcp(Formula *formula, Literal literal, Clause *antecedent);

void
delete_bcp_output(BcpOutput *bcp_output);


/*
 * Look for literals that are only used positively or negatively
 */
void
plp(Formula *formula);


/*
 * TODO: Gets a formula, applies a SAT solver on it, and returns
 * a sat model indicating if it is satisfiable or not.
 */
SatModel
solve(Formula *formula);


/*
 * TODO: Report SAT model to standard output.
 */
void
report(SatModel model);

void
sat(char *path);

#endif
