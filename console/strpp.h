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
     * @param str 要处理的字符串（按值传递，内部修改副本）。
     * @return std::string 处理后的新字符串。
     */
    inline std::string ltrim(std::string str) {
        auto it = std::find_if(str.begin(),
            str.end(),
            [](unsigned char uc) -> bool { return !isspace(uc); });
        str.erase(str.begin(), it);
        return str;
    }

    /**
     * @brief 移除字符串右侧的空白字符。
     * @param str 要处理的字符串。
     * @return std::string 处理后的新字符串。
     */
    inline std::string rtrim(std::string str) {
        auto it = std::find_if(str.rbegin(),
            str.rend(),
            [](unsigned char uc) -> bool { return !isspace(uc); });
        str.erase(it.base(), str.end());
        return str;
    }

    /**
     * @brief 移除字符串两侧的空白字符。
     * @param str 要处理的字符串。
     * @return std::string 处理后的新字符串。
     */
    inline std::string trim(std::string str) {
        return ltrim(rtrim(str));
    }

    /**
     * @brief 移除字符串左侧指定的字符集合。
     * @param str 要处理的字符串。
     * @param chars 要删除的字符集（只要字符出现在此集合中就被删除）。
     * @return std::string 处理后的新字符串。
     */
    inline std::string ltrim(std::string str, const std::string &chars) {
        auto it
            = std::find_if(str.begin(), str.end(), [&chars](unsigned char ch) {
                  return chars.find(ch) == std::string::npos;
              });
        str.erase(str.begin(), it);
        return str;
    }

    /**
     * @brief 移除字符串右侧指定的字符集合。
     * @param str 要处理的字符串。
     * @param chars 要删除的字符集。
     * @return std::string 处理后的新字符串。
     */
    inline std::string rtrim(std::string str, const std::string &chars) {
        auto it = std::find_if(
            str.rbegin(), str.rend(), [&chars](unsigned char ch) {
                return chars.find(ch) == std::string::npos;
            });
        str.erase(it.base(), str.end());
        return str;
    }

    /**
     * @brief 移除字符串两侧指定的字符集合。
     * @param str 要处理的字符串。
     * @param chars 要删除的字符集。
     * @return std::string 处理后的新字符串。
     */
    inline std::string trim(std::string str, const std::string &chars) {
        return ltrim(rtrim(str, chars), chars);
    }

    /**
     * @brief 将字符串转换为大写。
     * @param str 要转换的字符串。
     * @return std::string 大写形式。
     */
    inline std::string upper(std::string str) {
        for (char &ch : str) {
            if (ch >= 'a' && ch <= 'z') ch += 'A' - 'a';
        }
        return str;
    }

    /**
     * @brief 将字符串转换为小写。
     * @param str 要转换的字符串。
     * @return std::string 小写形式。
     */
    inline std::string lower(std::string str) {
        for (char &ch : str) {
            if (ch >= 'A' && ch <= 'Z') ch -= 'A' - 'a';
        }
        return str;
    }

    /**
     * @brief 将字符串转换为标题格式（每个单词首字母大写，其余小写）。
     * @param str 要转换的字符串。
     * @return std::string 标题格式的字符串。
     */
    inline std::string title(std::string str) {
        if (str.empty()) return "";
        if (str[0] >= 'a' && str[0] <= 'z') str[0] += 'A' - 'a';
        for (size_t i = 1; i < str.size(); ++i) {
            if (isspace((unsigned char)str[i - 1]) && str[i] >= 'a'
                && str[i] <= 'z')
                str[i] += 'A' - 'a';
            else if (str[i] >= 'A' && str[i] <= 'Z')
                str[i] -= 'A' - 'a';
        }
        return str;
    }

    /**
     * @struct PartitionResult
     * @brief 字符串分区结果，包含左部分、分隔符、右部分。
     */
    struct PartitionResult {
        std::string left; ///< 分隔符前的子串
        std::string middle; ///< 分隔符本身
        std::string right; ///< 分隔符后的子串

        /**
         * @brief 输出分区结果到流，格式为 ("left", "middle", "right")。
         * @param os 输出流。
         * @param pr 分区结果对象。
         * @return std::ostream& 流引用。
         */
        friend std::ostream &
        operator<<(std::ostream &os, const PartitionResult &pr) {
            return os << "(\"" << pr.left << "\", \"" << pr.middle << "\", \""
                      << pr.right << "\")";
        }
    };

    /**
     * @brief
     * 在字符串中查找第一个分隔符，并返回分隔符之前、分隔符本身、分隔符之后的三部分。
     * @param text 原始字符串。
     * @param sep 分隔符。
     * @return PartitionResult 分区结果。
     * @note 若未找到分隔符，则 left 为原字符串，middle 和 right 为空。
     */
    inline PartitionResult
    partition(const std::string &text, const std::string &sep) {
        size_t pos = text.find(sep);
        if (pos == std::string::npos) return PartitionResult{text, "", ""};
        return PartitionResult{
            text.substr(0, pos), sep, text.substr(pos + sep.size())};
    }

    /**
     * @brief 以分隔符分割字符串（类似 Python 的 split，默认按空格分割）。
     * @param text 要分割的字符串。
     * @param sep 分隔符，默认为空格 " "。
     * @return std::vector<std::string> 分割后的子串列表。
     * @note 连续的分隔符会产生空字符串子串。
     */
    inline std::vector<std::string>
    split(std::string text, const std::string &sep = " ") {
        std::vector<std::string> vec;
        size_t                   start = 0;
        size_t                   end;
        size_t                   sep_len = sep.length();
        while ((end = text.find(sep, start)) != std::string::npos) {
            vec.emplace_back(text, start, end - start);
            start = end + sep_len;
        }
        vec.emplace_back(text, start);
        return vec;
    }

    /**
     * @brief 以分隔符连接容器中的字符串元素。
     * @tparam T 元素类型（必须支持输出到 std::stringstream）。
     * @param vec 元素容器。
     * @param sep 分隔符，默认为空字符串。
     * @return std::string 连接后的字符串。
     */
    template <class T>
    inline std::string
    join(const std::vector<T> &vec, const std::string &sep = "") {
        if (vec.empty()) return "";
        std::stringstream ss;
        auto              it = vec.begin();
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
    template <class... Args> std::string to_string(Args &&...args) {
        std::ostringstream oss;
        int                _[] = {0, (oss << std::forward<Args>(args), 0)...};
        (void)_;
        return oss.str();
    }

    /**
     * @brief 将字符串中的值提取到多个参数中，参数类型由调用者指定。
     * @tparam Args 参数类型包。
     * @param str 包含要提取值的字符串，值之间应以空格分隔。
     * @param args 要提取值的参数列表，按顺序对应字符串中的值。
     */
    template <class... Args> void from_string(std::string str, Args &...args) {
        std::istringstream iss(str);
        int                _[] = {0, (iss >> args, 0)...};
        (void)_;
    }

    /**
     * @class f_string
     * @brief 格式化字符串类，支持使用 `%` 运算符进行占位符 `{}` 替换。
     * @details 继承自 std::string，通过 `operator%` 将第一个 `{}`
     * 替换为参数的字符串表示。 若字符串中不包含 `{}` 则抛出 bad_format 异常。
     *
     * 使用示例：
     * @code
     * f_string fmt = "Hello, {}!"_f;
     * std::string result = fmt % "world";  // "Hello, world!"
     * @endcode
     */
    class f_string : public std::string {
    public:
        using std::string::string;

        /**
         * @brief 用参数替换第一个 `{}` 占位符。
         * @tparam T 参数类型。
         * @param t 要替换的值。
         * @return f_string 替换后的新 f_string 对象。
         * @throw bad_format 若当前字符串中不包含 `{}`。
         */
        template <class T> f_string operator%(const T &t) {
            auto pos = this->find("{}");
            if (pos != std::string::npos) {
                auto bak = *this;
                bak.replace(pos, 2, uni_to_str(t));
                return bak;
            }
            throw BadFormat("Bad Format");
        }
    };

    /** @} */ // end of strpp group
}
