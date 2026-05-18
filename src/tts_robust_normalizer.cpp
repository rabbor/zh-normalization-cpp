// -*- c++ -*-
// TTS 鲁棒性正则化器 — C++ 实现

#include "tts_robust_normalizer.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

namespace imtts {
namespace zh_normalizer {
    namespace robust {
        // ═══════════════════════════════════════════════════════
        // 内部常量
        // ═══════════════════════════════════════════════════════

        static const int PROT_TAG = 0x0099;
        static const char* const kKeepHyphenPlaceholder = "___KEEP_HYPHEN_BEFORE_ZH_WETEXT___";
        static const char* const kProtectPrefix = "___PROT";

        // 中文结束标点（用于确保句末有标点）
        static const char kClosers[] = u8"\"')]}）】》〉」』"
                                       "\u2019\u201d";

        // ═══════════════════════════════════════════════════════
        // UTF-8 辅助
        // ═══════════════════════════════════════════════════════

        int Utf8CharLen(unsigned char byte)
        {
            if (byte < 0x80)
                return 1;
            if (byte < 0xC0)
                return 1; // continuation byte, treat as 1
            if (byte < 0xE0)
                return 2;
            if (byte < 0xF0)
                return 3;
            return 4;
        }

        // 从 UTF-8 序列解码一个代码点，返回代码点并设置 char_len。
        static uint32_t DecodeUtf8(const char* s, int& char_len)
        {
            unsigned char c = static_cast<unsigned char>(s[0]);
            if (c < 0x80) {
                char_len = 1;
                return c;
            }
            if ((c & 0xE0) == 0xC0) {
                char_len = 2;
                return ((c & 0x1F) << 6) | (static_cast<unsigned char>(s[1]) & 0x3F);
            }
            if ((c & 0xF0) == 0xE0) {
                char_len = 3;
                return ((c & 0x0F) << 12) | ((static_cast<unsigned char>(s[1]) & 0x3F) << 6) |
                    (static_cast<unsigned char>(s[2]) & 0x3F);
            }
            char_len = 4;
            return ((c & 0x07) << 18) | ((static_cast<unsigned char>(s[1]) & 0x3F) << 12) |
                ((static_cast<unsigned char>(s[2]) & 0x3F) << 6) |
                (static_cast<unsigned char>(s[3]) & 0x3F);
        }

        bool IsCjkOrKanaChar(uint32_t cp)
        {
            // CJK Unified Ideographs Extension A: U+3400 - U+4DBF
            if (cp >= 0x3400 && cp <= 0x4DBF)
                return true;
            // CJK Unified Ideographs: U+4E00 - U+9FFF
            if (cp >= 0x4E00 && cp <= 0x9FFF)
                return true;
            // Hiragana: U+3040 - U+309F
            if (cp >= 0x3040 && cp <= 0x309F)
                return true;
            // Katakana: U+30A0 - U+30FF
            if (cp >= 0x30A0 && cp <= 0x30FF)
                return true;
            return false;
        }

        bool IsCjkChar(const char* s, int& char_len)
        {
            uint32_t cp = DecodeUtf8(s, char_len);
            return (cp >= 0x3400 && cp <= 0x4DBF) || (cp >= 0x4E00 && cp <= 0x9FFF);
        }

        bool IsCjkOrKana(const char* s, int& char_len)
        {
            return IsCjkOrKanaChar(DecodeUtf8(s, char_len));
        }

        // 检查字符串中是否包含中日韩文字（用于语言检测）
        static bool ContainsCjk(const std::string& s)
        {
            const char* p = s.c_str();
            while (*p) {
                int len = Utf8CharLen(static_cast<unsigned char>(*p));
                if (len > 1) {
                    uint32_t cp = DecodeUtf8(p, len);
                    if (cp >= 0x4E00 && cp <= 0x9FFF)
                        return true;
                    if (cp >= 0x3400 && cp <= 0x4DBF)
                        return true;
                }
                p += len;
            }
            return false;
        }

        // 检查字符是否为中文逗号/分号/冒号等
        static bool IsZhDelimiter(uint32_t cp)
        {
            return (cp == 0x3001 || cp == 0x3002 || // 、 。
                    cp == 0xFF0C || cp == 0xFF0E || // ， ．
                    cp == 0xFF1A || cp == 0xFF1B || // ： ；
                    cp == 0xFF01 || cp == 0xFF1F || // ！ ？
                    cp == 0xFF08 || cp == 0xFF09 || // （ ）
                    cp == 0x3010 || cp == 0x3011 || // 【 】
                    cp == 0x300C || cp == 0x300D || // 「 」
                    cp == 0x300E || cp == 0x300F || // 『 』
                    cp == 0x3014 || cp == 0x3015 || // 〔 〕
                    cp == 0xFF3B || cp == 0xFF3D || // ［ ］
                    cp == 0x300A || cp == 0x300B);  // 《 》
        }

        // 检查字符是否为中文/日文标点
        static bool IsCjkPunct(uint32_t cp)
        {
            return (cp >= 0x3000 && cp <= 0x303F) || // CJK Symbols and Punctuation
                (cp >= 0xFF00 && cp <= 0xFFEF) ||    // Halfwidth and Fullwidth Forms
                IsZhDelimiter(cp);
        }

        // ═══════════════════════════════════════════════════════
        // 辅助：字符串处理
        // ═══════════════════════════════════════════════════════

        static std::string CollapseSpaces(const std::string& s)
        {
            static std::regex re(R"( {2,})");
            return std::regex_replace(s, re, " ");
        }

        static std::string Strip(const std::string& s)
        {
            size_t start = 0;
            while (start < s.size() && (s[start] == ' ' || s[start] == '\t'))
                ++start;
            size_t end = s.size();
            while (end > start && (s[end - 1] == ' ' || s[end - 1] == '\t'))
                --end;
            return s.substr(start, end - start);
        }

        // 判断字符类别：用于空格处理的边界检测
        // 返回: 'C'=CJK汉字, 'K'=假名, 'D'=数字, 'L'=拉丁字母, 'P'=受保护token, 'O'=其他
        static char CharCategory(const char* s, int& char_len)
        {
            unsigned char c = static_cast<unsigned char>(s[0]);
            if (c < 0x80) {
                char_len = 1;
                if (c >= '0' && c <= '9')
                    return 'D';
                if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
                    return 'L';
                return 'O';
            }
            uint32_t cp = DecodeUtf8(s, char_len);
            if (cp >= 0x4E00 && cp <= 0x9FFF)
                return 'C';
            if (cp >= 0x3400 && cp <= 0x4DBF)
                return 'C';
            if (cp >= 0x3040 && cp <= 0x30FF)
                return 'K';
            if (cp == 0xFF10) { // fullwidth 0
                // check for fullwidth digits
            }
            // fullwidth digits 0xFF10-0xFF19
            if (cp >= 0xFF10 && cp <= 0xFF19)
                return 'D';
            return 'O';
        }

        // 向前看一个 UTF-8 字符的类别（不移动指针）
        static char PeekCategory(const char* s)
        {
            int len;
            return CharCategory(s, len);
        }

        // ═══════════════════════════════════════════════════════
        // 阶段 1: 基础清洗
        // ═══════════════════════════════════════════════════════

        std::string BaseCleanup(const std::string& text)
        {
            std::string s = text;

            // 统一换行
            s = std::regex_replace(s, std::regex("\r\n"), "\n");
            s = std::regex_replace(s, std::regex("\r"), "\n");
            // 全角空格 → 半角空格
            s = std::regex_replace(s, std::regex(u8"\u3000"), " ");

            // 移除零宽字符 U+200B - U+200D, U+FEFF
            // 注意：不能用 regex 字节范围，会误删中文 UTF-8 字节
            {
                std::string tmp;
                tmp.reserve(s.size());
                for (size_t i = 0; i < s.size();) {
                    unsigned char c0 = static_cast<unsigned char>(s[i]);
                    if (c0 == 0xE2 && i + 2 < s.size() &&
                        static_cast<unsigned char>(s[i + 1]) == 0x80 &&
                        static_cast<unsigned char>(s[i + 2]) >= 0x8B &&
                        static_cast<unsigned char>(s[i + 2]) <= 0x8D) {
                        i += 3; // skip U+200B, U+200C, U+200D
                    } else if (c0 == 0xEF && i + 2 < s.size() &&
                               static_cast<unsigned char>(s[i + 1]) == 0xBB &&
                               static_cast<unsigned char>(s[i + 2]) == 0xBF) {
                        i += 3; // skip U+FEFF (BOM)
                    } else {
                        tmp += s[i];
                        ++i;
                    }
                }
                s = tmp;
            }

            // 移除其他控制字符（保留换行、制表、空格）
            std::string clean;
            clean.reserve(s.size());
            for (size_t i = 0; i < s.size();) {
                unsigned char c = s[i];
                int len = Utf8CharLen(c);
                if (len == 1) {
                    if (c == '\n' || c == '\t' || c == ' ') {
                        clean += s[i];
                    } else if (c >= 0x20) {
                        clean += s[i];
                    }
                    i += 1;
                } else {
                    clean.append(s, i, len);
                    i += len;
                }
            }
            return clean;
        }

        // ═══════════════════════════════════════════════════════
        // 阶段 2: Markdown 与换行处理
        // ═══════════════════════════════════════════════════════

        std::string NormalizeMarkdownAndLines(const std::string& text)
        {
            std::string s = text;

            // Markdown 链接: [text](url) → text url
            static std::regex md_link(R"(\[([^\[\]]+?)\]\((https?://[^)\s]+)\))");
            s = std::regex_replace(s, md_link, "$1 $2");

            std::vector<std::string> lines;
            std::istringstream iss(s);
            std::string raw;
            while (std::getline(iss, raw)) {
                std::string line = Strip(raw);
                if (line.empty())
                    continue;

                // 标题 # 或 //
                static std::regex heading(R"(^(?:#{1,6}\s+|//\s*))");
                line = std::regex_replace(line, heading, "");
                // 引用 >
                static std::regex quote(R"(^>\s+)");
                line = std::regex_replace(line, quote, "");
                // 无序列表
                static std::regex ul(R"(^[-*+]\s+)");
                line = std::regex_replace(line, ul, "");
                // 有序列表
                static std::regex ol(R"(^\d+[.)]\s+)");
                line = std::regex_replace(line, ol, "");

                lines.push_back(line);
            }

            if (lines.empty())
                return "";

            std::string merged;
            merged = lines[0];
            for (size_t i = 1; i < lines.size(); ++i) {
                merged = EnsureTerminalPunctuation(merged);
                merged += lines[i];
            }
            return merged;
        }

        // ═══════════════════════════════════════════════════════
        // 阶段 3: 流程箭头
        // ═══════════════════════════════════════════════════════

        std::string NormalizeFlowArrows(const std::string& text)
        {
            std::string s = text;
            // ASCII 箭头: ->, =>, <-, <= 及其变体
            // 注意：不能用 [...] 字符类放多字节 UTF-8 字符，std::regex 按字节匹配会破坏中文
            static std::regex arrow_ascii(R"(\s*(?:<[-=]+>|[-=]+>|<[-=]+)\s*)");
            s = std::regex_replace(s, arrow_ascii, u8"，");

            // Unicode 箭头：逐个替换，避免字节级字符类问题
            const char* arrows[] = {
                u8"\u2192", u8"\u2190", u8"\u2194", // → ← ↔
                u8"\u21D2", u8"\u21D0", u8"\u21D4", // ⇒ ⇐ ⇔
                u8"\u27F6", u8"\u27F5", u8"\u27F7", // ⟶ ⟵ ⟷
                u8"\u27F9", u8"\u27F8", u8"\u27FA", // ⟹ ⟸ ⟺
                u8"\u21A6", u8"\u21A4",             // ↦ ↤
                u8"\u21AA", u8"\u21A9",             // ↪ ↩
            };
            for (const char* arrow : arrows) {
                size_t pos = 0;
                size_t alen = std::strlen(arrow);
                while ((pos = s.find(arrow, pos)) != std::string::npos) {
                    // 去掉前后空格
                    size_t start = pos;
                    while (start > 0 && s[start - 1] == ' ')
                        --start;
                    size_t end = pos + alen;
                    while (end < s.size() && s[end] == ' ')
                        ++end;
                    s.replace(start, end - start, u8"，");
                    pos = start + 3; // strlen("，") = 3
                }
            }
            return s;
        }

        // ═══════════════════════════════════════════════════════
        // 阶段 4: 保护高风险 token
        // ═══════════════════════════════════════════════════════

        std::pair<std::string, std::vector<std::string>> ProtectSpans(const std::string& text)
        {
            std::vector<std::string> prot;
            std::string s = text;

            // 模式按优先级排列（URL 必须最先匹配，防止 http 被其他模式捕获）
            struct PatternPair {
                std::regex re;
                int priority;
            };

            // URL
            static std::regex url_re(R"(https?://[^\s　，。！？；、）】》〉」』]+)");
            // URL
            // static std::regex url_re(R"(https?://[^\s\u3000，。！？；、）】》〉」』]+)");
            // Email
            static std::regex email_re(
                R"([A-Za-z0-9._%+\-]+@[A-Za-z0-9.\-]+\.[A-Za-z]{2,}(?![A-Za-z0-9.\-]))");
            // Mention
            static std::regex mention_re(R"(@[A-Za-z0-9_]{1,32})");
            // Reddit
            static std::regex reddit_re(R"((?:u|r)/[A-Za-z0-9_]+)");
            // Hashtag
            static std::regex hashtag_re(R"(#(?!\s)[^\s#]+)");
            // Dot token: `.map` / `.env` / `.gitignore`
            static std::regex
                dot_token_re(R"(\.(?=[A-Za-z0-9._\-]*[A-Za-z0-9])[A-Za-z0-9._\-]+)");
            // File-like: must have at least one `.`, `/`, `-`, `:`, or `+` among letters
            // 注意：必须用自定义 delimiter 避免 ) 被当作 raw string 终止符
            static std::regex filelike_re(
                R"_re_((?=[A-Za-z0-9._/+\:\-]*[A-Za-z])(?=[A-Za-z0-9._/+\:\-]*[./+\:\-])[A-Za-z0-9][A-Za-z0-9._/+\:\-]*(?![A-Za-z0-9_]))_re_");

            // 替换函数
            auto replace = [&prot](const std::string& s, const std::regex& re) {
                std::string result;
                result.reserve(s.size());
                auto it = std::sregex_iterator(s.begin(), s.end(), re);
                auto end = std::sregex_iterator();
                size_t last = 0;
                for (; it != end; ++it) {
                    size_t pos = it->position();
                    // libc++ 不支持 (?<!...) 负向后顾，手动检查前驱字符
                    // 替代 (?<![A-Za-z0-9_])：前驱不是字母、数字、下划线
                    if (pos > 0) {
                        unsigned char pc = static_cast<unsigned char>(s[pos - 1]);
                        if (std::isalnum(pc) || pc == '_')
                            continue;
                    }
                    result.append(s, last, pos - last);
                    int idx = static_cast<int>(prot.size());
                    prot.push_back(it->str());
                    result += kProtectPrefix;
                    result += std::to_string(idx);
                    result += "___";
                    last = static_cast<size_t>(pos + it->length());
                }
                result.append(s, last, s.size() - last);
                return result;
            };

            // 按优先级应用（文件路径可能包含 . ，先保护完整路径）
            s = replace(s, url_re);
            // std::cout << "after url_re: " << s << std::endl;
            s = replace(s, email_re);
            // std::cout << "after email_re: " << s << std::endl;
            s = replace(s, mention_re);
            // std::cout << "after mention_re: " << s << std::endl;
            s = replace(s, reddit_re);
            // std::cout << "after reddit_re: " << s << std::endl;
            s = replace(s, hashtag_re);
            // std::cout << "after hashtag_re: " << s << std::endl;
            s = replace(s, filelike_re);
            s = replace(s, dot_token_re);

            return { s, prot };
        }

        std::string RestoreSpans(const std::string& text,
                                 const std::vector<std::string>& protected_spans)
        {
            std::string s = text;
            for (size_t i = 0; i < protected_spans.size(); ++i) {
                std::string tag = std::string(kProtectPrefix) + std::to_string(i) + "___";
                size_t pos = 0;
                while ((pos = s.find(tag, pos)) != std::string::npos) {
                    s.replace(pos, tag.size(), protected_spans[i]);
                    pos += protected_spans[i].size();
                }
            }
            return s;
        }

        // ═══════════════════════════════════════════════════════
        // 阶段 5: 下划线转空格
        // ═══════════════════════════════════════════════════════

        std::string NormalizeVisibleUnderscores(const std::string& text)
        {
            // 将 `_` 转成空格，但跳过受保护 token (___PROT{N}___)
            static std::regex prottag(R"(___PROT\d+___)");

            std::string result;
            result.reserve(text.size());
            const char* p = text.c_str();
            while (*p) {
                // 检查是否为保护标签
                std::cmatch m;
                if (std::regex_search(p, m, prottag) && m.position() == 0) {
                    result.append(m[0].first, m[0].length());
                    p = m[0].second;
                    continue;
                }
                if (*p == '_') {
                    result += ' ';
                } else {
                    int len = Utf8CharLen(static_cast<unsigned char>(*p));
                    result.append(p, len);
                    p += len - 1; // 将在循环末尾 +1
                }
                ++p;
            }
            return result;
        }

        // ═══════════════════════════════════════════════════════
        // 阶段 6: 空格规则
        // ═══════════════════════════════════════════════════════

        std::string NormalizeSpaces(const std::string& text)
        {
            std::string s = text;

            // 1. 统一连续空白为单个空格
            s = CollapseSpaces(s);

            // 2. CJK 内部删除空格
            // 注: std::regex 对 Unicode 范围支持有限，这里用逐字符处理
            // 手动实现: CJK/假名 之间删除空格
            {
                std::string result;
                result.reserve(s.size());
                const char* p = s.c_str();
                while (*p) {
                    int len;
                    char cat = CharCategory(p, len);
                    if (cat == 'C' || cat == 'K') {
                        // 检查后面的空格
                        const char* next = p + len;
                        while (*next == ' ')
                            ++next;
                        if (*next) {
                            int nlen;
                            char ncat = CharCategory(next, nlen);
                            if (ncat == 'C' || ncat == 'K') {
                                // CJK/假名 → 跳过空格 → CJK/假名: 删除空格
                                result.append(p, len);
                                p = next;
                                continue;
                            }
                        }
                    }
                    result.append(p, len);
                    p += len;
                }
                s = result;
            }

            // 3. CJK/假名 与 纯数字之间：删除空格
            {
                std::string result;
                result.reserve(s.size());
                const char* p = s.c_str();
                while (*p) {
                    int len;
                    char cat = CharCategory(p, len);
                    if (cat == 'C' || cat == 'K') {
                        const char* next = p + len;
                        while (*next == ' ')
                            ++next;
                        if (next != p + len && *next) {
                            int nlen;
                            char ncat = CharCategory(next, nlen);
                            if (ncat == 'D') {
                                result.append(p, len);
                                p = next;
                                continue;
                            }
                        }
                    } else if (cat == 'D') {
                        const char* next = p + len;
                        while (*next == ' ')
                            ++next;
                        if (next != p + len && *next) {
                            int nlen;
                            char ncat = CharCategory(next, nlen);
                            if (ncat == 'C' || ncat == 'K') {
                                result.append(p, len);
                                p = next;
                                continue;
                            }
                        }
                    }
                    result.append(p, len);
                    p += len;
                }
                s = result;
            }

            // 4. CJK/假名 与拉丁/protected token 相邻：保留或补 1 个空格
            {
                std::string result;
                result.reserve(s.size());
                const char* p = s.c_str();
                while (*p) {
                    // 检测 protected token ___PROT\d+___
                    if (std::strncmp(p, kProtectPrefix, 7) == 0) {
                        // 找到 token 结尾
                        const char* end = p + 7;
                        while (*end >= '0' && *end <= '9')
                            ++end;
                        if (std::strncmp(end, "___", 3) == 0) {
                            end += 3;
                            // token 前面如果是 CJK，补空格
                            if (!result.empty()) {
                                // 向后扫描 result 找最后一个 UTF-8 字符
                                int ri = (int)result.size() - 1;
                                while (ri > 0 &&
                                       (static_cast<unsigned char>(result[ri]) & 0xC0) == 0x80)
                                    --ri;
                                int rlen;
                                uint32_t rcp = DecodeUtf8(&result[ri], rlen);
                                bool prev_cjk = (rcp >= 0x4E00 && rcp <= 0x9FFF) ||
                                    (rcp >= 0x3400 && rcp <= 0x4DBF) ||
                                    (rcp >= 0x3040 && rcp <= 0x30FF);
                                if (prev_cjk && result.back() != ' ')
                                    result += ' ';
                            }
                            result.append(p, end - p);
                            // token 后面如果是 CJK，确保一个空格
                            const char* after = end;
                            while (*after == ' ')
                                ++after;
                            if (*after) {
                                int alen;
                                char acat = CharCategory(after, alen);
                                if (acat == 'C' || acat == 'K') {
                                    result += ' ';
                                    p = end;
                                    while (*p == ' ')
                                        ++p;
                                } else {
                                    // 非 CJK，保留原有空格
                                    p = end;
                                }
                            } else {
                                p = end;
                            }
                            continue;
                        }
                    }
                    int len;
                    char cat = CharCategory(p, len);
                    if (cat == 'C' || cat == 'K') {
                        const char* next = p + len;
                        if (*next == ' ') {
                            ++next;
                        }
                        if (*next) {
                            int nlen;
                            char ncat = CharCategory(next, nlen);
                            if (ncat == 'L' || std::strncmp(next, kProtectPrefix, 7) == 0) {
                                result.append(p, len);
                                result += ' ';
                                p = p + len;
                                // 跳过已有的空格
                                while (*p == ' ')
                                    ++p;
                                continue;
                            }
                        }
                    } else if (cat == 'L') {
                        const char* next = p + len;
                        if (*next == ' ')
                            ++next;
                        if (*next) {
                            int nlen;
                            char ncat = CharCategory(next, nlen);
                            if (ncat == 'C' || ncat == 'K') {
                                result.append(p, len);
                                result += ' ';
                                p = p + len;
                                while (*p == ' ')
                                    ++p;
                                continue;
                            }
                        }
                    }
                    result.append(p, len);
                    p += len;
                }
                s = result;
            }

            // 5. 再次压缩连续空格
            s = CollapseSpaces(s);

            // 6. 中文标点前后不保留空格（手动处理，避免字符类字节匹配）
            {
                std::string result;
                result.reserve(s.size());
                const char* p = s.c_str();
                while (*p) {
                    unsigned char uc = static_cast<unsigned char>(*p);
                    int len = Utf8CharLen(uc);
                    uint32_t cp = (uc >= 0x80) ? DecodeUtf8(p, len) : uc;
                    // 后标点（前面空格要删）
                    bool is_after =
                        (cp == 0xFF0C || cp == 0x3002 || cp == 0xFF01 || cp == 0xFF1F ||
                         cp == 0xFF1B || cp == 0xFF1A || cp == 0x3001 || cp == 0x201C ||
                         cp == 0x201D || cp == 0x300F || cp == 0x300D || cp == 0x3011 ||
                         cp == 0xFF09 || cp == 0x300B);
                    if (is_after) {
                        while (!result.empty() && result.back() == ' ')
                            result.pop_back();
                        result.append(p, len);
                        p += len;
                        while (*p == ' ')
                            ++p;
                        continue;
                    }
                    // 前标点（后面空格要删）
                    bool is_before =
                        (cp == 0xFF08 || cp == 0x3010 || cp == 0x300C || cp == 0x300E ||
                         cp == 0x300A || cp == 0x201C || cp == 0x2018);
                    if (is_before) {
                        result.append(p, len);
                        p += len;
                        while (*p == ' ')
                            ++p;
                        continue;
                    }
                    result.append(p, len);
                    p += len;
                }
                s = result;
            }

            // 7. ASCII 标点前不留空格
            static std::regex ascii_punct(R"(\s+([,.;!?]))");
            s = std::regex_replace(s, ascii_punct, "$1");

            s = CollapseSpaces(s);
            return Strip(s);
        }

        // ═══════════════════════════════════════════════════════
        // 阶段 7: 结构性标点
        // ═══════════════════════════════════════════════════════

        std::string NormalizeStructuralPunctuation(const std::string& text)
        {
            std::string s = text;

            // [] → ""
            static std::regex bracket_sq(R"(\[\s*([^\[\]]+?)\s*\])");
            s = std::regex_replace(s, bracket_sq, "\"$1\"");

            // {} → ""
            static std::regex bracket_cu(R"(\{\s*([^{}]+?)\s*\})");
            s = std::regex_replace(s, bracket_cu, "\"$1\"");

            // 【】、〖〗、『』、「」 → ""（手动匹配，避免字节级字符类）
            {
                const char* openers[] = { u8"\u3010", u8"\u3016", u8"\u300E", u8"\u300C" };
                const char* closers[] = { u8"\u3011", u8"\u3017", u8"\u300F", u8"\u300D" };
                for (int bi = 0; bi < 4; ++bi) {
                    std::string result;
                    result.reserve(s.size());
                    const char* p = s.c_str();
                    size_t olen = std::strlen(openers[bi]);
                    size_t clen = std::strlen(closers[bi]);
                    while (*p) {
                        if (std::strncmp(p, openers[bi], olen) == 0) {
                            const char* start = p + olen;
                            while (*start == ' ')
                                ++start;
                            const char* end = std::strstr(start, closers[bi]);
                            if (end) {
                                const char* content_end = end;
                                while (content_end > start && *(content_end - 1) == ' ')
                                    --content_end;
                                result += '"';
                                result.append(start, content_end - start);
                                result += '"';
                                p = end + clen;
                                continue;
                            }
                        }
                        int chl = Utf8CharLen(static_cast<unsigned char>(*p));
                        result.append(p, chl);
                        p += chl;
                    }
                    s = result;
                }
            }

            // 《》保留（不拆开，作为书名号始终保留）
            // （无需处理）

            // 流程箭头
            s = NormalizeFlowArrows(s);

            // 长破折号/多连字符 → 句边界
            static std::regex long_dash(R"(\s*(?:—|–|―|-){2,}\s*)");
            s = std::regex_replace(s, long_dash, u8"。");

            return s;
        }

        // ═══════════════════════════════════════════════════════
        // 阶段 8: 重复标点
        // ═══════════════════════════════════════════════════════

        std::string NormalizeRepeatedPunctuation(const std::string& text)
        {
            std::string s = text;

            // 省略号/连续句点 → 。
            static std::regex ellipsis(R"((?:\.{3,}|…{2,}|……+))");
            s = std::regex_replace(s, ellipsis, u8"。");

            // 同类重复标点压缩（手动处理，避免字符类字节匹配）
            {
                std::string result;
                result.reserve(s.size());
                const char* p = s.c_str();
                while (*p) {
                    unsigned char uc = static_cast<unsigned char>(*p);
                    int len = Utf8CharLen(uc);
                    uint32_t cp = (uc >= 0x80) ? DecodeUtf8(p, len) : uc;
                    // 。 或 ． 重复
                    if (cp == 0x3002 || cp == 0xFF0E) {
                        result.append(p, len);
                        const char* q = p + len;
                        while (*q) {
                            int ql;
                            uint32_t qc = (static_cast<unsigned char>(*q) >= 0x80)
                                ? DecodeUtf8(q, ql)
                                : (ql = 1, (uint32_t)(unsigned char)*q);
                            if (qc == 0x3002 || qc == 0xFF0E)
                                q += ql;
                            else
                                break;
                        }
                        p = q;
                        continue;
                    }
                    // ， 或 , 重复
                    if (cp == 0xFF0C || cp == ',') {
                        result += u8"\uFF0C";
                        const char* q = p + len;
                        while (*q) {
                            int ql;
                            uint32_t qc = (static_cast<unsigned char>(*q) >= 0x80)
                                ? DecodeUtf8(q, ql)
                                : (ql = 1, (uint32_t)(unsigned char)*q);
                            if (qc == 0xFF0C || qc == ',')
                                q += ql;
                            else
                                break;
                        }
                        p = q;
                        continue;
                    }
                    // ! 或 ！ 重复
                    if (cp == '!' || cp == 0xFF01) {
                        const char* q = p + len;
                        bool has_q_mark = false;
                        while (*q) {
                            int ql;
                            uint32_t qc = (static_cast<unsigned char>(*q) >= 0x80)
                                ? DecodeUtf8(q, ql)
                                : (ql = 1, (uint32_t)(unsigned char)*q);
                            if (qc == '!' || qc == 0xFF01)
                                q += ql;
                            else if (qc == '?' || qc == 0xFF1F) {
                                has_q_mark = true;
                                q += ql;
                            } else
                                break;
                        }
                        if (q == p + len) {
                            result.append(p, len);
                            p = q;
                            continue;
                        }
                        result += has_q_mark ? u8"\uFF1F\uFF01" : u8"\uFF01";
                        p = q;
                        continue;
                    }
                    // ? 或 ？ 重复
                    if (cp == '?' || cp == 0xFF1F) {
                        const char* q = p + len;
                        bool has_e_mark = false;
                        while (*q) {
                            int ql;
                            uint32_t qc = (static_cast<unsigned char>(*q) >= 0x80)
                                ? DecodeUtf8(q, ql)
                                : (ql = 1, (uint32_t)(unsigned char)*q);
                            if (qc == '?' || qc == 0xFF1F)
                                q += ql;
                            else if (qc == '!' || qc == 0xFF01) {
                                has_e_mark = true;
                                q += ql;
                            } else
                                break;
                        }
                        if (q == p + len) {
                            result.append(p, len);
                            p = q;
                            continue;
                        }
                        result += has_e_mark ? u8"\uFF1F\uFF01" : u8"\uFF1F";
                        p = q;
                        continue;
                    }
                    result.append(p, len);
                    p += len;
                }
                s = result;
            }

            return s;
        }

        // ═══════════════════════════════════════════════════════
        // 阶段 9: 句末标点
        // ═══════════════════════════════════════════════════════

        std::string EnsureTerminalPunctuation(const std::string& text)
        {
            if (text.empty())
                return text;

            // 从末尾往前找最后一个非空格字符，使用 UTF-8 感知的遍历
            // 先跳过末尾空白
            int i = static_cast<int>(text.size()) - 1;
            while (i >= 0 && (text[i] == ' ' || text[i] == '\t'))
                --i;
            if (i < 0)
                return text + u8"\u3002";

            // 跳过末尾闭合符号（用 UTF-8 解码）
            while (i >= 0) {
                unsigned char c = static_cast<unsigned char>(text[i]);
                if (c < 0x80) {
                    if (c == '"' || c == '\'' || c == ')' || c == ']' || c == '}') {
                        --i;
                        continue;
                    }
                    break;
                }
                // 多字节字符：向前找 UTF-8 起始字节
                int start = i;
                while (start > 0 && (static_cast<unsigned char>(text[start]) & 0xC0) == 0x80)
                    --start;
                int clen;
                uint32_t cp = DecodeUtf8(&text[start], clen);
                if (cp == 0xFF09 || cp == 0x3011 || cp == 0x300B || cp == 0x3009 ||
                    cp == 0x300D || cp == 0x300F || cp == 0x3015 || cp == 0xFF3D ||
                    cp == 0x2019 || cp == 0x201D) {
                    i = start - 1;
                    continue;
                }
                break;
            }
            if (i < 0)
                return text + u8"\u3002";

            // 检查 i 处的字符是否为终结标点
            unsigned char c = static_cast<unsigned char>(text[i]);
            if (c < 0x80) {
                // ASCII 句号转全角
                if (c == '.')
                    return text.substr(0, i) + u8"\u3002" + text.substr(i + 1);
                if (c == '!' || c == '?' || c == ',')
                    return text;
            } else {
                int start = i;
                while (start > 0 && (static_cast<unsigned char>(text[start]) & 0xC0) == 0x80)
                    --start;
                int clen;
                uint32_t cp = DecodeUtf8(&text[start], clen);
                if (cp == 0x3002 || cp == 0xFF0E || cp == 0xFF0C || cp == 0xFF01 ||
                    cp == 0xFF1F || cp == 0x3001 || cp == 0xFF1A || cp == 0xFF1B ||
                    cp == 0x2026 || cp == 0x2025)
                    return text;
                if (IsCjkPunct(cp))
                    return text;
            }
            return text + u8"\u3002";
        }

        std::string EnsureTerminalPunctuationByLine(const std::string& text)
        {
            if (text.empty())
                return text;

            std::vector<std::string> lines;
            std::istringstream iss(text);
            std::string raw;
            while (std::getline(iss, raw)) {
                std::string line = Strip(raw);
                if (!line.empty()) {
                    line = EnsureTerminalPunctuation(line);
                }
                lines.push_back(line);
            }

            std::string result;
            for (size_t i = 0; i < lines.size(); ++i) {
                if (i > 0)
                    result += '\n';
                result += lines[i];
            }
            return Strip(result);
        }

        // ═══════════════════════════════════════════════════════
        // 主函数: normalize_tts_text
        // ═══════════════════════════════════════════════════════

        std::string NormalizeTtsText(const std::string& text)
        {
            ////std::cout << "start base cleanup" << std::endl;
            std::string s = BaseCleanup(text);
            // std::cout << "start normalize markdown and lines" << std::endl;
            s = NormalizeMarkdownAndLines(s);
            // std::cout << "start normalize flow arrows" << std::endl;
            s = NormalizeFlowArrows(s);
            // std::cout << "start protect spans: " << s << std::endl;
            auto [protected_text, prot] = ProtectSpans(s);
            s = protected_text;
            // std::cout << "start normalize visible underscores" << std::endl;

            s = NormalizeVisibleUnderscores(s);
            // std::cout << "start normalize spaces" << std::endl;
            s = NormalizeSpaces(s);
            // std::cout << "start normalize structural punctuation" << std::endl;
            s = NormalizeStructuralPunctuation(s);
            // std::cout << "start normalize repeated punctuation" << std::endl;
            s = NormalizeRepeatedPunctuation(s);
            // std::cout << "start normalize spaces" << std::endl;
            s = NormalizeSpaces(s);

            // std::cout << "start restore spans" << std::endl;
            s = RestoreSpans(s, prot);
            // std::cout << "start strip" << std::endl;
            s = Strip(s);
            // std::cout << "start terminal punctuation" << std::endl;
            return EnsureTerminalPunctuationByLine(s);
        }

        // Pre-Sparrowhawk: 完整 normalize 但保留 ___PROTn___ 不还原
        std::pair<std::string, std::vector<std::string>> NormalizeTtsTextPre(const std::string& text)
        {
            std::string s = BaseCleanup(text);
            s = NormalizeMarkdownAndLines(s);
            s = NormalizeFlowArrows(s);
            auto [protected_text, prot] = ProtectSpans(s);
            s = protected_text;
            s = NormalizeVisibleUnderscores(s);
            s = NormalizeSpaces(s);
            s = NormalizeStructuralPunctuation(s);
            s = NormalizeRepeatedPunctuation(s);
            s = NormalizeSpaces(s);
            s = Strip(s);
            // 不调用 RestoreSpans 和 EnsureTerminalPunctuation
            return { s, prot };
        }

        // Post-Sparrowhawk: 还原 protected spans + 最终 normalize
        std::string NormalizeTtsTextPost(const std::string& text,
                                         const std::vector<std::string>& protected_spans)
        {
            std::string s = RestoreSpans(text, protected_spans);
            return NormalizeTtsText(s);
        }

        // ═══════════════════════════════════════════════════════
        // 主函数: rewrite_hyphens_before_zh_wetext
        // ═══════════════════════════════════════════════════════

        std::string RewriteHyphensBeforeZhWetext(const std::string& text)
        {
            std::string s = text;
            if (s.find('-') == std::string::npos)
                return s;

            // 使用 placeholder 保护需要保留的连字符
            const std::string ph = kKeepHyphenPlaceholder;

            // 1. 保留行首负号:  ^-2 → ^_PH_2
            static std::regex head_neg(R"(^\s*-\s*(?=\d))");
            {
                std::string result;
                auto it = std::sregex_iterator(s.begin(), s.end(), head_neg);
                if (it != std::sregex_iterator()) {
                    size_t last = 0;
                    for (auto end = std::sregex_iterator(); it != end; ++it) {
                        result.append(s, last, it->position() - last);
                        std::string m = it->str();
                        // 保留前缀空格，替换连字符
                        size_t dash = m.find('-');
                        m.replace(dash, 1, ph);
                        result += m;
                        last = it->position() + it->length();
                    }
                    result.append(s, last, s.size() - last);
                    s = result;
                }
            }

            // 2. 保留分隔符后负号: x=-2 / (-2)
            static std::regex delim_neg(R"(([=:+*/,(，:：；;（【\[{])\s*-\s*(?=\d))");
            s = std::regex_replace(s, delim_neg, "$1" + ph);

            // 3. 保留中文上下文负号: 为-2 / 计算-2
            //    (CJK 字符 + 可选空格 + `-` + 可选空格 + 数字)
            {
                std::string result;
                result.reserve(s.size());
                const char* p = s.c_str();
                while (*p) {
                    int len;
                    if (IsCjkChar(p, len)) {
                        // 检查后面是否有 - 后接数字
                        const char* q = p + len;
                        while (*q == ' ')
                            ++q;
                        if (*q == '-') {
                            const char* r = q + 1;
                            while (*r == ' ')
                                ++r;
                            if (*r >= '0' && *r <= '9') {
                                result.append(p, len);
                                result += ph;
                                while (*p == ' ')
                                    ++p; // 跳过空格
                                if (*p == '-')
                                    ++p; // 跳过连字符
                                p = r;   // 跳到数字
                                continue;
                            }
                        }
                        result.append(p, len);
                        p += len;
                        continue;
                    }
                    result += *p;
                    ++p;
                }
                s = result;
            }

            // 4. 保留数字范围/日期: 10-3 / 2024-05-01
            static std::regex num_range(R"((\d)\s*-\s*(?=\d))");
            s = std::regex_replace(s, num_range, "$1" + ph);

            // 5. CJK-连字符-CJK → 停顿边界: 请求接入-身份判定 → 请求接入，身份判定
            {
                std::string result;
                result.reserve(s.size());
                const char* p = s.c_str();
                while (*p) {
                    int len;
                    if (IsCjkChar(p, len)) {
                        const char* q = p + len;
                        while (*q == ' ')
                            ++q;
                        if (*q == '-') {
                            const char* r = q + 1;
                            while (*r == ' ')
                                ++r;
                            if (*r) {
                                int rlen;
                                if (IsCjkChar(r, rlen)) {
                                    result.append(p, len);
                                    result += u8"，";
                                    p = r;
                                    continue;
                                }
                            }
                        }
                        result.append(p, len);
                        p += len;
                        continue;
                    }
                    result += *p;
                    ++p;
                }
                s = result;
            }

            // 6. 其余连字符 → 空格: A-B → A B
            static std::regex other_hyphen(R"(([^\s\-])\s*-\s*(?=[^\s\-]))");
            s = std::regex_replace(s, other_hyphen, "$1 ");

            // 压缩空格并还原占位符
            s = CollapseSpaces(s);
            // 只去除尾部空白，保留前导空格
            {
                size_t end = s.size();
                while (end > 0 && (s[end - 1] == ' ' || s[end - 1] == '\t'))
                    --end;
                s = s.substr(0, end);
            }

            // 还原受保护的连字符占位符
            {
                size_t pos = 0;
                while ((pos = s.find(ph, pos)) != std::string::npos) {
                    s.replace(pos, ph.size(), "-");
                    pos += 1;
                }
            }

            return s;
        }

        // ═══════════════════════════════════════════════════════
        // CollapseSpellout: Sparrowhawk 后处理
        // 合并被 CHAR 规则按字符拆分的 ASCII 序列
        // 规则：连续的 "X Y Z"(每个 X/Y/Z 为单个 ASCII 可打印字符)合并为 "XYZ"
        // ═══════════════════════════════════════════════════════

        std::string CollapseSpellout(const std::string& text)
        {
            if (text.empty()) return text;

            std::string result;
            result.reserve(text.size());
            const char* p = text.c_str();

            while (*p) {
                // 检测是否为“单个 ASCII 可打印字符 + 空格 + 单个 ASCII 可打印字符”序列
                unsigned char uc = static_cast<unsigned char>(*p);
                if (uc >= 33 && uc <= 126) {
                    // p 指向一个 ASCII 可打印字符
                    // 检查后面是否 " X" (space + single ascii)
                    const char* ahead = p + 1;
                    if (*ahead == ' ') {
                        unsigned char next_uc = static_cast<unsigned char>(*(ahead + 1));
                        if (next_uc >= 33 && next_uc <= 126) {
                            // 确认这是 spellout 序列，开始收集
                            std::string collected;
                            collected += *p;
                            const char* q = ahead;
                            while (*q == ' ') {
                                unsigned char qu = static_cast<unsigned char>(*(q + 1));
                                if (qu >= 33 && qu <= 126) {
                                    // q+1 是单个 ASCII，检查它后面是否为空格或结束
                                    const char* after = q + 2;
                                    if (*after == ' ' || *after == '\0' ||
                                        static_cast<unsigned char>(*after) >= 128) {
                                        // q+1 确实是单字符词
                                        collected += *(q + 1);
                                        q = q + 2;
                                    } else {
                                        // q+1 后面还有 ASCII，说明它不是单字符词
                                        break;
                                    }
                                } else {
                                    break;
                                }
                            }
                            if (collected.size() > 1) {
                                // 成功合并了多个字符
                                result += collected;
                                p = q;
                                continue;
                            }
                        }
                    }
                }
                // 默认：复制当前字符
                if (uc < 128) {
                    result += *p;
                    ++p;
                } else {
                    int len = Utf8CharLen(uc);
                    result.append(p, len);
                    p += len;
                }
            }
            return result;
        }
    }
}
} // namespace robust
