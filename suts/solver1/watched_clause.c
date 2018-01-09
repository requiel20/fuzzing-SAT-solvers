#include "watched_clause.h"

#include <stdbool.h>
#include <stdlib.h>

#include "literal.h"
#include "clause.h"

// Quick dirty fix to get the DEBUG define
#include "solver.h"


WatchedClause
init_watched_clause(Clause *clause) {
    int literals[NWATCHED_LITERALS] = {-1, -1};
    WatchedClause this = {clause, literals};
    if (is_literal(*clause)) {
        this.literals[0] = 0;
    } else if (clause->nliterals >= 2) {
        this.literals[0] = 0;
        this.literals[1] = 1;
    }

    return this;
}

char *
wtoa(WatchedClause this) {
    Clause clause = *this.clause;
    char* sclause = malloc((clause.nliterals * LENGTH_LITERAL + 3 /*+ 2 * ( 2 + LENGTH_LITERAL)*/) * sizeof(char));
    sclause[0] = '\0';

    ChainedLiteral* literal = clause.literals;
    for (int index = 0; literal; index++) {
        strncat(sclause, ltoa(literal->literal), LENGTH_LITERAL);
        // Add '*' if watched literal
        if (equals(get(clause, (this.literals)[0]), literal->literal)
            || equals(get(clause, (this.literals)[1]), literal->literal))
        {
            strncat(sclause, "*", 1);
        }

        if (index + 1 < clause.nliterals) {
            strncat(sclause, " ", 1);
        }
        literal = literal->next;
    }
    //strncat(sclause, " [", 2);
    //strncat(sclause, ltoa(get(clause, this.literals[0])), LENGTH_LITERAL);
    //strncat(sclause, ":", 1);
    //strncat(sclause, ltoa(get(clause, this.literals[1])), LENGTH_LITERAL);
    //strncat(sclause, "]", 1);

    return sclause;
}

void
print_debug_w_clause(WatchedClause this) {
    printf("    Printing watched clause [%s]: index(%d): %s, index(%d): %s\n",
                ctoa(*this.clause),
                this.literals[0], ltoa(get(*this.clause, this.literals[0])),
                this.literals[1], ltoa(get(*this.clause, this.literals[1])));  

}

WatchedClause
copy_watched_clause(WatchedClause a) {
    WatchedClause this = init_watched_clause(copy_clause(*a.clause));
    
    this.literals[0] = a.literals[0];
    this.literals[1] = a.literals[1];

    return this;
}

/*
 * @this's watched literals
 */
Literal *
get_literals(WatchedClause this) {
    // Init return array
    Literal* literals = malloc(NWATCHED_LITERALS * sizeof(Literal));

    // Retrieve every watched literal
    // TODO: can be computed in O(nlogn) instead of O(n2) by sorting @this.literals
    for (int index = 0; index < NWATCHED_LITERALS; index++) {
        literals[index] = get(*(this.clause), this.literals[index]);
    }

    return literals;
}

/*
 * Decidable (0 <= @this.clause.size() <= 2)
 */
bool
is_decidable(WatchedClause this) {
    // TODO
    return false;
}

/*
 * Compare @a to @b
 */
int
compare(const void* a, const void* b) {
    return *(int*)a - *(int*)b;
}


/*
 * Remove @literal from @this and update watched literals
 */
int
remove_watched(WatchedClause *this, Literal literal) {
    int position = remove_literal(this->clause, literal);
    
    // If literal was watched, replace it with a new one
    for (int index = 0; index < NWATCHED_LITERALS; index++) {
        if (this->literals[index] == position) {
            qsort(this->literals, NWATCHED_LITERALS, sizeof(int), compare);
            for (int i = 0; i < NWATCHED_LITERALS; i++) {
                if (this->literals[i] != i) {
                    this->literals[position] = i;
                    break;
                }
            }
            break;
        }
    }

    return position;
}

/*
 * Return true if one of the watched literals is assigned False 
 * the parameter @literal is the one that was assigned true.
 * 
 */
bool
check_watched_clauses(WatchedClause this, Literal literal) {
    Literal * literals = get_literals(this);

    bool ret = is_opposite(
        literals[0], literal) || is_opposite(literals[1], literal);
    if (!ret) {

    }
    free(literals);

    return ret;
}

void
delete_watched_clause(WatchedClause *this)
{
    if (!this)
        return;

    delete_clause(this->clause);
    free(this);
}
