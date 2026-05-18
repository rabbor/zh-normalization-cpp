#include "tts_robust_normalizer.h"

#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "sparrowhawk/normalizer.h"
#include "tts_robust_normalizer.h"

DEFINE_string(config, "", "Path to the configuration proto.");
DEFINE_string(path_prefix, "./", "Optional path prefix if not relative.");

void NormalizeInput(const string& input, speech::sparrowhawk::Normalizer* normalizer)
{
    // Phase 1: Pre-normalize（保留 ___PROTn___ 不还原，保护 URL/文件名等不被后续步骤破坏）
    auto [preText, spans] = imtts::zh_normalizer::robust::NormalizeTtsTextPre(input);
    // Phase 2: Rewrite hyphens（___PROTn___ 不含连字符，安全）
    std::string actualInput = imtts::zh_normalizer::robust::RewriteHyphensBeforeZhWetext(preText);
    // Phase 3: Sparrowhawk
    const std::vector<string> sentences = normalizer->SentenceSplitter(actualInput);
    for (const auto& sentence : sentences) {
        std::cout << "sentence:" << sentence << std::endl;
    }
    std::string normalizedOutput;
    for (const auto& sentence : sentences) {
        string output;
        normalizer->Normalize(sentence, &output);
        normalizedOutput += output;
    }
    // Phase 4: 合并被 CHAR 规则拆分的英文字符
    normalizedOutput = imtts::zh_normalizer::robust::CollapseSpellout(normalizedOutput);
    // Phase 5: 还原 protected spans + 最终 normalize
    std::string actualOutput = imtts::zh_normalizer::robust::NormalizeTtsTextPost(normalizedOutput, spans);
    std::cout << "actualOutput:" << actualOutput << std::endl;
}

int main(int argc, char** argv)
{
    if (argc < 3) {
        std::cerr << "Usage:   " << argv[0] << "--config=[proto file] --path_prefix=[proto file dir]" << std::endl;
        std::cerr << "example: " << argv[0] << "--config=xx.proto --path_prefix=/tmp/" << std::endl;
        return 1;
    }
    using speech::sparrowhawk::Normalizer;
    std::set_new_handler(FailedNewHandler);
    SET_FLAGS(argv[0], &argc, &argv, true);
    std::unique_ptr<Normalizer> normalizer;
    normalizer.reset(new Normalizer());
    CHECK(normalizer->Setup(FLAGS_config, FLAGS_path_prefix));
    string input;

    while (std::getline(std::cin, input)) {
        NormalizeInput(input, normalizer.get());
    }
    return 0;
}
