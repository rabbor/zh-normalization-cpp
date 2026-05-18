# whitelist verbalizer 测试
# 白名单有两种输入：
# 1. name: "..." → 直接输出引号内文本（普通白名单词 + 儿化音保留）
# 2. erhua: "儿" → 保留"儿"（若 remove_erhua=false）

# 测试普通白名单 B2B → B to B
echo 'name: "B to B"' | thraxrewrite-tester --far=./whitelist.far --rules=WHITELIST --show_details

# 测试普通白名单 P2P
echo 'name: "P to P"' | thraxrewrite-tester --far=./whitelist.far --rules=WHITELIST --show_details

# 测试儿化音保留（name 格式）
echo 'name: "儿"' | thraxrewrite-tester --far=./whitelist.far --rules=WHITELIST --show_details

# 测试 erhua 字段格式（若 classify 输出 erhua: "儿"）
echo 'erhua: "儿"' | thraxrewrite-tester --far=./whitelist.far --rules=WHITELIST --show_details
