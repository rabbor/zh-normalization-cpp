# cardinal verbalizer 测试
# 输入格式：classify 输出经 Sparrowhawk TokenParser 剥去 tokens{} 后的结果
# cardinal { integer: "..." } → 直接提取 integer 字段值

# 测试基数词
echo 'cardinal { integer: "一百二十三" }' | thraxrewrite-tester --far=./cardinal.far --rules=CARDINAL --show_details

# 测试带小数
echo 'cardinal { integer: "三点一四" }' | thraxrewrite-tester --far=./cardinal.far --rules=CARDINAL --show_details

# 测试负数
echo 'cardinal { integer: "负十二" }' | thraxrewrite-tester --far=./cardinal.far --rules=CARDINAL --show_details

# 测试电话号码
echo 'cardinal { integer: "幺八五二零八九三幺幺二" }' | thraxrewrite-tester --far=./cardinal.far --rules=CARDINAL --show_details

# 测试百分数
echo 'cardinal { integer: "百分之九十九" }' | thraxrewrite-tester --far=./cardinal.far --rules=CARDINAL --show_details
