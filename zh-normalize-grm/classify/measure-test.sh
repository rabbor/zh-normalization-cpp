# 测试公制单位
echo "10kg" | thraxrewrite-tester --far=./measure.far --rules=MEASURE --show_details

# 测试带小数的度量
echo "2.5℃" | thraxrewrite-tester --far=./measure.far --rules=MEASURE --show_details

# 测试距离单位
echo "5km" | thraxrewrite-tester --far=./measure.far --rules=MEASURE --show_details

# 测试速率（分子/分母）
echo "10km/h" | thraxrewrite-tester --far=./measure.far --rules=MEASURE --show_details

# 测试 k 前缀
echo "10kΩ" | thraxrewrite-tester --far=./measure.far --rules=MEASURE --show_details

# 测试中文单位
echo "5斤" | thraxrewrite-tester --far=./measure.far --rules=MEASURE --show_details
