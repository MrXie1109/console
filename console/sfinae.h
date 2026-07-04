/**
 * @file sfinae.h
 * @brief 提供编译期类型特征检测（SFINAE
 * 工具），用于判断容器、可调用对象、迭代器、下标访问、字符串、可打印类型、字符类型等。
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
#include <iterator>
#include <random>
#include <type_traits>

#include "outfwd.h"

namespace console {
    /**
     * @defgroup sfinae 类型特征
     * @brief 编译期检测类型特性的模板工具。
     * @{
     */

    /**
     * @struct is_container
     * @brief 检测类型是否为容器（支持 std::begin 和 std::end）。
     * @tparam T 待检测的类型。
     */
    template <class T, class = void> struct is_container : std::false_type {};

    /// @cond INTERNAL
    template <class T>
    struct is_container<T,
        typename std::enable_if<sizeof(decltype(std::begin(std::declval<T>())))
                                && sizeof(decltype(std::end(
                                    std::declval<T>())))>::type>
        : std::true_type {};
    /// @endcond

    /**
     * @struct is_callable
     * @brief 检测类型是否可作为函数对象以给定参数调用（返回 void 或可转换为
     * void）。
     * @tparam F 待检测的类型。
     * @tparam Args 调用参数类型包。
     */
    template <class F, class = void, class... Args>
    struct is_callable : std::false_type {};

    /// @cond INTERNAL
    template <class F, class... Args>
    struct is_callable<F,
        typename std::enable_if<std::is_convertible<
            decltype(std::declval<F>()(std::declval<Args>()...)),
            void>::value>::type,
        Args...> : std::true_type {};
    /// @endcond

    /**
     * @struct is_iterator
     * @brief 检测类型是否为迭代器（具有 iterator_category）。
     * @tparam T 待检测的类型。
     */
    template <class T, class = void> struct is_iterator : std::false_type {};

    /// @cond INTERNAL
    template <class T>
    struct is_iterator<T,
        typename std::enable_if<sizeof(
            typename std::iterator_traits<T>::iterator_category)>::type>
        : std::true_type {};
    /// @endcond

    /**
     * @struct has_subscript
     * @brief 检测类型是否支持下标操作符（如 T[Idx]）。
     * @tparam T 待检测的类型。
     * @tparam Idx 下标索引类型。
     */
    template <class T, class Idx, class = void>
    struct has_subscript : std::false_type {};

    /// @cond INTERNAL
    template <class T, class Idx>
    struct has_subscript<T,
        Idx,
        typename std::enable_if<sizeof(
            decltype(std::declval<T>()[std::declval<Idx>()]))>::type>
        : std::true_type {};
    /// @endcond

    /**
     * @struct is_string
     * @brief 检测类型是否为字符串类型（char*、std::string、std::string_view
     * 等）。
     * @tparam T 待检测的类型。
     */
    template <class T, class = void> struct is_string : std::false_type {};

    // 各种指针字符串特化
    /// @cond INTERNAL
    template <> struct is_string<char *> : std::true_type {};
    template <> struct is_string<signed char *> : std::true_type {};
    template <> struct is_string<unsigned char *> : std::true_type {};
    template <> struct is_string<wchar_t *> : std::true_type {};
    template <> struct is_string<char16_t *> : std::true_type {};
    template <> struct is_string<char32_t *> : std::true_type {};
    template <> struct is_string<const char *> : std::true_type {};
    template <> struct is_string<const signed char *> : std::true_type {};
    template <> struct is_string<const unsigned char *> : std::true_type {};
    template <> struct is_string<const wchar_t *> : std::true_type {};
    template <> struct is_string<const char16_t *> : std::true_type {};
    template <> struct is_string<const char32_t *> : std::true_type {};

    template <class CharT, class Traits, class Alloc>
    struct is_string<std::basic_string<CharT, Traits, Alloc>> : std::true_type {
    };

#if __cplusplus >= 201703L
    template <class CharT, class Traits>
    struct is_string<std::basic_string_view<CharT, Traits>> : std::true_type {};
#endif
    /// @endcond

    /**
     * @struct is_printable
     * @brief 检测类型是否支持输出到 std::ostream（即定义了 operator<<）。
     * @tparam T 待检测的类型。
     */
    template <class T, class = void> struct is_printable : std::false_type {};

    /// @cond INTERNAL
    template <class T>
    struct is_printable<T,
        typename std::enable_if<sizeof(decltype(std::declval<std::ostream &>()
                                                << std::declval<T>()))>::type>
        : std::true_type {};
    /// @endcond

    /**
     * @struct is_char
     * @brief 检测类型是否为字符类型（char、wchar_t、char16_t、char32_t 等）。
     * @tparam T 待检测的类型。
     */
    template <class T, class = void> struct is_char : std::false_type {};

    // 字符类型特化
    /// @cond INTERNAL
    template <> struct is_char<char> : std::true_type {};
    template <> struct is_char<signed char> : std::true_type {};
    template <> struct is_char<unsigned char> : std::true_type {};
    template <> struct is_char<wchar_t> : std::true_type {};
    template <> struct is_char<char16_t> : std::true_type {};
    template <> struct is_char<char32_t> : std::true_type {};
    /// @endcond

    /**
     * @struct uniform_distribution_impl
     * @brief 取得适配分布的主模板。
     * @tparam T 待检测的类型。
     */
    template <typename T, typename = void> struct uniform_distribution_impl;

    /// @cond INTERNAL
    /// @brief 特化：整数类型 → uniform_int_distribution
    template <typename T>
    struct uniform_distribution_impl<T,
        typename std::enable_if<std::is_integral<T>::value>::type> {
        using type = std::uniform_int_distribution<T>;
    };

    /// @brief 特化：浮点类型 → uniform_real_distribution
    template <typename T>
    struct uniform_distribution_impl<T,
        typename std::enable_if<std::is_floating_point<T>::value>::type> {
        using type = std::uniform_real_distribution<T>;
    };
    /// @endcond

    // 便利别名模板（enable_if 快捷方式）
    /**
     * @brief 启用若 T 是容器。
     */
    template <class T>
    using enable_if_container =
        typename std::enable_if<is_container<T>::value>::type;

    /**
     * @brief 启用若 T 不是容器。
     */
    template <class T>
    using enable_if_not_container =
        typename std::enable_if<!is_container<T>::value>::type;

    /**
     * @brief 启用若 F 可以 Args... 参数调用。
     */
    template <class F, class... Args>
    using enable_if_callable =
        typename std::enable_if<is_callable<F, Args...>::value>::type;

    /**
     * @brief 启用若 F 不可以 Args... 参数调用。
     */
    template <class F, class... Args>
    using enable_if_not_callable =
        typename std::enable_if<!is_callable<F, Args...>::value>::type;

    /**
     * @brief 启用若 T 是迭代器。
     */
    template <class T>
    using enable_if_iterator =
        typename std::enable_if<is_iterator<T>::value>::type;

    /**
     * @brief 启用若 T 不是迭代器。
     */
    template <class T>
    using enable_if_not_iterator =
        typename std::enable_if<!is_iterator<T>::value>::type;

    /**
     * @brief 启用若 T（decay 后）是字符串类型。
     */
    template <class T>
    using enable_if_string = typename std::enable_if<
        is_string<typename std::decay<T>::type>::value>::type;

    /**
     * @brief 启用若 T（decay 后）不是字符串类型。
     */
    template <class T>
    using enable_if_not_string = typename std::enable_if<
        !is_string<typename std::decay<T>::type>::value>::type;

    /**
     * @brief 启用若 T 可打印。
     */
    template <class T>
    using enable_if_printable =
        typename std::enable_if<is_printable<T>::value>::type;

    /**
     * @brief 启用若 T 不可打印。
     */
    template <class T>
    using enable_if_not_printable =
        typename std::enable_if<!is_printable<T>::value>::type;

    /**
     * @brief 启用若 T（decay 后）是字符类型。
     */
    template <class T>
    using enable_if_char = typename std::enable_if<
        is_char<typename std::decay<T>::type>::value>::type;

    /**
     * @brief 启用若 T（decay 后）不是字符类型。
     */
    template <class T>
    using enable_if_not_char = typename std::enable_if<
        !is_char<typename std::decay<T>::type>::value>::type;

    /**
     * @brief 取得对印类型所对应的均匀分布。
     */
    template <typename T>
    using uniform_distribution_t = typename uniform_distribution_impl<T>::type;

    /** @} */ // end of sfinae group
}
