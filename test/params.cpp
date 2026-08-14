/**
 * @file params.cpp
 * @brief 测试参数包包装器模块 (Params, params)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/params.h"

#include <string>

#include "../include/test.h"

/**
 * @brief 测试 Params 空参数包。
 * @note 期望：空参数包 apply 执行无参函数。
 */
TEST(ParamsEmpty) {
    console::Params<> p;
    bool              called = false;
    p.apply([&called]() { called = true; });
    ASSERT_TRUE(called);
}

/**
 * @brief 测试 Params 单参数 int。
 * @note 期望：apply 正确传递 int 参数。
 */
TEST(ParamsSingleInt) {
    console::Params<int> p(42);
    int                  result = p.apply([](int x) { return x * 2; });
    ASSERT_EQ(84, result);
}

/**
 * @brief 测试 Params 单参数 string。
 * @note 期望：apply 正确传递 string 参数。
 */
TEST(ParamsSingleString) {
    console::Params<std::string> p("hello");
    std::string                  result
        = p.apply([](const std::string &s) { return s + " world"; });
    ASSERT_EQ(std::string("hello world"), result);
}

/**
 * @brief 测试 Params 多参数 int, string, double。
 * @note 期望：apply 正确传递所有参数。
 */
TEST(ParamsMultipleIntStringDouble) {
    console::Params<int, std::string, double> p(42, "hello", 3.14);
    std::string result = p.apply([](int a, const std::string &b, double c) {
        return b + " " + std::to_string(a) + " " + std::to_string(c);
    });
    ASSERT_STRCONTAINS(result.c_str(), "hello 42 3.140000");
}

/**
 * @brief 测试 Params 多参数 int, int, int。
 * @note 期望：apply 正确传递多个整数参数。
 */
TEST(ParamsMultipleInts) {
    console::Params<int, int, int> p(1, 2, 3);
    int result = p.apply([](int a, int b, int c) { return a + b + c; });
    ASSERT_EQ(6, result);
}

/**
 * @brief 测试 params 便捷函数。
 * @note 期望：正确推导类型并构造 Params。
 */
TEST(ParamsFunction) {
    auto        p      = console::params(10, "test", 2.5);
    std::string result = p.apply([](int a, const std::string &b, double c) {
        return b + " " + std::to_string(a + static_cast<int>(c));
    });
    ASSERT_EQ(std::string("test 12"), result);
}

/**
 * @brief 测试 params 数组参数退化。
 * @note 期望：数组退化为指针。
 */
TEST(ParamsArrayDecay) {
    const char *arr = "hello";
    auto        p   = console::params(arr);
    std::string result
        = p.apply([](const char *s) { return std::string(s) + " world"; });
    ASSERT_EQ(std::string("hello world"), result);
}

/**
 * @brief 测试 params 混合类型。
 * @note 期望：正确处理混合类型。
 */
TEST(ParamsMixedTypes) {
    auto        p      = console::params(100, 3.14f, "test", true);
    std::string result = p.apply([](int a, float b, const char *c, bool d) {
        return std::string(c) + " " + std::to_string(a) + " "
               + std::to_string(static_cast<int>(b)) + " "
               + (d ? "true" : "false");
    });
    ASSERT_STRCONTAINS(result.c_str(), "test 100 3 true");
}

/**
 * @brief 测试 Params 返回复合类型。
 * @note 期望：apply 返回任意类型。
 */
TEST(ParamsReturnComplex) {
    console::Params<int, int> p(3, 4);
    auto                      result
        = p.apply([](int a, int b) { return std::pair<int, int>{a, b}; });
    ASSERT_EQ(3, result.first);
    ASSERT_EQ(4, result.second);
}

/**
 * @brief 测试 Params 修改外部变量。
 * @note 期望：apply 中修改外部变量。
 */
TEST(ParamsModifyExternal) {
    console::Params<int> p(42);
    int                  external = 0;
    p.apply([&external](int x) { external = x; });
    ASSERT_EQ(42, external);
}

/**
 * @brief 测试 Params 嵌套 apply。
 * @note 期望：嵌套 apply 正确工作。
 */
TEST(ParamsNestedApply) {
    console::Params<int, int> p(2, 3);
    auto                      sum = p.apply([](int a, int b) {
        return console::params(a, b).apply([](int x, int y) { return x + y; });
    });
    ASSERT_EQ(5, sum);
}

/**
 * @brief 测试 Params 空参数包 apply 返回 void。
 * @note 期望：空参数包 apply 返回 void。
 */
TEST(ParamsEmptyApplyVoid) {
    console::Params<> p;
    int               counter = 0;
    p.apply([&counter]() { ++counter; });
    ASSERT_EQ(1, counter);
}

/**
 * @brief 测试 Params 字符串字面量参数。
 * @note 期望：字符串字面量正确传递。
 */
TEST(ParamsStringLiteral) {
    auto        p      = console::params("hello");
    std::string result = p.apply([](const char *s) { return std::string(s); });
    ASSERT_EQ(std::string("hello"), result);
}

/**
 * @brief 测试 Params 大参数包。
 * @note 期望：多个参数正确传递。
 */
TEST(ParamsLargePack) {
    auto p = console::params(1, 2, 3, 4, 5, 6, 7, 8, 9, 10);
    int  sum
        = p.apply([](int  a,
                      int b,
                      int c,
                      int d,
                      int e,
                      int f,
                      int g,
                      int h,
                      int i,
                      int j) { return a + b + c + d + e + f + g + h + i + j; });
    ASSERT_EQ(55, sum);
}

/**
 * @brief 测试 Params 类型推导稳定性。
 * @note 期望：多次使用 params 推导一致。
 */
TEST(ParamsTypeDeduction) {
    auto   p1  = console::params(1, 2.5);
    auto   p2  = console::params(3, 4.5);
    double sum = p1.apply([&](int a, double b) {
        return p2.apply([&](int c, double d) {
            return static_cast<double>(a + c) + b + d;
        });
    });
    ASSERT_NEAR(11.0, sum, 0.001);
}

/**
 * @brief 测试 Params 值语义。
 * @note 期望：Params 按值存储参数。
 */
TEST(ParamsValueSemantics) {
    int  x     = 42;
    auto p     = console::params(x);
    x          = 100;
    int result = p.apply([](int a) { return a; });
    ASSERT_EQ(42, result);
}

#ifndef NOMAIN
TEST_MAIN
#endif
