# date verbalizer 测试
# 输入格式：经 TokenParser 重排后字段顺序为 year → month → day → preserve_order
# date { year: "..." month: "..." day: "..." preserve_order: true } → 年月日拼接

# 测试完整日期（年月日）
echo 'date { year: "二零二五年" month: "五月" day: "九日" preserve_order: true }' | thraxrewrite-tester --far=./date.far --rules=DATE --show_details

# 测试仅年月
echo 'date { year: "二零二四年" month: "三月" preserve_order: true }' | thraxrewrite-tester --far=./date.far --rules=DATE --show_details

# 测试仅月日
echo 'date { month: "十二月" day: "二十五日" preserve_order: true }' | thraxrewrite-tester --far=./date.far --rules=DATE --show_details

# 测试仅年份
echo 'date { year: "一九九八年" preserve_order: true }' | thraxrewrite-tester --far=./date.far --rules=DATE --show_details
