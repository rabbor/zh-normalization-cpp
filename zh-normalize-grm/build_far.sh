#!/usr/bin/env bash
# 一键编译全部 .grm → .far

set -euo pipefail
cd "$(dirname "$0")"


echo ">> 编译 classify/"
(
  cd classify
  for g in byte util cardinal date time money measure fraction math sport whitelist char punctuation preprocessor; do
    echo "   - ${g}.grm"
    thraxcompiler --input_grammar=${g}.grm --output_far=${g}.far
  done
  echo "   - tokenize_and_classify.grm"
  thraxcompiler --input_grammar=tokenize_and_classify.grm \
                --output_far=tokenize_and_classify.far
)

echo ">> 编译 verbalize/"
(
  cd verbalize
  for g in byte util char cardinal date time money measure fraction math sport whitelist  postprocessor verbalize; do
    echo "   - ${g}.grm"
    thraxcompiler --input_grammar=${g}.grm --output_far=${g}.far
  done
)

echo ">> 完成。生成的 .far 文件："
find . -maxdepth 2 -name "*.far" | sort
