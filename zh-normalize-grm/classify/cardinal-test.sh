# 测试普通数字
echo "1025201.00" | thraxrewrite-tester --far=./cardinal.far --rules=CARDINAL --show_details

# 测试负数
echo "-12.03" | thraxrewrite-tester --far=./cardinal.far --rules=CARDINAL --show_details

# 测试正数
echo "+741.25" | thraxrewrite-tester --far=./cardinal.far --rules=CARDINAL --show_details

# 测试电话（11位）
echo "18520893112" | thraxrewrite-tester --far=./cardinal.far --rules=CARDINAL --show_details

# 测试IP地址
echo "172.16.4.14" | thraxrewrite-tester --far=./cardinal.far --rules=CARDINAL --show_details

# 测试带连字符的卡号
echo "153-225-647" | thraxrewrite-tester --far=./cardinal.far --rules=CARDINAL --show_details
