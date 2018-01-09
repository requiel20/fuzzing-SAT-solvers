#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define TAG(ptr) ((int*) ((uintptr_t) (ptr) | 1))
#define UNTAG(ptr) (int*) ((uintptr_t) (ptr) & ~1)
#define IS_TAGGED(ptr) ((uintptr_t) (ptr) & 1)

typedef struct {
  int nbvars;
  int nbclauses;
  int** clauses;
  int* clause_lit_count;
  char* assignments;
  int** lit_to_clauses;
  int* lit_to_clauses_count;
} model;

typedef int var;

bool DPLL(model phi);
bool consistent(model phi);
bool contains_empty(model phi);
model assign(var i, char val, model phi);
model copy(model phi, model new, bool alloc);
var choose_literal(model phi);
var find_unit_literal(model phi);
var find_pure_literal(model phi);
void remove_lit(model phi, int clause_index, var lit);

char* latest_assignment;
int binary_search(int* arr, int size, int x);

//-----------------------------------------------------------------------------
int abscmpfunc (const void * a, const void * b) {
   return ( abs(*(int*)a) - abs(*(int*)b) );
}

//-----------------------------------------------------------------------------
int main(int argc, char **argv) {
    if (argc != 2) {
        printf("sat accepts only 1 argument which is the filename of the formula.\n");
        exit(EXIT_FAILURE);
    }
    char line[255];
    FILE *fp = fopen(argv[1], "r");

    if (!fp) {
      printf("%s: No such file or directory.\n", argv[1]);
      exit(EXIT_FAILURE);
    }

    // Find first line (skip comments)
    while (fgets(line, 255, fp) && line[0] == 'c');

    model phi;

    sscanf(line, "p cnf %d %d", &phi.nbvars, &phi.nbclauses);

    phi.clauses = malloc(phi.nbclauses * sizeof(int*));
    phi.clause_lit_count = calloc(phi.nbclauses, sizeof(int));
    phi.lit_to_clauses = calloc(phi.nbvars, sizeof(int*));
    phi.lit_to_clauses_count = calloc(phi.nbvars, sizeof(int));
    phi.assignments = malloc(phi.nbvars * sizeof(char));

    //  Parsing ---------------------------------------------------------------
    for (int i = 0; i < phi.nbclauses; i++) {
      phi.clauses[i] = malloc(phi.nbvars * sizeof(int));

      fgets(line, 255, fp);
      int lit;

      char* atom = strtok(line, " ");
      while (atom != NULL) {
        sscanf(atom, "%d", &lit);
        if (!lit) break;

        if (abs(lit) > phi.nbvars) {
          printf("Invalid variable: %d\n", lit);
          exit(EXIT_FAILURE);
        }

        phi.clauses[i][phi.clause_lit_count[i]++] = lit;

        atom = strtok (NULL, " ");

        int lit_index = abs(lit) - 1;
        int sign = lit > 0 ? 1 : -1;

        if (!phi.lit_to_clauses[lit_index]) {
          phi.lit_to_clauses[lit_index] = malloc(phi.nbclauses * sizeof(int));
        }
        phi.lit_to_clauses[lit_index][phi.lit_to_clauses_count[lit_index]++] = (i + 1) * sign;
      }
    }

    fclose(fp);

    // Sort lit_to_clauses
    for (int i = 0; i < phi.nbvars; i++) {
      if (!phi.lit_to_clauses[i]) continue;
      qsort(phi.lit_to_clauses[i], phi.lit_to_clauses_count[i], sizeof(int), abscmpfunc);
    }

    // Sort literals in clauses
    for (int i = 0; i < phi.nbclauses; i++) {
      qsort(phi.clauses[i], phi.clause_lit_count[i], sizeof(int), abscmpfunc);
    }

    if (DPLL(phi)) {
      printf("SAT\n");
      for (int i = 0; i < phi.nbvars - 1; i++) {
        char ass = latest_assignment[i];
        if (!ass) ass = 1; // if unassigned, assign 1

        printf("%d ", (i + 1) * ass);
      }
      char ass = latest_assignment[phi.nbvars - 1];
      if (!ass) ass = 1; // if unassigned, assign 1
      printf("%d\n", phi.nbvars * ass);
    } else {
      printf("UNSAT\n");
    }

    exit(EXIT_SUCCESS);
}

model assign(var lit, char val, model phi) {
  int lit_index = abs(lit) - 1;

  phi.assignments[lit_index] = val;

  int* lit_to_clauses = phi.lit_to_clauses[lit_index];
  int clause_count = phi.lit_to_clauses_count[lit_index];
  for (int j = 0; j < clause_count; j++) {
    int clause = lit_to_clauses[j];
    int clause_index = abs(clause) - 1;

    if(phi.clauses[clause_index]) {
      if (clause * val > 0) { // true after assignment
        // Remove clause
        phi.clauses[clause_index] = TAG(phi.clauses[clause_index]);
      } else { // false after assignment
        remove_lit(phi, clause_index, lit);
      }
    }

  }

  return phi;
}

bool consistent(model phi) {
  for (int i = 0; i < phi.nbclauses; i++) {
    if (!(IS_TAGGED(phi.clauses[i]))) return false;
  }
  return true;

}

bool contains_empty(model phi) {
  for (int i = 0; i < phi.nbclauses; i++) {
    if (!(IS_TAGGED(phi.clauses[i])) && phi.clause_lit_count[i] == 0) return true;
  }

  return false;
}

var find_unit_literal(model phi) {
  for (int i = 0; i < phi.nbclauses; i++) {
    if (!(IS_TAGGED(phi.clauses[i])) && phi.clause_lit_count[i] == 1) {
      return phi.clauses[i][0];
    }
  }
  return 0;
}

var find_pure_literal(model phi) {
  for (int i = 0; i < phi.nbvars; i++) {
    if (phi.assignments[i]) continue; // already assigned var

    int sign = 0;
    bool pure = true;

    int clause_count = phi.lit_to_clauses_count[i];
    for (int j = 0; j < clause_count; j++) {
      if (IS_TAGGED(phi.clauses[abs(phi.lit_to_clauses[i][j]) - 1])) { // deleted clause
          continue;
      } else {
        if (!sign) {
          sign = phi.lit_to_clauses[i][j] > 0 ? 1 : -1;
        } else if (sign != (phi.lit_to_clauses[i][j] > 0 ? 1 : -1)) {
          pure = false;
          break;
        }
      }
    }

    if (pure && sign) {
      return (sign * (i + 1));
    }

  }
  return 0;
}

var choose_literal(model phi) {
  for (int i = 0; i < phi.nbclauses; i++) {
    if(!IS_TAGGED(phi.clauses[i])) {
      return abs(phi.clauses[i][0]);
    }
  }

  return 0;
}

//  Main DPLL algorithm -------------------------------------------------------
bool DPLL(model phi) {
  latest_assignment = phi.assignments;

  // BCP
  var unit_literal;
  while ((unit_literal = find_unit_literal(phi))) {
    assign(unit_literal, (unit_literal > 0 ? 1 : -1), phi);
  }

  // PLP
  var pure_literal;
  while ((pure_literal = find_pure_literal(phi))) {
    assign(pure_literal, (pure_literal > 0 ? 1 : -1), phi);
  }

  if (consistent(phi)) {
    return true;
  }

  if (contains_empty(phi)) {
    return false;
  }

  int l = choose_literal(phi);

  static model m;
  m = copy(phi, m, true);
  if (DPLL(assign(l, 1, m))) return true;
  return DPLL(assign(l, -1, copy(phi, m, false)));

}

model copy(model phi, model new, bool alloc) {
  new.nbvars = phi.nbvars;
  new.nbclauses = phi.nbclauses;

  if (alloc) {
    new.clauses = malloc(new.nbclauses * sizeof(int*));
    new.clause_lit_count = calloc(phi.nbclauses, sizeof(int));
    new.assignments = malloc(new.nbvars * sizeof(char));
    new.lit_to_clauses = calloc(new.nbvars, sizeof(int*));
    new.lit_to_clauses_count = calloc(new.nbvars, sizeof(int));
  }

  for (int i = 0; i < new.nbclauses; i++) {
    if (IS_TAGGED(phi.clauses[i])) {
      new.clauses[i] = phi.clauses[i];
      continue;
    }

    int lit_count = phi.clause_lit_count[i];
    new.clause_lit_count[i] = lit_count;

    if (alloc) {
      new.clauses[i] = malloc(lit_count * sizeof(int));
    } else {
      new.clauses[i] = UNTAG(new.clauses[i]);
    }

    memcpy(new.clauses[i], phi.clauses[i], lit_count * sizeof(int));
  }

  for (int i = 0; i < new.nbvars; i++) {
    int clause_count = phi.lit_to_clauses_count[i];

    if (alloc) {
      new.lit_to_clauses[i] = malloc(clause_count * sizeof(int));
    }

    memcpy(new.lit_to_clauses[i], phi.lit_to_clauses[i], clause_count * sizeof(int));
  }

  memcpy(new.lit_to_clauses_count, phi.lit_to_clauses_count, new.nbvars * sizeof(int));
  memcpy(new.assignments, phi.assignments, new.nbvars * sizeof(char));

  return new;
}

void remove_lit(model phi, int clause_index, var lit) {
  int* clause = phi.clauses[clause_index];
  int* clause_size = &phi.clause_lit_count[clause_index];

  int pos = binary_search(clause, *clause_size, lit);
  if (pos < 0) return;

  for (int i = pos + 1; i < *clause_size; i++) {
    clause[pos++] = clause[i];
  }

  (*clause_size)--;

}

int binary_search(int* arr, int size, int x) {
  int l = 0;
  int r = size;
  x = abs(x);

  while (l <= r) {
    int m = l + (r - l) / 2;

    if (abs(arr[m]) == x)
      return m;

    if (abs(arr[m]) < x) {
      l = m + 1;
    } else {
      r = m - 1;
    }
  }

  return -1;
}
