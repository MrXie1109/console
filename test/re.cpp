/**
 * @file re.cpp
 * @brief 测试正则表达式模块 (BasicRegex, re)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/re.h"

#include <string>

#include "../include/test.h"

/**
 * @brief 测试 Regex 简单匹配。
 * @note 期望：正确匹配简单模式。
 */
TEST(RegexMatchSimple) {
    console::Regex r("hello");
    auto           m = r.match("hello");
    ASSERT_TRUE(static_cast<bool>(m));
    ASSERT_EQ(std::string("hello"), m.group());
}

/**
 * @brief 测试 Regex match 不匹配。
 * @note 期望：不匹配返回 false。
 */
TEST(RegexMatchNoMatch) {
    console::Regex r("hello");
    auto           m = r.match("world");
    ASSERT_FALSE(static_cast<bool>(m));
    ASSERT_EQ(std::string(""), m.group());
}

/**
 * @brief 测试 Regex search 查找子串。
 * @note 期望：正确查找子串。
 */
TEST(RegexSearchSubstring) {
    console::Regex r("world");
    auto           m = r.search("hello world");
    ASSERT_TRUE(static_cast<bool>(m));
    ASSERT_EQ(std::string("world"), m.group());
    ASSERT_EQ(6, m.start());
    ASSERT_EQ(11, m.end());
    auto span = m.span();
    ASSERT_EQ(6, span.first);
    ASSERT_EQ(11, span.second);
}

/**
 * @brief 测试 Regex search 不匹配。
 * @note 期望：不匹配返回 false。
 */
TEST(RegexSearchNoMatch) {
    console::Regex r("xyz");
    auto           m = r.search("hello world");
    ASSERT_FALSE(static_cast<bool>(m));
}

/**
 * @brief 测试 Regex 捕获组。
 * @note 期望：正确提取捕获组。
 */
TEST(RegexCaptureGroup) {
    console::Regex r("(\\d+)-(\\w+)");
    auto           m = r.match("123-abc");
    ASSERT_TRUE(static_cast<bool>(m));
    ASSERT_EQ(std::string("123-abc"), m.group(0));
    ASSERT_EQ(std::string("123"), m.group(1));
    ASSERT_EQ(std::string("abc"), m.group(2));
    auto groups = m.groups();
    ASSERT_EQ(2, groups.size());
    ASSERT_EQ(std::string("123"), groups[0]);
    ASSERT_EQ(std::string("abc"), groups[1]);
}

/**
 * @brief 测试 Regex 捕获组位置。
 * @note 期望：正确返回捕获组位置。
 */
TEST(RegexCapturePosition) {
    console::Regex r("(\\d+)-(\\w+)");
    auto           m = r.match("123-abc");
    ASSERT_TRUE(static_cast<bool>(m));
    ASSERT_EQ(0, m.start(0));
    ASSERT_EQ(7, m.end(0));
    ASSERT_EQ(0, m.start(1));
    ASSERT_EQ(3, m.end(1));
    ASSERT_EQ(4, m.start(2));
    ASSERT_EQ(7, m.end(2));
}

/**
 * @brief 测试 Regex findall。
 * @note 期望：返回所有匹配的子串。
 */
TEST(RegexFindAll) {
    console::Regex           r("\\d+");
    auto                     results  = r.findall("a1 b22 c333 d4444");
    std::vector<std::string> expected = {"1", "22", "333", "4444"};
    ASSERT_EQ(expected, results);
}

/**
 * @brief 测试 Regex findall 无匹配。
 * @note 期望：返回空 vector。
 */
TEST(RegexFindAllNoMatch) {
    console::Regex r("\\d+");
    auto           results = r.findall("abc def");
    ASSERT_TRUE(results.empty());
}

/**
 * @brief 测试 Regex split。
 * @note 期望：按正则表达式分割字符串。
 */
TEST(RegexSplit) {
    console::Regex           r("\\s+");
    auto                     parts    = r.split("one two  three   four");
    std::vector<std::string> expected = {"one", "two", "three", "four"};
    ASSERT_EQ(expected, parts);
}

/**
 * @brief 测试 Regex split 带最大分割数。
 * @note 期望：限制分割次数。
 */
TEST(RegexSplitMaxSplit) {
    console::Regex           r("\\s+");
    auto                     parts    = r.split("one two three four", 2);
    std::vector<std::string> expected = {"one", "two", "three four"};
    ASSERT_EQ(expected, parts);
}

/**
 * @brief 测试 Regex sub 替换。
 * @note 期望：正确替换匹配的子串。
 */
TEST(RegexSub) {
    console::Regex r("\\d+");
    auto           result = r.sub("X", "a1 b22 c333");
    ASSERT_EQ(std::string("aX bX cX"), result);
}

/**
 * @brief 测试 Regex sub 限制替换次数。
 * @note 期望：限制替换次数。
 */
TEST(RegexSubCount) {
    console::Regex r("\\d+");
    auto           result = r.sub("X", "a1 b22 c333", 2);
    ASSERT_EQ(std::string("aX bX c333"), result);
}

/**
 * @brief 测试 Regex subn 返回替换次数。
 * @note 期望：返回替换后的字符串和替换次数。
 */
TEST(RegexSubn) {
    console::Regex r("\\d+");
    auto [result, count] = r.subn("X", "a1 b22 c333");
    ASSERT_EQ(std::string("aX bX cX"), result);
    ASSERT_EQ(3, count);
}

/**
 * @brief 测试 Regex subn 限制替换次数。
 * @note 期望：限制替换次数。
 */
TEST(RegexSubnCount) {
    console::Regex r("\\d+");
    auto [result, count] = r.subn("X", "a1 b22 c333", 2);
    ASSERT_EQ(std::string("aX bX c333"), result);
    ASSERT_EQ(2, count);
}

/**
 * @brief 测试 re::compile 函数。
 * @note 期望：正确编译正则表达式。
 */
TEST(ReCompile) {
    auto r = console::re::compile("hello");
    auto m = r.match("hello");
    ASSERT_TRUE(static_cast<bool>(m));
}

/**
 * @brief 测试 re::search 函数。
 * @note 期望：正确搜索匹配。
 */
TEST(ReSearch) {
    auto m = console::re::search("world", "hello world");
    ASSERT_TRUE(static_cast<bool>(m));
    ASSERT_EQ(std::string("world"), m.group());
}

/**
 * @brief 测试 re::match 函数。
 * @note 期望：完整匹配。
 */
TEST(ReMatch) {
    auto m = console::re::match("hello", "hello");
    ASSERT_TRUE(static_cast<bool>(m));
    ASSERT_EQ(std::string("hello"), m.group());
    auto m2 = console::re::match("world", "hello");
    ASSERT_FALSE(static_cast<bool>(m2));
}

/**
 * @brief 测试 re::findall 函数。
 * @note 期望：返回所有匹配。
 */
TEST(ReFindAll) {
    auto results = console::re::findall("\\d+", "a1 b22 c333");
    std::vector<std::string> expected = {"1", "22", "333"};
    ASSERT_EQ(expected, results);
}

/**
 * @brief 测试 re::split 函数。
 * @note 期望：按正则分割。
 */
TEST(ReSplit) {
    auto parts = console::re::split("\\s+", "one two three");
    std::vector<std::string> expected = {"one", "two", "three"};
    ASSERT_EQ(expected, parts);
}

/**
 * @brief 测试 re::sub 函数。
 * @note 期望：正确替换。
 */
TEST(ReSub) {
    auto result = console::re::sub("\\d+", "X", "a1 b22 c333");
    ASSERT_EQ(std::string("aX bX cX"), result);
}

/**
 * @brief 测试 re::escape 函数。
 * @note 期望：正确转义特殊字符。
 */
TEST(ReEscape) {
    auto escaped = console::re::escape("a.b*c?");
    // 转义后每个特殊字符前加反斜杠
    ASSERT_STRCONTAINS(escaped.c_str(), "\\.");
    ASSERT_STRCONTAINS(escaped.c_str(), "\\*");
    ASSERT_STRCONTAINS(escaped.c_str(), "\\?");
}

/**
 * @brief 测试 BasicRegex 宽字符版本。
 * @note 期望：宽字符正则正确工作。
 */
TEST(WRegexBasic) {
    console::WRegex r(L"hello");
    auto            m = r.match(L"hello");
    ASSERT_TRUE(static_cast<bool>(m));
    ASSERT_EQ(std::wstring(L"hello"), m.group());
}

/**
 * @brief 测试 WRegex 捕获组。
 * @note 期望：宽字符捕获组正确工作。
 */
TEST(WRegexCapture) {
    console::WRegex r(L"(\\d+)-(\\w+)");
    auto            m = r.match(L"123-abc");
    ASSERT_TRUE(static_cast<bool>(m));
    ASSERT_EQ(std::wstring(L"123-abc"), m.group(0));
    ASSERT_EQ(std::wstring(L"123"), m.group(1));
    ASSERT_EQ(std::wstring(L"abc"), m.group(2));
}

/**
 * @brief 测试 WRegex findall。
 * @note 期望：宽字符 findall 正确工作。
 */
TEST(WRegexFindAll) {
    console::WRegex           r(L"\\d+");
    auto                      results  = r.findall(L"a1 b22 c333");
    std::vector<std::wstring> expected = {L"1", L"22", L"333"};
    ASSERT_EQ(expected, results);
}

/**
 * @brief 测试 WRegex split。
 * @note 期望：宽字符 split 正确工作。
 */
TEST(WRegexSplit) {
    console::WRegex           r(L"\\s+");
    auto                      parts    = r.split(L"one two three");
    std::vector<std::wstring> expected = {L"one", L"two", L"three"};
    ASSERT_EQ(expected, parts);
}

/**
 * @brief 测试 WRegex sub。
 * @note 期望：宽字符 sub 正确工作。
 */
TEST(WRegexSub) {
    console::WRegex r(L"\\d+");
    auto            result = r.sub(L"X", L"a1 b22 c333");
    ASSERT_EQ(std::wstring(L"aX bX cX"), result);
}

/**
 * @brief 测试 Regex 使用标志。
 * @note 期望：标志正确影响匹配。
 */
TEST(RegexFlags) {
    console::Regex r("hello", std::regex::icase);
    auto           m = r.match("HELLO");
    ASSERT_TRUE(static_cast<bool>(m));
    ASSERT_EQ(std::string("HELLO"), m.group());
}

/**
 * @brief 测试 Regex fullmatch 别名。
 * @note 期望：fullmatch 等同于 match。
 */
TEST(RegexFullMatch) {
    console::Regex r("hello");
    auto           m = r.fullmatch("hello");
    ASSERT_TRUE(static_cast<bool>(m));
    auto m2 = r.fullmatch("hello world");
    ASSERT_FALSE(static_cast<bool>(m2));
}

/**
 * @brief 测试 Match 空字符串。
 * @note 期望：空字符串匹配正确。
 */
TEST(RegexMatchEmpty) {
    console::Regex r("");
    auto           m = r.match("");
    ASSERT_TRUE(static_cast<bool>(m));
    ASSERT_EQ(std::string(""), m.group());
}

#ifndef NOMAIN
TEST_MAIN
#endif
