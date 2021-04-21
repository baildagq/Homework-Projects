def F(n):
    print('call F',n)
    return n
x=5
def f(n):
    i=2
    def f(n):
        g=1
        def g():
            print('call g')
            print('i =',i)
            print('x =',x)
            return F(n)
        t=g
        g='666'
        return t()
    print('main n:',None)
    print('f(n):',f(n))
    i+=1
    print('f(n):',f(n))
print('__name__:',__name__)
if __name__=='__main__':
    f(None)
    f(False)


