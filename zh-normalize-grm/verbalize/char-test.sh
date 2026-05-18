# char verbalizer 测试
# 输入格式：name: "..." → 直接提取引号内文本
# char/sport/math/whitelist 在 classify 端均输出为 name: "..."，共享此 verbalizer

# 测试单个中文字符
echo 'name: "你"' | thraxrewrite-tester --far=./char.far --rules=NAME --show_details

# 测试标点符号
echo 'name: "，"' | thraxrewrite-tester --far=./char.far --rules=NAME --show_details

# 测试英文字符
echo 'name: "A"' | thraxrewrite-tester --far=./char.far --rules=NAME --show_details

# 测试多字符内容（sport/math 等产出）
echo 'name: "中国 三比二 韩国"' | thraxrewrite-tester --far=./char.far --rules=NAME --show_details
