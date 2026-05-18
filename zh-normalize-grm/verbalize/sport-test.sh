# sport verbalizer 测试
# sport 在 classify 端输出为 name: "..."，与 char 共享 NAME verbalizer
# name: "..." → 直接提取引号内文本

# 测试体育比分
echo 'name: "中国 三比二 韩国"' | thraxrewrite-tester --far=./sport.far --rules=SPORT --show_details

# 测试俱乐部比分
echo 'name: "AC米兰 二比零 国际米兰"' | thraxrewrite-tester --far=./sport.far --rules=SPORT --show_details
