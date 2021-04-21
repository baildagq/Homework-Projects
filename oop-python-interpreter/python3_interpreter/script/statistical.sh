#!/bin/bash
find . | grep -E '(\.cc)' | awk '{system("ls -al " $1)}' | awk 'BEGIN{s=0}{s+=$5;print $9,$5,s}END{print s}'
find . | grep -E '(\.h)' | awk '{system("ls -al " $1)}' | awk 'BEGIN{s=0}{s+=$5;print $9,$5,s}END{print s}'
find . | grep -E '(python_ast.cc)|(graminit.cc)' | awk '{system("ls -al " $1)}' | awk 'BEGIN{s=0}{s+=$5;print $9,$5,s}END{print s}'
find . | grep -E '(python_ast.h)|(graminit.h)' | awk '{system("ls -al " $1)}' | awk 'BEGIN{s=0}{s+=$5;print $9,$5,s}END{print s}'
