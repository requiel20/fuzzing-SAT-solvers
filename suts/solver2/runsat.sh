#!/bin/bash
export UBSAN_OPTIONS=halt_on_error=false
export ASAN_OPTIONS=halt_on_error=false
./sat $1
