/**
 * @file input.cpp
 * @brief 测试控制台输入模块。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */
#include "../include/input.h"

#include <sstream>
#include <string>

#include "../include/test.h"

/**
 * @brief 测试 input 读取字符串。
 * @note 期望：正确读取输入的字符串。
 */
TEST(InputString) {
    std::istringstream                iss("hello\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    std::string result = console::input<std::string>("Enter: ", settings);
    ASSERT_EQ(std::string("hello"), result);
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "Enter: ");
}

/**
 * @brief 测试 input 读取整数。
 * @note 期望：正确读取输入的整数。
 */
TEST(InputInt) {
    std::istringstream                iss("42\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    int result = console::input<int>("Enter number: ", settings);
    ASSERT_EQ(42, result);
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "Enter number: ");
}

/**
 * @brief 测试 input 读取浮点数。
 * @note 期望：正确读取输入的浮点数。
 */
TEST(InputDouble) {
    std::istringstream                iss("3.14\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    double result = console::input<double>("Enter pi: ", settings);
    ASSERT_NEAR(3.14, result, 0.001);
}

/**
 * @brief 测试 input 读取多种类型。
 * @note 期望：正确读取不同类型。
 */
TEST(InputMultipleTypes) {
    std::istringstream                iss("hello\n3.14\n42\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    std::string s = console::input<std::string>("Enter string: ", settings);
    double      d = console::input<double>("Enter double: ", settings);
    int         i = console::input<int>("Enter int: ", settings);
    ASSERT_EQ(std::string("hello"), s);
    ASSERT_NEAR(3.14, d, 0.001);
    ASSERT_EQ(42, i);
}

/**
 * @brief 测试 input_line 读取整行。
 * @note 期望：正确读取包含空格的整行。
 */
TEST(InputLine) {
    std::istringstream                iss("Hello World!\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    std::string result = console::input_line("Enter: ", settings);
    ASSERT_EQ(std::string("Hello World!"), result);
}

/**
 * @brief 测试 input_line 读取空行。
 * @note 期望：读取空行返回空字符串。
 */
TEST(InputLineEmpty) {
    std::istringstream                iss("\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    std::string result = console::input_line("Enter: ", settings);
    ASSERT_EQ(std::string(""), result);
}

/**
 * @brief 测试 input_line 读取多行。
 * @note 期望：正确读取多行中的一行。
 */
TEST(InputLineMultiple) {
    std::istringstream                iss("Line 1\nLine 2\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    std::string result1 = console::input_line("Enter line 1: ", settings);
    std::string result2 = console::input_line("Enter line 2: ", settings);
    ASSERT_EQ(std::string("Line 1"), result1);
    ASSERT_EQ(std::string("Line 2"), result2);
}

/**
 * @brief 测试 input_number 读取数字。
 * @note 期望：正确读取 long double 类型。
 */
TEST(InputNumber) {
    std::istringstream                iss("3.14159\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    long double result = console::input_number("Enter number: ", settings);
    ASSERT_NEAR(3.14159, result, 0.00001);
}

/**
 * @brief 测试 input_with_range 范围内数字。
 * @note 期望：正确读取范围内的数字。
 */
TEST(InputWithRangeValid) {
    std::istringstream                iss("5\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    int                               result
        = console::input_with_range<int>(1, 10, "Enter 1-10: ", settings);
    ASSERT_EQ(5, result);
}

/**
 * @brief 测试 input_char 读取字符。
 * @note 期望：正确读取第一个字符。
 */
TEST(InputChar) {
    std::istringstream                iss("A\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    char result = console::input_char("Enter char: ", settings);
    ASSERT_EQ('A', result);
}

/**
 * @brief 测试 input_char 读取多字符只取第一个。
 * @note 期望：只取第一个字符，忽略其余。
 */
TEST(InputCharFirstOnly) {
    std::istringstream                iss("ABC\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    char result = console::input_char("Enter char: ", settings);
    ASSERT_EQ('A', result);
}

/**
 * @brief 测试 input_yes_or_no 返回 true。
 * @note 期望：输入 'Y' 或 'y' 返回 true。
 */
TEST(InputYesOrNoTrue) {
    std::istringstream                iss("Y\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    bool result = console::input_yes_or_no("Continue? ", settings);
    ASSERT_TRUE(result);
    std::istringstream                iss2("y\n");
    console::BasicInputSettings<char> settings2{oss, iss2};
    result = console::input_yes_or_no("Continue? ", settings2);
    ASSERT_TRUE(result);
}

/**
 * @brief 测试 input_yes_or_no 返回 false。
 * @note 期望：输入 'N' 或 'n' 返回 false。
 */
TEST(InputYesOrNoFalse) {
    std::istringstream                iss("N\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    bool result = console::input_yes_or_no("Continue? ", settings);
    ASSERT_FALSE(result);
    std::istringstream                iss2("n\n");
    console::BasicInputSettings<char> settings2{oss, iss2};
    result = console::input_yes_or_no("Continue? ", settings2);
    ASSERT_FALSE(result);
}

/**
 * @brief 测试 input_all 读取全部内容。
 * @note 期望：读取从当前位置到 EOF 的所有字符。
 */
TEST(InputAll) {
    std::istringstream                iss("Line 1\nLine 2\nLine 3\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    std::string result = console::input_all("Read all: ", settings);
    ASSERT_EQ(std::string("Line 1\nLine 2\nLine 3\n"), result);
}

/**
 * @brief 测试 input_all 空流。
 * @note 期望：空流返回空字符串。
 */
TEST(InputAllEmpty) {
    std::istringstream                iss("");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    std::string result = console::input_all("Read all: ", settings);
    ASSERT_EQ(std::string(""), result);
}

/**
 * @brief 测试宽字符版本 input。
 * @note 期望：正确读取宽字符输入。
 */
TEST(WInput) {
    std::wistringstream                  iss(L"hello\n");
    std::wostringstream                  woss;
    console::BasicInputSettings<wchar_t> settings{woss, iss};
    std::wstring result = console::input<std::wstring>(L"Enter: ", settings);
    ASSERT_EQ(std::wstring(L"hello"), result);
}

/**
 * @brief 测试宽字符版本 input_line。
 * @note 期望：正确读取宽字符行。
 */
TEST(WInputLine) {
    std::wistringstream                  iss(L"Hello World!\n");
    std::wostringstream                  woss;
    console::BasicInputSettings<wchar_t> settings{woss, iss};
    std::wstring result = console::input_line<wchar_t>(L"Enter: ", settings);
    ASSERT_EQ(std::wstring(L"Hello World!"), result);
}

/**
 * @brief 测试宽字符版本 input_char。
 * @note 期望：正确读取宽字符。
 */
TEST(WInputChar) {
    std::wistringstream                  iss(L"A\n");
    std::wostringstream                  woss;
    console::BasicInputSettings<wchar_t> settings{woss, iss};
    wchar_t result = console::input_char(L"Enter: ", settings);
    ASSERT_EQ(L'A', result);
}

/**
 * @brief 测试全局默认 input_settings 是否可用。
 * @note 期望：全局设置存在且可访问。
 */
TEST(GlobalInputSettings) {
    console::BasicInputSettings<char> &settings = console::input_settings;
    (void)settings; // 避免未使用警告
    ASSERT_TRUE(true);
}

/**
 * @brief 测试全局默认 w_input_settings 是否可用。
 * @note 期望：全局宽字符设置存在且可访问。
 */
TEST(GlobalWInputSettings) {
    console::BasicInputSettings<wchar_t> &settings = console::w_input_settings;
    (void)settings;
    ASSERT_TRUE(true);
}

/**
 * @brief 测试 input 无提示参数。
 * @note 期望：无提示时正常读取。
 */
TEST(InputNoPrompt) {
    std::istringstream                iss("test\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    std::string result = console::input<std::string>("", settings);
    ASSERT_EQ(std::string("test"), result);
}

/**
 * @brief 测试 input_line 无提示参数。
 * @note 期望：无提示时正常读取。
 */
TEST(InputLineNoPrompt) {
    std::istringstream                iss("test\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    std::string result = console::input_line("", settings);
    ASSERT_EQ(std::string("test"), result);
}

/**
 * @brief 测试 input_number 无提示参数。
 * @note 期望：无提示时正常读取。
 */
TEST(InputNumberNoPrompt) {
    std::istringstream                iss("3.14\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    long double result = console::input_number("", settings);
    ASSERT_NEAR(3.14, result, 0.001);
}

/**
 * @brief 测试 input_with_range 使用默认提示。
 * @note 期望：使用空提示时正常读取。
 */
TEST(InputWithRangeNoPrompt) {
    std::istringstream                iss("5\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    int result = console::input_with_range<int>(1, 10, "", settings);
    ASSERT_EQ(5, result);
}

/**
 * @brief 测试 input_char 使用默认提示。
 * @note 期望：使用空提示时正常读取。
 */
TEST(InputCharNoPrompt) {
    std::istringstream                iss("X\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    char result = console::input_char("", settings);
    ASSERT_EQ('X', result);
}

/**
 * @brief 测试 input_yes_or_no 使用默认提示。
 * @note 期望：使用默认提示时正常读取。
 */
TEST(InputYesOrNoDefaultPrompt) {
    std::istringstream                iss("Y\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    bool result = console::input_yes_or_no("", settings);
    ASSERT_TRUE(result);
}

/**
 * @brief 测试 input_all 使用默认提示。
 * @note 期望：使用默认提示时正常读取。
 */
TEST(InputAllDefaultPrompt) {
    std::istringstream                iss("content\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    std::string                       result = console::input_all("", settings);
    ASSERT_EQ(std::string("content\n"), result);
}

/**
 * @brief 测试 input 读取 long long。
 * @note 期望：正确读取 64 位整数。
 */
TEST(InputLongLong) {
    std::istringstream                iss("9223372036854775807\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    long long result = console::input<long long>("Enter: ", settings);
    ASSERT_EQ(9223372036854775807LL, result);
}

/**
 * @brief 测试 input 读取 unsigned 类型。
 * @note 期望：正确读取无符号整数。
 */
TEST(InputUnsigned) {
    std::istringstream                iss("4294967295\n");
    std::ostringstream                oss;
    console::BasicInputSettings<char> settings{oss, iss};
    unsigned int result = console::input<unsigned int>("Enter: ", settings);
    ASSERT_EQ(4294967295U, result);
}

#ifndef NOMAIN
TEST_MAIN
#endif
