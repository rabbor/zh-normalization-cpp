# verbalize ALL 规则测试
# verbalize.grm 的 ALL = raw @ POSTPROCESS
# Sparrowhawk 对每个 token 调用 ALL，输入为剥去 tokens{} 后的 semiotic class 结构
# 输出为最终口语文本（经后处理：删黑名单词 + 全角→半角）

# 测试 cardinal
echo 'cardinal { integer: "一百二十三" }' | thraxrewrite-tester --far=./verbalize.far --rules=ALL --show_details

# 测试 date
echo 'date { year: "二零二五年" month: "五月" day: "九日" preserve_order: true }' | thraxrewrite-tester --far=./verbalize.far --rules=ALL --show_details

# 测试 time
echo 'time { hours: "十四点" minutes: "三十分" preserve_order: true }' | thraxrewrite-tester --far=./verbalize.far --rules=ALL --show_details

# 测试 money（经 TokenParser 重排：amount 在前，currency 在后）
echo 'money { amount { integer_part: "一百" } currency: "美元" }' | thraxrewrite-tester --far=./verbalize.far --rules=ALL --show_details

# 测试 fraction（经 TokenParser 重排：denominator 在前，numerator 在后）
echo 'fraction { denominator: "四" numerator: "三" }' | thraxrewrite-tester --far=./verbalize.far --rules=ALL --show_details

# 测试 measure
echo 'measure { cardinal { integer: "十" } units: "千克" }' | thraxrewrite-tester --far=./verbalize.far --rules=ALL --show_details

# 测试 char (name 字段)
echo 'name: "你"' | thraxrewrite-tester --far=./verbalize.far --rules=ALL --show_details

# 测试 whitelist (name 字段)
echo 'name: "B to B"' | thraxrewrite-tester --far=./verbalize.far --rules=ALL --show_details

# 测试后处理生效（全角→半角）
echo 'name: "Ａ"' | thraxrewrite-tester --far=./verbalize.far --rules=ALL --show_details

# 测试 RAW 规则（不含后处理，调试用）
echo 'cardinal { integer: "一百" }' | thraxrewrite-tester --far=./verbalize.far --rules=RAW --show_details

echo 'time { hours: "九点"  minutes: "二十五分" preserve_order: true }' | thraxrewrite-tester --far=./verbalize.far --rules=RAW --show_details

