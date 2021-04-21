# 必要的公共知识:

## 常用终端命令

```bash
# 退程序必备
# Control+C 强制退出当前程序
# Control+D 结束输入
```


```bash
#包管理器，必装
brew #mac
apt-get #linux
```

```bash
#一般规范的工具都会有帮助文档，当然自己写的就算了
man #显示帮助文档
ls --help #显示ls的帮助，多是命令行参数说明
man --help # e.g.
```

```bash
cd #切换到特定目录(绝对路径/相对路径)
cd ~ #切换到家目录
cd / #切换到根目录
cd . #切换到当前目录
cd .. #切换到父目录
cd ./a #切换到当前目录下a目录
cd /a #切换到根目录下a目录
```
```bash
ls #显示当前目录下的文件
ls -a #显示隐藏文件
ls -l # 显示文件的详细信息
ls -al #显示文件(包括隐藏文件)的详细信息
```
```bash
#高危命令预警，并不存在回收站，使用前确认了解命令含义
rm #删除文件
rm -d #删除目录
rm -r #递归删除
rm -f #去掉删除警告
rm -rf #从此再无烦恼
```
```bash
cp a b #复制文件a到b
cp -r #递归复制
```
```bash
cat a #显示文件a的内容
mkdir a #建立文件夹a
mv a b #移动a到b
```

```bash
echo 1 2 #将参数1 2写到标准输出
```

```bash
clang-format #代码格式化工具，可能会用
```

### 一些暂且写上不加完整解释的命令
```bash
sudo #获取管理员权限，玩火请谨慎
touch #创建文件
find #递归显示文件名
grep #在文件中查找字符串
rsync #高级cp
iconv #编码转换
tar
zip
unzip
top
jobs
kill
chmod
ln
du
head
tail
more
less
tmux
ssh
#etc.
```

## C++
### 命令行下编译
```bash
#假定当前目录下有code.cpp a.in
g++ code.cpp #不加-o默认生成a.out
./a.out #运行当前目录下的a.out
./a.out < a.in #标准输入重定向到a.in
./a.out > a.out #标准输出重定向到a.out
./a.out < a.in > a.out #标准输入输出重定向
echo 1 2 | ./a.out #管道机制，后者输入重定向到前者输出
```


## Python3

### 为何不是Python2
Python2.x到3.x有大量不兼容的改动，本项目将以Python3为目标语言

### 语言特点
解释型语言，无需编译，运行效率较低

完全面对对象，一切皆对象

大量现有库的支持

etc.

### pip
建议安装pip进行python包管理

安装后可使用形如如下语句

```bash
pip install numpy #妈妈再也不用担心我的线性代数
```

### IPython
建议安装ipython便于进行学习和调试

安装后使用如下语句可在程序运行中直接调出IPython调试

```python
from IPython import embed;embed()
```

## Git

### 为何用Git
同时维护多个版本代码，即可以同时开发不同功能或修复已有问题

永久保存各版本代码，避免rm惨案

有问题时可追溯代码，便于开(甩)发(锅)

合并不同版本代码，便于独立开发

服务器上保存代码，同步代码无需使用物理手段

etc.

### 常用命令
```bash
git clone
git remote
git add
git rm
git commit
git push
git pull
git fetch
git merge
```

## Gitee
### 为何使用Gitee
另一个选择是Github，相比之下这个没有墙会快一点
### 如何使用
加入项目，即可pull/push

使用命令ssh-keygen生成秘钥，将公钥加入gitee账户，即可通过ssh克隆本项目，免去账号密码的困扰


