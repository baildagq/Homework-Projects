#!/bin/bash
[ -e src ] || exit 1
bazel build pgenmain || exit 1
../bazel-bin/python3_interpreter/pgenmain Grammar/Grammar ./include/pgen/graminit.h ./src/pgen/graminit.cc || exit 1
bazel build asdlmain || exit 1
../bazel-bin/python3_interpreter/asdlmain ./Grammar/Python.asdl ./include/python/python_ast.h ./src/python/python_ast.cc || exit 1
