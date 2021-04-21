# OOP-project:Python语言解释器

# 项目必选功能：
## 类型:
```python
[1,2*3] #int
['1',"2",'''3'''] #str
[True,False] #bool
```

## 变量赋值
```python
a=1
a='233'
a=True
```

## 运算符
```python
(1+2-3)*4/5 #算术运算符
2==4 #比较运算符
2<<2 #移位运算符
(not True) and False or True #布尔运算符
```

## if-else
```python
if 1:
	print(0)
if False:
	print(1)
elif False:
	if True:
		print(2)
else:
	print(3)
```
## while
```python
i=0
while True:
	i+=1
	if i<=4:
		continue
	print(i)
	if i==10:
		break
```

## for
```python
for i in '0123456789':
	print(i)
```

## 函数
```python
def Pow(x,y):
	return x**y
```

# Bonus功能
## class
```python
class CLS(object):
	def __init__(self):
		self.cnt=0
```
## 容器
### tuple
```python
(1,)
(1,'2')
```
### list
```python
[1,'2',3,True]
```
### set
```python
{1,'2'}
```
### dict
```python
{1:'壹','贰':2}
```
## 生成器
```python
range(0,10,2) #极为重要，实现基础功能时考虑特判一版
```
```python
def Range(n):
	i=0
	while i<n:
		yield i
		i+=1
```
## 列表推导
```python
[[i*j for j in range(5)] for i in range(5)]
```

## 装饰器
```python
def wrapper(f):
    def _wrapper():
        print('before')
        f()
        print('after')
    return _wrapper
@wrapper
def test():
    print('666')
test()

```

## lambda表达式
```python
f=lambda x:x**2
f(4)
```
