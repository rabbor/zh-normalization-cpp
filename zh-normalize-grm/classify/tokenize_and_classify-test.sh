# 测试纯数字
echo "100" | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

# 测试中文 + 数字混合
echo "今天是2025年" | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

# 测试日期
echo "2025年5月9日" | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

# 测试时间
echo "14:30" | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

# 测试货币
echo '$100' | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

# 测试分数
echo "3/4" | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

# 测试白名单
echo "B2B" | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

# 测试儿化音
echo "儿" | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

# 测试纯中文
echo "你好世界" | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

# 测试中文含标点
echo "你好，世界！" | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

# 测试繁体预处理
echo "開發網絡" | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

# 测试多类型混合句子
echo '我花了$50买了3斤苹果' | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

# 测试多类型混合句子
echo '早上9:25上学' | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

echo '8:00 a.m.准时开会！' | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details


echo '我存了 1000万！' | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details
echo '我存了1000万！' | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details

echo '他说:"生活好啊！"' | thraxrewrite-tester --far=./tokenize_and_classify.far --rules=TOKENIZE_AND_CLASSIFY --show_details
