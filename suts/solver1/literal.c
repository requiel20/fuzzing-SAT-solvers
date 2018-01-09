#include "literal.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>


/*
 * Convert int to literal
 */
Literal *
init_literal(int n) {
    Literal* this = malloc(sizeof(Literal));

    if (n >= 0) {
        this->var_id = n;
        this->negated = false;
    } else {
        this->var_id = -1 * n;
        this->negated = true;
    }

    return this;
}

Literal *
copy_literal(Literal this) {
    return init_literal(ltoi(this));
}

int
ltoi (Literal this) {
    return !this.negated ? this.var_id: -1 * this.var_id;
}

char *
ltoa(Literal this) {
    // TODO: To avoid bufferoverflow in the future, need to get number of digit
    // in USHRT_MAX  
    char* str = malloc(LENGTH_LITERAL * sizeof(char));
    sprintf(str, "%u", ltoi(this));
    return str;
}

/*
 * Return true if @this is the opposite of @other
 */
bool
is_opposite(Literal this, Literal other) {
    return same_id(this, other) && !equals(this, other);
}

/*
 * Return true if @this equals @other
 */
bool
equals(Literal this, Literal other) {
    return same_id(this, other) && this.negated == other.negated;
}

/*
 * Return true if @this.var_id equals @other.var_id
 */
bool
same_id(Literal this, Literal other) {
    return this.var_id == other.var_id;
}

