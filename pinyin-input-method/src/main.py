import argparse
import os
import sys
import getopt
import model
from common import config


def options():
    parser = argparse.ArgumentParser(description='Pinyin Input Method')
    parser.add_argument(
        '--input', default='../data/input.txt', type=str, help='inputfile')
    parser.add_argument(
        '--output', default='../data/output.txt', type=str, help='outputfile')
    parser.add_argument(
        '--data', default='../data/dataset/sina_news_utf8/2016-02.txt', type=str, help='datafile')
    parser.add_argument(
        '--train',action='store_true', default=False, help='whether train, default = false, action true')
    return parser.parse_args()


def main():
    args = options()

    if args.train == True:
        mdl = model.Model()
        mdl.train(args.data)
        mdl.train_3gram(args.data)
        mdl.save()
    else:
        if os.path.exists(config.model_path) == False or os.path.exists(config.dict_path) == False:
            print("Model has not been prepared, please train firstly, normally exit...")
            return
        mdl = model.Model()

        with open(args.input, "r") as inputfile:
            with open(args.output, "w") as outputfile:
                for seq, line in enumerate(inputfile.readlines()):
                    print("making ", seq)
                    py_list = line.lower().strip().split(" ")

                    # pre-process pinyin
                    for i in range(len(py_list)):
                        if py_list[i] == 'qv':
                            py_list[i] = 'qu'
                        elif py_list[i] == 'xv':
                            py_list[i] = 'xu'

                    ans = mdl.predict2(py_list)
                    outputfile.writelines(ans + '\n')

if __name__ == '__main__':
    sys.exit(main())
