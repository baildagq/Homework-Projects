with open("../data/dataset/sina_news_utf8/2016-02.txt", "r") as source, open("../data/text.txt", "w") as output:
    line = source.readlines()
    start = 135
    for k in range(start, start + 3):
        txt = line[k].strip().split("。")
        for i in txt:
            j = i.strip().split("，")
            for h in j:
                output.write(h + "\n")

