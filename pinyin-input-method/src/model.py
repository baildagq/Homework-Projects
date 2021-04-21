import os
import json
import math
import datetime
import numpy as np
from common import config


class Model():
    def __init__(self):
        # py2ch  : dict according a py to find a ch_list
        # sub2ch : list according a sub to find a ch
        # ch2sub : dict according a ch to find a sub
        # gram2sub : dict according a 2-gram-ch to find a sub

        # c1 是每个字出现的频数矩阵 size = 6763 * 1
        # c2 是每个2元字出现的频数矩阵 size = 6763 * 6763
        # c3 是高频2元字 与 所有字 组合成的 3元字出现的频数矩阵
        self.sub2ch = self.make_sub2ch(config.all_ch_txt_path)
        if os.path.exists(config.model_path) == False or os.path.exists(config.dict_path) == False:
            print("making dict .....")
            self.py2ch = self.make_py2ch_dict(config.py2ch_txt_path)
            self.ch2sub = self.make_ch2sub_dict(self.sub2ch)
            self.gram2sub = {}
            print("making arrarys .....")
            self.c1 = np.zeros(config.ch_num, dtype='int32')
            self.c2 = np.zeros((config.ch_num, config.ch_num), dtype='int32')
            self.c3 = np.zeros((config.ch_num, config.ch_num), dtype='int32')
        else:
            self.load()

    def load(self):
        print("loading dict.....")
        with open(config.dict_path, "r") as f:
            line = f.readlines()
            self.py2ch = json.loads(line[0])
            self.ch2sub = json.loads(line[1])
            self.gram2sub = json.loads(line[2])
        print("loading arrays.....")
        arrays = np.load(config.model_path)
        self.c1 = arrays["c1"]
        self.c2 = arrays["c2"]
        self.c3 = arrays["c3"]

    def save(self):
        print(f"saving gram2sub dict to:{config.dict_path}....")
        with open(config.dict_path, "w") as f:
            f.write(json.dumps(self.py2ch) + "\n")
            f.write(json.dumps(self.ch2sub) + "\n")
            f.write(json.dumps(self.gram2sub) + "\n")
        print(f"saveing model to: {config.model_path}....")
        np.savez(config.model_path, c1=self.c1, c2=self.c2, c3=self.c3)

    def make_py2ch_dict(self, datapath):
        py2ch_dict = {}
        with open(datapath, "r", encoding="utf-8") as f:
            for line in f.readlines():
                _list = line.strip().split(" ")
                py2ch_dict[_list[0]] = _list[1:]
        return py2ch_dict

    def make_sub2ch(self, datapath):
        _list = []
        with open(datapath, "r") as f:
            text = f.read().rstrip()
            for ch in text:
                _list.append(ch)
        return _list

    def make_ch2sub_dict(self, sub2ch):
        ch2sub_dict = {}
        ch2sub_dict = {x: i for i, x in enumerate(sub2ch)}
        return ch2sub_dict

    def takesecond(self, em):
        return em[0]

    def make_gram2sub_dict(self):
        print(f"makeing high 2gram to sub dict ....")
        flat_c2 = []
        for i in range(config.ch_num):
            for j in range(config.ch_num):
                flat_c2.append([self.c2[i][j], i, j])
        print("sorting flat_c2 ... ")
        flat_c2.sort(key=self.takesecond, reverse=True)

        for i in range(config.ch_num):
            self.gram2sub[self.sub2ch[flat_c2[i][1]] +
                          self.sub2ch[flat_c2[i][2]]] = i

    def train(self, datapath):
        print(f"training dataset: {datapath} ....")
        # # for rzd class provided json file
        with open(datapath, "r") as f:
            for line in f.readlines():
                data = json.loads(line)
                for text in [data['html'], data['title']]:
                    len_text = len(text)
                    for i in range(len_text):
                        # 1-gram
                        if i < len_text and text[i] in self.ch2sub.keys():
                            a = self.ch2sub[text[i]]
                            self.c1[a] = self.c1[a] + 1
                            # 2-gram
                            if i + 1 < len_text and text[i + 1] in self.ch2sub.keys():
                                b = self.ch2sub[text[i + 1]]
                                self.c2[a][b] = self.c2[a][b] + 1

    def train_3gram(self, datapath):
        self.make_gram2sub_dict()
        print(f"training 3-gram dataset: {datapath} ....")

        # for rzd class provided json file
        with open(datapath, "r") as f:
            for line in f.readlines():
                data = json.loads(line)
                for text in [data['html'], data['title']]:
                    len_text = len(text)
                    for i in range(len_text):
                        if i + 2 < len_text:
                            if text[i] + text[i + 1] in self.gram2sub.keys():
                                sub1 = self.gram2sub[text[i] + text[i + 1]]
                                if text[i + 2] in self.ch2sub.keys():
                                    sub2 = self.ch2sub[text[i + 2]]
                                    self.c3[sub1][sub2] = self.c3[sub1][sub2] + 1

    # predict part
    def make_ch_array(self, py_list):
        ch_array = []
        for i in range(len(py_list)):
            ch_array.append(self.py2ch[py_list[i]])
        return ch_array

    def predict(self, py_list):
        tmp_total_num = 7000
        ch_array = self.make_ch_array(py_list)
        prob_array = []  # 记录到各层各个节点处的最大概率
        ans_array = []  # 记录到达每层每个节点处，此时的字符串
        l = len(py_list)
        for i in range(l):
            prob_list = []
            ans_list = []
            if i == 0:
                for j in range(len(ch_array[i])):
                    num = self.ch2sub[ch_array[i][j]]
                    prob_list.append(+ math.log(self.c1[num] +
                                                1) - math.log(tmp_total_num))
                    ans_list.append(ch_array[i][j])
            else:
                for j in range(len(ch_array[i])):
                    prob = 0
                    pre_seq = 0
                    # 对第i层第j个汉字，遍历与上层每个汉字的组合
                    for k in range(len(ch_array[i - 1])):
                        # 查询c2 ，找到上层第k个汉字与本层第j个汉字2元组的频数
                        a = self.ch2sub[ch_array[i - 1][k]]
                        b = self.ch2sub[ch_array[i][j]]
                        c1_prob = self.c1[a]
                        c2_prob = self.c2[a, b]

                        # current node prob
                        alpha = 0.0
                        tmp_prob = prob_array[i - 1][k] + math.log(
                            c2_prob + c1_prob + 1) - math.log(c1_prob + tmp_total_num)

                        if prob < tmp_prob:
                            prob = tmp_prob
                            pre_seq = k

                    prob_list.append(prob)
                    ans_list.append(ans_array[i - 1][pre_seq] + ch_array[i][j])
            prob_array.append(prob_list)
            ans_array.append(ans_list)

        ch = ans_array[l - 1][prob_array[l - 1].index(max(prob_array[l - 1]))]

        return ch

    def predict2(self, py_list):
        ch_array = self.make_ch_array(py_list)
        prob_array = []  # 记录到各层各个节点处的最大概率
        prob_array.append(None)
        ans_array = []  # 记录到达每层每个节点处，此时的字符串

        ans_array.append(ch_array[0])

        length = len(py_list)

        if length == 1:
            _list = self.py2ch[py_list[0]]
            _list1 = [self.c1[self.ch2sub[ch]] for ch in _list]
            ch = self.num2ch[_list1.index(max(_list1))]
            return

        for i in range(1, length):
            prob_list = []
            ans_list = []
            pre_list = []

            if i == 1:
                for j in range(len(ch_array[i])):
                    prob_l = []
                    ans_l = []
                    for k in range(len(ch_array[i - 1])):
                        sub1 = self.ch2sub[ch_array[i - 1][k]]
                        sub2 = self.ch2sub[ch_array[i][j]]
                        cur_prob = math.log(
                            self.c2[sub1][sub2] + 1) - math.log(7000)
                        prob_l.append(cur_prob)
                        ans_l.append(ch_array[i - 1][k] + ch_array[i][j])
                        
                        # print(self.sub2ch[sub1], self.sub2ch[sub2], cur_prob)

                    prob_list.append(prob_l)
                    ans_list.append(ans_l)

            else:
                for j in range(len(ch_array[i])):
                    bef_1 = 0
                    bef_2 = 0

                    prob_l = []
                    ans_l = []

                    for k in range(len(ch_array[i - 1])):
                        prob = -1000000
                        for l in range(len(ch_array[i - 2])):
                            if ch_array[i - 2][l] + ch_array[i - 1][k] in self.gram2sub.keys():
                                c = self.ch2sub[ch_array[i][j]]
                                b = self.ch2sub[ch_array[i - 1][k]]
                                a = self.ch2sub[ch_array[i - 2][l]]
                                c2_prob = self.c2[a][b]

                                sub = self.gram2sub[ch_array[i - 2]
                                                    [l] + ch_array[i - 1][k]]
                                c3_prob = self.c3[sub][c]

                                cur_prob = prob_array[i - 1][k][l] + math.log(
                                    c3_prob + 1) - math.log(c2_prob + 7000)
                            else:
                                c = self.ch2sub[ch_array[i][j]]
                                b = self.ch2sub[ch_array[i - 1][k]]
                                a = self.ch2sub[ch_array[i - 2][l]]
                                c2_prob = self.c2[b][c]
                                c1_prob = self.c1[b]
                                cur_prob = prob_array[i - 1][k][l] + math.log(
                                    c2_prob + 1) - math.log(c1_prob + 7000) - 0.9
                            if prob < cur_prob:
                                prob = cur_prob
                                bef_1 = k
                                bef_2 = l

                        prob_l.append(prob)
                        ans_l.append(
                            ans_array[i - 1][k][bef_2] + ch_array[i][j])

                    prob_list.append(prob_l)
                    ans_list.append(ans_l)

            prob_array.append(prob_list)
            ans_array.append(ans_list)

        ch = max(((x1, x2) for l1, l2 in zip(ans_array[-1], prob_array[-1])
                  for x1, x2 in zip(l1, l2)),
                 key=lambda x: x[1])[0]

        return ch
