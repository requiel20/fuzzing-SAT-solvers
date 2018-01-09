#!/usr/bin/env python

import sys

# Get the file name and read the last arguments as the sat model.
# Users might invoke this script as this:
#   satmodelvalidator.py file 1 2 3
#   satmodelvalidator.py file "1 2 3"
# We parse can both.
satfile = sys.argv[1]
modelstr = str.join(' ', sys.argv[2:]).split(' ')  # Parse model
# Just in case someone leaves a 0 in the end
if modelstr[-1] == "0":
    modelstr = modelstr[:-1]
model = [int(i) > 0 for i in modelstr]  # Translate numbers into boolean

with open(satfile) as f:
    content = f.readlines()

# Remove whitespace characters like `\n` at the end of each line
for linenum, line in enumerate([x.strip() for x in content]):
    if line.startswith('p'):
        varnum = int(line.strip().split(' ')[2])
        if varnum != len(model):
            print("MODEL FAILED: file: {}".format(satfile))
            print("Model dictates {} variables. Found: {}".format(varnum, len(model)))
            sys.exit(1)
        continue
    if line.startswith('c'):
        continue

    line = [int(it) for it in line.split(' ')[:-1]]  # Remove last 0 and cast to int

    # For every var index in line, get its model value from 'model'. If the var
    # index is negative, NOT the 'model' value.
    bools = [model[abs(var) - 1] if var > 0 else not model[abs(var) - 1] for var in line]

    if any(bools) is False:  # OR all variables in a line
        print("MODEL FAILED: file: {}\nline {}: {}".format(satfile, linenum + 1, line))
        sys.exit(1)

print("MODEL OK")
sys.exit(0)
