// -*- c++ -*-
// TTS 鲁棒性正则化器（C++ 实现）

// 用途:
//   在 C++ Sparrowhawk 归一化管道中，对中文文本做预处理和/或后处理，

#ifndef TTS_ROBUST_NORMALIZER_H_
#define TTS_ROBUST_NORMALIZER_H_

#include <string>
#include <utility>
#include <vector>

namespace imtts {
namespace zh_normalizer {
    namespace robust {

        // ────────────────────────────────────────────────────────
        // 主入口
        // ────────────────────────────────────────────────────────

        // 对 TTS 输入做鲁棒性正则化清洗。
        // 适用于中文文本的预处理（robust_pre）和后处理（robust_post）。
        std::string NormalizeTtsText(const std::string& text);

        // Pre-Sparrowhawk: 完整 normalize 但保留 ___PROTn___ 不还原
        // 返回 {处理后文本, protected_spans}
        std::pair<std::string, std::vector<std::string>> NormalizeTtsTextPre(const std::string& text);

        // Post-Sparrowhawk: 还原 protected spans + 最终 normalize
        std::string NormalizeTtsTextPost(const std::string& text,
                                         const std::vector<std::string>& protected_spans);

        // 保护中文 WeText 不会被误读为"减"的连字符。
        // 对应 Python _rewrite_hyphens_before_zh_wetext。
        // 规则:
        //   数字-数字     保留   (如 10-3, 2024-05-01)
        //   明显负号保留           (如 -2, x=-2, 为-2)
        //   中文-中文 → 停顿边界  (如 请求接入-身份判定 → 请求接入，身份判定)
        //   其余连字符 → 空格     (如 A-B → A B, GPU-A100 → GPU A100)
        std::string RewriteHyphensBeforeZhWetext(const std::string& text);

        // ────────────────────────────────────────────────────────
        // 内部辅助（公开以便测试）
        // ────────────────────────────────────────────────────────

        // UTF-8 辅助
        bool IsCjkChar(const char* s, int& char_len);
        bool IsCjkOrKana(const char* s, int& char_len);
        bool IsCjkOrKanaChar(uint32_t cp);
        int Utf8CharLen(unsigned char byte);

        // 保护/还原受保护 token（URL / Email / 文件名等）
        std::pair<std::string, std::vector<std::string>> ProtectSpans(const std::string& text);
        std::string RestoreSpans(const std::string& text,
                                 const std::vector<std::string>& protected_spans);

        // 各子阶段
        std::string BaseCleanup(const std::string& text);
        std::string NormalizeMarkdownAndLines(const std::string& text);
        std::string NormalizeFlowArrows(const std::string& text);
        std::string NormalizeVisibleUnderscores(const std::string& text);
        std::string NormalizeSpaces(const std::string& text);
        std::string NormalizeStructuralPunctuation(const std::string& text);
        std::string NormalizeRepeatedPunctuation(const std::string& text);
        std::string EnsureTerminalPunctuation(const std::string& text);
        std::string EnsureTerminalPunctuationByLine(const std::string& text);

        // Sparrowhawk 后处理：合并被按字符拆分的 ASCII 序列
        // 例："C h a o f a n" → "Chaofan"，". m a p" → ".map"
        std::string CollapseSpellout(const std::string& text);

    }
}
} // namespace robust

#endif // TTS_ROBUST_NORMALIZER_H_
