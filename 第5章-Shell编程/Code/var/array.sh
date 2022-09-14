#定义数组
array1=(1 2 3 4 5 6)
array2=(
10
9
8
7
)

# 获取数组元素
echo ${array1[2]}
echo ${array2[@]}

array2[0]=100

echo ${array2[@]}


# 获得长度
echo ${#array1[@]}
echo ${#array2[*]}
