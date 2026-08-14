/**
 * @file test.h
 * @brief 极简C++测试框架，基于宏与异常实现断言与结果统计。
 *        支持单元测试的注册、执行、结果输出与汇总报告。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 */

/*
Copyright (c) 2026 MrXie1109

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include <cstring>
#include <iomanip>
#include <iostream>
#include <string>

#include "colorful.h"
#include "csexc.h"
#include "fmt.h"
#include "time.h"

namespace console {
#ifndef CONSOLE_NO_TEST

    /**
     * @brief 获取通过的测试用例数量。
     * @return 已通过的测试用例数量。
     * @note 你不应该使用内部细节。
     */
    inline int &passed_count() {
        static int count = 0;
        return count;
    }

    /**
     * @brief 获取失败的测试用例数量。
     * @return 已失败的测试用例数量。
     * @note 你不应该使用内部细节。
     */
    inline int &failed_count() {
        static int count = 0;
        return count;
    }

    /**
     * @brief 构造守卫对象，程序结束时输出摘要。
     * @note 你不应该使用内部细节。
     */
    inline void pre_summary() {
        struct Guard {
            Guard() { std::cout << "=== Start Test ===\n" << std::endl; }

            ~Guard() {
                int passed = passed_count();
                int failed = failed_count();
                int total  = passed + failed;
                std::cout << "=== Test Summary ===" << std::endl;
                std::cout << "Passed: " << passed << std::endl;
                std::cout << "Failed: " << failed << std::endl;
                std::cout << "Total:  " << total << std::endl;
                if (total > 0)
                    std::cout
                        << "Rate:   " << std::fixed << std::setprecision(1)
                        << (static_cast<double>(passed) / total * 100) << "%"
                        << std::endl;
                else
                    std::cout << "Rate:   N/A" << std::endl;
            }
        };
        static Guard guard;
    }

    /**
     * @brief 定义一个测试用例。
     * @details 定义一个测试用例，自动注册并在程序启动时执行。
     *          测试通过时输出绿色[PASS]，失败时输出红色[FAIL]。
     *          最终汇总输出通过/失败统计与通过率。
     * @param test_name 测试用例的名称。
     * @example 以下是测试用例的格式。
     * @code
     * TEST(OnePlusOneEqualsTwo) {
     *     ASSERT_EQ(1 + 1, 2);
     * }
     * @endcode
     */
#define TEST(test_name)                                                        \
    static void console_test_##test_name();                                    \
    static int  console_result_##test_name = [] {                              \
        using namespace ::console;                                            \
        pre_summary();                                                        \
        try {                                                                 \
            console_test_##test_name();                                       \
            ColorGuard cg(color::Green);                                      \
            std::cout << "[PASS] " #test_name "\n\n";                         \
            ++passed_count();                                                 \
            return 0;                                                         \
        } catch (const ::console::AssertError &e) {                           \
            ColorGuard cg(color::Red);                                        \
            std::cout << console::format(                                     \
                "[FAIL] {}\n    Assert Failed ({}:{}) - {}\n\n",              \
                #test_name,                                                   \
                __FILE__,                                                     \
                __LINE__,                                                     \
                e.what());                                                    \
            ++failed_count();                                                 \
            return 1;                                                         \
        } catch (const std::system_error &e) {                                \
            ColorGuard cg(color::Red);                                        \
            std::cout << console::format(                                     \
                "[FAIL] {}\n    Threw Exception ({}:{})\n\twhat(): "          \
                 "{}\n\tcode(): {}\n\n",                                       \
                #test_name,                                                   \
                __FILE__,                                                     \
                __LINE__,                                                     \
                e.what(),                                                     \
                e.code());                                                    \
            ++failed_count();                                                 \
            return 1;                                                         \
        } catch (const std::exception &e) {                                   \
            ColorGuard cg(color::Red);                                        \
            std::cout << console::format(                                     \
                "[FAIL] {}\n    Threw Exception ({}:{})\n\twhat(): {}\n\n",   \
                #test_name,                                                   \
                __FILE__,                                                     \
                __LINE__,                                                     \
                e.what());                                                    \
            ++failed_count();                                                 \
            return 1;                                                         \
        } catch (...) {                                                       \
            ColorGuard cg(color::Red);                                        \
            std::cout << console::format(                                     \
                "[FAIL] {}\n    Threw Exception ({}:{})\n\n",                 \
                #test_name,                                                   \
                __FILE__,                                                     \
                __LINE__);                                                    \
            ++failed_count();                                                 \
            return 1;                                                         \
        }                                                                     \
    }();                                                                       \
    static void console_test_##test_name()

#define EXPR_WARP(expression) "`" #expression "`"

    /**
     * @brief 断言条件为真。
     * @param condition 要断言的条件。
     * @throw AssertError 条件不满足时抛出异常。
     */
#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) throw console::AssertError(#condition);              \
    } while (false)

    /**
     * @brief 断言条件为假。
     * @param condition 要断言的条件。
     * @throw AssertError 条件满足时抛出异常。
     */
#define ASSERT_FALSE(condition)                                                \
    do {                                                                       \
        if (condition) throw console::AssertError("!(" #condition ")");        \
    } while (false)

    /**
     * @brief 断言两个值相等。
     * @param expected 预期值。
     * @param actual 实际值。
     * @throw AssertError 值不相等时抛出异常。
     * @note 不要用这个比较浮点数，用ASSERT_NEAR。
     */
#define ASSERT_EQ(expected, actual)                                            \
    do {                                                                       \
        if ((expected) != (actual))                                            \
            throw console::AssertError(                                        \
                console::format("{} == {}\n\texpected: {}\n\tactual:   {}",    \
                    EXPR_WARP(expected),                                       \
                    EXPR_WARP(actual),                                         \
                    expected,                                                  \
                    actual));                                                  \
    } while (false)

    /**
     * @brief 断言两个值不相等。
     * @param expected 预期值。
     * @param actual 实际值。
     * @throw AssertError 值相等时抛出异常。
     */
#define ASSERT_NE(expected, actual)                                            \
    do {                                                                       \
        if ((expected) == (actual))                                            \
            throw console::AssertError(                                        \
                console::format("{} != {}\n\texpected: {}\n\tactual:   {}",    \
                    EXPR_WARP(expected),                                       \
                    EXPR_WARP(actual),                                         \
                    expected,                                                  \
                    actual));                                                  \
    } while (false)

    /**
     * @brief 断言第一个值小于第二个值。
     * @param a 第一个值。
     * @param b 第二个值。
     * @throw AssertError 第一个值不小于第二个值时抛出异常。
     */
#define ASSERT_LT(a, b)                                                        \
    do {                                                                       \
        if ((a) >= (b))                                                        \
            throw console::AssertError(                                        \
                console::format("{} < {}\n\tleft:  {}\n\tright: {}",           \
                    EXPR_WARP(a),                                              \
                    EXPR_WARP(b),                                              \
                    a,                                                         \
                    b));                                                       \
    } while (false)

    /**
     * @brief 断言第一个值大于第二个值。
     * @param a 第一个值。
     * @param b 第二个值。
     * @throw AssertError 第一个值不大于第二个值时抛出异常。
     */
#define ASSERT_GT(a, b)                                                        \
    do {                                                                       \
        if ((a) <= (b))                                                        \
            throw console::AssertError(                                        \
                console::format("{} > {}\n\tleft:  {}\n\tright: {}",           \
                    EXPR_WARP(a),                                              \
                    EXPR_WARP(b),                                              \
                    a,                                                         \
                    b));                                                       \
    } while (false)

    /**
     * @brief 断言第一个值小于等于第二个值。
     * @param a 第一个值。
     * @param b 第二个值。
     * @throw AssertError 第一个值不小于等于第二个值时抛出异常。
     */
#define ASSERT_LE(a, b)                                                        \
    do {                                                                       \
        if ((a) > (b))                                                         \
            throw console::AssertError(                                        \
                console::format("{} <= {}\n\tleft:  {}\n\tright: {}",          \
                    EXPR_WARP(a),                                              \
                    EXPR_WARP(b),                                              \
                    a,                                                         \
                    b));                                                       \
    } while (false)

    /**
     * @brief 断言第一个值大于等于第二个值。
     * @param a 第一个值。
     * @param b 第二个值。
     * @throw AssertError 第一个值不大于等于第二个值时抛出异常。
     */
#define ASSERT_GE(a, b)                                                        \
    do {                                                                       \
        if ((a) < (b))                                                         \
            throw console::AssertError(                                        \
                console::format("{} >= {}\n\tleft:  {}\n\tright: {}",          \
                    EXPR_WARP(a),                                              \
                    EXPR_WARP(b),                                              \
                    a,                                                         \
                    b));                                                       \
    } while (false)

    /**
     * @brief 断言两个浮点数接近相等。
     * @param expected 期望值。
     * @param actual 实际值。
     * @param eps 允许的误差范围。
     * @throw AssertError 两个浮点数不接近相等时抛出异常。
     */
#define ASSERT_NEAR(expected, actual, eps)                                     \
    do {                                                                       \
        auto _e = (expected);                                                  \
        auto _a = (actual);                                                    \
        if ((_e - _a) > (eps) || (_a - _e) > (eps))                            \
            throw console::AssertError(console::format(                        \
                "{} \u2248 {} [\u00B1{}]\n\texpected: "                        \
                "{:.20f}\n\tactual:   {:.20f}\n\tepsilon:  {:.20f}",           \
                EXPR_WARP(expected),                                           \
                EXPR_WARP(actual),                                             \
                eps,                                                           \
                _e,                                                            \
                _a,                                                            \
                eps));                                                         \
    } while (false)

    /**
     * @brief 断言指针为null。
     * @param ptr 指针。
     * @throw AssertError 指针不为null时抛出异常。
     */
#define ASSERT_NULL(ptr)                                                       \
    do {                                                                       \
        if ((ptr) != nullptr)                                                  \
            throw console::AssertError(                                        \
                console::format("{} == nullptr\n\tptr: {:#x}",                 \
                    EXPR_WARP(ptr),                                            \
                    reinterpret_cast<uintptr_t>(ptr)));                        \
    } while (false)

    /**
     * @brief 断言指针不为null。
     * @param ptr 指针。
     * @throw AssertError 指针为null时抛出异常。
     */
#define ASSERT_NOTNULL(ptr)                                                    \
    do {                                                                       \
        if ((ptr) == nullptr)                                                  \
            throw console::AssertError(                                        \
                EXPR_WARP(ptr) " == nullptr\n\tptr: nullptr");                 \
    } while (false)

    /**
     * @brief 辅助函数：安全地将 C 字符串转换为显示字符串。
     */
    inline std::string ptr_to_str(const char *s) {
        if (s == nullptr) return "nullptr";
        return "\"" + std::string(s) + "\"";
    }

    /**
     * @brief 断言C风格字符串相等。
     * @param expected 预期字符串。
     * @param actual 实际字符串。
     * @throw AssertError 字符串不相等时抛出异常。
     */
#define ASSERT_STREQ(expected, actual)                                         \
    do {                                                                       \
        const char *_e = (expected);                                           \
        const char *_a = (actual);                                             \
        if (_e == nullptr || _a == nullptr) {                                  \
            if (_e != _a)                                                      \
                throw console::AssertError(console::format(                    \
                    "{} == {}\n\texpected: {}\n\tactual:   {}",                \
                    EXPR_WARP(expected),                                       \
                    EXPR_WARP(actual),                                         \
                    console::ptr_to_str(_e),                                   \
                    console::ptr_to_str(_a)));                                 \
        } else if (std::strcmp(_e, _a) != 0) {                                 \
            throw console::AssertError(                                        \
                console::format("{} == {}\n\texpected: {}\n\tactual:   {}",    \
                    EXPR_WARP(expected),                                       \
                    EXPR_WARP(actual),                                         \
                    console::ptr_to_str(_e),                                   \
                    console::ptr_to_str(_a)));                                 \
        }                                                                      \
    } while (false)

    /**
     * @brief 断言C风格字符串不相等。
     * @param expected 预期字符串。
     * @param actual 实际字符串。
     * @throw AssertError 字符串相等时抛出异常。
     */
#define ASSERT_STRNE(expected, actual)                                         \
    do {                                                                       \
        const char *_e = (expected);                                           \
        const char *_a = (actual);                                             \
        if (_e == nullptr || _a == nullptr) {                                  \
            if (_e != _a)                                                      \
                throw console::AssertError(console::format(                    \
                    "{} != {}\n\texpected: {}\n\tactual:   {}",                \
                    EXPR_WARP(expected),                                       \
                    EXPR_WARP(actual),                                         \
                    console::ptr_to_str(_e),                                   \
                    console::ptr_to_str(_a)));                                 \
        } else if (std::strcmp(_e, _a) == 0) {                                 \
            throw console::AssertError(                                        \
                console::format("{} != {}\n\texpected: {}\n\tactual:   {}",    \
                    EXPR_WARP(expected),                                       \
                    EXPR_WARP(actual),                                         \
                    console::ptr_to_str(_e),                                   \
                    console::ptr_to_str(_a)));                                 \
        }                                                                      \
    } while (false)

    /**
     * @brief 断言C风格字符串相等（不区分大小写）。
     * @param a 字符串A。
     * @param b 字符串B。
     * @throw AssertError 字符串不相等时抛出异常。
     */
#define ASSERT_STRCASEEQ(a, b)                                                 \
    do {                                                                       \
        const char *_a = (a);                                                  \
        const char *_b = (b);                                                  \
        if (_a == nullptr || _b == nullptr) {                                  \
            if (_a != _b)                                                      \
                throw console::AssertError(console::format(                    \
                    "{} == {} [case-insensitive]\n\texpected: {}\n\t"          \
                    "actual:   {}",                                            \
                    EXPR_WARP(a),                                              \
                    EXPR_WARP(b),                                              \
                    console::ptr_to_str(_a),                                   \
                    console::ptr_to_str(_b)));                                 \
        } else {                                                               \
            bool        _eq = true;                                            \
            const char *_pa = _a;                                              \
            const char *_pb = _b;                                              \
            while (*_pa != '\0' && *_pb != '\0') {                             \
                if (static_cast<char>(std::tolower(*_pa))                      \
                    != static_cast<char>(std::tolower(*_pb))) {                \
                    _eq = false;                                               \
                    break;                                                     \
                }                                                              \
                ++_pa;                                                         \
                ++_pb;                                                         \
            }                                                                  \
            if (*_pa != *_pb) _eq = false;                                     \
            if (!_eq) {                                                        \
                throw console::AssertError(console::format(                    \
                    "{} == {} [case-insensitive]\n\texpected: {}\n\t"          \
                    "actual:   {}",                                            \
                    EXPR_WARP(a),                                              \
                    EXPR_WARP(b),                                              \
                    console::ptr_to_str(_a),                                   \
                    console::ptr_to_str(_b)));                                 \
            }                                                                  \
        }                                                                      \
    } while (false)

    /**
     * @brief 断言C风格字符串包含子串。
     * @param str 字符串。
     * @param substr 子串。
     * @throw AssertError 字符串不包含子串时抛出异常。
     */
#define ASSERT_STRCONTAINS(str, substr)                                        \
    do {                                                                       \
        const char *_str    = (str);                                           \
        const char *_substr = (substr);                                        \
        if (_str == nullptr || _substr == nullptr                              \
            || std::strstr(_str, _substr) == nullptr) {                        \
            throw console::AssertError(                                        \
                console::format("{} contains {}\n\tstr:    {}\n\tsubstr: {}",  \
                    EXPR_WARP(str),                                            \
                    EXPR_WARP(substr),                                         \
                    console::ptr_to_str(_str),                                 \
                    console::ptr_to_str(_substr)));                            \
        }                                                                      \
    } while (false)

    /**
     * @brief 断言字符串不包含指定子串。
     * @param str 字符串。
     * @param substr 子串。
     */
#define ASSERT_STRNOTCONTAINS(str, substr)                                     \
    do {                                                                       \
        const char *_str    = (str);                                           \
        const char *_substr = (substr);                                        \
        if (_str == nullptr || _substr == nullptr                              \
            || std::strstr(_str, _substr) != nullptr) {                        \
            throw console::AssertError(                                        \
                console::format("{} does not contain {}\n\tstr:    {}\n\t"     \
                                "substr: {}",                                  \
                    EXPR_WARP(str),                                            \
                    EXPR_WARP(substr),                                         \
                    console::ptr_to_str(_str),                                 \
                    console::ptr_to_str(_substr)));                            \
        }                                                                      \
    } while (false)

/**
     * @brief 断言表达式抛出指定类型的异常。
     * @param expression 表达式。
     * @param exception_type 异常类型。
     * @throw AssertError 表达式未抛出指定类型的异常时抛出异常。
     */
#define ASSERT_THROWS(expression, exception_type)                              \
    do {                                                                       \
        try {                                                                  \
            expression;                                                        \
            throw console::AssertError(                                        \
                EXPR_WARP(expression) " should throw " #exception_type);       \
        } catch (const exception_type &) {                                     \
        } catch (...) {                                                        \
            throw console::AssertError(                                        \
                EXPR_WARP(expression) " should throw " #exception_type);       \
        }                                                                      \
    } while (false)

/**
     * @brief 断言表达式不抛出异常。
     * @param expression 表达式。
     * @throw AssertError 表达式抛出异常时抛出异常。
     */
#define ASSERT_NO_THROW(expression)                                            \
    do {                                                                       \
        try {                                                                  \
            expression;                                                        \
        } catch (...) {                                                        \
            throw console::AssertError(                                        \
                EXPR_WARP(expression) " should not throw");                    \
        }                                                                      \
    } while (false)

/**
     * @brief 断言表达式抛出任意类型的异常。
     * @param expression 表达式。
     * @throw AssertError 表达式未抛出异常时抛出异常。
     */
#define ASSERT_THROWS_ANY(expression)                                          \
    do {                                                                       \
        try {                                                                  \
            expression;                                                        \
        } catch (...) {                                                        \
            break;                                                             \
        }                                                                      \
        throw console::AssertError(EXPR_WARP(expression) " should throw");     \
    } while (false)

/**
     * @brief 断言容器包含指定元素。
     * @param container 容器。
     * @param element 元素。
     * @throw AssertError 容器不包含元素时抛出异常。
     */
#define ASSERT_CONTAINS(container, element)                                    \
    do {                                                                       \
        const auto &_c     = (container);                                      \
        const auto &_e     = (element);                                        \
        bool        _found = false;                                            \
        for (const auto &_item : _c)                                           \
            if (_item == _e) {                                                 \
                _found = true;                                                 \
                break;                                                         \
            }                                                                  \
        if (!_found)                                                           \
            throw console::AssertError(                                        \
                EXPR_WARP(container) " contains " EXPR_WARP(element));         \
    } while (false)

/**
     * @brief 断言容器大小等于指定值。
     * @param container 容器。
     * @param n 期望大小。
     * @throw AssertError 容器大小不等于指定值时抛出异常。
     */
#define ASSERT_SIZE_EQ(container, n)                                           \
    do {                                                                       \
        if ((container).size() != static_cast<std::size_t>(n))                 \
            throw console::AssertError(                                        \
                console::format("{}.size() == {}\n\tsize: {}\n\texpected: {}", \
                    EXPR_WARP(container),                                      \
                    EXPR_WARP(n),                                              \
                    (container).size(),                                        \
                    static_cast<std::size_t>(n)));                             \
    } while (false)

/**
     * @brief 断言容器为空。
     * @param container 容器。
     * @throw AssertError 容器不为空时抛出异常。
     */
#define ASSERT_EMPTY(container)                                                \
    do {                                                                       \
        if (!(container).empty())                                              \
            throw console::AssertError(                                        \
                console::format("{} is empty\n\tsize: {}",                     \
                    EXPR_WARP(container),                                      \
                    (container).size()));                                      \
    } while (false)

/**
     * @brief 断言容器非空。
     * @param container 容器。
     * @throw AssertError 容器为空时抛出异常。
     */
#define ASSERT_NOT_EMPTY(container)                                            \
    do {                                                                       \
        if ((container).empty())                                               \
            throw console::AssertError(                                        \
                EXPR_WARP(container) " is not empty\n\tsize: 0");              \
    } while (false)

/**
     * @brief 断言超时。
     * @param expr 表达式。
     * @param ms 超时时间(毫秒)。
     * @throw AssertError 表达式执行时间超过指定时间时抛出异常。
     */
#define ASSERT_TIMEOUT(expr, ms)                                               \
    do {                                                                       \
        auto _duration = timer([&] { expr; });                                 \
        auto _limit    = milliseconds(ms);                                     \
        if (_duration > _limit)                                                \
            throw console::AssertError(                                        \
                console::format("{} timed out after {}\n\telapsed: "           \
                                "{}\n\tlimit:   {}",                           \
                    EXPR_WARP(expr),                                           \
                    _limit,                                                    \
                    _duration,                                                 \
                    _limit));                                                  \
    } while (false)

/**
     * @brief 断言性能。
     * @param expr 表达式。
     * @param ops 操作次数。
     * @param ms 超时时间(毫秒)。
     * @throw AssertError 表达式执行时间超过指定时间时抛出异常。
     */
#define ASSERT_PERFORMANCE(expr, ops, ms)                                      \
    do {                                                                       \
        auto _duration = timer([&] {                                           \
            for (auto i = 0ULL; i < ops; ++i) expr;                            \
        });                                                                    \
        auto _limit    = milliseconds(ms);                                     \
        if (_duration > _limit)                                                \
            throw console::AssertError(console::format(                        \
                "{} times {} timed out after {}\n\telapsed: {}\n\t"            \
                "limit:   {}\n\tops:     {}",                                  \
                EXPR_WARP(ops),                                                \
                EXPR_WARP(expr),                                               \
                _limit,                                                        \
                _duration,                                                     \
                _limit,                                                        \
                ops));                                                         \
    } while (false)

/// @brief 测试主函数(其实就是个空壳，没有任何逻辑)。
#define TEST_MAIN                                                              \
    int main() {}

#endif
}
