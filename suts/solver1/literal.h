#ifndef LITERAL_H
#define LITERAL_H

#define LENGTH_LITERAL 21
#define ABS(x) (((x) < 0) ? -(x) : (x))

#include <stdbool.h>

typedef struct literal {
    unsigned int var_id;
    bool negated;
} Literal;

/*
 * Convert int to literal
 */
Literal *
init_literal(int n);

Literal *
copy_literal(Literal this);

int
ltoi (Literal this);

char *
ltoa(Literal this);

/*
 * Return true if @this is the opposite of @other 
 */
bool
is_opposite(Literal this, Literal other);

/*
 * Return true if @this equals @other
 */
bool
equals(Literal this, Literal other);

/*
 * Return true if @this.var_id equals @other.var_id
 */
bool
same_id(Literal this, Literal other);

#endif
