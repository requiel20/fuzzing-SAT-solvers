#!/usr/bin/python3

import argparse
import collections
import glob
import os
import subprocess
import textwrap

################################################################################
# Argument parsing
################################################################################

parser = argparse.ArgumentParser(formatter_class=argparse.RawDescriptionHelpFormatter, description=textwrap.dedent("""\
This utility is provided to help you with the func mode of the fuzzer. It
should be called as following:

    $ ./sat-follow-up.py /path/to/SUT /path/to/inputs path/to/follow-up-tests

It will output the following:
    * Total achieved coverage, provided that the SUT binary is compiled with
    gcov enabled (i.e. "-ftest-coverage -fprofile-arcs" arguments passed
    to gcc)
    * Tests that yielded a non-zero return code
    * Tests that did not yield a correct answer, as provided by the associated
    expectations file
    * Follow-up tests which are missing expectation files
    * Follow-up tests which output something other than "SAT" or "UNSAT"
    * Input tests for which the number of provided follow-up tests is different
    that specified in the spec

In addition, the optional parameter `--emit_log` can be passed to the script
with a file path as value. A log of all the collected information will be
saved in that file. The same, and even more information will be printed to
standard output.

This scripts expects that the binaries of the SUTs provided are compiled with
gcov coverage enabled (i.e. `-fprofile-arcs -ftest-coverage`).

For any issues with the script, please email the course assistants:
    * Anastasios Andronidis -- a.andronidis15@imperial.ac.uk
    * Andrei Lascu -- andrei.lascu10@imperial.ac.uk
    
"""
))
parser.add_argument("SUT_path",  type=str,
    help="Path to binary containing the system under test.")
parser.add_argument("inputs_path", type=str,
    help="Path to folder containing input DIMACS-format tests.")
parser.add_argument("follow_up_tests_path", type=str,
    help="Path to folder containing follow-up tests generated from the input\
    tests.")
parser.add_argument("--emit_log", type=str, default="",
    help="If set, will print information to file given by passed value.")
args = parser.parse_args()

################################################################################
# Helper functions
################################################################################

def CheckSAT(in_string):
    if "UNSAT" in in_string:
        return "UNSAT"
    if "SAT" in in_string:
        return "SAT"
    return "INVALID"

def GetExpectedSAT(init_sat, fup_test):
    fup_exp_file = os.path.splitext(fup_test)[0] + ".txt"
    if not os.path.isfile(fup_exp_file):
        print("%s: Could not find corresponding expectation file." %
            (fup_test))
        return None
    with open(fup_exp_file, 'r') as fup_exp_file_fd:
        for line in fup_exp_file_fd:
            if line.startswith(init_sat):
                return line.split("->")[1].strip()
    return None

################################################################################
# Main
################################################################################

follow_up_tests_count = 50
logged_info = collections.OrderedDict()
logged_info["crash"] = {"desc": "Tests for which SUT had abnormal termination:"}
logged_info["wrong_count"] = {"desc": "Incorrect number of follow-up tests:"}
logged_info["miss_xpect"] = {"desc": "Follow-up tests with missing expectation files"}
logged_info["wrong_out"] = {"desc": "Follow-up tests giving an unexpected output:"}
logged_info["wrong_sat"] = {"desc": "Follow-up tests with satisfiability different than set in expectation file:"}
logged_info["cov"] = {"desc": "Total coverage:"}
for key in logged_info.keys():
    logged_info[key]["content"] = []

def LogInfo(category, info):
    logged_info[category]["content"].append(info)

input_tests = glob.glob(args.inputs_path + "/*.cnf")
SUT_folder = os.path.dirname(args.SUT_path)
SUT_bin = "./" + os.path.basename(args.SUT_path)
for gcda in glob.glob(os.path.dirname(args.SUT_path) + "*.gcda"):
    os.remove(gcda)
for input_test in input_tests:
    test_name = os.path.splitext(os.path.basename(input_test))[0]
    initial_test_proc = subprocess.Popen(["./" + SUT_bin, os.path.abspath(input_test)],\
                            stdout = subprocess.PIPE,
                            stderr = subprocess.PIPE,
                            cwd = SUT_folder)
    initial_test_proc_output = initial_test_proc.communicate()
    if initial_test_proc.returncode != 0:
        print("%s: Abnormal termination for initial test (return code %d):" %
            (input_test, initial_test_proc.returncode))
        print(initial_test_proc_output[1].decode("utf-8"))
        LogInfo("crash", (input_test, "return code " + str(initial_test_proc.returncode)))
        continue
    initial_test_sat = CheckSAT(initial_test_proc_output[0].decode("utf-8"))
    if not initial_test_sat:
        print("%s: Unexpected output for initial test (%s)." %
            (input_test, initial_test_proc_output[0].decode("utf-8")))
        continue

    follow_up_tests = glob.glob(args.follow_up_tests_path + "/" \
                                + test_name + "*.cnf")
    if not len(follow_up_tests) == follow_up_tests_count:
        print("%s: Found %d variant files, expected %d." %
            (input_test, len(follow_up_tests), follow_up_tests_count))
        LogInfo("wrong_count", (input_test, \
            "found %d, expected %d" % \
            (len(follow_up_tests), follow_up_tests_count)))
        continue
    for fup_test in follow_up_tests:
        fup_test_exp_sat = GetExpectedSAT(initial_test_sat, fup_test)
        if not fup_test_exp_sat:
            print("%s: Could not find corresponding expectation for %s." %
                (fup_test, initial_test_sat))
            LogInfo("miss_xpect", (fup_test, "missing expectation for " + initial_test_sat))
            continue
        fup_test_proc = subprocess.Popen([SUT_bin, os.path.abspath(fup_test)],
                            stdout = subprocess.PIPE,
                            stderr = subprocess.PIPE,
                            cwd = SUT_folder)
        fup_test_proc_output = fup_test_proc.communicate()
        if fup_test_proc.returncode != 0:
            print("%s: Abnormal termination (return code %d):" % \
                (fup_test, fup_test_proc.returncode))
            print(fup_test_proc_output[1].decode("utf-8"))
            LogInfo("crash", (fup_test, "return code " + str(fup_test_proc.returncode)))
        fup_test_proc_stdout = fup_test_proc_output[0].decode("utf-8")
        fup_test_sat = CheckSAT(fup_test_proc_stdout)
        if not fup_test_sat:
            print("%s: Unexpected output from SUT (%s)." % \
                (fup_test, fup_test_proc_stdout))
            LogInfo("wrong_out", (fup_test, "got " + fup_test_proc_stdout))
        elif not fup_test_sat == fup_test_exp_sat:
            print("%s: Found %s, expected %s." % \
                (fup_test, fup_test_sat, fup_test_exp_sat))
            LogInfo("wrong_sat", (fup_test, "got %s, expected %s" % (fup_test_sat, fup_test_exp_sat)))

gcov_proc = subprocess.Popen(["gcov --no-output " + os.path.dirname(args.SUT_path) + "/*.c"],
                                shell=True,
                                stdout = subprocess.PIPE,
                                stderr = subprocess.PIPE)
gcov_proc_out = gcov_proc.communicate()
gcov_proc_stdout = [x for x in gcov_proc_out[0].decode("utf-8").split("\n") if x != ""]
if not gcov_proc_stdout:
    print("Could not find required gcov files to compute coverage; ensure the SUT binary path points to the same folder where it was compiled in.")
    LogInfo("cov", ("Ensure that SUT_path points to location where SUT binary was compiled", ""))
else:
    total_coverage = gcov_proc_stdout[len(gcov_proc_stdout) - 1]
    LogInfo("cov", (gcov_proc_out[0].decode("utf-8").replace("\n", "\n\t"), ""))
    print(total_coverage)

if args.emit_log != "":
    with open(args.emit_log, 'w') as log_writer:
        for key in logged_info.keys():
            log_writer.write(logged_info[key]["desc"] + "\n")
            for content in logged_info[key]["content"]:
                log_writer.write("\t" + content[0])
                if key != "cov":
                    log_writer.write(": " + content[1])
                log_writer.write("\n")
            log_writer.write(80 * "=" + "\n")
