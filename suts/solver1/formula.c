#include "formula.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "clause.h"
#include "literal.h"
#include "watched_clause.h"

#define MAX_LINE_LENGTH 1024
#define EXTRA_SPACE_FORMULA 10


LitTable
init_unassigned_table(int nvars, Assignment *assignments)
{
    LitTable table = NULL;
    for (int i = 0; i < nvars; i++) {
        double neg_score = (double) assignments[i].neg_score;
        double score = (double) assignments[i].score;
        Literal literal = {i + 1, 0};

        // We first store a normal literal, i.e. p.
        literal.negated = 0;
        table = insert_literal(table, literal, score);

        // Now, we store negated literal.
        literal.negated = 1;
        table = insert_literal(table, literal, neg_score);
    }
    return table;
}

LearntClauses *
init_learnt_clauses()
{
    LearntClauses *l_clauses = malloc(sizeof(LearntClauses));
    WatchedClause *clauses = malloc(NLEARNT_CLAUSES * sizeof(WatchedClause));
    memset(clauses, 0, NLEARNT_CLAUSES * sizeof(WatchedClause));
    l_clauses->clauses = clauses;
    l_clauses->size = 0;
    l_clauses->n_add = 0;
    return l_clauses;
}

Formula *
init_formula_empty() {
    Formula* this = malloc(sizeof(Formula));
    this->l_clauses = NULL;
    this->watched_clauses = NULL;
    this->assignments = NULL;
    this->nclauses = 0;
    this->nvars = 0;
    return this;
}

Formula *
init_formula_from_strings(char* input, int nclauses, int nvars) {
    char* cnf = strdup(input);
    Clause* clauses = malloc(nclauses * sizeof(Clause));

    int index = 0;
    char* sclause = cnf;
    while (sclause)
    {
        Clause* clause = init_clause_empty();
        char* sliteral = strtok(strdup(sclause), " ");
        while (sliteral != NULL) {
            int literal = 0;
            sscanf(sliteral, "%d", &literal);
            if (literal != 0) {
                add_int(clause, literal);
            }

            if (strstr(sliteral, "\n")) {
                break;
            }
            sliteral = strtok(NULL, " ");
        }
        clauses[index] = *clause;
        
        // Move to next line
        index++;
        sclause = strchr(sclause, '\n');
        // if not the end of the formula
        if (sclause) {
            sclause++;
        }
    }
    return init_formula_from_clauses(clauses, nclauses, nvars);
}

Formula *
init_formula_from_file(char* path) {
    // Open file in read mode
    FILE* file = fopen(path, "r");
    if (!file) {
        fprintf(stderr, "Error while openning the file %s.\n", path);
        exit(EXIT_FAILURE);
    }

    int nclauses = -1;
    int nvars = -1;
    do {
        // Read first character of the line
        char first;
        do {
            fscanf(file, "%c ", &first);
        } while (first == '\n');

        // If 'c', it is a comment
        if (first == 'c') {
            // Then we ignore it
            fscanf(file, "%*[0-9a-zA-Z ]s");
        } else if (first == 'p') {
            // We have a proposition
            fscanf(file, "%*s %d %d", &nvars, &nclauses);
        } else {
            // Input file not valid
            perror("Input file is not valid");
            exit(EXIT_FAILURE);
        }
    } while (nclauses == -1);

    // Read all clauses
    // I assume input is well formated (i.e. @nclauses matches number of lines)
    Clause* clauses = malloc(nclauses * sizeof(Clause));
    Assignment *assignments = init_assignments(nvars);
    for (int index = 0; index < nclauses; index++) {
        // Clause to be append
        Clause* clause = init_clause_empty();
        int literal;
        // Scan literal while not 0
        do {
            fscanf(file, "%d", &literal);
            if (literal != 0) {
                int var_id = ABS(literal);
                if (literal < 0) {
                    assignments[var_id - 1].neg_score++;
                } else {
                    assignments[var_id - 1].score++;
                }
                add_int(clause, literal);
            }
        } while (literal != 0);
        clauses[index] = *clause;
    }
    LitTable unassigned_lits = init_unassigned_table(nvars, assignments);

    // Close the file
    fclose(file);

    // Init formula from scanned clauses
    Formula *formula = init_formula_from_clauses(clauses, nclauses, nvars);
    formula->unassigned_lits = unassigned_lits;
    formula->assignments = assignments;
    return formula;
}


Assignment *
init_assignments(int nvars)
{
    // Default assignment info indicates (obviously) that literal is
    // unassigned.
    Assignment *assignments = malloc(nvars * sizeof(Assignment));
    for (int i = 0; i < nvars; i++) {
        Assignment assignment = {0, NULL, UNASSIGNED, 0.0, 0.0};
        assignments[i] = assignment;
    }
    return assignments;
}


ATracker *
init_assignment_tracker()
{
    ATracker *assign_tracker = malloc(
        MAX_DECISION_LEVEL * sizeof(ATracker));
    memset(assign_tracker, 0, MAX_DECISION_LEVEL * sizeof(ATracker));
    return assign_tracker;
}


Formula *
init_formula_from_clauses(Clause* cnf, int nclauses, int nvars) {
    WCList *wc = malloc(nvars * sizeof(WCList));
    memset(wc, 0, nvars * sizeof(WCList));
    Formula* this = init_formula_empty();
    this->w_clauses = wc;

    this->l_clauses = init_learnt_clauses();
    WatchedClause *watched_clauses = malloc(nclauses * sizeof(WatchedClause));
    for (int i = 0; i < nclauses; i++){
        watched_clauses[i] = init_watched_clause(&cnf[i]);
        int index1 = watched_clauses[i].literals[0];
        int index2 = watched_clauses[i].literals[1];

        Clause *clause = watched_clauses[i].clause;

        Literal literal1 = get(*clause, index1);
        Literal literal2 = get(*clause, index2);

        add_w_clause(this, literal1, &watched_clauses[i]);
        add_w_clause(this, literal2, &watched_clauses[i]);
    }

    this->watched_clauses = watched_clauses;
    this->nclauses = nclauses;
    this->nvars = nvars;
    this->decision_level = 1;
    this->assignments = NULL;
    this->assignment_trackers = init_assignment_tracker();
    this->unassigned_lits = NULL;
    this->nassigned = 0;
    
    return this;
}

char *
ftoa(Formula this) {
    /*
     * In the worst case there are nvars in each clause
     * Each clause printed has nvars (literal + spaces), 2 parenthethis,
     * 4 spaces, 2 * and /\  this.nvars * LENGTH_LITERAL + this.nvars + 10
     * for each clause.
     */
    int size = this.nvars * (LENGTH_LITERAL + 1) + (
            EXTRA_SPACE_FORMULA * this.nclauses);
    char *sformula = malloc(size * sizeof(char));
    sformula[0] = '\0';

    for (int index = 0; index < this.nclauses; index++) {
        Clause clause = *this.watched_clauses[index].clause;
        int length = clause.nliterals * LENGTH_LITERAL + 1;
        strncat(sformula, "( ", 2);
        strncat(sformula, wtoa(this.watched_clauses[index]), length);
        strncat(sformula, " )", 2);
        if (index != this.nclauses - 1)
            strncat(sformula, " /\\ ", 4);
    }
    
    return sformula;
}

/*
 * All clauses from @this containing @literal
 * 
 * --> it might be more efficient to have for each literal 
 *     an array of the clauses in which it is used
 */
Formula *
get_all_clauses(Formula this, Literal literal) {
    Clause* clauses = malloc(this.nclauses * sizeof(Clause));

    int index = 0;
    for (int i = 0; i < this.nclauses; i++) {
        if (check_var(*this.watched_clauses[i].clause, literal.var_id) >= 0) {
            clauses[index] = *this.watched_clauses[i].clause;
            index++;
        }
    }

    return init_formula_from_clauses(clauses, index, this.nvars);
}


/*
 * Assign @literal to true in @this
 */
int
assign(Formula* this, Literal literal, Clause *antecedent)
{
    enum assigned_value value = (
        literal.negated ? ASSIGNED_FALSE : ASSIGNED_TRUE);
    if (literal.var_id > (uint32_t) this->nvars || literal.var_id == 0) {
        fprintf(stderr, "Error: You cannot assign an unknown literal '%s'.",
                ltoa(literal));
        exit(EXIT_FAILURE);
    }

    Assignment assignment = this->assignments[literal.var_id - 1];

    if (assignment.value != UNASSIGNED) {
        fprintf(stderr, "Error: You are going to re-assign literal '%s'.",
                ltoa(literal));
        exit(EXIT_FAILURE);
    }
    // Remove literals associated with the variable from the LitTable, i.e.
    // the table of unassigned literals.
    double score = literal.negated ?
        assignment.neg_score : assignment.score;
    double compl_score = literal.negated ?
        assignment.score : assignment.neg_score;
    Literal compl = {literal.var_id, !literal.negated};
    this->unassigned_lits = delete_literal(
        this->unassigned_lits, literal, score);
    this->unassigned_lits = delete_literal(
        this->unassigned_lits, compl, compl_score);
    this->nassigned++;

    assignment.decision_level = this->decision_level;
    assignment.antecedent = antecedent;
    assignment.value = value;
    this->assignments[literal.var_id - 1] = assignment;

    ATracker curr_assignments = (
            this->assignment_trackers[this->decision_level - 1]);
    ATracker new = malloc(sizeof(AssignmentTracker));
    new->var_id = literal.var_id;
    if (!curr_assignments) {
        new->next = NULL;
    } else {
        new->next = curr_assignments;
    }
    this->assignment_trackers[this->decision_level - 1] = new;
    return 0;
}

void
revoke_assignment(Formula *this, uint32_t var_id)
{
    Assignment assignment = this->assignments[var_id - 1];
    AssignedValue value = assignment.value;
    int negated = value == ASSIGNED_TRUE ? 0 : 1;
    int comp = value == ASSIGNED_TRUE ? 1: 0;
    double score = (value == ASSIGNED_TRUE) ?
        assignment.score : assignment.neg_score;
    double compl_score = (value == ASSIGNED_TRUE) ?
        assignment.neg_score : assignment.score;

    assignment.value = UNASSIGNED;
    assignment.antecedent = NULL;
    assignment.decision_level = -1;

    this->assignments[var_id - 1] = assignment;

    Literal literal = {var_id, negated};
    Literal complement = {var_id, comp};
    if (score > 0)
        this->unassigned_lits = insert_literal(
            this->unassigned_lits, literal, score);
    if (compl_score > 0)
        this->unassigned_lits = insert_literal(
            this->unassigned_lits, complement, compl_score);
}

/*
 * Cancel @var_id assignment at @this only if it has been assigned at the
 * current decision level.
 */
int
cancel(Formula* this, uint32_t var_id)
{
    if (var_id > (uint32_t) this->nvars || var_id == 0) {
        return 0;
    }
    if (this->decision_level <= 0) {
        fprintf(stderr, "Error: Formula is in an inconsistent state."
                " Decision level '%d' found.", this->decision_level);
        exit(EXIT_FAILURE);
    }

    ATracker assignments = (
            this->assignment_trackers[this->decision_level - 1]);

    bool var_found = false;
    ATracker prev = assignments;
    ATracker head = assignments->next;

    if (prev->var_id == var_id) {
        var_found = true;
        this->assignment_trackers[this->decision_level - 1] = head;
    }
    while (head && !var_found) {
        if (head->var_id != var_id) {
            prev = head;
            head = head->next;
            continue;
        }
        prev->next = head->next;
        free(head);
        var_found = true;
        break;

    }

    if (!var_found) {
        fprintf(stderr, "Error: Variable %d has not been assigned at the"
                " current decision level %d. You cannot revoke it.",
                var_id, this->decision_level);
        return -1;
    }
    revoke_assignment(this, var_id);
    this->nassigned--;
    return 0;
}

void
add_learnt_clause(Formula *this, Clause *learnt_clause)
{
    if (!this) {
        fprintf(stderr, "Error: You cannot add a learnt clause to an `NULL`"
                " formula");
        exit(EXIT_FAILURE);
    }

    if (!learnt_clause) {
        fprintf(stderr, "Error: You cannot add a `NULL` clause` to formula");
        exit(EXIT_FAILURE);
    }

    LearntClauses *l_clauses = this->l_clauses;
    if (l_clauses->size < NLEARNT_CLAUSES) {
        l_clauses->clauses[l_clauses->n_add++] = init_watched_clause(
            learnt_clause);
        l_clauses->size++;
        return;
    }
 
    if (l_clauses->n_add == NLEARNT_CLAUSES) {
        l_clauses->n_add = 0;
    }
    l_clauses->clauses[l_clauses->n_add++] = init_watched_clause(
        learnt_clause);
    update_scores(this, learnt_clause);
}


void
decaying_sum(Formula *this)
{
    for (int i = 0; i < this->nvars; i++) {
        Literal literal = {i + 1, 0};
        Literal comp =  {i + 1, 1};

        Assignment assignment = this->assignments[i];

        double score = assignment.score;
        double comp_score = assignment.neg_score;

        this->unassigned_lits = delete_literal(
            this->unassigned_lits, literal, score);
        this->unassigned_lits = delete_literal(
            this->unassigned_lits, comp, comp_score);

        score /= 2;
        comp_score /= 2;

        this->unassigned_lits = insert_literal(
            this->unassigned_lits, literal, score);
        this->unassigned_lits = insert_literal(
            this->unassigned_lits, comp, comp_score);

        this->assignments[i].score = score;
        this->assignments[i].neg_score = comp_score;
    }
}


void
update_scores(Formula *this, Clause *learnt_clause)
{
    ChainedLiteral *literals = learnt_clause->literals;
    while (literals) {
        Literal literal = literals->literal;
        double prev_score;
        if (literal.negated) {
            prev_score = this->assignments[literal.var_id - 1].neg_score;
            this->assignments[literal.var_id - 1].neg_score++;
        } else {
            prev_score = this->assignments[literal.var_id - 1].score;
            this->assignments[literal.var_id - 1].score++;
        }
        LitTable tmp = delete_literal(
            this->unassigned_lits, literal, prev_score);
        this->unassigned_lits = insert_literal(tmp, literal, prev_score++);
        literals = literals->next;
    }
    decaying_sum(this);
}

void
delete_learnt_clauses(LearntClauses *l_clauses)
{
    if (!l_clauses) {
        return;
    }

    free(l_clauses->clauses);
    free(l_clauses);
}

void
delete_formula(Formula *this)
{
    if (!this) {
        return;
    }
    delete_learnt_clauses(this->l_clauses);
    free(this->w_clauses);
    free(this->watched_clauses);
    free(this->assignments);
    free(this->assignment_trackers);
    free(this->unassigned_lits);
    free(this);
}

void
add_w_clause(Formula *formula, Literal literal, WatchedClause *clause)
{
    if (!clause || !formula || !literal.var_id) {
        fprintf(stderr, "Error: Cannot apply remove_w_clause() on `NULL`"
                " values.\n");
        exit(EXIT_FAILURE);
    }
    WCList wl = malloc(sizeof(WCItem));
    wl->clause = clause;
    if (!formula->w_clauses[literal.var_id - 1]) {
        wl->next = NULL;
    } else
        wl->next = (WCItem *) formula->w_clauses[literal.var_id - 1];
    formula->w_clauses[literal.var_id - 1] = wl;
}


void
remove_w_clause(Formula *formula, Literal literal, WatchedClause *w_clause)
{
    if (!formula || !literal.var_id) {
        fprintf(stderr, "Error: Cannot apply remove_w_clause() on `NULL`"
                " values.\n");
        exit(EXIT_FAILURE);
    }
    WCList prev = (WCList) formula->w_clauses[literal.var_id - 1];

    if (!prev) {
        return;
    }
    WCList head = (WCList) prev->next;

    Clause *curr_clause = w_clause->clause;

    Clause *cmp = diff(*curr_clause, *prev->clause->clause);
    if (!cmp->nliterals) {
        formula->w_clauses[literal.var_id - 1] = head;
        return;
    }
    delete_clause(cmp);
    while (head) {
        cmp = diff(*curr_clause, *(head->clause->clause));
        if (cmp->nliterals > 0) {
            prev = head;
            head = head->next;
            delete_clause(cmp);
            continue;
        }
        delete_clause(cmp);
        prev->next = head->next;
        free(head);
        formula->w_clauses[literal.var_id - 1] = prev;
        break;
    }
}
