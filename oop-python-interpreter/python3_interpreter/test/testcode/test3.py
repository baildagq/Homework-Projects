

def deepdiv(i, j):
    while (i % j == 0):
        i //= j
        print ('%d*' % j)
        if (i // j == 1):
            break
    if i % j == 0:
        print ('%d' % j)
        i //= j
    return i


def is_prime(num):
    j = 2
    flag = True
    while (j * j <= num):
        if num % j == 0:
            flag = False
            break
        j += 1
    if flag:
        print ("%d is a prime." % num)
        return True
    else:
        print ("%d is not a prime." % num)
        return False


def main():
    for i in range(100, 150):
        if is_prime(i):
            continue
        j = 2
        temp = i
        print ('%d = ' % temp)
        while (temp != 1):
            if (temp == j):
                print(j)
                break
            temp = deepdiv(temp, j)
            if temp == 1:
                break
            j += 1


if __name__ == "__main__":
    main()
