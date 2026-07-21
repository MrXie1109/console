/**
 * @file input.h
 * @brief
 * 提供丰富的控制台输入函数，支持类型安全输入、范围检查、行读取、确认输入等。
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
#include <cfloat>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

#include "strpp.h"

namespace console {
    /**
     * @struct BasicInputSettings
     * @brief 输入/输出流设置，用于自定义 input 函数的输入输出目标。
     * @tparam CharT 字符类型。
     * @details 默认使用 std::cout 和 std::cin，可修改以实现重定向或测试。
     */
    template <class CharT = char>
    struct BasicInputSettings {
        std::basic_ostream<CharT> &os; ///< 输出提示信息的流
        std::basic_istream<CharT> &is; ///< 读取输入的流
    };

    /** @brief BasicInputSettings<char> 的类型别名。 */
    using InputSettings = BasicInputSettings<char>;

    /** @brief BasicInputSettings<wchar_t> 的类型别名。 */
    using WInputSettings = BasicInputSettings<wchar_t>;

    static BasicInputSettings<> input_settings{
        std::cout, std::cin}; ///< 全局默认输入设置

    static BasicInputSettings<wchar_t> w_input_settings{
        std::wcout, std::wcin}; ///< 全局默认宽字符输入设置

    /**
     * @brief 从标准输入读取一个值，支持类型模板。
     * @tparam T 要读取的类型，默认为 std::string。
     * @tparam CharT 字符类型。
     * @param prompt 提示字符串。
     * @param is 输入设置（默认使用全局 input_settings）。
     * @return T 读取的值。
     * @note 若输入失败（如类型不匹配），会清空错误状态并重新提示，直到成功。
     */
    template <class T = std::string, class CharT = char>
    T input(const CharT                 *prompt = "",
        const BasicInputSettings<CharT> &is     = input_settings) {
        T tmp;
        while (true) {
            is.os << prompt << std::flush;
            is.is >> tmp;
            if (!is.is) {
                is.is.clear();
                is.is.ignore(
                    std::numeric_limits<std::streamsize>::max(), CharT('\n'));
                is.os << "Stream Error!" << std::endl;
                continue;
            }
            is.is.ignore(
                std::numeric_limits<std::streamsize>::max(), CharT('\n'));
            return tmp;
        }
    }

    /**
     * @brief 读取一行宽字符字符串。
     * @param prompt 提示信息。
     * @param is 输入设置。
     * @return T 读取的值。
     * @note 本质上是语法糖。
     */
    template <class T = std::string>
    T w_input(const wchar_t  *prompt = L"",
        const WInputSettings &is     = w_input_settings) {
        T tmp;
        while (true) {
            is.os << prompt << std::flush;
            is.is >> tmp;
            if (!is.is) {
                is.is.clear();
                is.is.ignore(
                    std::numeric_limits<std::streamsize>::max(), wchar_t('\n'));
                is.os << "Stream Error!" << std::endl;
                continue;
            }
            is.is.ignore(
                std::numeric_limits<std::streamsize>::max(), wchar_t('\n'));
            return tmp;
        }
    }

    /**
     * @brief 读取一个 long double 类型的数字。
     * @param prompt 提示字符串（默认为 "Type a number: "）。
     * @param is 输入设置。
     * @return long double 读取的数字。
     */
    inline long double input_number(const char *prompt = "Type a number: ",
        const InputSettings                    &is     = input_settings) {
        return input<long double>(prompt, is);
    }

    /**
     * @brief 读取一整行字符串（包含空格）。
     * @param prompt 提示字符串。
     * @param is 输入设置。
     * @return std::basic_string<CharT> 读取的行（不含换行符）。
     */
    template <class CharT = char>
    std::basic_string<CharT> input_line(const CharT *prompt = "",
        const BasicInputSettings<CharT>             &is     = input_settings) {
        std::basic_string<CharT> tmp;
        is.os << prompt << std::flush;
        if (is.is.peek() == CharT('\n')) is.is.get();
        std::getline(is.is, tmp);
        return tmp;
    }

    /**
     * @brief 读取一个在指定范围内的数字。
     * @tparam T 输入类型。
     * @tparam CharT 字符类型。
     * @param min 最小值（包含）。
     * @param max 最大值（包含）。
     * @param prompt 提示字符串。
     * @param is 输入设置。
     * @return T 验证后的数字。
     * @note 若输入超出范围，会输出错误信息并重新提示。
     */
    template <class T, class CharT = char>
    T input_with_range(T                 min,
        T                                max,
        const CharT                     *prompt = "",
        const BasicInputSettings<CharT> &is     = input_settings) {
        T tmp;
        while (true) {
            tmp = input<T>(prompt, is);
            if (tmp < min) {
                is.os << "less than the minimum value of " << min << std::endl;
                continue;
            }
            if (tmp > max) {
                is.os << "Greater than the maximum value of " << max
                      << std::endl;
                continue;
            }
            return tmp;
        }
    }

    /**
     * @brief 读取一个字符。
     * @tparam CharT 字符类型。
     * @param prompt 提示字符串。
     * @param is 输入设置。
     * @return CharT 读取的第一个字符。
     */
    template <class CharT = char>
    CharT input_char(const CharT        *prompt = "",
        const BasicInputSettings<CharT> &is     = input_settings) {
        is.os << prompt << std::flush;
        CharT tmp = static_cast<CharT>(is.is.get());
        is.is.ignore(std::numeric_limits<std::streamsize>::max(), CharT('\n'));
        return tmp;
    }

    /**
     * @brief 读取一个 y/n 确认，返回布尔值。
     * @param prompt 提示字符串。
     * @param is 输入设置。
     * @return bool true 若输入 'Y'/'y'，false 若输入 'N'/'n'。
     */
    inline bool input_yes_or_no(const char *prompt = "Type yes or no: ",
        const InputSettings                &is     = input_settings) {
        while (true) {
            char tmp = input_char(prompt, is);
            if (tmp == 'Y' || tmp == 'y')
                return true;
            else if (tmp == 'N' || tmp == 'n')
                return false;
            else
                is.os << "Please type yes or no." << std::endl;
        }
    }

    /**
     * @brief 读取输入流中剩余的全部内容（直到 EOF）。
     * @param prompt 提示字符串。
     * @param is 输入设置。
     * @return std::basic_string<CharT> 从当前位置到流末尾的所有字符。
     */
    template <class CharT = char>
    std::basic_string<CharT> input_all(const CharT *prompt = "",
        const BasicInputSettings<CharT>            &is     = input_settings) {
        is.os << prompt;
        return {std::istreambuf_iterator<CharT>(is.is),
            std::istreambuf_iterator<CharT>()};
    }
}
