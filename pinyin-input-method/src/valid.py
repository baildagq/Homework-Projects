line_total = 0
line_right = 0
word_total = 0
word_right = 0

with open("../data/pure.txt") as real, open("../data/ans.txt") as myans:
    for l1, l2 in zip(real, myans):
        l1 = l1.strip()
        l2 = l2.strip()
        
        line_total += 1
        if all(x == y for x, y in zip(l1, l2)):
            line_right += 1

        word_total += len(l1)
        word_right += sum(x==y for x, y in zip(l1, l2))


print("line total: ", line_total, "line ac: ", line_right / line_total)
print("word total: ", word_total, "word ac: ", word_right / word_total)
