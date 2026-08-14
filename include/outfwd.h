/**
 * @file outfwd.h
 * @brief 为 STL 容器提供输出流运算符的前向声明。
 * @details 该头文件声明了 console 命名空间中针对各种 STL 容器的 operator<<
 * 重载，用于支持将这些容器直接输出到 std::basic_ostream。实际定义在 output.h 中。
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
#include <array>
#include <deque>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <valarray>
#include <vector>

namespace console {
    /**
     * @defgroup output_forward_decl 输出流运算符前向声明
     * @brief 为各种 STL 容器声明的 operator<< 重载。
     * @{
     */

    /// @brief std::vector 输出声明。
    template <class CharT, class Traits, class T>
    std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &, const std::vector<T> &);

    /// @brief std::deque 输出声明。
    template <class CharT, class Traits, class T>
    std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &, const std::deque<T> &);

    /// @brief std::list 输出声明。
    template <class CharT, class Traits, class T>
    std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &, const std::list<T> &);

    /// @brief std::forward_list 输出声明。
    template <class CharT, class Traits, class T>
    std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &, const std::forward_list<T> &);

    /// @brief std::array 输出声明。
    template <class CharT, class Traits, class T, size_t n>
    std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &, const std::array<T, n> &);

    /// @brief std::set 输出声明。
    template <class CharT, class Traits, class T>
    std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &, const std::set<T> &);

    /// @brief std::map 输出声明。
    template <class CharT, class Traits, class K, class V>
    std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &, const std::map<K, V> &);

    /// @brief std::multiset 输出声明。
    template <class CharT, class Traits, class T>
    std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &, const std::multiset<T> &);

    /// @brief std::multimap 输出声明。
    template <class CharT, class Traits, class K, class V>
    std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &, const std::multimap<K, V> &);

    /// @brief std::unordered_set 输出声明。
    template <class CharT, class Traits, class T>
    std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &, const std::unordered_set<T> &);

    /// @brief std::unordered_map 输出声明。
    template <class CharT, class Traits, class K, class V>
    std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &, const std::unordered_map<K, V> &);

    /// @brief std::unordered_multiset 输出声明。
    template <class CharT, class Traits, class T>
    std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &,
        const std::unordered_multiset<T> &);

    /// @brief std::unordered_multimap 输出声明。
    template <class CharT, class Traits, class K, class V>
    std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &,
        const std::unordered_multimap<K, V> &);

    /// @brief std::pair 输出声明。
    template <class CharT, class Traits, class T, class U>
    std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &, const std::pair<T, U> &);

    /// @brief std::tuple 输出声明。
    template <class CharT, class Traits, class... Args>
    std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &, const std::tuple<Args...> &);

    /// @brief std::valarray 输出声明。
    template <class CharT, class Traits, class T>
    std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &, const std::valarray<T> &);

    /** @} */
}
