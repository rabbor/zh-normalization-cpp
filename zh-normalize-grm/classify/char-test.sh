# 测试单个字符（兜底规则）
echo "A" | thraxrewrite-tester --far=./char.far --rules=CHAR --show_details

echo "中" | thraxrewrite-tester --far=./char.far --rules=CHAR --show_details

echo "!" | thraxrewrite-tester --far=./char.far --rules=CHAR --show_details

echo "1" | thraxrewrite-tester --far=./char.far --rules=CHAR --show_details

echo "啊" | thraxrewrite-tester --far=./char.far --rules=CHAR --show_details
