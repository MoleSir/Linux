# 1、显示普通字符串
echo "Hello World!"
# 双引号可以省略
echo Hello World!

# 2、显示转义字符
# \" 表示转义
echo "\"Hello World!\""
# 双引号也可以省略
echo \"Hello World!\"

# 3、显示变量
read name
echo "My name is: ${name}"

# 4、显示换行
echo -e "Ok ! \n" # -e 开启转义

# 5、显示不换行
echo -e "OK! \c" # -e 开启转义，\c 不换行

# 6、显示结果定向至文件
echo "Hello World!" > hello.txt

# 7、原样输出字符串，不进行转义或取变量（使用单引号）
echo '${name}\n'

# 8、显示命令执行结果
echo `date`
