#!/bin/bash
#########################################################################
# File Name: bat.sh
# Author: stout
# mail: 2817253603@qq.com
# Created Time: Mon 13 May 2019 09:25:51 PM DST
#########################################################################


for ((i = 100; i >= 2; i -= 2)); do
    ../Compete/bin/Compete.exe ../TestCases/${i}.dll ./Release/Strategy.dll ./result/result${i}.txt 5
done
