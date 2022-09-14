# 直接输出字符串
echo "Hello World!"
# 注意，printf 是不会自动换行的
printf "Hello World!\n"

# 格式化输出字符串
printf "%-10s %-8s %-4s\n" "姓名" "性别" "体重kg"
printf "%-10s %-8s %-4.2f\n" "叶晨" "男" "56.9"
printf "%-10s %-8s %-4.2f\n" "徐钰婷" "女" "51.2"
printf "%-10s %-8s %-4.2f\n" "憨包" "男" "100.0"

# 使用双引号
printf "%d %s\n" 1 "abc"

# 使用单引号
printf '%d %s\n' 1 "abc"

# 甚至不需要引号
printf %s abcdef

# 格式只指定一个参数，但多出的参数仍会在按照格式输出
printf %s abc def
printf "%s\n" abc def
printf "%s %s %s\n" a b c d e f g h i j 

# 如果没有参数，那么 %s 用 NULL 代替，%d 用0
printf "%s and %d\n"
