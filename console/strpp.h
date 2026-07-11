/**
 * @file strpp.h
 * @brief 提供字符串处理工具函数和格式化类。
 * @details
 * 包含去除空白字符、大小写转换、分割与连接、分区、可变参数转字符串、格式化字符串（f_string）等功能。
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
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <initializer_list>
#include <locale>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "csexc.h"
#include "output.h"

namespace console {
    /**
     * @defgroup strpp 字符串处理
     * @brief 字符串修剪、转换、分割、连接、格式化等工具。
     * @{
     */

    /**
     * @brief 移除字符串左侧的空白字符（空格、制表符等）。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @param str 要处理的字符串（按值传递，内部修改副本）。
     * @return std::basic_string<CharT, Traits, Allocator> 处理后的新字符串。
     */
    template <class CharT, class Traits, class Allocator>
    std::basic_string<CharT, Traits, Allocator>
    ltrim(std::basic_string<CharT, Traits, Allocator> str) {
        auto it = std::find_if(str.begin(),
            str.end(),
            [](unsigned char uc) -> bool { return !isspace(uc); });
        str.erase(str.begin(), it);
        return str;
    }

    /**
     * @brief 移除字符串右侧的空白字符。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @param str 要处理的字符串（按值传递，内部修改副本）。
     * @return std::basic_string<CharT, Traits, Allocator> 处理后的新字符串。
     */
    template <class CharT, class Traits, class Allocator>
    std::basic_string<CharT, Traits, Allocator>
    rtrim(std::basic_string<CharT, Traits, Allocator> str) {
        auto it = std::find_if(str.rbegin(),
            str.rend(),
            [](unsigned char uc) -> bool { return !isspace(uc); });
        str.erase(it.base(), str.end());
        return str;
    }

    /**
     * @brief 移除字符串两侧的空白字符。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @param str 要处理的字符串（按值传递，内部修改副本）。
     * @return std::basic_string<CharT, Traits, Allocator> 处理后的新字符串。
     */
    template <class CharT, class Traits, class Allocator>
    std::basic_string<CharT, Traits, Allocator>
    trim(std::basic_string<CharT, Traits, Allocator> str) {
        auto front = std::find_if(str.begin(),
            str.end(),
            [](unsigned char uc) -> bool { return !isspace(uc); });
        if (front == str.end()) {
            str.clear();
            return str;
        }
        auto back = std::find_if(str.rbegin(),
            str.rend(),
            [](unsigned char uc) -> bool { return !isspace(uc); });
        str.erase(back.base(), str.end());
        str.erase(str.begin(), front);
        return str;
    }

    /**
     * @brief 移除字符串左侧指定的字符集合。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @param str 要处理的字符串（按值传递，内部修改副本）。
     * @param chars 要删除的字符集（只要字符出现在此集合中就被删除）。
     * @return std::basic_string<CharT, Traits, Allocator> 处理后的新字符串。
     */
    template <class CharT, class Traits, class Allocator>
    std::basic_string<CharT, Traits, Allocator>
    ltrim(std::basic_string<CharT, Traits, Allocator>      str,
        const std::basic_string<CharT, Traits, Allocator> &chars) {
        auto it
            = std::find_if(str.begin(), str.end(), [&chars](unsigned char ch) {
                  return chars.find(ch)
                         == std::basic_string<CharT, Traits, Allocator>::npos;
              });
        str.erase(str.begin(), it);
        return str;
    }

    /**
     * @brief 移除字符串右侧指定的字符集合。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @param str 要处理的字符串（按值传递，内部修改副本）。
     * @param chars 要删除的字符集。
     * @return std::basic_string<CharT, Traits, Allocator> 处理后的新字符串。
     */
    template <class CharT, class Traits, class Allocator>
    std::basic_string<CharT, Traits, Allocator>
    rtrim(std::basic_string<CharT, Traits, Allocator>      str,
        const std::basic_string<CharT, Traits, Allocator> &chars) {
        auto it = std::find_if(
            str.rbegin(), str.rend(), [&chars](unsigned char ch) {
                return chars.find(ch)
                       == std::basic_string<CharT, Traits, Allocator>::npos;
            });
        str.erase(it.base(), str.end());
        return str;
    }

    /**
     * @brief 移除字符串两侧指定的字符集合。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @param str 要处理的字符串（按值传递，内部修改副本）。
     * @param chars 要删除的字符集。
     * @return std::basic_string<CharT, Traits, Allocator> 处理后的新字符串。
     */
    template <class CharT, class Traits, class Allocator>
    std::basic_string<CharT, Traits, Allocator>
    trim(std::basic_string<CharT, Traits, Allocator>       str,
        const std::basic_string<CharT, Traits, Allocator> &chars) {
        auto front
            = std::find_if(str.begin(), str.end(), [&chars](unsigned char ch) {
                  return chars.find(ch)
                         == std::basic_string<CharT, Traits, Allocator>::npos;
              });
        if (front == str.end()) {
            str.clear();
            return str;
        }
        auto back = std::find_if(
            str.rbegin(), str.rend(), [&chars](unsigned char ch) {
                return chars.find(ch)
                       == std::basic_string<CharT, Traits, Allocator>::npos;
            });
        str.erase(back.base(), str.end());
        str.erase(str.begin(), front);
        return str;
    }

    /**
     * @brief 将字符串转换为大写。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @param str 要处理的字符串（按值传递，内部修改副本）。
     * @param loc 本地化对象，默认为当前全局 locale。
     * @return std::basic_string<CharT, Traits, Allocator> 大写形式。
     */
    template <class CharT, class Traits, class Allocator>
    std::basic_string<CharT, Traits, Allocator>
    upper(std::basic_string<CharT, Traits, Allocator> str,
        const std::locale                            &loc = std::locale{}) {
        for (auto &ch : str) ch = std::toupper(ch, loc);
        return str;
    }

    /**
     * @brief 将字符串转换为小写。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @param str 要处理的字符串（按值传递，内部修改副本）。
     * @param loc 本地化对象，默认为当前全局 locale。
     * @return std::basic_string<CharT, Traits, Allocator> 小写形式。
     */
    template <class CharT, class Traits, class Allocator>
    std::basic_string<CharT, Traits, Allocator>
    lower(std::basic_string<CharT, Traits, Allocator> str,
        const std::locale                            &loc = std::locale{}) {
        for (auto &ch : str) ch = std::tolower(ch, loc);
        return str;
    }

    /**
     * @brief 将字符串转换为标题格式（每个单词首字母大写，其余小写）。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @param str 要处理的字符串（按值传递，内部修改副本）。
     * @param loc 本地化对象，默认为当前全局 locale。
     * @return std::basic_string<CharT, Traits, Allocator> 标题格式的字符串。
     */
    template <class CharT, class Traits, class Allocator>
    std::basic_string<CharT, Traits, Allocator>
    title(std::basic_string<CharT, Traits, Allocator> str,
        const std::locale                            &loc = std::locale{}) {
        using string_type = std::basic_string<CharT, Traits, Allocator>;
        if (str.empty()) return string_type{};
        str[0] = std::toupper(str[0], loc);
        for (size_t i = 1; i < str.size(); ++i) {
            if (std::isspace(str[i - 1], loc))
                str[i] = std::toupper(str[i], loc);
            else
                str[i] = std::tolower(str[i], loc);
        }
        return str;
    }

    /**
     * @struct BasicPartitionResult
     * @brief 字符串分区结果，包含左部分、分隔符、右部分。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     */
    template <class CharT = char,
        class Traits      = std::char_traits<CharT>,
        class Allocator   = std::allocator<CharT>>
    struct BasicPartitionResult {
        using string_type = std::basic_string<CharT, Traits, Allocator>;

        string_type left; ///< 分隔符前的子串
        string_type middle; ///< 分隔符本身
        string_type right; ///< 分隔符后的子串

        /**
         * @brief 输出分区结果到流，格式为 ("left", "middle", "right")。
         * @param os 输出流。
         * @param pr 分区结果对象。
         * @return std::basic_ostream<CharT, Traits>& 流引用。
         */
        friend std::basic_ostream<CharT, Traits> &
        operator<<(std::basic_ostream<CharT, Traits> &os,
            const BasicPartitionResult               &pr) {
            return os << "BasicPartitionResult(\"" << pr.left << "\", \""
                      << pr.middle << "\", \"" << pr.right << "\")";
        }
    };

    /** @brief BasicPartitionResult<char> 的类型别名。 */
    using PartitionResult = BasicPartitionResult<char>;
    /** @brief BasicPartitionResult<wchar_t> 的类型别名。 */
    using WPartitionResult = BasicPartitionResult<wchar_t>;
    /** @brief BasicPartitionResult<char16_t> 的类型别名。 */
    using U16PartitionResult = BasicPartitionResult<char16_t>;
    /** @brief BasicPartitionResult<char32_t> 的类型别名。 */
    using U32PartitionResult = BasicPartitionResult<char32_t>;

    /**
     * @brief 在字符串中查找第一个分隔符，并返回分隔符之前、分隔符本身、分隔符之后的三部分。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @param text 原始字符串。
     * @param sep 分隔符。
     * @return PartitionResult<CharT, Traits, Allocator> 分区结果。
     * @note 若未找到分隔符，则 left 为原字符串，middle 和 right 为空。
     */
    template <class CharT, class Traits, class Allocator>
    BasicPartitionResult<CharT, Traits, Allocator>
    partition(const std::basic_string<CharT, Traits, Allocator> &text,
        const std::basic_string<CharT, Traits, Allocator>       &sep) {
        using string_type = std::basic_string<CharT, Traits, Allocator>;
        size_t pos        = text.find(sep);
        if (pos == string_type::npos)
            return BasicPartitionResult<CharT, Traits, Allocator>{
                text, string_type{}, string_type{}};
        return BasicPartitionResult<CharT, Traits, Allocator>{
            text.substr(0, pos), sep, text.substr(pos + sep.size())};
    }

    /**
     * @brief 以分隔符分割字符串（类似 Python 的 split，默认按空格分割）。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @param text 要分割的字符串。
     * @param sep 分隔符，默认为空格 " "。
     * @return std::vector<std::basic_string<CharT, Traits, Allocator>> 分割后的子串列表。
     * @note 连续的分隔符会产生空字符串子串。
     */
    template <class CharT, class Traits, class Allocator>
    std::vector<std::basic_string<CharT, Traits, Allocator>>
    split(const std::basic_string<CharT, Traits, Allocator> &text,
        const std::basic_string<CharT, Traits, Allocator>   &sep
        = std::basic_string<CharT, Traits, Allocator>{1, CharT{' '}}) {
        using string_type = std::basic_string<CharT, Traits, Allocator>;
        std::vector<string_type> vec;
        size_t                   sep_len = sep.length();
        if (sep_len == 0) {
            vec.push_back(text);
            return vec;
        }
        size_t start = 0;
        size_t end;
        while ((end = text.find(sep, start)) != string_type::npos) {
            vec.emplace_back(text, start, end - start);
            start = end + sep_len;
        }
        vec.emplace_back(text, start);
        return vec;
    }

    /**
     * @brief 以分隔符连接容器中的字符串元素。
     * @tparam T 元素类型（必须支持输出到 std::basic_ostringstream<CharT, Traits>）。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @param vec 元素容器。
     * @param sep 分隔符，默认为空字符串。
     * @return std::basic_string<CharT, Traits, Allocator> 连接后的字符串。
     */
    template <class T,
        class CharT     = char,
        class Traits    = std::char_traits<CharT>,
        class Allocator = std::allocator<CharT>>
    std::basic_string<CharT, Traits, Allocator> join(const std::vector<T> &vec,
        const std::basic_string<CharT, Traits, Allocator>                 &sep
        = std::basic_string<CharT, Traits, Allocator>{}) {
        using string_type = std::basic_string<CharT, Traits, Allocator>;
        if (vec.empty()) return string_type{};
        std::basic_ostringstream<CharT, Traits> ss;
        auto                                    it = vec.begin();
        ss << *it;
        while (++it != vec.end()) ss << sep << *it;
        return ss.str();
    }

    /**
     * @brief 将任意多个参数转换为字符串并拼接（无分隔符）。
     * @tparam Args 参数类型包。
     * @param args 要转换的参数。
     * @return std::string 所有参数按顺序拼接的结果。
     */
    template <class... Args>
    std::string to_string(Args &&...args) {
        std::ostringstream oss;
        int                _[] = {0, (oss << std::forward<Args>(args), 0)...};
        (void)_;
        return oss.str();
    }

    /**
     * @brief 将字符串中的值提取到多个参数中，参数类型由调用者指定。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @tparam Args 参数类型包。
     * @param str 包含要提取值的字符串，值之间应以空格分隔。
     * @param args 要提取值的参数列表，按顺序对应字符串中的值。
     */
    template <class CharT, class Traits, class Allocator, class... Args>
    void from_string(
        std::basic_string<CharT, Traits, Allocator> str, Args &...args) {
        std::basic_istringstream<CharT, Traits> iss(str);
        int                                     _[] = {0, (iss >> args, 0)...};
        (void)_;
    }

    /**
     * @class BasicFString
     * @brief 格式化字符串类，支持使用 `%` 运算符进行占位符 `{}` 替换。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Allocator 内存分配器类型。
     * @details 继承自 std::basic_string，通过 `operator%` 将第一个 `{}`
     * 替换为参数的字符串表示。 若字符串中不包含 `{}` 则抛出 bad_format 异常。
     *
     * 使用示例：
     * @code
     * FString fmt = "Hello, {}!"_f;
     * std::string result = fmt % "world";  // "Hello, world!"
     * @endcode
     */
    template <class CharT = char,
        class Traits      = std::char_traits<CharT>,
        class Allocator   = std::allocator<CharT>>
    class BasicFString : public std::basic_string<CharT, Traits, Allocator> {
        using base_type = std::basic_string<CharT, Traits, Allocator>;

    public:
        using base_type::base_type;

        /**
         * @brief 用参数替换第一个 `{}` 占位符。
         * @tparam T 参数类型。
         * @param t 要替换的值。
         * @return BasicFString 替换后的新 BasicFString 对象。
         * @throw bad_format 若当前字符串中不包含 `{}`。
         */
        template <class T>
        BasicFString operator%(const T &t) {
            static const base_type placeholder{CharT('{'), CharT('}')};
            auto                   pos = this->find(placeholder);
            if (pos != base_type::npos) {
                auto bak = *this;
                bak.replace(pos, 2, to_string(t));
                return bak;
            }
            throw BadFormat("Bad Format");
        }
    };

    /** @brief BasicFString<char> 的类型别名。 */
    using FString = BasicFString<char>;
    /** @brief BasicFString<wchar_t> 的类型别名。 */
    using WFString = BasicFString<wchar_t>;
    /** @brief BasicFString<char16_t> 的类型别名。 */
    using U16FString = BasicFString<char16_t>;
    /** @brief BasicFString<char32_t> 的类型别名。 */
    using U32FString = BasicFString<char32_t>;

    // ──────────────────────────────────────────────
    // C 字符串字面量重载（const char*, const wchar_t*, const char16_t*, const char32_t*）
    // 构造对应的 basic_string 后转发给泛型版本
    // ──────────────────────────────────────────────

    /** @copydoc ltrim(basic_string) */
    inline std::string ltrim(const char *str) {
        return ltrim(std::string(str));
    }
    /** @copydoc ltrim(basic_string) */
    inline std::wstring ltrim(const wchar_t *str) {
        return ltrim(std::wstring(str));
    }
    /** @copydoc ltrim(basic_string) */
    inline std::u16string ltrim(const char16_t *str) {
        return ltrim(std::u16string(str));
    }
    /** @copydoc ltrim(basic_string) */
    inline std::u32string ltrim(const char32_t *str) {
        return ltrim(std::u32string(str));
    }

    /** @copydoc rtrim(basic_string) */
    inline std::string rtrim(const char *str) {
        return rtrim(std::string(str));
    }
    /** @copydoc rtrim(basic_string) */
    inline std::wstring rtrim(const wchar_t *str) {
        return rtrim(std::wstring(str));
    }
    /** @copydoc rtrim(basic_string) */
    inline std::u16string rtrim(const char16_t *str) {
        return rtrim(std::u16string(str));
    }
    /** @copydoc rtrim(basic_string) */
    inline std::u32string rtrim(const char32_t *str) {
        return rtrim(std::u32string(str));
    }

    /** @copydoc trim(basic_string) */
    inline std::string trim(const char *str) {
        return trim(std::string(str));
    }
    /** @copydoc trim(basic_string) */
    inline std::wstring trim(const wchar_t *str) {
        return trim(std::wstring(str));
    }
    /** @copydoc trim(basic_string) */
    inline std::u16string trim(const char16_t *str) {
        return trim(std::u16string(str));
    }
    /** @copydoc trim(basic_string) */
    inline std::u32string trim(const char32_t *str) {
        return trim(std::u32string(str));
    }

    /** @copydoc ltrim(basic_string, basic_string) */
    inline std::string ltrim(const char *str, const std::string &chars) {
        return ltrim(std::string(str), chars);
    }
    /** @copydoc ltrim(basic_string, basic_string) */
    inline std::wstring ltrim(const wchar_t *str, const std::wstring &chars) {
        return ltrim(std::wstring(str), chars);
    }
    /** @copydoc ltrim(basic_string, basic_string) */
    inline std::u16string
    ltrim(const char16_t *str, const std::u16string &chars) {
        return ltrim(std::u16string(str), chars);
    }
    /** @copydoc ltrim(basic_string, basic_string) */
    inline std::u32string
    ltrim(const char32_t *str, const std::u32string &chars) {
        return ltrim(std::u32string(str), chars);
    }

    /** @copydoc rtrim(basic_string, basic_string) */
    inline std::string rtrim(const char *str, const std::string &chars) {
        return rtrim(std::string(str), chars);
    }
    /** @copydoc rtrim(basic_string, basic_string) */
    inline std::wstring rtrim(const wchar_t *str, const std::wstring &chars) {
        return rtrim(std::wstring(str), chars);
    }
    /** @copydoc rtrim(basic_string, basic_string) */
    inline std::u16string
    rtrim(const char16_t *str, const std::u16string &chars) {
        return rtrim(std::u16string(str), chars);
    }
    /** @copydoc rtrim(basic_string, basic_string) */
    inline std::u32string
    rtrim(const char32_t *str, const std::u32string &chars) {
        return rtrim(std::u32string(str), chars);
    }

    /** @copydoc trim(basic_string, basic_string) */
    inline std::string trim(const char *str, const std::string &chars) {
        return trim(std::string(str), chars);
    }
    /** @copydoc trim(basic_string, basic_string) */
    inline std::wstring trim(const wchar_t *str, const std::wstring &chars) {
        return trim(std::wstring(str), chars);
    }
    /** @copydoc trim(basic_string, basic_string) */
    inline std::u16string
    trim(const char16_t *str, const std::u16string &chars) {
        return trim(std::u16string(str), chars);
    }
    /** @copydoc trim(basic_string, basic_string) */
    inline std::u32string
    trim(const char32_t *str, const std::u32string &chars) {
        return trim(std::u32string(str), chars);
    }

    /** @copydoc upper */
    inline std::string
    upper(const char *str, const std::locale &loc = std::locale{}) {
        return upper(std::string(str), loc);
    }
    /** @copydoc upper */
    inline std::wstring
    upper(const wchar_t *str, const std::locale &loc = std::locale{}) {
        return upper(std::wstring(str), loc);
    }
    /** @copydoc upper */
    inline std::u16string
    upper(const char16_t *str, const std::locale &loc = std::locale{}) {
        return upper(std::u16string(str), loc);
    }
    /** @copydoc upper */
    inline std::u32string
    upper(const char32_t *str, const std::locale &loc = std::locale{}) {
        return upper(std::u32string(str), loc);
    }

    /** @copydoc lower */
    inline std::string
    lower(const char *str, const std::locale &loc = std::locale{}) {
        return lower(std::string(str), loc);
    }
    /** @copydoc lower */
    inline std::wstring
    lower(const wchar_t *str, const std::locale &loc = std::locale{}) {
        return lower(std::wstring(str), loc);
    }
    /** @copydoc lower */
    inline std::u16string
    lower(const char16_t *str, const std::locale &loc = std::locale{}) {
        return lower(std::u16string(str), loc);
    }
    /** @copydoc lower */
    inline std::u32string
    lower(const char32_t *str, const std::locale &loc = std::locale{}) {
        return lower(std::u32string(str), loc);
    }

    /** @copydoc title */
    inline std::string
    title(const char *str, const std::locale &loc = std::locale{}) {
        return title(std::string(str), loc);
    }
    /** @copydoc title */
    inline std::wstring
    title(const wchar_t *str, const std::locale &loc = std::locale{}) {
        return title(std::wstring(str), loc);
    }
    /** @copydoc title */
    inline std::u16string
    title(const char16_t *str, const std::locale &loc = std::locale{}) {
        return title(std::u16string(str), loc);
    }
    /** @copydoc title */
    inline std::u32string
    title(const char32_t *str, const std::locale &loc = std::locale{}) {
        return title(std::u32string(str), loc);
    }

    // ———— C 字符串混用重载（basic_string + const CharT*） ————

    /** @copydoc partition */
    template <class CharT, class Traits, class Allocator>
    BasicPartitionResult<CharT, Traits, Allocator>
    partition(const std::basic_string<CharT, Traits, Allocator> &text,
        const CharT                                             *sep) {
        return partition(
            text, std::basic_string<CharT, Traits, Allocator>(sep));
    }

    /** @copydoc split */
    template <class CharT, class Traits, class Allocator>
    std::vector<std::basic_string<CharT, Traits, Allocator>>
    split(const std::basic_string<CharT, Traits, Allocator> &text,
        const CharT                                         *sep) {
        return split(text, std::basic_string<CharT, Traits, Allocator>(sep));
    }

    // ———— C 字符串字面量重载（const CharT* + basic_string） ————

    /** @copydoc partition */
    inline BasicPartitionResult<>
    partition(const char *text, const std::string &sep) {
        return partition(std::string(text), sep);
    }
    /** @copydoc partition */
    inline BasicPartitionResult<wchar_t>
    partition(const wchar_t *text, const std::wstring &sep) {
        return partition(std::wstring(text), sep);
    }
    /** @copydoc partition */
    inline BasicPartitionResult<char16_t>
    partition(const char16_t *text, const std::u16string &sep) {
        return partition(std::u16string(text), sep);
    }
    /** @copydoc partition */
    inline BasicPartitionResult<char32_t>
    partition(const char32_t *text, const std::u32string &sep) {
        return partition(std::u32string(text), sep);
    }

    /** @copydoc split */
    inline std::vector<std::string>
    split(const char *text, const std::string &sep = " ") {
        return split(std::string(text), sep);
    }
    /** @copydoc split */
    inline std::vector<std::wstring>
    split(const wchar_t *text, const std::wstring &sep = L" ") {
        return split(std::wstring(text), sep);
    }
    /** @copydoc split */
    inline std::vector<std::u16string>
    split(const char16_t *text, const std::u16string &sep = u" ") {
        return split(std::u16string(text), sep);
    }
    /** @copydoc split */
    inline std::vector<std::u32string>
    split(const char32_t *text, const std::u32string &sep = U" ") {
        return split(std::u32string(text), sep);
    }

    /** @} */ // end of strpp group
}
