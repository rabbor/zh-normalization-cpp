# math verbalizer 测试
# math 在 classify 端输出为 name: "..."，与 char 共享 NAME verbalizer
# name: "..." → 直接提取引号内文本

# 测试加法 1+2=3
echo 'name: "一加二等于三"' | thraxrewrite-tester --far=./math.far --rules=MATH --show_details

# 测试比较 3>1
echo 'name: "三大于一"' | thraxrewrite-tester --far=./math.far --rules=MATH --show_details

# 测试乘法
echo 'name: "二乘三等于六"' | thraxrewrite-tester --far=./math.far --rules=MATH --show_details
