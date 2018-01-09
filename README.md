# fuzzing-SAT-solvers
A fuzzer that generates inputs aimed to "break" SAT solvers. SAT solvers provided under the "suts" directory.

This black-box fuzzer has been implemented using the Python programming language. It can be run in two different modes: func and ub. In func mode the fuzzer tries to make the Systems Under Test (SUTs) produce wrong results, while in ub mode the fuzzer tries to make the SUTs exibhit undefined behaviour.
