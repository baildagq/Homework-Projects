#########################################################################
# File Name: valid.sh
# Author: stout
# mail: 2817253603@qq.com
# Created Time: Sun 21 Apr 2019 08:13:02 PM CST
#########################################################################
#!/bin/bash

python3 gen_text.py 
python3 gen_py.py
python3 main.py --input ../data/pinyin.txt --output ../data/ans.txt
python3 test.py
