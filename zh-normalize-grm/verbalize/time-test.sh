# time verbalizer 测试
# 输入格式：经 TokenParser 重排后字段顺序为 zone → hours → minutes → seconds → preserve_order
# time { hours: "..." minutes: "..." preserve_order: true } → 拼接

# 测试 14:30
echo 'time { hours: "十四点" minutes: "三十分" preserve_order: true }' | thraxrewrite-tester --far=./time.far --rules=TIME --show_details

# 测试带秒 14:30:05
echo 'time { hours: "十四点" minutes: "三十分" seconds: "零五秒" preserve_order: true }' | thraxrewrite-tester --far=./time.far --rules=TIME --show_details

# 测试带时区 上午9:00
echo 'time { zone: "上午" hours: "九点" minutes: "零分" preserve_order: true }' | thraxrewrite-tester --far=./time.far --rules=TIME --show_details

# 测试下午时间
echo 'time { zone: "下午" hours: "三点" minutes: "十五分" preserve_order: true }' | thraxrewrite-tester --far=./time.far --rules=TIME --show_details
