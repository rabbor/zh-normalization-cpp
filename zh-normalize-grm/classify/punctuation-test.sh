# 测试中文逗号（中等停顿）
echo "，" | thraxrewrite-tester --far=./punctuation.far --rules=PUNCT --show_details

# 测试中文句号（长停顿）
echo "。" | thraxrewrite-tester --far=./punctuation.far --rules=PUNCT --show_details

# 测试英文逗号
echo "," | thraxrewrite-tester --far=./punctuation.far --rules=PUNCT --show_details

# 测试中文感叹号
echo "！" | thraxrewrite-tester --far=./punctuation.far --rules=PUNCT --show_details

# 测试中文问号
echo "？" | thraxrewrite-tester --far=./punctuation.far --rules=PUNCT --show_details

# 测试英文句号
echo "." | thraxrewrite-tester --far=./punctuation.far --rules=PUNCT --show_details
