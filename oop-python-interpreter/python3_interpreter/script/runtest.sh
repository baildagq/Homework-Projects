#!/bin/bash
[ -e src ] || exit 1
./script/gencode.sh || exit 1
bazel build main || exit 1
../bazel-bin/python3_interpreter/main ./test/testcode/test.py || exit 1
