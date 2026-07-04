/**
 * @file re.h
 * @brief 提供正则表达式匹配、搜索、替换、分割等操作，接口类似 Python 的 re
 * 模块。
 * @details 封装了 C++ 标准库的 std::regex，提供更便捷的面向对象和函数式接口。
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
#include <regex>
#include <string>
#include <vector>

#include "iter.h"

namespace console {
    /**
     * @class Regex
     * @brief 正则表达式对象，封装编译后的模式，提供匹配、搜索、替换等功能。
     * @details 该类类似于 Python 的 re.compile 返回的对象，支持链式操作。
     */
    class Regex {
    private:
        std::regex pattern; ///< 编译后的正则表达式

    public:
        /**
         * @brief 构造一个 Regex 对象。
         * @param pat 正则表达式字符串。
         * @param flags 语法标志，默认为 ECMAScript。
         */
        Regex(const std::string  &pat,
            std::regex::flag_type flags = std::regex::ECMAScript) :
            pattern(pat, flags) {}

        /**
         * @class Match
         * @brief 匹配结果对象，包含匹配信息。
         * @details 提供访问匹配组、位置索引等功能，类似 Python 的 re.Match
         * 对象。
         */
        class Match {
        private:
            std::smatch match; ///< 底层 std::smatch 对象
            bool        success; ///< 是否匹配成功

        public:
            /// @brief 默认构造，表示不成功的匹配。
            Match() : success(false) {}

            /// @brief 从 std::smatch 构造。
            Match(const std::smatch &m) : match(m), success(true) {}

            /**
             * @brief 获取指定捕获组的内容。
             * @param n 组索引（0 表示整个匹配，默认为
             * 0）。若索引无效则返回空字符串。
             * @return std::string 匹配的子串。
             */
            std::string group(int n = 0) const {
                if (!success || n < 0 || n >= int(match.size())) {
                    return "";
                }
                return match[n].str();
            }

            /**
             * @brief 获取所有捕获组（不包括整个匹配）的内容。
             * @return std::vector<std::string> 各捕获组的字符串列表。
             */
            std::vector<std::string> groups() const {
                std::vector<std::string> result;
                if (!success) return result;
                for (size_t i = 1; i < match.size(); ++i) {
                    result.push_back(match[i].str());
                }
                return result;
            }

            /**
             * @brief 获取指定捕获组在原始字符串中的起始索引。
             * @param n 组索引，默认为 0。
             * @return int 起始索引，若无效则返回 -1。
             */
            int start(int n = 0) const {
                if (!success || n < 0 || n >= int(match.size())) {
                    return -1;
                }
                return match.position(n);
            }

            /**
             * @brief
             * 获取指定捕获组在原始字符串中的结束索引（最后一个字符之后的位置）。
             * @param n 组索引，默认为 0。
             * @return int 结束索引，若无效则返回 -1。
             */
            int end(int n = 0) const {
                if (!success || n < 0 || n >= int(match.size())) {
                    return -1;
                }
                return match.position(n) + match.length(n);
            }

            /**
             * @brief 获取指定捕获组的起始和结束索引组成的 pair。
             * @param n 组索引，默认为 0。
             * @return std::pair<int, int> (start, end)。
             */
            std::pair<int, int> span(int n = 0) const {
                return {start(n), end(n)};
            }

            /**
             * @brief 检查匹配是否成功。
             * @return true 若匹配成功，否则 false。
             */
            explicit operator bool() const { return success; }
        };

        /**
         * @brief 在文本中搜索第一个匹配。
         * @param text 要搜索的字符串。
         * @return Match 匹配结果对象。
         */
        Match search(const std::string &text) const {
            std::smatch match;
            if (std::regex_search(text, match, pattern)) {
                return Match(match);
            }
            return Match();
        }

        /**
         * @brief 尝试从文本开头匹配整个模式。
         * @param text 要匹配的字符串。
         * @return Match 匹配结果对象。
         */
        Match match(const std::string &text) const {
            std::smatch match;
            if (std::regex_match(text, match, pattern)) {
                return Match(match);
            }
            return Match();
        }

        /**
         * @brief match 的别名，与 Python 的 re.fullmatch 语义相同。
         * @param text 要匹配的字符串。
         * @return Match 匹配结果对象。
         */
        Match fullmatch(const std::string &text) const { return match(text); }

        /**
         * @brief 查找所有不重叠的匹配，返回匹配字符串列表。
         * @param text 要搜索的字符串。
         * @return std::vector<std::string> 所有匹配的字符串。
         */
        std::vector<std::string> findall(const std::string &text) const {
            std::vector<std::string> result;
            auto                     begin
                = std::sregex_iterator(text.begin(), text.end(), pattern);
            auto end = std::sregex_iterator();

            for (auto it = begin; it != end; ++it) {
                result.push_back(it->str());
            }
            return result;
        }

        /**
         * @class Iterator
         * @brief 迭代器，用于遍历所有匹配。
         */
        class Iterator {
        private:
            std::sregex_iterator it;

        public:
            Iterator(std::sregex_iterator i) : it(i) {}

            bool operator==(const Iterator &other) const {
                return it == other.it;
            }
            bool operator!=(const Iterator &other) const {
                return it != other.it;
            }
            void  operator++() { ++it; }
            Match operator*() const { return Match(*it); }
        };

        /**
         * @brief 返回一个可迭代对象，遍历所有匹配的 Match 对象。
         * @param text 要搜索的字符串。
         * @return IteratorPair<std::sregex_iterator> 可用于范围 for 循环。
         */
        IteratorPair<std::sregex_iterator>
        finditer(const std::string &text) const {
            return {{std::sregex_iterator(text.begin(), text.end(), pattern)},
                {std::sregex_iterator()}};
        }

        /**
         * @brief 使用正则表达式分割字符串。
         * @param text 要分割的字符串。
         * @param maxsplit 最大分割次数，0 表示不限。
         * @return std::vector<std::string> 分割后的子串列表。
         */
        std::vector<std::string>
        split(const std::string &text, int maxsplit = 0) const {
            std::vector<std::string>   result;
            std::sregex_token_iterator it(
                text.begin(), text.end(), pattern, -1);
            std::sregex_token_iterator end;

            int count = 0;
            for (; it != end && (maxsplit <= 0 || count < maxsplit);
                 ++it, ++count) {
                result.push_back(*it);
            }

            if (maxsplit > 0 && count == maxsplit && it != end) {
                std::string remaining;
                for (; it != end; ++it) {
                    if (!remaining.empty())
                        remaining += *it;
                    else
                        remaining = *it;
                }
                result.push_back(remaining);
            }

            return result;
        }

        /**
         * @brief 替换匹配的子串（最多 count 次）。
         * @param repl 替换字符串。
         * @param text 原始文本。
         * @param count 最大替换次数，0 表示替换所有。
         * @return std::string 替换后的字符串。
         */
        std::string sub(const std::string &repl,
            const std::string             &text,
            int                            count = 0) const {
            if (count <= 0) {
                return std::regex_replace(text, pattern, repl);
            }

            std::string result;
            auto        begin
                = std::sregex_iterator(text.begin(), text.end(), pattern);
            auto end      = std::sregex_iterator();
            auto last     = text.begin();
            int  replaced = 0;

            for (auto it = begin; it != end && replaced < count;
                 ++it, ++replaced) {
                result.append(last, text.begin() + it->position());
                result.append(repl);
                last = text.begin() + it->position() + it->length();
            }
            result.append(last, text.end());

            return result;
        }

        /**
         * @brief 替换匹配的子串并返回替换次数。
         * @param repl 替换字符串。
         * @param text 原始文本。
         * @param count 最大替换次数，0 表示替换所有。
         * @return std::pair<std::string, int> (新字符串, 替换次数)。
         */
        std::pair<std::string, int> subn(const std::string &repl,
            const std::string                              &text,
            int                                             count = 0) const {
            if (count <= 0) {
                auto result = std::regex_replace(text, pattern, repl);
                auto begin
                    = std::sregex_iterator(text.begin(), text.end(), pattern);
                auto end = std::sregex_iterator();
                int  cnt = std::distance(begin, end);
                return {result, cnt};
            }

            std::string result;
            auto        begin
                = std::sregex_iterator(text.begin(), text.end(), pattern);
            auto end      = std::sregex_iterator();
            auto last     = text.begin();
            int  replaced = 0;

            for (auto it = begin; it != end && replaced < count;
                 ++it, ++replaced) {
                result.append(last, text.begin() + it->position());
                result.append(repl);
                last = text.begin() + it->position() + it->length();
            }
            result.append(last, text.end());

            return {result, replaced};
        }
    };

    /**
     * @namespace re
     * @brief 函数式正则表达式接口，模仿 Python 的 re 模块。
     */
    namespace re {
        /**
         * @brief 编译正则表达式并返回 Regex 对象。
         * @param pattern 正则字符串。
         * @param flags 语法标志。
         * @return Regex 编译后的对象。
         */
        inline Regex compile(const std::string &pattern,
            std::regex::flag_type flags = std::regex::ECMAScript) {
            return Regex(pattern, flags);
        }

        /**
         * @brief 搜索第一个匹配（函数式接口）。
         * @param pattern 正则字符串。
         * @param text 目标文本。
         * @return Regex::Match 匹配结果。
         */
        inline Regex::Match
        search(const std::string &pattern, const std::string &text) {
            return Regex(pattern).search(text);
        }

        /**
         * @brief 从开头匹配（函数式接口）。
         * @param pattern 正则字符串。
         * @param text 目标文本。
         * @return Regex::Match 匹配结果。
         */
        inline Regex::Match
        match(const std::string &pattern, const std::string &text) {
            return Regex(pattern).match(text);
        }

        /**
         * @brief 查找所有匹配的字符串（函数式接口）。
         * @param pattern 正则字符串。
         * @param text 目标文本。
         * @return std::vector<std::string> 匹配列表。
         */
        inline std::vector<std::string>
        findall(const std::string &pattern, const std::string &text) {
            return Regex(pattern).findall(text);
        }

        /**
         * @brief 返回一个可迭代对象，遍历所有匹配的 Match 对象（函数式接口）。
         * @param pattern 正则字符串。
         * @param text 目标文本。
         * @return console::IteratorPair<std::sregex_iterator> 可用于范围 for
         * 循环。
         */
        inline IteratorPair<std::sregex_iterator>
        finditer(const std::string &pattern, const std::string &text) {
            return Regex(pattern).finditer(text);
        }

        /**
         * @brief 分割字符串（函数式接口）。
         * @param pattern 正则字符串。
         * @param text 目标文本。
         * @param maxsplit 最大分割次数。
         * @return std::vector<std::string> 分割后的子串列表。
         */
        inline std::vector<std::string> split(const std::string &pattern,
            const std::string                                   &text,
            int                                                  maxsplit = 0) {
            return Regex(pattern).split(text, maxsplit);
        }

        /**
         * @brief 替换匹配的子串（函数式接口）。
         * @param pattern 正则字符串。
         * @param repl 替换字符串。
         * @param text 原始文本。
         * @param count 最大替换次数。
         * @return std::string 替换后的字符串。
         */
        inline std::string sub(const std::string &pattern,
            const std::string                    &repl,
            const std::string                    &text,
            int                                   count = 0) {
            return Regex(pattern).sub(repl, text, count);
        }

        /**
         * @brief 转义正则表达式中的特殊字符。
         * @param s 原始字符串。
         * @return std::string 转义后的字符串。
         */
        inline std::string escape(const std::string &s) {
            static std::regex special(R"([.^$*+?()\[\]{}|\\])");
            return std::regex_replace(s, special, R"(\$&)");
        }
    }
}
