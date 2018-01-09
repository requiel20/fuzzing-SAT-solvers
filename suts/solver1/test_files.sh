#!/bin/bash

# Test that sat solver constructs formulas with respect to the cnf files.
REPORT_DIR='./tests/report'

mkdir -p $REPORT_DIR

if [ "$(ls -A $REPORT_DIR)" ]; then
    rm $REPORT_DIR/*
fi

for file in ./tests/*.cnf
do
    basename=$(basename $file)
    diff -B <( cat $file | sed 's/ 0//g' | sed '/^c/d' | sed 1d) <( ./sat $file ) > tests/report/$basename.diff
done
