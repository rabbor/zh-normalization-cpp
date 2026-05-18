# 测试国家比分
echo "中国3:1韩国" | thraxrewrite-tester --far=./sport.far --rules=SPORT --show_details

# 测试俱乐部比分
echo "AC米兰2-0国际米兰" | thraxrewrite-tester --far=./sport.far --rules=SPORT --show_details

# 测试其他比分格式
echo "湖人110:95勇士" | thraxrewrite-tester --far=./sport.far --rules=SPORT --show_details
# 测试其他比分格式
echo '我:"' | thraxrewrite-tester --far=./sport.far --rules=SPORT --show_details
