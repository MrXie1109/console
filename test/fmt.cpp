/**
 * @file fmt.cpp
 * @brief 测试轻量级格式化字符串模块 (format)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */
#include "../include/fmt.h"

#include "../include/test.h"

/**
 * @brief 测试 format 基本字符串无占位符。
 * @note 期望：原样返回字符串。
 */
TEST(FormatNoPlaceholder) {
    std::string result = console::format("Hello, World!");
    ASSERT_EQ(std::string("Hello, World!"), result);
}

/**
 * @brief 测试 format 单个整数参数。
 * @note 期望：正确替换占位符。
 */
TEST(FormatSingleInt) {
    std::string result = console::format("The answer is {}", 42);
    ASSERT_EQ(std::string("The answer is 42"), result);
}

/**
 * @brief 测试 format 多个参数。
 * @note 期望：按顺序正确替换多个占位符。
 */
TEST(FormatMultipleArgs) {
    std::string result = console::format("{} + {} = {}", 1, 2, 3);
    ASSERT_EQ(std::string("1 + 2 = 3"), result);
}

/**
 * @brief 测试 format 字符串参数。
 * @note 期望：字符串参数正确替换。
 */
TEST(FormatStringArg) {
    std::string result = console::format("Hello, {}!", "world");
    ASSERT_EQ(std::string("Hello, world!"), result);
}

/**
 * @brief 测试 format 混合类型参数。
 * @note 期望：不同类型参数正确格式化。
 */
TEST(FormatMixedTypes) {
    std::string result = console::format("{} is {} years old", "Alice", 25);
    ASSERT_EQ(std::string("Alice is 25 years old"), result);
}

/**
 * @brief 测试 format 浮点数参数。
 * @note 期望：浮点数正确格式化。
 */
TEST(FormatFloat) {
    std::string result = console::format("PI is {}", 3.14159);
    ASSERT_STRCONTAINS(result.c_str(), "PI is 3.14159");
}

/**
 * @brief 测试 format 转义花括号 {{ 和 }}。
 * @note 期望：{{ 输出 {，}} 输出 }。
 */
TEST(FormatEscapeBraces) {
    std::string result = console::format("{{}}");
    ASSERT_EQ(std::string("{}"), result);
    result = console::format("{{Hello}}");
    ASSERT_EQ(std::string("{Hello}"), result);
    result = console::format("{{}} {}", "test");
    ASSERT_EQ(std::string("{} test"), result);
}

/**
 * @brief 测试 format 宽度格式化。
 * @note 期望：宽度设置正确。
 */
TEST(FormatWidth) {
    std::string result = console::format("{:10}", 42);
    ASSERT_EQ(std::string("        42"), result);
    result = console::format("{:<10}", 42);
    ASSERT_EQ(std::string("42        "), result);
}

/**
 * @brief 测试 format 填充字符。
 * @note 期望：填充字符正确。
 */
TEST(FormatFill) {
    std::string result = console::format("{:10}", 42);
    ASSERT_EQ(std::string("        42"), result);
    result = console::format("{:<10}", 42);
    ASSERT_EQ(std::string("42        "), result);
    result = console::format("{:*>10}", 42);
    ASSERT_EQ(std::string("********42"), result);
    result = console::format("{:*<10}", 42);
    ASSERT_EQ(std::string("42********"), result);
}

/**
 * @brief 测试 format 十六进制格式化。
 * @note 期望：十六进制输出正确。
 */
TEST(FormatHex) {
    std::string result = console::format("{:x}", 255);
    ASSERT_EQ(std::string("ff"), result);
    result = console::format("{:X}", 255);
    ASSERT_EQ(std::string("FF"), result);
    result = console::format("{:#x}", 255);
    ASSERT_EQ(std::string("0xff"), result);
    result = console::format("{:#X}", 255);
    ASSERT_EQ(std::string("0XFF"), result);
}

/**
 * @brief 测试 format 八进制格式化。
 * @note 期望：八进制输出正确。
 */
TEST(FormatOctal) {
    std::string result = console::format("{:o}", 255);
    ASSERT_EQ(std::string("377"), result);
    result = console::format("{:#o}", 255);
    ASSERT_EQ(std::string("0377"), result);
}

/**
 * @brief 测试 format 十进制格式化。
 * @note 期望：十进制输出正确。
 */
TEST(FormatDecimal) {
    std::string result = console::format("{:d}", 42);
    ASSERT_EQ(std::string("42"), result);
    result = console::format("{:+d}", 42);
    ASSERT_EQ(std::string("+42"), result);
    result = console::format("{:+d}", -42);
    ASSERT_EQ(std::string("-42"), result);
}

/**
 * @brief 测试 format 正数前加空格。
 * @note 期望：正数前加空格，负数前加负号。
 */
TEST(FormatSpace) {
    std::string result = console::format("{: }", 42);
    ASSERT_EQ(std::string(" 42"), result);
    result = console::format("{: }", -42);
    ASSERT_EQ(std::string("-42"), result);
}

/**
 * @brief 测试 format 精度格式化。
 * @note 期望：精度设置正确。
 */
TEST(FormatPrecision) {
    std::string result = console::format("{:.2f}", 3.14159);
    ASSERT_EQ(std::string("3.14"), result);
    result = console::format("{:.5f}", 3.14159);
    ASSERT_EQ(std::string("3.14159"), result);
    result = console::format("{:.2e}", 3.14159);
    ASSERT_STRCONTAINS(result.c_str(), "3.14");
}

/**
 * @brief 测试 format 科学计数法。
 * @note 期望：科学计数法输出正确。
 */
TEST(FormatScientific) {
    std::string result = console::format("{:e}", 1234.56);
    ASSERT_STRCONTAINS(result.c_str(), "1.23456");
    result = console::format("{:E}", 1234.56);
    ASSERT_STRCONTAINS(result.c_str(), "1.23456");
}

/**
 * @brief 测试 format 固定小数。
 * @note 期望：固定小数输出正确。
 */
TEST(FormatFixed) {
    std::string result = console::format("{:.2f}", 1234.567);
    ASSERT_EQ(std::string("1234.57"), result);
    result = console::format("{:.2F}", 1234.567);
    ASSERT_EQ(std::string("1234.57"), result);
}

/**
 * @brief 测试 format 通用格式。
 * @note 期望：通用格式输出正确。
 */
TEST(FormatGeneral) {
    std::string result = console::format("{:g}", 1234.567);
    ASSERT_STRCONTAINS(result.c_str(), "1234.57");
    result = console::format("{:G}", 1234.567);
    ASSERT_STRCONTAINS(result.c_str(), "1234.57");
}

/**
 * @brief 测试 format 空占位符与参数数量不匹配。
 * @note 期望：参数多于占位符时不报错（忽略多余参数）。
 */
TEST(FormatExtraArgs) {
    std::string result = console::format("Hello {}", "world", 42, 3.14);
    ASSERT_EQ(std::string("Hello world"), result);
}

/**
 * @brief 测试 format 参数少于占位符。
 * @note 期望：抛出 BadFormat 异常。
 */
TEST(FormatNotEnoughArgs) {
    ASSERT_THROWS(console::format("{} {}", 42), console::BadFormat);
    ASSERT_THROWS(console::format("{}{}", 42), console::BadFormat);
}

/**
 * @brief 测试 format 未匹配的花括号。
 * @note 期望：抛出 BadFormat 异常。
 */
TEST(FormatUnmatchedBraces) {
    ASSERT_THROWS(console::format("{"), console::BadFormat);
    ASSERT_THROWS(console::format("}"), console::BadFormat);
    ASSERT_THROWS(console::format("{hello"), console::BadFormat);
    ASSERT_THROWS(console::format("hello}"), console::BadFormat);
}

/**
 * @brief 测试 format '+' 和空格同时使用。
 * @note 期望：抛出 BadFormat 异常。
 */
TEST(FormatPlusAndSpace) {
    ASSERT_THROWS(console::format("{:+ }", 42), console::BadFormat);
    ASSERT_THROWS(console::format("{: +}", 42), console::BadFormat);
}

/**
 * @brief 测试 format '#' 与不支持的类型。
 * @note 期望：抛出 BadFormat 异常。
 */
TEST(FormatHashWithInvalidType) {
    ASSERT_THROWS(console::format("{:#s}", "hello"), console::BadFormat);
}

/**
 * @brief 测试 format 精度与不支持的类型。
 * @note 期望：抛出 BadFormat 异常。
 */
TEST(FormatPrecisionWithInvalidType) {
    ASSERT_THROWS(console::format("{:.2d}", 42), console::BadFormat);
}

/**
 * @brief 测试 format 布尔值。
 * @note 期望：布尔值输出 true 或 false。
 */
TEST(FormatBool) {
    std::string result = console::format("{}", true);
    ASSERT_EQ(std::string("true"), result);
    result = console::format("{}", false);
    ASSERT_EQ(std::string("false"), result);
}

/**
 * @brief 测试 format 空字符串。
 * @note 期望：返回空字符串。
 */
TEST(FormatEmptyString) {
    std::string result = console::format("");
    ASSERT_EQ(std::string(""), result);
}

/**
 * @brief 测试 format 宽字符版本 (wformat)。
 * @note 期望：宽字符格式化正确。
 */
TEST(WFormatBasic) {
    std::wstring result = console::format(L"Hello, {}!", L"world");
    ASSERT_EQ(std::wstring(L"Hello, world!"), result);
}

/**
 * @brief 测试 format 宽字符版本多个参数。
 * @note 期望：宽字符多个参数正确替换。
 */
TEST(WFormatMultipleArgs) {
    std::wstring result = console::format(L"{} + {} = {}", 1, 2, 3);
    ASSERT_EQ(std::wstring(L"1 + 2 = 3"), result);
}

/**
 * @brief 测试 format 宽字符版本宽度格式化。
 * @note 期望：宽度设置正确。
 */
TEST(WFormatWidth) {
    std::wstring result = console::format(L"{:10}", 42);
    ASSERT_EQ(std::wstring(L"        42"), result);
    result = console::format(L"{:*>10}", 42);
    ASSERT_EQ(std::wstring(L"********42"), result);
}

/**
 * @brief 测试 format 宽字符版本浮点数。
 * @note 期望：浮点数正确格式化。
 */
TEST(WFormatFloat) {
    std::wstring result = console::format(L"{:.2f}", 3.14159);
    ASSERT_EQ(std::wstring(L"3.14"), result);
}

/**
 * @brief 测试 format 宽字符版本十六进制。
 * @note 期望：十六进制输出正确。
 */
TEST(WFormatHex) {
    std::wstring result = console::format(L"{:x}", 255);
    ASSERT_EQ(std::wstring(L"ff"), result);
    result = console::format(L"{:#X}", 255);
    ASSERT_EQ(std::wstring(L"0XFF"), result);
}

/**
 * @brief 测试 format 宽字符版本转义花括号。
 * @note 期望：{{ 输出 {，}} 输出 }。
 */
TEST(WFormatEscapeBraces) {
    std::wstring result = console::format(L"{{}}");
    ASSERT_EQ(std::wstring(L"{}"), result);
}

#ifndef NOMAIN
TEST_MAIN
#endif
