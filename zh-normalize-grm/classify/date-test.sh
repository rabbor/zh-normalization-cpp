# 测试标准日期 YYYY/M/D
echo "2024/3/5" | thraxrewrite-tester --far=./date.far --rules=DATE --show_details

# 测试 ISO 格式 YYYY-MM-DD
echo "2024-03-05" | thraxrewrite-tester --far=./date.far --rules=DATE --show_details

# 测试 D/M/YYYY 格式
echo "05.03.2024" | thraxrewrite-tester --far=./date.far --rules=DATE --show_details

# 测试仅年月 YYYY/MM
echo "2024/03" | thraxrewrite-tester --far=./date.far --rules=DATE --show_details

# 测试仅月年 MM/YYYY
echo "03-2024" | thraxrewrite-tester --far=./date.far --rules=DATE --show_details

# 测试仅月日 MM/DD
echo "05/03" | thraxrewrite-tester --far=./date.far --rules=DATE --show_details
