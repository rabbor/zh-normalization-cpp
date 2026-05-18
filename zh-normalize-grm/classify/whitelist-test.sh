# 测试 B2B
echo "B2B" | thraxrewrite-tester --far=./whitelist.far --rules=WHITELIST --show_details

# 测试 B2C
echo "P2P" | thraxrewrite-tester --far=./whitelist.far --rules=WHITELIST --show_details

# 测试 C2C
echo "M.V.P" | thraxrewrite-tester --far=./whitelist.far --rules=WHITELIST --show_details

# 测试其他白名单词汇
echo "O2O" | thraxrewrite-tester --far=./whitelist.far --rules=WHITELIST --show_details

# 测试其他白名单词汇
echo "女儿" | thraxrewrite-tester --far=./whitelist.far --rules=WHITELIST --show_details

# 测试儿化音标记（单字“儿”作为儿化后缀）
echo "儿" | thraxrewrite-tester --far=./whitelist.far --rules=WHITELIST --show_details
