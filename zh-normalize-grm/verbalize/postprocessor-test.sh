# postprocessor 测试
# 测试各独立后处理 CDRewrite 规则
# 对应 Python: PostProcessor(remove_interjections, remove_puncts, full_to_half, tag_oov)

# ===== DEL_BLACKLIST: 删除黑名单感叹词 =====
# 测试删除"呃"
echo '呃今天天气不错' | thraxrewrite-tester --far=./postprocessor.far --rules=DEL_BLACKLIST --show_details

# 测试删除"啊"（如果在黑名单中）
echo '啊好的' | thraxrewrite-tester --far=./postprocessor.far --rules=DEL_BLACKLIST --show_details

# ===== TO_HALFWIDTH: 全角→半角 =====
# 测试全角字母
echo 'Ａ Ｂ Ｃ' | thraxrewrite-tester --far=./postprocessor.far --rules=TO_HALFWIDTH --show_details

# 测试全角数字
echo '１２３' | thraxrewrite-tester --far=./postprocessor.far --rules=TO_HALFWIDTH --show_details

# ===== DEL_PUNCT: 删除标点 =====
# 测试删除中文标点
echo '你好，世界！' | thraxrewrite-tester --far=./postprocessor.far --rules=DEL_PUNCT --show_details

# 测试删除英文标点
echo 'Hello, World!' | thraxrewrite-tester --far=./postprocessor.far --rules=DEL_PUNCT --show_details

# ===== POSTPROCESS: 默认组合（删黑名单 + 全角→半角） =====
echo '呃Ａ你好' | thraxrewrite-tester --far=./postprocessor.far --rules=POSTPROCESS --show_details

echo '呃今天天气不错' | thraxrewrite-tester --far=./postprocessor.far --rules=POSTPROCESS --show_details

# 测试删除"啊"（如果在黑名单中）
echo '啊好的' | thraxrewrite-tester --far=./postprocessor.far --rules=POSTPROCESS --show_details

# ===== TO_HALFWIDTH: 全角→半角 =====
# 测试全角字母
echo 'Ａ Ｂ Ｃ' | thraxrewrite-tester --far=./postprocessor.far --rules=POSTPROCESS --show_details

# 测试全角数字
echo '１２３' | thraxrewrite-tester --far=./postprocessor.far --rules=POSTPROCESS --show_details

# ===== DEL_PUNCT: 删除标点 =====
# 测试删除中文标点
echo '你好，世界！' | thraxrewrite-tester --far=./postprocessor.far --rules=POSTPROCESS --show_details

# 测试删除英文标点
echo 'Hello, World!' | thraxrewrite-tester --far=./postprocessor.far --rules=POSTPROCESS --show_details

