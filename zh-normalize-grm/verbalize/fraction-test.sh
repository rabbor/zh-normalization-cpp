# fraction verbalizer 测试
# 输入格式：经 TokenParser 重排后字段顺序为 denominator → numerator
# fraction { denominator: "..." numerator: "..." } → "X分之Y"

# 测试普通分数 3/4 → 四分之三
echo 'fraction { denominator: "四" numerator: "三" }' | thraxrewrite-tester --far=./fraction.far --rules=FRACTION --show_details

# 测试 1/2 → 二分之一
echo 'fraction { denominator: "二" numerator: "一" }' | thraxrewrite-tester --far=./fraction.far --rules=FRACTION --show_details

# 测试 7/8 → 八分之七
echo 'fraction { denominator: "八" numerator: "七" }' | thraxrewrite-tester --far=./fraction.far --rules=FRACTION --show_details
