// -*- c++ -*-
// 测试 tts_robust_normalizer 功能
// 编译后运行: ./test_tts_normalizer

#include "tts_robust_normalizer.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

using namespace imtts::zh_normalizer::robust;

static int g_failed = 0;
static int g_passed = 0;

// ─────────────────────────────────────
// 简单断言
// ─────────────────────────────────────

static void check(const std::string& name,
                  const std::string& input,
                  const std::string& expected)
{
    std::cout << "start test: " << name << std::endl;
    std::string actual = NormalizeTtsText(input);
    if (actual != expected) {
        ++g_failed;
        std::cerr << "FAIL [" << name << "]\n"
                  << "  input   : " << input << "\n"
                  << "  expected: " << expected << "\n"
                  << "  actual  : " << actual << "\n\n";
    } else {
        ++g_passed;
    }
    // 幂等性: 再次归一化不应改变结果
    std::string second = NormalizeTtsText(actual);
    if (second != actual) {
        ++g_failed;
        std::cerr << "FAIL [" << name << "_idempotence]\n"
                  << "  first : " << actual << "\n"
                  << "  second: " << second << "\n\n";
    }
}

// ─────────────────────────────────────
// 测试用例
// ─────────────────────────────────────

static void test_normalize_tts_text()
{
    check(u8"dot_map_sentence",
        u8"2026 年 3 月 31 日，安全研究员 Chaofan Shou (@Fried_rice) 发现 Anthropic 的 npm 包中暴露了 .map 文件，",
        u8"2026年3月31日，安全研究员 Chaofan Shou (@Fried_rice) 发现 Anthropic 的 npm 包中暴露了 .map 文件，");
    check(u8"dot_tokens", u8"别把 .env、.npmrc、.gitignore 提交上去。", "别把 .env、.npmrc、.gitignore 提交上去。");
    check(u8"file_names", u8"请检查 bundle.min.js、package.json 和 processing_moss_tts.py。", u8"请检查 bundle.min.js、package.json 和 processing_moss_tts.py。");
    check(u8"index_d_ts", u8"index.d.ts 里也有同样的问题。", u8"index.d.ts 里也有同样的问题。");
    check(u8"version_build", u8"Bug 的讨论可以精确到 v2.3.1 (Build 15)。", u8"Bug 的讨论可以精确到 v2.3.1 (Build 15)。");
    check(u8"version_rc", u8"3.0.0-rc.1 还不能上生产。", u8"3.0.0-rc.1 还不能上生产。");
    check(u8"jar_name", u8"fabric-api-0.91.3+1.20.2.jar 需要单独下载。", u8"fabric-api-0.91.3+1.20.2.jar 需要单独下载。");

    //2) URL / Email / mention / hashtag / Reddit
    check(u8"url", u8"仓库地址是 https://github.com/instructkr/claude-code", u8"仓库地址是 https://github.com/instructkr/claude-code。");
    check(u8"email", u8"联系邮箱：ops+tts@example.ai", u8"联系邮箱：ops+tts@example.ai。");
    check(u8"mention", u8"@Fried_rice 说这是 source map 暴露。", u8"@Fried_rice 说这是 source map 暴露。");
    check(u8"reddit", u8"去 r/singularity 看讨论。", u8"去 r/singularity 看讨论。");
    check(u8"hashtag_chain", u8"#张雪峰#张雪峰[话题]#张雪峰事件", u8"#张雪峰#张雪峰[话题]#张雪峰事件。");
    check(u8"mention_hashtag_boundary", u8"关注@biscuit0228_并转发#thetime_tbs", u8"关注 @biscuit0228_ 并转发 #thetime_tbs。");

    //3) bracket / 控制 token：统一转成双引号
    check(u8"speaker_bracket", u8"[S1]你好。[S2]收到。", u8"\"S1\"你好。\"S2\"收到。");
    check(u8"event_bracket", u8"请模仿 {whisper} 的语气说“别出声”。", u8"请模仿 \"whisper\" 的语气说“别出声”。");
    check(u8"order_bracket", u8"订单号：[AB-1234-XYZ]", u8"订单号：\"AB-1234-XYZ\"。");

    //4) 结构性符号：转成双引号或句边界，而不是直接删除
    check(u8"struct_headline", u8"〖重磅〗《新品发布》——现在开始！", u8"\"重磅\"《新品发布》。现在开始！");
    check(u8"struct_notice", u8"【公告】今天 20:00 维护——预计 30 分钟。", u8"\"公告\"今天20:00维护。预计30分钟。");
    check(u8"struct_quote_chain", u8"『特别提醒』「不要外传」", u8"\"特别提醒\"\"不要外传\"。");
    check(u8"struct_embedded_quote", u8"他说【重要通知】明天发布。", u8"他说\"重要通知\"明天发布。");
    check(u8"flow_arrow_chain", u8"请求接入 -> 身份与策略判定 -> 域服务处理", u8"请求接入，身份与策略判定，域服务处理。");
    check(u8"flow_arrow_no_space", u8"A->B", u8"A，B。");
    check(u8"flow_arrow_unicode", u8"配置中心→推理编排→运行时执行", u8"配置中心，推理编排，运行时执行。");

    //5) 嵌入式标题：保留
    check(u8"embedded_title", u8"我喜欢《哈姆雷特》这本书。", u8"我喜欢《哈姆雷特》这本书。");

    //6) 重复标点 / 社交噪声
    check(u8"noise_qe", u8"真的假的？？？！！！", u8"真的假的？！");
    check(u8"noise_ellipsis", u8"这个包把 app.js.map 也发上去了......太离谱了！！！", u8"这个包把 app.js.map 也发上去了。太离谱了！");
    check(u8"noise_ellipsis_cn", u8"【系统提示】请模仿{sad}低沉语气，说“今天下雨了……”", u8"\"系统提示\"请模仿\"sad\"低沉语气，说“今天下雨了。”");

    //7) 空格规则：英文压缩、中文删除、中英混排保留边界
    check(u8"english_spaces", u8"This   is   a   test.", u8"This is a test。");
    check(u8"chinese_spaces", u8"这 是　一 段  含有多种空白的文本。", u8"这是一段含有多种空白的文本。");
    check(u8"mixed_spaces_1", u8"这是Anthropic的npm包", u8"这是 Anthropic 的 npm 包。");
    check(u8"mixed_spaces_2", u8"今天update到v2.3.1了", u8"今天 update 到 v2.3.1 了。");
    check(u8"mixed_spaces_3", u8"处理app.js.map文件", u8"处理 app.js.map 文件。");
    check(u8"underscore_plain_1", u8"foo_bar", u8"foo bar。");
    check(u8"underscore_plain_2", u8"中文_ABC", u8"中文 ABC。");
    check(u8"underscore_protected_mention", u8"关注@foo_bar", u8"关注 @foo_bar。");

    //8) Markdown / 列表 / 换行
    check(u8"markdown_link", u8"详情见 [release note](https://github.com/example/release)", u8"详情见 release note https://github.com/example/release。");
    check(u8"markdown_heading", u8"//I made a free open source app to help with markdown files", u8"I made a free open source app to help with markdown files。");
    check(u8"list_lines", u8"- 修复 .map 泄露\n- 发布 v2.3.1", u8"修复 .map 泄露。发布 v2.3.1。");
    check(u8"numbered_lines", u8"1. 安装依赖\n2. 运行测试\n3. 发布 v2.3.1", u8"安装依赖。运行测试。发布 v2.3.1。");
    check(u8"newlines", u8"第一行\n第二行\n第三行", u8"第一行。第二行。第三行。");

    //9) 句末补标点
    check(u8"terminal_punct_plain", u8"今天发布", u8"今天发布。");
    check(u8"terminal_punct_quoted", u8"他说\"你好\"", u8"他说\"你好\"。");
    check(u8"terminal_punct_existing", u8"今天发布。", u8"今天发布。");
    check(u8"terminal_punct_newlines", u8"第一行\n第二行。", u8"第一行。第二行。");
    check(u8"terminal_punct_blank_lines", u8"第一行\n\n第二行", u8"第一行。第二行。");

    //10) 零宽字符 / 幂等性
    check("zero_width_url", "详见 https://x.com/\u200bSafety", "详见 https://x.com/Safety。");
}

// ─────────────────────────────────────
// rewrite_hyphens_before_zh_wetext 测试
// ─────────────────────────────────────

static void test_rewrite_hyphens()
{
    auto check_hyphen =
        [](const std::string& name, const std::string& input, const std::string& expected) {
            std::string actual = RewriteHyphensBeforeZhWetext(input);
            ++g_passed;
            if (actual != expected) {
                --g_passed;
                ++g_failed;
                std::cerr << "FAIL [" << name << "]\n"
                          << "  input   : " << input << "\n"
                          << "  expected: " << expected << "\n"
                          << "  actual  : " << actual << "\n\n";
            }
        };

    // 数字-数字保留
    check_hyphen("num_range_1", u8"10-3", u8"10-3");
    check_hyphen("num_range_2", u8"2024-05-01", u8"2024-05-01");

    // 负号保留
    check_hyphen("head_neg_1", u8"-2", u8"-2");
    check_hyphen("head_neg_2", u8"  -2", u8" -2");
    check_hyphen("delim_neg_1", u8"x=-2", u8"x=-2");
    check_hyphen("delim_neg_2", u8"(-2)", u8"(-2)");
    check_hyphen("zh_neg_1", u8"为-2", u8"为-2");
    check_hyphen("zh_neg_2", u8"为 -2", u8"为-2");

    // 中文-中文 → 停顿
    check_hyphen("zh_zh_1", u8"请求接入-身份判定", u8"请求接入，身份判定");
    check_hyphen("zh_zh_2", u8"请求接入 - 身份判定", u8"请求接入，身份判定");

    // 其余 → 空格
    check_hyphen("other_1", u8"A-B", u8"A B");
    check_hyphen("other_2", u8"A - B", u8"A B");
    check_hyphen("other_3", u8"GPU-A100", u8"GPU A100");
    check_hyphen("other_4", u8"中文-ABC", u8"中文 ABC");
    check_hyphen("other_5", u8"ABC-中文", u8"ABC 中文");
}

// ─────────────────────────────────────
int main()
{
    std::cout << "Running tests...\n\n";

    test_normalize_tts_text();
    std::cout << "[normalize_tts_text] passed=" << g_passed << " failed=" << g_failed << "\n";

    size_t prev_passed = g_passed;
    size_t prev_failed = g_failed;
    g_passed = 0;
    g_failed = 0;
    test_rewrite_hyphens();
    std::cout << "[rewrite_hyphens]  passed=" << g_passed << " failed=" << g_failed << "\n";

    g_passed += prev_passed;
    g_failed += prev_failed;

    if (g_failed > 0) {
        std::cerr << "\n*** Total failed: " << g_failed << " ***\n";
        return 1;
    }
    std::cout << "\nAll tests passed (" << g_passed << ").\n";
    return 0;
}
