#! /bin/bash

# 使用expr作数学运算
var1=`expr 2 + 2`
echo "两数之和为：${var1}"

a=10
b=20

# 加法
var2=`expr ${a} + ${b}`
echo "a + b = ${var2}"

# 减法
var3=`expr ${a} - ${b}`
echo "a - b = ${var3}"

# 乘法
var4=`expr ${a} \* ${b}`
echo "a * b = ${var4}"

# 除法
var5=`expr ${b} / ${a}`
echo "b / a = ${var5}"

# 取余
var6=`expr ${b} % ${a}`
echo "b % a = ${var6}"

# 相等比较运算符
if [ ${a} == ${b} ]
then
	echo "a 等于 b"
fi
if [ ${a} != ${b} ]
then 
	echo "a 不等于 b"
fi

