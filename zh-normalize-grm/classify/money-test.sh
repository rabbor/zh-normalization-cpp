# 测试美元符号
echo '$100' | thraxrewrite-tester --far=./money.far --rules=MONEY --input_mode="utf8"

# 测试人民币符号
echo '¥12.5' | thraxrewrite-tester --far=./money.far --rules=MONEY --show_details

# 测试人民币符号
echo 'CNY12.5' | thraxrewrite-tester --far=./money.far --rules=MONEY --show_details

# 测试 ISO 代码
echo 'USD100' | thraxrewrite-tester --far=./money.far --rules=MONEY --show_details

# 测试负数金额
echo '$-50' | thraxrewrite-tester --far=./money.far --rules=MONEY --show_details

# 测试小数金额
echo '$1.234' | thraxrewrite-tester --far=./money.far --rules=MONEY --show_details
