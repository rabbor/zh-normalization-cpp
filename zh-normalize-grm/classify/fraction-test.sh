# 测试基本分数
echo "1/2" | thraxrewrite-tester --far=./fraction.far --rules=FRACTION --show_details

# 测试其他分数
echo "3/4" | thraxrewrite-tester --far=./fraction.far --rules=FRACTION --show_details

# 测试带负数的分数
echo "-1/3" | thraxrewrite-tester --far=./fraction.far --rules=FRACTION --show_details

# 测试小数分数
echo "2.5/10" | thraxrewrite-tester --far=./fraction.far --rules=FRACTION --show_details
