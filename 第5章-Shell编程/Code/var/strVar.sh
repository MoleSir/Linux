name="YeChen"
str="Hello, I konw you are \"$name\"!\n"
echo -e $str

greetinh1="hello, ${name} !"
greeting2="hello, "$name" !"
echo ${greeting1}
echo ${greeting2}

# 使用单引号
greeting3='hello, '$name' !'
greeting4='hello, ${name} !'
echo ${greeting3}
echo ${greeting4}

# 字符串长度
str1="abcd"
echo ${#str1}

# 提取子串
str2="Hello world! I am YeChen."
echo ${str2:1:4}

# 查找
echo `expr index "$str2" io`
