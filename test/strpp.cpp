/**
 * @file strpp.cpp
 * @brief 测试字符串处理模块 (strpp)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/strpp.h"

#include <string>
#include <vector>

#include "../include/test.h"

/**
 * @brief 测试 ltrim 移除左侧空格。
 * @note 期望：移除左侧空格，保留右侧空格。
 */
TEST(StrppLtrim) {
    std::string s      = "  hello  ";
    std::string result = console::ltrim(s);
    ASSERT_EQ(std::string("hello  "), result);
}

/**
 * @brief 测试 rtrim 移除右侧空格。
 * @note 期望：移除右侧空格，保留左侧空格。
 */
TEST(StrppRtrim) {
    std::string s      = "  hello  ";
    std::string result = console::rtrim(s);
    ASSERT_EQ(std::string("  hello"), result);
}

/**
 * @brief 测试 trim 移除两侧空格。
 * @note 期望：移除两侧空格。
 */
TEST(StrppTrim) {
    std::string s      = "  hello  ";
    std::string result = console::trim(s);
    ASSERT_EQ(std::string("hello"), result);
}

/**
 * @brief 测试 trim 空字符串。
 * @note 期望：空字符串返回空。
 */
TEST(StrppTrimEmpty) {
    std::string s      = "";
    std::string result = console::trim(s);
    ASSERT_EQ(std::string(""), result);
}

/**
 * @brief 测试 trim 全部空格。
 * @note 期望：全部空格返回空。
 */
TEST(StrppTrimAllWhitespace) {
    std::string s      = "   \t  \n  ";
    std::string result = console::trim(s);
    ASSERT_EQ(std::string(""), result);
}

/**
 * @brief 测试 ltrim 自定义字符集。
 * @note 期望：移除左侧指定字符。
 */
TEST(StrppLtrimCustom) {
    std::string s      = "xxxhello";
    std::string result = console::ltrim(s, "x");
    ASSERT_EQ(std::string("hello"), result);
}

/**
 * @brief 测试 rtrim 自定义字符集。
 * @note 期望：移除右侧指定字符。
 */
TEST(StrppRtrimCustom) {
    std::string s      = "helloxxx";
    std::string result = console::rtrim(s, "x");
    ASSERT_EQ(std::string("hello"), result);
}

/**
 * @brief 测试 trim 自定义字符集。
 * @note 期望：移除两侧指定字符。
 */
TEST(StrppTrimCustom) {
    std::string s      = "xxxhelloxxx";
    std::string result = console::trim(s, "x");
    ASSERT_EQ(std::string("hello"), result);
}

/**
 * @brief 测试 trim 自定义字符集多处。
 * @note 期望：移除所有指定字符。
 */
TEST(StrppTrimCustomMultiple) {
    std::string s      = "abcHelloabc";
    std::string result = console::trim(s, "abc");
    ASSERT_EQ(std::string("Hello"), result);
}

/**
 * @brief 测试 upper 转大写。
 * @note 期望：字符串转为大写。
 */
TEST(StrppUpper) {
    std::string s      = "hello world";
    std::string result = console::upper(s);
    ASSERT_EQ(std::string("HELLO WORLD"), result);
}

/**
 * @brief 测试 lower 转小写。
 * @note 期望：字符串转为小写。
 */
TEST(StrppLower) {
    std::string s      = "HELLO WORLD";
    std::string result = console::lower(s);
    ASSERT_EQ(std::string("hello world"), result);
}

/**
 * @brief 测试 title 标题格式。
 * @note 期望：每个单词首字母大写，其余小写。
 */
TEST(StrppTitle) {
    std::string s      = "hello world test";
    std::string result = console::title(s);
    ASSERT_EQ(std::string("Hello World Test"), result);
}

/**
 * @brief 测试 title 已有大写。
 * @note 期望：单词首字母大写，其余小写。
 */
TEST(StrppTitleMixed) {
    std::string s      = "hELLO wORLD tEST";
    std::string result = console::title(s);
    ASSERT_EQ(std::string("Hello World Test"), result);
}

/**
 * @brief 测试 title 空字符串。
 * @note 期望：空字符串返回空。
 */
TEST(StrppTitleEmpty) {
    std::string s      = "";
    std::string result = console::title(s);
    ASSERT_EQ(std::string(""), result);
}

/**
 * @brief 测试 partition 找到分隔符。
 * @note 期望：正确分割为三部分。
 */
TEST(StrppPartitionFound) {
    std::string text   = "hello-world-test";
    auto        result = console::partition(text, "-");
    ASSERT_EQ(std::string("hello"), result.left);
    ASSERT_EQ(std::string("-"), result.middle);
    ASSERT_EQ(std::string("world-test"), result.right);
}

/**
 * @brief 测试 partition 未找到分隔符。
 * @note 期望：left 为原字符串，middle 和 right 为空。
 */
TEST(StrppPartitionNotFound) {
    std::string text   = "hello world";
    auto        result = console::partition(text, "-");
    ASSERT_EQ(std::string("hello world"), result.left);
    ASSERT_EQ(std::string(""), result.middle);
    ASSERT_EQ(std::string(""), result.right);
}

/**
 * @brief 测试 partition 空字符串。
 * @note 期望：空字符串返回空。
 */
TEST(StrppPartitionEmpty) {
    std::string text   = "";
    auto        result = console::partition(text, "-");
    ASSERT_EQ(std::string(""), result.left);
    ASSERT_EQ(std::string(""), result.middle);
    ASSERT_EQ(std::string(""), result.right);
}

/**
 * @brief 测试 split 默认按空格分割。
 * @note 期望：按空格分割字符串。
 */
TEST(StrppSplitDefault) {
    std::string              text     = "hello world test";
    auto                     result   = console::split(text);
    std::vector<std::string> expected = {"hello", "world", "test"};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 split 自定义分隔符。
 * @note 期望：按自定义分隔符分割。
 */
TEST(StrppSplitCustom) {
    std::string              text     = "hello-world-test";
    auto                     result   = console::split(text, "-");
    std::vector<std::string> expected = {"hello", "world", "test"};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 split 连续分隔符。
 * @note 期望：连续分隔符产生空字符串。
 */
TEST(StrppSplitConsecutive) {
    std::string              text     = "hello--world---test";
    auto                     result   = console::split(text, "--");
    std::vector<std::string> expected = {"hello", "world", "-test"};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 split 空分隔符。
 * @note 期望：空分隔符返回原字符串。
 */
TEST(StrppSplitEmptySep) {
    std::string              text     = "hello";
    auto                     result   = console::split(text, "");
    std::vector<std::string> expected = {"hello"};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 join 连接字符串。
 * @note 期望：用分隔符连接。
 */
TEST(StrppJoin) {
    std::vector<std::string> vec    = {"hello", "world", "test"};
    std::string              result = console::join(vec, "-");
    ASSERT_EQ(std::string("hello-world-test"), result);
}

/**
 * @brief 测试 join 空向量。
 * @note 期望：空向量返回空字符串。
 */
TEST(StrppJoinEmpty) {
    std::vector<std::string> vec;
    std::string              result = console::join(vec, "-");
    ASSERT_EQ(std::string(""), result);
}

/**
 * @brief 测试 join 单元素。
 * @note 期望：单元素返回该元素。
 */
TEST(StrppJoinSingle) {
    std::vector<std::string> vec    = {"hello"};
    std::string              result = console::join(vec, "-");
    ASSERT_EQ(std::string("hello"), result);
}

/**
 * @brief 测试 join 默认分隔符。
 * @note 期望：默认分隔符为空。
 */
TEST(StrppJoinDefaultSep) {
    std::vector<std::string> vec    = {"hello", "world"};
    std::string              result = console::join(vec);
    ASSERT_EQ(std::string("helloworld"), result);
}

/**
 * @brief 测试 partition C 字符串重载。
 * @note 期望：正确分割。
 */
TEST(StrppPartitionCString) {
    auto result = console::partition("hello-world", "-");
    ASSERT_EQ(std::string("hello"), result.left);
    ASSERT_EQ(std::string("-"), result.middle);
    ASSERT_EQ(std::string("world"), result.right);
}

/**
 * @brief 测试 split C 字符串重载。
 * @note 期望：正确分割。
 */
TEST(StrppSplitCString) {
    auto                     result   = console::split("hello world test");
    std::vector<std::string> expected = {"hello", "world", "test"};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 upper C 字符串重载。
 * @note 期望：转为大写。
 */
TEST(StrppUpperCString) {
    std::string result = console::upper("hello");
    ASSERT_EQ(std::string("HELLO"), result);
}

/**
 * @brief 测试 lower C 字符串重载。
 * @note 期望：转为小写。
 */
TEST(StrppLowerCString) {
    std::string result = console::lower("HELLO");
    ASSERT_EQ(std::string("hello"), result);
}

/**
 * @brief 测试 title C 字符串重载。
 * @note 期望：标题格式。
 */
TEST(StrppTitleCString) {
    std::string result = console::title("hello world");
    ASSERT_EQ(std::string("Hello World"), result);
}

/**
 * @brief 测试 to_string 多参数。
 * @note 期望：所有参数拼接。
 */
TEST(StrppToString) {
    std::string result = console::to_string("Hello", " ", 42, " ", 3.14);
    ASSERT_EQ(std::string("Hello 42 3.14"), result);
}

/**
 * @brief 测试 to_string 单参数。
 * @note 期望：单个参数转字符串。
 */
TEST(StrppToStringSingle) {
    std::string result = console::to_string(42);
    ASSERT_EQ(std::string("42"), result);
}

/**
 * @brief 测试 to_string 空参数。
 * @note 期望：返回空字符串。
 */
TEST(StrppToStringEmpty) {
    std::string result = console::to_string();
    ASSERT_EQ(std::string(""), result);
}

/**
 * @brief 测试 from_string 提取参数。
 * @note 期望：正确提取多个参数。
 */
TEST(StrppFromString) {
    std::string str = "42 3.14 hello";
    int         a;
    double      b;
    std::string c;
    console::from_string(str, a, b, c);
    ASSERT_EQ(42, a);
    ASSERT_NEAR(3.14, b, 0.001);
    ASSERT_EQ(std::string("hello"), c);
}

/**
 * @brief 测试 from_string 单参数。
 * @note 期望：正确提取单参数。
 */
TEST(StrppFromStringSingle) {
    std::string str = "42";
    int         a;
    console::from_string(str, a);
    ASSERT_EQ(42, a);
}

/**
 * @brief 测试 trim 宽字符。
 * @note 期望：宽字符修剪正确。
 */
TEST(StrppTrimWide) {
    std::wstring s      = L"  hello  ";
    std::wstring result = console::trim(s);
    ASSERT_EQ(std::wstring(L"hello"), result);
}

/**
 * @brief 测试 split 宽字符。
 * @note 期望：宽字符分割正确。
 */
TEST(StrppSplitWide) {
    std::wstring              text     = L"hello world test";
    auto                      result   = console::split(text, L" ");
    std::vector<std::wstring> expected = {L"hello", L"world", L"test"};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 join 宽字符。
 * @note 期望：宽字符连接正确。
 */
TEST(StrppJoinWide) {
    std::vector<std::wstring> vec    = {L"hello", L"world", L"test"};
    std::wstring              result = console::join(vec, L"-");
    ASSERT_EQ(std::wstring(L"hello-world-test"), result);
}

/**
 * @brief 测试 trim 自定义字符集空字符串。
 * @note 期望：空字符串返回空。
 */
TEST(StrppTrimCustomEmpty) {
    std::string s      = "";
    std::string result = console::trim(s, "x");
    ASSERT_EQ(std::string(""), result);
}

/**
 * @brief 测试 PartitionResult 流输出。
 * @note 期望：输出格式正确。
 */
TEST(StrppPartitionResultOutput) {
    console::PartitionResult pr{"left", "middle", "right"};
    std::ostringstream       oss;
    oss << pr;
    ASSERT_EQ(
        std::string("BasicPartitionResult(\"left\", \"middle\", \"right\")"),
        oss.str());
}

#ifndef NOMAIN
TEST_MAIN
#endif
