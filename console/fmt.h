/**
 * @file fmt.h
 * @brief 轻量级格式化字符串头文件库。
 * @details 在没有std::format的情况下，使用<iomanip>提供轻量级的格式化字符串功能。
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
#include <cctype>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "csexc.h"
#include "output.h"

namespace console {
    namespace details {
        /**
         * @brief 格式说明符结构。
         * @tparam CharT 字符类型。
         */
        template <class CharT = char>
        struct BasicFormatSpec {
            /**
             * @brief 填充字符。
             * @details 填充字符，默认空格。
             */
            CharT fill = static_cast<CharT>(' ');
            /**
             * @brief 对齐方式。
             * @details 支持 '>' 右对齐和 '<' 左对齐，不支持居中对齐。
             */
            CharT align = static_cast<CharT>('>');
            /**
             * @brief 最小宽度。
             * @details 最小宽度，默认0。
             */
            int width = 0;
            /**
             * @brief 精度。
             * @details 精度（浮点小数位数 / 字符串最大长度），-1为不限制。
             */
            int precision = -1;
            /**
             * @brief 格式类型。
             * @details 格式类型：d 十进制 x 小写十六进制 X 大写十六进制
             *                  o 八进制f 定点小数 e 科学计数法
             *                  E 大写科学计数法 g 通用格式 G 大写通用格式
             */
            CharT type = 0;
            /**
             * @brief 标志 '#'。
             * @details 标志 '#'：显示进制前缀（0x/0X/0）或强制显示小数点。
             */
            bool hasHash = false;
            /**
             * @brief 标志 '+'。
             * @details 标志 '+'：正数前显示 + 号。
             */
            bool hasPlus = false;
            /**
             * @brief 标志 ' '。
             * @details 标志 ' '：正数前显示空格（与 '+' 互斥）。
             */
            bool hasSpace = false;
        };

        /**
         * @brief 解析格式说明符。
         * @details 支持的语法：[[填充]对齐][宽度][:[标志][.精度][类型]]
         * @param specStr 格式字符串（不含花括号和前导冒号）。
         * @param spec 输出的格式说明符。
         * @throw BadFormat 格式非法时抛出。
         */
        template <class CharT = char>
        void parse_format_spec(const std::basic_string<CharT> &specStr,
            BasicFormatSpec<CharT>                            &spec) {
            size_t i   = 0;
            size_t len = specStr.size();
            if (len == 0) return;

            if (i + 1 < len) {
                CharT first  = specStr[i];
                CharT second = specStr[i + 1];
                if (first == static_cast<CharT>('0')
                    && std::isdigit(static_cast<unsigned char>(second))) {
                    spec.fill  = first;
                    spec.align = static_cast<CharT>('>');
                    i += 1;
                } else if (second == static_cast<CharT>('<')
                           || second == static_cast<CharT>('>')) {
                    if (first == static_cast<CharT>('{')
                        || first == static_cast<CharT>('}')) {
                        throw BadFormat("invalid fill character");
                    }
                    spec.fill  = first;
                    spec.align = second;
                    i += 2;
                } else if (first == static_cast<CharT>('<')
                           || first == static_cast<CharT>('>')) {
                    spec.align = first;
                    i += 1;
                }
            }

            if (i < len
                && std::isdigit(static_cast<unsigned char>(specStr[i]))) {
                size_t start = i;
                while (i < len
                       && std::isdigit(static_cast<unsigned char>(specStr[i])))
                    i++;
                spec.width = std::stoi(
                    std::string(specStr.begin() + start, specStr.begin() + i));
            }

            if (i < len && specStr[i] == static_cast<CharT>(':')) {
                i++;
                while (i < len
                       && (specStr[i] == static_cast<CharT>('#')
                           || specStr[i] == static_cast<CharT>('+')
                           || specStr[i] == static_cast<CharT>(' '))) {
                    if (specStr[i] == static_cast<CharT>('#'))
                        spec.hasHash = true;
                    else if (specStr[i] == static_cast<CharT>('+'))
                        spec.hasPlus = true;
                    else if (specStr[i] == static_cast<CharT>(' '))
                        spec.hasSpace = true;
                    i++;
                }
                if (i < len && specStr[i] == static_cast<CharT>('.')) {
                    i++;
                    size_t start = i;
                    while (
                        i < len
                        && std::isdigit(static_cast<unsigned char>(specStr[i])))
                        i++;
                    if (start == i) throw BadFormat("missing precision");
                    spec.precision = std::stoi(std::string(
                        specStr.begin() + start, specStr.begin() + i));
                }
                if (i < len
                    && std::isalpha(static_cast<unsigned char>(specStr[i]))) {
                    spec.type = specStr[i];
                    i++;
                }
                if (i < len) {
                    throw BadFormat(
                        "unexpected characters at end: "
                        + std::string(specStr.begin() + i, specStr.end()));
                }
            } else if (i < len) {
                std::basic_string<CharT> remaining = specStr.substr(i);
                if (remaining[0] == static_cast<CharT>('.')) {
                    i++;
                    size_t start = i;
                    while (
                        i < len
                        && std::isdigit(static_cast<unsigned char>(specStr[i])))
                        i++;
                    if (start == i) throw BadFormat("missing precision");
                    spec.precision = std::stoi(std::string(
                        specStr.begin() + start, specStr.begin() + i));
                    if (i < len
                        && std::isalpha(
                            static_cast<unsigned char>(specStr[i]))) {
                        spec.type = specStr[i];
                        i++;
                    }
                    if (i < len) {
                        throw BadFormat(
                            "unexpected characters after precision: "
                            + std::string(specStr.begin() + i, specStr.end()));
                    }
                } else {
                    // 标志 + [.精度] + [类型]
                    size_t j = 0;
                    while (j < remaining.size()
                           && (remaining[j] == static_cast<CharT>('#')
                               || remaining[j] == static_cast<CharT>('+')
                               || remaining[j] == static_cast<CharT>(' '))) {
                        if (remaining[j] == static_cast<CharT>('#'))
                            spec.hasHash = true;
                        else if (remaining[j] == static_cast<CharT>('+'))
                            spec.hasPlus = true;
                        else if (remaining[j] == static_cast<CharT>(' '))
                            spec.hasSpace = true;
                        j++;
                    }
                    if (j < remaining.size()
                        && remaining[j] == static_cast<CharT>('.')) {
                        j++;
                        size_t prec_start = j;
                        while (j < remaining.size()
                               && std::isdigit(
                                   static_cast<unsigned char>(remaining[j])))
                            j++;
                        if (prec_start == j)
                            throw BadFormat("missing precision");
                        spec.precision = std::stoi(
                            std::string(remaining.begin() + prec_start,
                                remaining.begin() + j));
                    }
                    if (j < remaining.size()) {
                        if (remaining.size() - j == 1
                            && std::isalpha(
                                static_cast<unsigned char>(remaining[j]))) {
                            spec.type = remaining[j];
                            j++;
                        } else {
                            throw BadFormat("unexpected character '"
                                            + std::string(1,
                                                static_cast<char>(remaining[j]))
                                            + "'");
                        }
                    }
                    if (j < remaining.size()) {
                        throw BadFormat("unexpected characters: "
                                        + std::string(remaining.begin() + j,
                                            remaining.end()));
                    }
                }
            }

            if (spec.hasHash) {
                char t = static_cast<char>(spec.type);
                if (t != 0 && t != 'd' && t != 'x' && t != 'X' && t != 'o'
                    && t != 'f' && t != 'F' && t != 'g' && t != 'G') {
                    throw BadFormat("'#' not allowed with type '"
                                    + std::string(1, t) + "'");
                }
            }
            if (spec.hasPlus && spec.hasSpace) {
                throw BadFormat("'+' and space cannot both appear");
            }
            if (spec.precision >= 0) {
                char t = static_cast<char>(spec.type);
                if (t != 0 && t != 'f' && t != 'F' && t != 'e' && t != 'E'
                    && t != 'g' && t != 'G' && t != 's') {
                    throw BadFormat("precision not allowed with type '"
                                    + std::string(1, t) + "'");
                }
            }
        }

        /**
         * @brief 将格式说明符应用到输出流。
         * @tparam CharT 字符类型。
         * @param os 输出流。
         * @param spec 格式说明符。
         */
        template <class CharT = char>
        void apply_format(
            std::basic_ostream<CharT> &os, const BasicFormatSpec<CharT> &spec) {
            if (spec.width > 0) os << std::setw(spec.width);
            if (spec.fill != static_cast<CharT>(' '))
                os << std::setfill(spec.fill);
            if (spec.align == static_cast<CharT>('<'))
                os << std::left;
            else if (spec.align == static_cast<CharT>('>'))
                os << std::right;
            if (spec.precision >= 0) os << std::setprecision(spec.precision);
            if (spec.type != 0) {
                switch (static_cast<char>(spec.type)) {
                case 'd':
                    os << std::dec;
                    break;
                case 'x':
                    os << std::hex << std::nouppercase;
                    break;
                case 'X':
                    os << std::hex << std::uppercase;
                    break;
                case 'o':
                    os << std::oct;
                    break;
                case 'f':
                case 'F':
                    os << std::fixed;
                    break;
                case 'e':
                    os << std::scientific;
                    break;
                case 'E':
                    os << std::scientific << std::uppercase;
                    break;
                case 'g':
                    os << std::defaultfloat;
                    break;
                case 'G':
                    os << std::defaultfloat << std::uppercase;
                    break;
                default:
                    break;
                }
            }
            if (spec.hasHash) os << std::showbase;
            if (spec.hasPlus) os << std::showpos;
        }

        // 辅助：算术类型正数前加空格
        template <class CharT, class T>
        static typename std::enable_if<std::is_arithmetic<T>::value, void>::type
        apply_space_if_nonneg(std::basic_ostream<CharT> &os, const T &value) {
            if (value >= 0) os << static_cast<CharT>(' ');
        }

        template <class CharT, class T>
        static
            typename std::enable_if<!std::is_arithmetic<T>::value, void>::type
            apply_space_if_nonneg(
                std::basic_ostream<CharT> &os, const T &value) {}

        /**
         * @brief 格式化单个值。
         * @tparam CharT 字符类型。
         * @tparam T 值类型。
         * @param os 输出流。
         * @param value 要格式化的值。
         * @param spec 格式说明符。
         */
        template <typename CharT = char, typename T>
        void format_value(std::basic_ostream<CharT> &os,
            const T                                 &value,
            const BasicFormatSpec<CharT>            &spec) {
            std::ios_base::fmtflags oldFlags     = os.flags();
            CharT                   oldFill      = os.fill();
            std::streamsize         oldPrecision = os.precision();
            apply_format(os, spec);
            if (spec.hasSpace) apply_space_if_nonneg(os, value);
            put(os, value);
            os.flags(oldFlags);
            os.fill(oldFill);
            os.precision(oldPrecision);
        }

        /**
         * @brief 格式化实现（无参数版本，递归终止）。
         * @tparam CharT 字符类型。
         * @param os 输出流。
         * @param fmt 格式字符串。
         * @param pos 当前解析位置。
         * @param argIndex 参数索引。
         */
        template <class CharT = char>
        void format_impl(std::basic_ostream<CharT> &os,
            const std::basic_string<CharT>         &fmt,
            size_t                                 &pos,
            size_t                                  argIndex) {
            while (pos < fmt.size()) {
                CharT ch = fmt[pos];
                if (ch == static_cast<CharT>('{') && pos + 1 < fmt.size()
                    && fmt[pos + 1] == static_cast<CharT>('{')) {
                    os << static_cast<CharT>('{');
                    pos += 2;
                    continue;
                }
                if (ch == static_cast<CharT>('}') && pos + 1 < fmt.size()
                    && fmt[pos + 1] == static_cast<CharT>('}')) {
                    os << static_cast<CharT>('}');
                    pos += 2;
                    continue;
                }
                if (ch == static_cast<CharT>('{')) {
                    size_t start = pos;
                    pos++;
                    size_t closePos = fmt.find(static_cast<CharT>('}'), pos);
                    if (closePos == std::basic_string<CharT>::npos) {
                        throw BadFormat("Unmatched '{' at position "
                                        + std::to_string(start));
                    }
                    throw BadFormat(
                        "Not enough arguments for placeholder at position "
                        + std::to_string(start));
                } else if (ch == static_cast<CharT>('}')) {
                    size_t start = pos;
                    pos++;
                    throw BadFormat(
                        "Unmatched '}' at position " + std::to_string(start));
                } else {
                    os << ch;
                    pos++;
                }
            }
        }

        /**
         * @brief 格式化实现（有参数版本）。
         * @tparam CharT 字符类型。
         * @tparam T 第一个参数类型。
         * @tparam Args 剩余参数类型。
         * @param os 输出流。
         * @param fmt 格式字符串。
         * @param pos 当前解析位置。
         * @param argIndex 参数索引。
         * @param first 第一个参数。
         * @param rest 剩余参数。
         */
        template <class CharT = char, typename T, typename... Args>
        void format_impl(std::basic_ostream<CharT> &os,
            const std::basic_string<CharT>         &fmt,
            size_t                                 &pos,
            size_t                                  argIndex,
            const T                                &first,
            const Args &...rest) {
            while (pos < fmt.size()) {
                CharT ch = fmt[pos];
                if (ch == static_cast<CharT>('{') && pos + 1 < fmt.size()
                    && fmt[pos + 1] == static_cast<CharT>('{')) {
                    os << static_cast<CharT>('{');
                    pos += 2;
                    continue;
                }
                if (ch == static_cast<CharT>('}') && pos + 1 < fmt.size()
                    && fmt[pos + 1] == static_cast<CharT>('}')) {
                    os << static_cast<CharT>('}');
                    pos += 2;
                    continue;
                }
                if (ch == static_cast<CharT>('{')) {
                    size_t start = pos;
                    pos++;
                    size_t closePos = fmt.find(static_cast<CharT>('}'), pos);
                    if (closePos == std::basic_string<CharT>::npos) {
                        throw BadFormat("Unmatched '{' at position "
                                        + std::to_string(start));
                    }
                    std::basic_string<CharT> placeholder
                        = fmt.substr(pos, closePos - pos);
                    if (!placeholder.empty()
                        && placeholder[0] == static_cast<CharT>(':')) {
                        placeholder = placeholder.substr(1);
                    }
                    pos = closePos + 1;
                    BasicFormatSpec<CharT> spec;
                    try {
                        parse_format_spec(placeholder, spec);
                    } catch (const BadFormat &e) {
                        throw BadFormat("Invalid format specifier '"
                                        + std::string(placeholder.begin(),
                                            placeholder.end())
                                        + "' at position "
                                        + std::to_string(start + 1) + ": "
                                        + e.what());
                    }
                    format_value(os, first, spec);
                    format_impl(os, fmt, pos, argIndex + 1, rest...);
                    return;
                } else {
                    os << ch;
                    pos++;
                }
            }
        }
    }

    using FormatSpec  = details::BasicFormatSpec<char>;
    using WFormatSpec = details::BasicFormatSpec<wchar_t>;

    /**
     * @brief 格式化字符串（char 版）。
     * @tparam Args 参数类型。
     * @param fmt 格式字符串。
     * @param args 要格式化的参数。
     * @return std::string 格式化后的字符串。
     * @throw BadFormat 格式错误时抛出。
     */
    template <typename... Args>
    std::string format(const std::string &fmt, const Args &...args) {
        std::ostringstream oss;
        size_t             pos = 0;
        try {
            details::format_impl(oss, fmt, pos, 0, args...);
        } catch (const BadFormat &) {
            throw;
        } catch (const std::exception &e) {
            throw BadFormat(std::string("Internal error: ") + e.what());
        }
        return oss.str();
    }

    /**
     * @brief 格式化字符串（wchar_t 版）。
     * @tparam Args 参数类型。
     * @param fmt 格式字符串。
     * @param args 要格式化的参数。
     * @return std::wstring 格式化后的宽字符串。
     * @throw BadFormat 格式错误时抛出。
     */
    template <typename... Args>
    std::wstring wformat(const std::wstring &fmt, const Args &...args) {
        std::wostringstream oss;
        size_t              pos = 0;
        try {
            details::format_impl(oss, fmt, pos, 0, args...);
        } catch (const BadFormat &) {
            throw;
        } catch (const std::exception &e) {
            throw BadFormat(std::string("Internal error: ") + e.what());
        }
        return oss.str();
    }

    /**
     * @brief 格式化字符串。
     * @tparam Args 参数类型。
     * @param format_str 格式字符串。
     * @param args 要格式化的参数。
     * @return BasicOutput 格式化后的输出对象。
     * @throw BadFormat 格式错误时抛出。
     */
    template <class CharT, class Traits>
    template <typename... Args>
    BasicOutput<CharT, Traits> &BasicOutput<CharT, Traits>::fmt(
        const std::basic_string<CharT, Traits> &format_str,
        const Args &...args) {
        size_t pos = 0;
        details::format_impl(os, format_str, pos, 0, args...);
        os << end;
        if (isFlush) os << std::flush;
        return *this;
    }

    /**
     * @brief 格式化字符串。
     * @tparam Args 参数类型。
     * @param format_str 格式字符串。
     * @param args 要格式化的参数。
     * @return BasicOutput 格式化后的输出对象。
     * @throw BadFormat 格式错误时抛出。
     */
    template <class CharT, class Traits>
    template <typename... Args>
    BasicOutput<CharT, Traits> &BasicOutput<CharT, Traits>::fmt(
        const CharT *format_str, const Args &...args) {
        return fmt(std::basic_string<CharT, Traits>(format_str), args...);
    }
}
