# 测试繁体到简体转换
echo "繁體" | thraxrewrite-tester --far=./preprocessor.far --rules=PREPROCESS --show_details

echo "漢字" | thraxrewrite-tester --far=./preprocessor.far --rules=PREPROCESS --show_details

echo "中國" | thraxrewrite-tester --far=./preprocessor.far --rules=PREPROCESS --show_details

echo "書寫" | thraxrewrite-tester --far=./preprocessor.far --rules=PREPROCESS --show_details
