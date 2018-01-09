#ifndef _VAR_
#define _VAR_

#include <stdlib.h>

// ---------- Bool datatype ----------
typedef short bool;
#define unassigned 2
#define true 1
#define false 0
// -----------------------------------

typedef struct var *Var;
struct var {
    unsigned int id;    // Unique variable > 0 (literal) id (Eg.: 1, 2, 3)
    bool sign;          // true = positive, false = negated, unassigned = not defined
};

Var VARinit(unsigned int id, bool sign);
bool VARisPositive(Var v);

Var neg(Var p);

#endif
