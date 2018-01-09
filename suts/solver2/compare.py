from __future__ import print_function

import os
import subprocess
import sys


def compare(formula):
    # Run minisat
    try:
        output = subprocess.check_output('./minisat/minisat {}'.format(formula),
                                         shell=True)
    except subprocess.CalledProcessError as e:
        output = e.output

    if output.splitlines()[-1] == 'SATISFIABLE':
        minisat_result = 'SAT'
    else:
        minisat_result = 'UNSAT'

    # Run our SAT solver
    try:
        output = subprocess.check_output('./sat {}'.format(formula),
                                         shell=True)
    except subprocess.CalledProcessError as e:
        print(e)
        output = e.output

    our_result = output.splitlines()[0]

    # Check result
    if minisat_result == our_result:
        passed = 'OKAY'
    else:
        passed = 'FAIL'

    print('{} {}\n\tminisat: {}\n\tus: {}'.format(
        formula, passed, minisat_result, our_result)
    )


def is_formula(file):
    return file.split('.')[-1] == 'cnf'


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print('Usage: {} [formula | folder]'.format(sys.argv[0]))

        sys.exit(0)

    input = sys.argv[1]

    if is_formula(input):
        compare(input)
    else:
        files = os.listdir(input)

        for formula in filter(is_formula, files):
            path = os.path.join(input, formula)

            compare(path)
