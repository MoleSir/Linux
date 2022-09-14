# 关系运算符
#! /bin/bash

a=10
b=20

# 检查两个数是否相等 -eq --> equal
if [ ${a} -eq ${b} ]
then
	echo "${a} -eq ${b} -> a 等于 b"
else
	echo "${a} -eq ${b} -> a 不等于 b"
fi

# 检查两个数字是否不等他 -ne --> not equal 
if [ ${a} -ne ${b} ]
then
	echo "${a} -ne ${b} -> a 不等于 b"       
else
	echo "$a -ne $b -> a 等于 b"
fi

# 检查左边是否大于右边 -gt --> greater than
if [ $a -gt $b ]
then
	echo "$a -gt $b -> a 大于 b"
else
	echo "$a -gt $b -> a 小于等于 b"
fi

# 检查左边是否小于右边 -lt --> less than
if [ $a -lt $b ]
then
	echo "$a -lt $b --> a 小于 b"
else
	echo "$a -lt $b --> a 不小于 b"
fi

# 判断左边是否大于等于右边 -ge greater equal
# 判断左边是否大于等于右边 -le less equal












