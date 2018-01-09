#!/bin/bash

set -e
set -u

CURR_PATH=$(cd "$(dirname "$0")"; pwd -P)
TESTS_PATH="${CURR_PATH}/tests"
SAT_BIN="${CURR_PATH}/sat"
MODEL_CHECKER="${CURR_PATH}/satmodelvalidator.py"

if [[ ! -f ${SAT_BIN} ]]; then
    echo 'sat binary does not exist!'
    exit 1
fi

SCORE=0

for TESTFILE in $(echo ${TESTS_PATH}/*.cnf); do

    echo "--------------------------------------------"

    # Execute the sat solver by passing a test file of a sat formula
    RET=0
    OUTPUT=$(timeout -s 9 30 ${SAT_BIN} "${TESTFILE}") || RET=${?}
    if [[ ${RET} == 137 ]]; then
        echo "TIMEOUT: ${TESTFILE}"
        SCORE=$(( ${SCORE} + 0 ))
        continue
    elif [[ ${RET} != 0 ]]; then
        echo "CRASHED: ${TESTFILE}"
        SCORE=$(( ${SCORE} + 0 ))
        continue
    fi
    # RET=0
    # OUTPUT=$(timeout -s 9 30 minisat "${TESTFILE}" lala.txt) || RET=${?}
    # OUTPUT=$(cat lala.txt)

    RESULT=$(echo "${OUTPUT}" | head -n1)
    EXPECTED=$(head -n1 "${TESTFILE%.cnf}.result")

    # Check for any differences between the sat output and the expected result
    if [[ ${RESULT} != ${EXPECTED} ]]; then
        echo "FAILED: ${TESTFILE}"
        echo "Expected: ${EXPECTED}, got: ${RESULT}"
        if [[ ${EXPECTED} == "SAT\n" ]]; then
            SCORE=$(( ${SCORE} - 6 ))
        else
            SCORE=$(( ${SCORE} - 3 ))
        fi
    else
        # If the result is correct we need to check the model if the answer is SAT
        if [[ ${RESULT} == "SAT" ]]; then
            if [[ $(echo "${OUTPUT}" | wc -l) != 2 ]]; then
                echo "FAILED: ${TESTFILE}"
                echo "Missing Model"
                SCORE=$(( ${SCORE} + 1 ))
                continue
            fi

            MODEL=$(echo "${OUTPUT}" | tail -n1)
            MODEL_OUTPUT=""
            RET=0
            MODEL_OUTPUT=$(${MODEL_CHECKER} "${TESTFILE}" "${MODEL}") || RET=${?}
            if [[ ${RET} != 0 ]]; then
                echo "${MODEL_OUTPUT}"
                SCORE=$(( ${SCORE} + 1 ))
                continue
            fi
        fi

        echo "PASSED: ${TESTFILE}"
        SCORE=$(( ${SCORE} + 2 ))
    fi
done

echo "--------------------------------------------"
echo "Total Score: ${SCORE}"
