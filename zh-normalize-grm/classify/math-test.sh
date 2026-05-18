# 测试加法
echo "1+2=3" | thraxrewrite-tester --far=./math.far --rules=MATH --show_details

# 测试比较运算符
echo "3>1" | thraxrewrite-tester --far=./math.far --rules=MATH --show_details

# 测试比值
echo "1:2" | thraxrewrite-tester --far=./math.far --rules=MATH --show_details

# 测试乘除
echo "5×2=10" | thraxrewrite-tester --far=./math.far --rules=MATH --show_details

# 测试范围符号
echo "5~10" | thraxrewrite-tester --far=./math.far --rules=MATH --show_details

# 测试小于等于
echo "3<=5" | thraxrewrite-tester --far=./math.far --rules=MATH --show_details
