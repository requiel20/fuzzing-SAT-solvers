#include "clause.h"

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "literal.h"
// Quick dirty fix to get the DEBUG define
#include "solver.h"

ChainedLiteral *
init_chained_literal_from_int(int literal) {
    return init_chained_literal_next_from_int(literal, NULL);
}

ChainedLiteral *
init_chained_literal(Literal literal) {
    return init_chained_literal_next(literal, NULL);
}

ChainedLiteral *
init_chained_literal_next_from_int(int literal, Literal* next) {
    ChainedLiteral* this = malloc(sizeof(ChainedLiteral));
    // TODO: Fix memory leak
    // Create init_literal(Literal* this, int value)
    this->literal = *init_literal(literal);
    this->next = (next) ? init_chained_literal(*next) : NULL;
    return this;
}

ChainedLiteral *
init_chained_literal_next(Literal literal, Literal* next) {
    ChainedLiteral* this = malloc(sizeof(ChainedLiteral));
    this->literal = literal;
    this->next = (next) ? init_chained_literal(*next) : NULL;
    return this;
}

Clause *
init_clause_empty() {
    Clause* this = malloc(sizeof(Clause));
    this->literals = NULL;
    this->nliterals = 0;
    return this;
}

Clause *
init_clause_from_int(int* literals, int nliterals) {
    // nliterals+1 because there is a need to set last value to NULL
    Literal* list = malloc((nliterals+1) * sizeof(Literal));

    // Convert int to literals
    for (int index = 0; index < nliterals; index++) {
        list[index] = *init_literal(literals[index]);
    }

    return init_clause_from_literals(list);
}

Clause *
init_clause_from_literals(Literal * literals) {
    Clause* this = init_clause_empty();

    ChainedLiteral* index = this->literals;
    while (literals) {
        // Create the new literal
        ChainedLiteral* chained = init_chained_literal_next(*literals, NULL);
        // if there is no literal in the clause
        if (!index) {
            this->literals = chained;
        } else {
            index->next = chained;
        }
        // Stay at the head of the list
        index = chained;
        this->nliterals++;
        // Get to the next literal
        literals++;
    }

    return this;
}

char *
ctoa(Clause this) {
    char* sclause = malloc((this.nliterals * LENGTH_LITERAL + 1) * sizeof(char));
    sclause[0] = '\0';

    ChainedLiteral* literal = this.literals;
    for (int index = 0; literal; index++) {
        strncat(sclause, ltoa(literal->literal), LENGTH_LITERAL);
        if (index + 1 < this.nliterals) {
            strncat(sclause, " ", 1);
        }
        literal = literal->next;
    }

    return sclause;
}

/*
 * Return a copy of a
 */
Clause *
copy_clause(Clause a) {
    Clause* this = init_clause_empty();

    ChainedLiteral *iA = a.literals;
    ChainedLiteral *index = NULL;
    ChainedLiteral *head = NULL;
    while (iA) {
        ChainedLiteral* copied = init_chained_literal(iA->literal);
        if (!index)
            head = copied;
        else
            index->next = copied;
        index = copied;
        iA = iA->next;
    }

    this->literals = head;
    this->nliterals = a.nliterals;

    return this;
}

/*
 * Create a new clause from @a and @b
 */
Clause *
merge(Clause a, Clause b, uint32_t resolvent) {
    Clause* this = copy_clause(a);

    ChainedLiteral* iB = b.literals;
    // Adding all @b 's elements if not duplicate and remove literals
    // of resolvent variable.
    bool resolvent_found = false;
    while (iB) {
        Literal current = iB->literal;
        ChainedLiteral* index = a.literals;
        bool discarded = false;
        while (index) {
            bool is_resolvent = (
                resolvent == current.var_id
                && is_opposite(current, index->literal)
            );
            if (is_resolvent) {
                resolvent_found = true;
                remove_literal(this, index->literal);
                this->nliterals--;
            }

            discarded = (
                is_resolvent || equals(current, index->literal) || discarded);
            index = index->next;
        }

        // if it is a new literal.
        if (!discarded) {
            add(this, current);
        }
        iB = iB->next;
    }
    if (!resolvent_found) {
        free(this);
        return NULL;
    }
    return this;
}

/*
 * List of literals common to @a and @b
 */
Clause *
diff(Clause a, Clause b) {
    Clause* this = init_clause_empty();

    // For every elements in @a
    ChainedLiteral *a_lits = a.literals;
    while (a_lits) {
        ChainedLiteral* index = b.literals;
        // If current element is in @b, add it
        while (index) {
            if (equals(a_lits->literal, index->literal)) {
                add(this, index->literal);
            }
            index = index->next;
        }
        a_lits = a_lits->next;
    }

    return this;
}

/*
 * Remove @literal from @this
 */
int
remove_literal(Clause *this, Literal literal) {
    ChainedLiteral* index = this->literals;
    
    // if the literal to remove is the head of the list
    if (equals(index->literal, literal)) {
        this->literals = index->next;
        free(index);
        return 0;
    }

    ChainedLiteral* prec = index;
    index = index->next;
    // Position of the removed item
    int position = 1;
    // Iterate through all elements of @this
    while (index) {
        // If current element is to be remove
        if (equals(index->literal, literal)) {
            // Link previous element to the next one
            prec->next = index->next;
            // Freeing removed element
            free(index);
            //this->nliterals--;
            return position;
        }
        // Moving to next element in the list
        prec = index;
        index = index->next;
        position++;
    }

    return -1;
}

/*
 * Add @literal to the end of @this without checking if it is already in the clause
 */
void
add(Clause* this, Literal literal) {
    ChainedLiteral* chained = init_chained_literal(literal);
    if (is_empty(*this)) {
       this->literals = chained; 
    } else {
        ChainedLiteral* index = this->literals;
        while (index->next) {
            index = index->next;
        }
        index->next = chained;
    }
    this->nliterals++;
}

/*
 * Add @literal to the end of @this without checking if it is already in the clause
 */
void
add_int(Clause* this, int literal) {
    ChainedLiteral* chained = init_chained_literal_from_int(literal);
    if (is_empty(*this)) {
       this->literals = chained; 
    } else {
        ChainedLiteral* index = this->literals;
        while (index->next) {
            index = index->next;
        }
        index->next = chained;
    }
    this->nliterals++;
}

/*
 * Get i-th literal of @this
 */
Literal
get(Clause this, int i) {
    // If wanted element does not exist
    #if defined(DEBUG_GET)
        printf("Getting %i-th element of [%s]: ", i, ctoa(this));
    #endif

    if (i >= this.nliterals) {
        #if defined(DEBUG_GET)
            printf("%s (%i > size of clause)\n", ltoa(*init_literal(0)), i);
        #endif
        return *init_literal(0);
    }

    ChainedLiteral* index = this.literals;
    while (i > 0) {
        index = index->next;
        i--;
    }

    #if defined(DEBUG_GET)
            printf("%s\n", ltoa(index->literal));
    #endif

    return index->literal;
}

int
check_var(Clause this, uint32_t var_id) {
    while (this.literals) {
        if (this.literals->literal.var_id == var_id) {
            return !this.literals->literal.negated;
        }
        this.literals++;
    }
    return -1;
}

/*
 * True if clause is empty
 */
bool
is_empty(Clause this) {
    return this.nliterals == 0;
}

/*
 * True if clause is a literal
 */
bool
is_literal(Clause this) {
    return this.nliterals == 1;
}

void
delete_clause(Clause *this)
{
    ChainedLiteral *literals = this->literals;
    while (literals) {
        ChainedLiteral *lit_to_rm = literals;
        literals = literals->next;
        free(lit_to_rm);
    }
    free(this);
}
