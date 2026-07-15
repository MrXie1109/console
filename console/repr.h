/**
 * @file repr.h
 * @brief 提供类似 Python 的 repr() 函数，用于生成对象的可读字符串表示。
 * @details 该头文件通过 SFINAE 技术重载 repr
 * 函数，支持字符串、字符、布尔值、nullptr、函数指针、 可打印类型（有
 * operator<<）以及不可打印类型（输出类型信息及地址）。
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
#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>

#include "sfinae.h"
#ifdef __GNUG__
#include <cxxabi.h>
#endif

namespace console {
    /**
     * @defgroup repr 类型表示
     * @brief 提供类型名称和值的可读表示。
     * @{
     */

    /**
     * @brief 获取类型信息的可读名称（跨平台 demangle），返回 std::string。
     * @param ti std::type_info 对象。
     * @return std::string 人类可读的类型名。
     */
    inline std::string tiname_impl(const std::type_info &ti) {
#ifdef _MSC_VER
        return ti.name();
#elif defined(__GNUG__)
        int                                     status = 0;
        std::unique_ptr<char, void (*)(void *)> result(
            abi::__cxa_demangle(ti.name(), nullptr, nullptr, &status),
            std::free);
        return (status == 0) ? result.get() : ti.name();
#else
        return ti.name();
#endif
    }

    /** @brief tiname_impl 的 char 版别名。 */
    inline std::string tiname(const std::type_info &ti) {
        return tiname_impl(ti);
    }
    /** @brief tiname_impl 的 wchar_t 版。 */
    inline std::wstring wtiname(const std::type_info &ti) {
        auto s = tiname_impl(ti);
        return std::wstring(s.begin(), s.end());
    }

#ifndef CONSOLE_PLAIN_REPR
    /**
     * @brief 输出字符串类型（std::string, const char* 等）的表示，带双引号。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam T 字符串类型（由 enable_if_string 约束）。
     * @param value 要输出的值。
     * @param os 输出流，默认为 std::cout。
     */
    template <class T,
        class CharT  = char,
        class Traits = std::char_traits<CharT>>
    typename std::enable_if<
        is_string<typename std::decay<T>::type>::value>::type
    repr(T &&value, std::basic_ostream<CharT, Traits> &os = std::cout) {
        os << CharT('"') << value << CharT('"');
    }

    /**
     * @brief 输出字符类型的表示，带单引号。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam T 字符类型（由 enable_if_char 约束）。
     * @param value 要输出的字符。
     * @param os 输出流，默认为 std::cout。
     */
    template <class T,
        class CharT  = char,
        class Traits = std::char_traits<CharT>>
    typename std::enable_if<is_char<typename std::decay<T>::type>::value>::type
    repr(T &&value, std::basic_ostream<CharT, Traits> &os = std::cout) {
        os << CharT('\'') << value << CharT('\'');
    }

    /**
     * @brief 输出布尔值的表示（"true" 或 "false"）。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam T 必须是 bool 类型。
     * @param value 布尔值。
     * @param os 输出流。
     */
    template <class T,
        class CharT  = char,
        class Traits = std::char_traits<CharT>>
    typename std::enable_if<
        std::is_same<typename std::decay<T>::type, bool>::value>::type
    repr(T &&value, std::basic_ostream<CharT, Traits> &os = std::cout) {
        os << (value ? "true" : "false");
    }

    /**
     * @brief 输出 nullptr 的表示。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam T 必须是 std::nullptr_t。
     * @param value nullptr。
     * @param os 输出流。
     */
    template <class T,
        class CharT  = char,
        class Traits = std::char_traits<CharT>>
    typename std::enable_if<
        std::is_same<typename std::decay<T>::type, std::nullptr_t>::value>::type
    repr(T &&, std::basic_ostream<CharT, Traits> &os = std::cout) {
        os << "<nullptr>";
    }

    /**
     * @brief 输出函数指针的表示，格式为 "<function at 地址>"。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam Ret 函数返回类型。
     * @tparam Args 函数参数类型包。
     * @param f 函数指针。
     * @param os 输出流。
     */
    template <class CharT, class Traits, class Ret, class... Args>
    void
    repr(Ret (*f)(Args...), std::basic_ostream<CharT, Traits> &os = std::cout) {
        if (f)
            os << "<function at " << (void *)f << '>';
        else
            os << "<nullptr>";
    }

    /**
     * @brief 输出可打印类型（定义了 operator<<）的表示，直接输出值。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam T 类型条件：不是 bool，不是字符串，不是字符，不是函数指针，且
     * is_printable<T>::value 为 true。
     * @param value 要输出的值。
     * @param os 输出流。
     */
    template <class T,
        class CharT  = char,
        class Traits = std::char_traits<CharT>>
    typename std::enable_if<
        !std::is_same<typename std::decay<T>::type, bool>::value
        && !std::is_same<typename std::decay<T>::type, std::nullptr_t>::value
        && !is_string<typename std::decay<T>::type>::value
        && !is_char<typename std::decay<T>::type>::value
        && !std::is_function<typename std::decay<T>::type>::value
        && is_basic_printable<CharT, Traits, typename std::decay<T>::type>::
            value>::type
    repr(T &&value, std::basic_ostream<CharT, Traits> &os = std::cout) {
        os << value;
    }

    /**
     * @brief 输出不可打印类型的表示，格式为 "<'类型名' object at 地址>"。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam T 类型条件：不是 nullptr，不是字符串，不是字符，且
     * is_basic_printable<T>::value 为 false。
     * @param value 要输出的对象。
     * @param os 输出流。
     */
    template <class T,
        class CharT  = char,
        class Traits = std::char_traits<CharT>>
    typename std::enable_if<
        !std::is_same<typename std::decay<T>::type, std::nullptr_t>::value
        && !is_string<typename std::decay<T>::type>::value
        && !is_char<typename std::decay<T>::type>::value
        && !is_basic_printable<CharT, Traits, typename std::decay<T>::type>::
            value>::type
    repr(T &&value, std::basic_ostream<CharT, Traits> &os = std::cout) {
        os << "<'" << tiname_impl(typeid(typename std::decay<T>::type))
           << "' object at " << &value << '>';
    }

    template <class T,
        class CharT  = char,
        class Traits = std::char_traits<CharT>>
    void repr(std::reference_wrapper<T>    v,
        std::basic_ostream<CharT, Traits> &os = std::cout) {
        repr<CharT, Traits>(v.get(), os);
    }

#else
    template <class T,
        class CharT  = char,
        class Traits = std::char_traits<CharT>>
    void repr(T &&value, std::basic_ostream<CharT, Traits> &os = std::cout) {
        os << std::forward<T>(value);
    }
#endif

    /** @} */ // end of repr group
}
