py2ch = open("../model/pinyin2ch.txt")
allch = open("../model/all-ch.txt")
py2ch_dict = {}

for line in py2ch.readlines():
    _list = line.strip().split(" ")
    py2ch_dict[_list[0]] = _list[1:]


allc = set(allch.read().strip())

with open("../data/text.txt", "r") as inputfile:
    with open("../data/pinyin.txt", "w") as outputfile, open("../data/pure.txt", "w") as sourcefile:
        for line in inputfile.readlines():
            outline1 = ''
            outline2 = ''
            for i in line:
                if i in allc:
                    outline1 = outline1 + i

                    cnt = 0
                    for py in py2ch_dict.keys():
                        if cnt == 1:
                            continue
                        if i in py2ch_dict[py]:
                            outline2 = outline2 + " " + py
                            cnt = cnt  + 1

            sourcefile.write(outline1 + '\n')
            outputfile.write(outline2 + '\n')
