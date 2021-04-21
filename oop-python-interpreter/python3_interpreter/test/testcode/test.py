# trivial operator
x=233
a = 2018
b = 1951
c = 21
d = 60
e = True
f = False
g = "Tsinghua"
h = "???"
print(e,a,b,c,f)
print (a // c - (b // d * c + 5 * d) * (90 - d))
print (-5 + 7 * 90 // 6 // 5 * 3 + 100 * (-5))
print ((c >= d) and (a > b) or (a < 7) and (b < 9))
print (e and f or (a < b)) 
print (e and f or e) 
print (1 << 2 + c >> 1 * 2 - 6 // (-3))
print (9 & 8 - 6 | 7 * 2 + 9)

PI = 3

# while loop

x=1231273

while x>1:
    print(x)
    if x%2==0:
        x//=2
    else:
        x=x*3+1
else:
    print(x,x,x,x,x*10)

# string add

s=''
n=1
while n<=6:
    s=s+'6'
    s+='~6~'
    n+=1
print('str:',s)

# if

if x>=5:
    print(x)
elif x<2:
    print(x,x,x)
else:
    print('666',x)

# trivial function

def is_prime(n):
    '''print('n:',n)'''
    i=2
    while i<n:
        '''print('test:',i,n)'''
        if n%i==0:
            return False
        i+=1
    return True

# function with closure

def factor(n):
    i=2
    def f(n):
        t=1 # cell
        def t():
            if i<=4: # free
                print('call f',i)
                print('show x',x)
            return is_prime(n)
        alias=t
        t=666
        return alias()
    #print('factor')
    print(n,'=')
    while n>1:
        if f(i):
            c=0
            while n%i==0:
                n//=i
                c+=1
            if c>0:
                print(i,'**',c)
        i+=1

# recursion with closure

def R(n):
    if n==0:
        return 1
    i=1
    def g():
        print('call g','n:',n,'i:',i)
    g()
    i+=1
    g()
    return n*R(n-1)

def main():
    y=3
    print(y)
    print(2)
    i=2
    while i<=100:
        '''print('i:',i,is_prime(i))'''
        if is_prime(i):
            print(i,'is a prime')
        factor(i)
        i+=1
    print(R(5))

# fake __name__
if __name__ == "__main__":
    print(__name__)
    print(13+123)
    print(main())
    i=0

    # flow stmt (break & continue)

    def f():
        print('call f',i)
        if i%2==0:
            return 'even'
        for j in range(i+1):
            if j==21 and i<=42 and i%j==0:
                print('fun',i,j)
                return j
        return 'odd'
    i=0
    while i<100:
        if i%14==0:
            print('while continue')
            i+=7
            continue
        print(f())
        if i==49:
            print('while break')
            break
        i+=7

    for i in range(0,100,7):
        if i%14==0:
            print('for continue')
            continue
        print(f())
        if i==49:
            print('for break')
            break

    # __add__(only for extensibility, incomplete, __sub__ not exist)

    print(233,int.__add__(1,2))
    t=1
    print(t.__add__(2))

    # iterable object

    a,b=b,c,=1,2
    print(a,b+123)

# decorator like

def wrapper(f):
    def _f():
        print('begin')
        f()
        print('end')
    return _f

def wrapper_test():
    print('wrapper_test')
wrapper_test=wrapper(wrapper_test)

wrapper_test()

# class (beta)

def f():
    i=0
    class A(object):
        a=1
        def __call__(self):
            self.t=233
            print('A __call__',i)
        def __init_(self):
            print('A __init__','i:')
    class B():
        def f(self):
            print(i)
            print('B f()',i)
        def __call__(self):
            self.t='B'
            print('B __call__',i)
    print(A.__base__)
    print(B.__base__)
    class CLS(A,B):# inheritance
        def __init_(self):
            print('CLS __init__','i:',i)
    print(CLS.__bases__)
    i=666
    a=CLS()
    i+=1
    a()
    print(a.t)
    a.f()
    i-=1
    print(a.a)
    a.a=a.f
    a.a()
    a.c=123
    print(a.c)
    a.t=a.c
    b=a
    b.t=1
    a.cc=a
    a.cc.d=1
    print('t:',a.t,'c:',a.c,'d:',a.d)
    a.d=CLS()
    a.d()
    print(a.d.t)
    print('type a: %s type CLS: %s'%(type(a),type(CLS)))
f()
print(f)


# for iterable object

l=(1,2,3,4,1,2,1)

for t in l:
    print(t)

for i in range(len(l)):
    print('%s[%d]'%('l',i),l[i])
for i in range(4,0,-2):
    print('%s->[%d]'%('l',i),l[i])

for t in 'show':
    print(t)



# tuple.count, formatter of str

i=1
s='123'
print('count %d: in %s'%(i,l),l.count(i))
print('count %s:'%f,l.count(f))
print('count %s:'%s,l.count(s))

# slice

print('''12351'''[5::-1])
print("""151123"""[-3:-2:1])
print("12315"[1:6:])
print('121513'[:])
print('1251153'[2:5:])
print('12123'[-3::])

# global & nonlocal
def f():
    global i
    i,t=2,3
    ff=None
    def g():
        nonlocal t,ff
        t=4
        ff=(2,3,3)
    print(i)
    g()
    print(t)
    print(ff)
print(i)
f()
print(i)


# list

x=(1,2,3,4,5,6,7)
x=x[::2]
print(x)

x=[int(1),str(2),bool(3),4]
print(x)
for i in x:
    print(i)
print(x)
x[1]=t
x.append(x) # fust4fun
x.append('312')
x.append(f)
print(x)

i=0
for item in x:
    print(item if i%2==1 else '%d-%d'%(i,i)) # ifexpr
    i+=1

# list comprehension

a=None
a=[[(i,j) for j in range(3)] for i in range(5)]
print(a)
a=[(i,j) for i in range(10) if i%3==0 for j in range(5) if j%4!=2]
print(a)
print([(list(range(-5,l))+list(range(r,5)))*2 for r in range(1,6,2) for l in range(5,-5,-3)])


# dict

a={13142:1,'True':23,True:'131',(123):int,(51,52):True}
a[0]=1
a['1']=f
print(a[0],a['1'])
a[0]=2
print(a[0],a['1'])
print(a)
print(a.keys()) # return tuple, not dict_keys
for i in a.keys():
    print('%s:%s'%(i,a[i]))
for i,j in a.items():
    print('items:',i,j)

t='12341'

# bulitin type

print(int)
print(str)
print(object)
print(tuple)
print(list)
print(bool)
print(type(1))
print(type(int))
print(type(object))
print(type(type)) # not real type


# member function of list

x=[4,'123',3,6,f]
for i in x:
    print(i)

y=x.copy()
y.remove(3)
print(x)
x.clear()
for i in y:
    print(i)
print(x)


#generator
print('~~~~~')
def Range(a,b,c):
    print('param:',a,b,c)
    yield f
    for i in range(a,b,c):
        yield i
        if i%2==0:
            yield int
            print('testtest')
            return
        else:
            yield type
t=Range(1,6,2)
print(t)
t=[{'data':[l,r]*2,False:True} for r in Range(1,6,2) for l in Range(5,-5,-3)]
print('len:',len(t))
print(t)
t=[[l,r] for r in Range(1,3,2) for l in Range(2,-2,-1)]
print('len:',len(t))
print(t)
print(sum([[1,2,3,4,],list(range(5,-2,-1))],[]))
