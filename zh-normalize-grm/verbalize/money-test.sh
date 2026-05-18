# money verbalizer 测试
# 输入格式：经 TokenParser 重排后字段顺序为 amount → currency
# money { amount { integer_part: "..." } currency: "..." } → "金额+币种"

# 测试 $100 → 一百美元
echo 'money { amount { integer_part: "一百" } currency: "美元" }' | thraxrewrite-tester --far=./money.far --rules=MONEY --show_details

# 测试 ¥12.5 → 十二点五元
echo 'money { amount { integer_part: "十二点五" } currency: "元" }' | thraxrewrite-tester --far=./money.far --rules=MONEY --show_details

# 测试 USD100 → 一百美元
echo 'money { amount { integer_part: "一百" } currency: "美元" }' | thraxrewrite-tester --far=./money.far --rules=MONEY --show_details

# 测试含小数部分
echo 'money { amount { integer_part: "九十九" fractional_part: "九" } currency: "美元" }' | thraxrewrite-tester --far=./money.far --rules=MONEY --show_details
