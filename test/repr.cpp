/**
 * @file repr.cpp
 * @brief 测试类型表示模块 (repr, tiname)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/repr.h"

#include <cstdlib>
#include <sstream>
#include <string>

#include "../include/test.h"

// 定义一个可打印的 Point 结构体
struct Point {
    int x, y;

    friend std::ostream &operator<<(std::ostream &os, const Point &p) {
        os << "Point(" << p.x << ", " << p.y << ")";
        return os;
    }
};

/**
 * @brief 测试 repr int 类型。
 * @note 期望：输出数字本身。
 */
TEST(ReprInt) {
    std::ostringstream oss;
    console::repr(42, oss);
    ASSERT_EQ(std::string("42"), oss.str());
}

/**
 * @brief 测试 repr double 类型。
 * @note 期望：输出浮点数。
 */
TEST(ReprDouble) {
    std::ostringstream oss;
    console::repr(3.14, oss);
    std::string str = oss.str();
    ASSERT_STRCONTAINS(str.c_str(), "3.14");
}

/**
 * @brief 测试 repr 字符串字面量。
 * @note 期望：输出带双引号的字符串。
 */
TEST(ReprStringLiteral) {
    std::ostringstream oss;
    console::repr("hello", oss);
    ASSERT_EQ(std::string("\"hello\""), oss.str());
}

/**
 * @brief 测试 repr std::string。
 * @note 期望：输出带双引号的字符串。
 */
TEST(ReprStdString) {
    std::ostringstream oss;
    std::string        s = "world";
    console::repr(s, oss);
    ASSERT_EQ(std::string("\"world\""), oss.str());
}

/**
 * @brief 测试 repr char。
 * @note 期望：输出带单引号的字符。
 */
TEST(ReprChar) {
    std::ostringstream oss;
    console::repr('a', oss);
    ASSERT_EQ(std::string("'a'"), oss.str());
}

/**
 * @brief 测试 repr bool true。
 * @note 期望：输出 "true"。
 */
TEST(ReprBoolTrue) {
    std::ostringstream oss;
    console::repr(true, oss);
    ASSERT_EQ(std::string("true"), oss.str());
}

/**
 * @brief 测试 repr bool false。
 * @note 期望：输出 "false"。
 */
TEST(ReprBoolFalse) {
    std::ostringstream oss;
    console::repr(false, oss);
    ASSERT_EQ(std::string("false"), oss.str());
}

/**
 * @brief 测试 repr nullptr。
 * @note 期望：输出 "<nullptr>"。
 */
TEST(ReprNullptr) {
    std::ostringstream oss;
    console::repr(nullptr, oss);
    ASSERT_EQ(std::string("<nullptr>"), oss.str());
}

/**
 * @brief 测试 repr 函数指针。
 * @note 期望：输出 "<function at 地址>"。
 */
TEST(ReprFunctionPointer) {
    std::ostringstream oss;
    console::repr(&rand, oss);
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "<function at ");
    ASSERT_STRCONTAINS(output.c_str(), ">");
}

/**
 * @brief 测试 repr 空函数指针。
 * @note 期望：输出 "<nullptr>"。
 */
TEST(ReprNullFunctionPointer) {
    std::ostringstream oss;
    void (*f)() = nullptr;
    console::repr(f, oss);
    ASSERT_EQ(std::string("<nullptr>"), oss.str());
}

/**
 * @brief 测试 repr 自定义可打印类型。
 * @note 期望：输出 operator<< 的结果。
 */
TEST(ReprPrintableType) {
    Point              p{10, 20};
    std::ostringstream oss;
    console::repr(p, oss);
    ASSERT_EQ(std::string("Point(10, 20)"), oss.str());
}

/**
 * @brief 测试 repr 不可打印类型。
 * @note 期望：输出 "<'类型名' object at 地址>"。
 */
TEST(ReprNonPrintableType) {
    struct NonPrintable {
        int x;
    };
    NonPrintable       np{42};
    std::ostringstream oss;
    console::repr(np, oss);
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "<'");
    ASSERT_STRCONTAINS(output.c_str(), "' object at ");
    ASSERT_STRCONTAINS(output.c_str(), ">");
}

/**
 * @brief 测试 wtiname 获取宽字符类型名。
 * @note 期望：返回宽字符串类型名。
 */
TEST(WTiName) {
    auto         name     = console::wtiname(typeid(int));
    std::wstring expected = L"int";
    if (name != expected) expected = L"i";
    ASSERT_TRUE(name == expected || name == L"i");
}

/**
 * @brief 测试 repr 引用包装器。
 * @note 期望：正确解引用并输出。
 */
TEST(ReprReferenceWrapper) {
    int                x   = 42;
    auto               ref = std::ref(x);
    std::ostringstream oss;
    console::repr(ref, oss);
    ASSERT_EQ(std::string("42"), oss.str());
}

/**
 * @brief 测试 repr 空字符串。
 * @note 期望：输出 ""。
 */
TEST(ReprEmptyString) {
    std::ostringstream oss;
    console::repr("", oss);
    ASSERT_EQ(std::string("\"\""), oss.str());
}

/**
 * @brief 测试 repr 含引号的字符串。
 * @note 期望：输出带双引号的字符串（不转义内部引号）。
 */
TEST(ReprStringWithQuotes) {
    std::ostringstream oss;
    console::repr("hello\"world", oss);
    ASSERT_EQ(std::string("\"hello\"world\""), oss.str());
}

/**
 * @brief 测试 repr 负数。
 * @note 期望：输出负数。
 */
TEST(ReprNegativeInt) {
    std::ostringstream oss;
    console::repr(-42, oss);
    ASSERT_EQ(std::string("-42"), oss.str());
}

#ifndef NOMAIN
TEST_MAIN
#endif
