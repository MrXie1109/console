/**
 * @file defer.h
 * @brief RAII的作用域宏，在退出作用域时按逆序执行推迟的代码。
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
#include <type_traits>
#include <utility>

namespace console {
    /**
     * @brief 推迟执行的RAII守卫类。
     * @tparam F 推迟执行的函数类型。
     * @note 除非不使用defer宏，否则通常不需要直接使用。
     */
    template <class F>
    class DeferGuard {
        F func; ///< 推迟执行的函数。

    public:
        /**
         * @brief 构造函数，接受推迟执行的函数。
         * @param f 推迟执行的函数。
         */
        DeferGuard(const F &f) : func(f) {}

        /**
         * @brief 构造函数，接受推迟执行的函数。
         * @param f 推迟执行的函数。
         */
        DeferGuard(F &&f) : func(std::move(f)) {}

        /**
         * @brief 析构函数，执行推迟执行的函数。
         */
        ~DeferGuard() { func(); }
    };

    /**
     * @brief 工厂函数，创建DeferGuard对象。
     * @param f 推迟执行的函数。
     * @return DeferGuard对象。
     */
    template <class F>
    DeferGuard<typename std::decay<F>::type> make_defer(F &&f) {
        return std::forward<F>(f);
    }

#ifndef CONSOLE_DEFER_NO_MACRO

#define CONSOLE_CONCAT_IMPL(a, b) a##b

#define CONSOLE_CONCAT(a, b) CONSOLE_CONCAT_IMPL(a, b)

    /**
     * @brief defer宏，用于在作用域退出时执行代码。
     * @param ... 要执行的代码。
     * @example
     * @code
     * {
     *     int *ptr = new int[42];
     *     defer(delete[] ptr);
     * }
     * @endcode
     */
#define defer(...)                                                             \
    auto CONSOLE_CONCAT(_dg_, __LINE__) = make_defer([&] { __VA_ARGS__; })

#endif
}
