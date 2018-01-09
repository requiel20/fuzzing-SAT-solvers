import random
import os


FOLLOW_UP_TESTS = 'follow-up-tests'
SS_UU = 'SAT->SAT\nUNSAT->UNSAT\n'
SS_UUNKOWN = 'SAT->SAT\nUNSAT->UNKNOWN\n'
SUNKNOWN_UU = 'SAT->UNKNOWN\nUNSAT->UNSAT\n'


def swap_literals(formula):
    result = []

    for clause in formula:
        copy = list(clause)

        del copy[-1]
        random.shuffle(copy)
        copy.append(0)

        result.append(copy)

    return result


def add_clauses(no_of_vars, no_of_clauses, formula):
    result = []

    for clause in formula:
        copy = list(clause)
        result.append(copy)

    no_of_new_clauses = random.randint(0, no_of_clauses)

    for i in range(0, no_of_new_clauses):
        new_clause = []

        # Generate clause of size between 1 to 10
        for j in range(0, random.randint(1, 10)):
            new_clause.append(random.randint(1, no_of_vars))

        new_clause.append(0)
        result.append(new_clause)

    return result, no_of_new_clauses


def delete_clauses(no_of_clauses, formula):
    no_of_deleted_clauses = random.randint(0, no_of_clauses)

    result = list(formula)[0:no_of_clauses - no_of_deleted_clauses]

    return result, no_of_deleted_clauses


def swap_clauses(formula):
    copy = list(formula)
    random.shuffle(copy)

    return copy


def transform(formula, no_of_clauses, no_of_vars):
    # Apply a set of transformations based on these rules:
    # 0 - 4 Swap clauses (1)
    # 5 - 9 Swap literals inside a clause (2)
    # 10 - 14 Add clauses (3)
    # 15 - 19 Remove Clauses (4)
    # 20 - 24 (1+2)
    # 25 - 29 (1+3)
    # 30 - 34 (1+4)
    # 35 - 39 (2+3)
    # 40 - 44 (1+2+3)
    # 45 - 49 (1+2+4)

    # result is of the form: [(formula, no_of_clauses, sat_string)]
    result = []

    for i in range(0, 5):
        new_formula = (swap_clauses(formula), no_of_clauses, SS_UU)

        result.append(new_formula)

    for i in range(5, 10):
        new_formula = (swap_literals(formula), no_of_clauses, SS_UU)

        result.append(new_formula)

    for i in range(10, 15):
        new_formula, no_of_added_clauses = add_clauses(no_of_vars, no_of_clauses, formula)

        result.append((new_formula, no_of_added_clauses + no_of_clauses, SUNKNOWN_UU))

    for i in range(15, 20):
        new_formula, no_of_deleted_clauses = delete_clauses(no_of_clauses, formula)

        result.append((new_formula, no_of_clauses - no_of_deleted_clauses, SS_UUNKOWN))

    for i in range(20, 25):
        new_formula = swap_literals(swap_clauses(formula))

        result.append((new_formula, no_of_clauses, SS_UU))

    for i in range(25, 30):
        new_formula, no_of_added_clauses = add_clauses(no_of_vars, no_of_clauses, swap_clauses(formula))

        result.append((new_formula, no_of_clauses + no_of_added_clauses, SUNKNOWN_UU))

    for i in range(30, 35):
        new_formula, no_of_deleted_clauses = delete_clauses(no_of_clauses, swap_clauses(formula))

        result.append((new_formula, no_of_clauses - no_of_deleted_clauses, SS_UUNKOWN))

    for i in range(35, 40):
        new_formula, no_of_added_clauses = add_clauses(no_of_vars, no_of_clauses, swap_literals(formula))

        result.append((new_formula, no_of_clauses + no_of_added_clauses, SUNKNOWN_UU))

    for i in range(40, 45):
        new_formula, no_of_added_clauses = add_clauses(no_of_vars, no_of_clauses, swap_literals(swap_clauses(formula)))

        result.append((new_formula, no_of_clauses + no_of_added_clauses, SUNKNOWN_UU))

    for i in range(45, 50):
        new_formula, no_of_deleted_clauses = delete_clauses(no_of_clauses, swap_literals(swap_clauses(formula)))

        result.append((new_formula, no_of_clauses - no_of_deleted_clauses, SS_UUNKOWN))

    return result


def do_func(sut_path, inputs_path, seed):
    if seed is not None:
        random.seed(seed)
    else:
        random.seed()

    for x in range(1, 21):
        with open(os.path.join(inputs_path, str(x).zfill(2) + '.cnf')) as formula:
            first_line = formula.readline()

            p, cnf, no_of_vars, no_of_clauses = first_line.strip().split()

            clauses = []

            for line in formula:
                clauses.append(line.split())

        transformations = transform(clauses, int(no_of_clauses), int(no_of_vars))

        if not os.path.exists(FOLLOW_UP_TESTS):
            os.makedirs(FOLLOW_UP_TESTS)

        for i, transformation in enumerate(transformations):
            print('Formula {}: Writing transformation {}'.format(x, i))

            cnf_filename = os.path.join(FOLLOW_UP_TESTS, '{}_{}.cnf'.format(str(x).zfill(2), str(i).zfill(2)))
            txt_filename = os.path.join(FOLLOW_UP_TESTS, '{}_{}.txt'.format(str(x).zfill(2), str(i).zfill(2)))

            with open(cnf_filename, 'w') as new_cnf, open(txt_filename, 'w') as new_txt:
                formula, new_no_of_clauses, sat_string = transformation

                # Write first line
                first_line = 'p cnf {} {}\n'.format(no_of_vars, new_no_of_clauses)
                new_cnf.write(first_line)

                # Write clauses
                for line in formula:
                    new_line = ' '.join(map(lambda lit: str(lit), line)) + '\n'
                    new_cnf.write(new_line)

                # Write sat_string
                new_txt.write(sat_string)
