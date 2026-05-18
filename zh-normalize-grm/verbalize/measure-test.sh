# measure verbalizer 测试
# 输入格式（简单量）：measure { cardinal { integer: "..." } units: "..." }
# 输入格式（速率）：measure { decimal { numerator: "..." denominator: "..." } }

# 测试简单度量 10千克
echo 'measure { cardinal { integer: "十" } units: "千克" }' | thraxrewrite-tester --far=./measure.far --rules=MEASURE --show_details

# 测试简单度量 3米
echo 'measure { cardinal { integer: "三" } units: "米" }' | thraxrewrite-tester --far=./measure.far --rules=MEASURE --show_details

# 测试速率 60km/h → 每小时六十公里
echo 'measure { decimal { numerator: "六十公里" denominator: "小时" } }' | thraxrewrite-tester --far=./measure.far --rules=MEASURE --show_details

# 测试速率 10m/s → 每秒十米
echo 'measure { decimal { numerator: "十米" denominator: "秒" } }' | thraxrewrite-tester --far=./measure.far --rules=MEASURE --show_details
