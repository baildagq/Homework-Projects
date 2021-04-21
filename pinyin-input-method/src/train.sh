#########################################################################
# File Name: train.sh
# Author: stout
# mail: 2817253603@qq.com
# Created Time: Thu 18 Apr 2019 10:44:00 PM CST
#########################################################################
#!/bin/bash


path=$1
files=$(ls $path)
for filename in $files
do
    python3 main.py --train --data ../data/dataset/sina_news_utf8/$filename
done

