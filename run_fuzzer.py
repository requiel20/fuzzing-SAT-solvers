import subprocess
import sys
from glob import glob

import func
import ub

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print('usage: run_fuzzer ub|func')
        exit(1)

    mode = sys.argv[1]
    input_paths = None
    run_fuzzer_func = None

    if mode == 'ub':
        run_fuzzer_func = ub.do_ub
        input_paths = '.'
    elif mode == 'func':
        run_fuzzer_func = func.do_func
        input_paths = 'inputs/'
    else:
        print('usage: run_fuzzer ub|func')
        exit(1)

    sut_paths = glob('suts/*')

    for path in sut_paths:
        result = subprocess.Popen(['make', mode], cwd=path)

    for path in sut_paths:
        print("running on " + path)
        run_fuzzer_func(path, input_paths, None)

    for path in sut_paths:
        result = subprocess.Popen(['make', 'clean'], cwd=path)
