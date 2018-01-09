import os
import random
import re
import string
import subprocess
import tempfile

from shutil import copy2

SAVED_INPUTS_PATH = "fuzzed-tests/"
saved_inputs_id = 0

REGEXES = {
    "INTMIN_NEGATED_REGEX": re.compile('^.*runtime.+negation'),
    "NULLPOINTER_REGEX": re.compile('^.*runtime.+null pointer'),
    "SHIFT_ERROR_REGEX": re.compile('^.*runtime.+shift'),
    "USE_AFTER_FREE_REGEX": re.compile('^==.*AddressSanitizer: heap-use-after-free'),
    "HEAP_BUFFER_OVERFLOW_REGEX": re.compile('^==.*AddressSanitizer: heap-buffer-overflow'),
    "STACK_BUFFER_OVERFLOW_REGEX": re.compile('^==.*AddressSanitizer: stack-buffer-overflow'),
    "SIGNED_INTEGER_OVERFLOW_REGEX": re.compile('^.*runtime.+signed integer')
}

INTERSTING_CNFS = ["p cnf 0 0",
                   "p cnf 10 20\n!\"#$%&'()*+,-./:;<=>?@[\]^_`{|}~",
                   "p cnf 10 20\n\n\n0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>?@[\]^_`{|}~ 	\n0\n00123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ!\"#$%&'()*+,-./:;<=>?@[\]^_`{|}~ 	\n",
                   "p cnf 10 20\n\n\n01234567890123456789"]

DIGITS_NO_ZERO = string.digits.replace("0", "")


def do_ub(sut_path, inputs_path, seed):
    if seed is not None:
        random.seed(seed)
    else:
        random.seed()

    i = 0

    while True:
        if i <= len(INTERSTING_CNFS) - 1:
            cnf = INTERSTING_CNFS[i]
            print("Sending interesting cnf")
        else:
            if i % 2 == 0:
                print("Sending input")
                cnf = create_input()
            else:
                print("Sending garbage")
                cnf = create_garbage()

        input_file = make_cnf_file(cnf)

        sut_output = run_sut(input_file, sut_path)

        if sut_output is not None:
            ubs = check_ub(sut_output)

            if ubs > 0:
                save_input(sut_path, input_file)

        input_file.close()

        i += 1


def create_input():
    number_of_formulas = 100000
    NUMBER_OF_LITERALS = 999
    formulas_width = 10

    cnf = "p cnf " + str(NUMBER_OF_LITERALS) + " " + str(number_of_formulas) + "\n999 0\n"
    for i in range(0, number_of_formulas):
        for j in range(0, formulas_width):
            cnf += ["", "-"][int(random.random() * 1)]
            for k in range(0, 3):
                cnf += DIGITS_NO_ZERO[int(random.random() * 9)]
            cnf += " "
        cnf += "0\n"

    return cnf


def create_garbage():
    cnf = "p cnf 10 20\n"

    while True:
        choice = random.randint(0, 5)
        if choice == 0:
            if len(cnf) >= 20:
                break
        elif choice == 1:
            cnf += string.punctuation
        elif choice == 2:
            cnf += string.printable
        elif choice == 3:
            cnf += string.digits
        elif choice == 4:
            cnf += '0'
        elif choice == 5:
            cnf += '\n'

    return cnf


def make_cnf_file(cnf):
    input_file = tempfile.NamedTemporaryFile(mode='w')
    input_file.write(cnf)
    input_file.flush()
    return input_file


def run_sut(input_file, sut_path):
    result = subprocess.Popen(["./runsat.sh", input_file.name], stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                              cwd=sut_path)

    try:
        sut_output, sut_error = result.communicate(timeout=20)
    except subprocess.TimeoutExpired:
        result.kill()
        sut_output, sut_error = result.communicate()


    sut_output_printable = sut_output.decode('ascii').split('\n')

    for line in sut_output_printable:
        print(line)

    return sut_error


def save_input(sut_path, temp_file):
    global saved_inputs_id

    os.makedirs(os.path.join(sut_path, SAVED_INPUTS_PATH), exist_ok=True)

    output_file = open(os.path.join(sut_path, SAVED_INPUTS_PATH, "interesting_input{}.txt".format(saved_inputs_id)),
                       'w+')

    copy2(temp_file.name, output_file.name)

    saved_inputs_id = (saved_inputs_id + 1) % 20


def check_ub(sut_error):
    ubs = 0
    for line in sut_error.decode('ascii ').split('\n'):
        for key, value in REGEXES.items():
            if value.match(line):
                print(line)
                ubs += 1

    return ubs
