#!/bin/bash
#####################################
# Author: stout                     #
# mail: 2817253603@qq.com           #
# Created Time: 2019-12-13 11:15:48 #
#####################################

mkdir build
cd build
cmake .. -DBACKEND=Linux
make
