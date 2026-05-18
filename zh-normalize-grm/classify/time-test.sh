# 测试基本时间 HH:MM
echo "3:30" | thraxrewrite-tester --far=./time.far --rules=TIME --show_details

# 测试带秒时间 HH:MM:SS
echo "03:30:05" | thraxrewrite-tester --far=./time.far --rules=TIME --show_details

# 测试带 am/pm 时间
echo "10:30 am" | thraxrewrite-tester --far=./time.far --rules=TIME --show_details

# 测试 24 小时制
echo "15:00" | thraxrewrite-tester --far=./time.far --rules=TIME --show_details

# 测试时间范围
echo "3:30 am~5:50 pm" | thraxrewrite-tester --far=./time.far --rules=TIME --show_details

# 测试全角冒号
echo "3：30" | thraxrewrite-tester --far=./time.far --rules=TIME --show_details

