/**
 * @file re.h
 * @brief 提供正则表达式匹配、搜索、替换、分割等操作，接口类似 Python 的 re 模块。
 * @details 封装了 C++ 标准库的 std::basic_regex，提供更便捷的面向对象和函数式接口。
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
     * @class BasicRegex
     * @brief 正则表达式对象，封装编译后的模式，提供匹配、搜索、替换等功能。
     * @tparam CharT 字符类型（char 或 wchar_t）。
     * @tparam Traits 字符特征类型。
     */
    template <class CharT = char, class Traits = std::char_traits<CharT>>
    class BasicRegex {
    public:
        using string_type = std::basic_string<CharT, Traits>;
        using regex_type  = std::basic_regex<CharT>;
        using match_type
            = std::match_results<typename string_type::const_iterator>;
        using flag_type = typename regex_type::flag_type;
        using iterator_type
            = std::regex_iterator<typename string_type::const_iterator,
                CharT,
                std::regex_traits<CharT>>;
        using token_iterator
            = std::regex_token_iterator<typename string_type::const_iterator,
                CharT,
                std::regex_traits<CharT>>;

    private:
        regex_type pattern;

    public:
        BasicRegex(const string_type &pat,
            flag_type flags = regex_type::ECMAScript) : pattern(pat, flags) {}

        /**
         * @class Match
         * @brief 匹配结果对象，保存文本副本和位置信息，不依赖迭代器有效性。
         */
        class Match {
        private:
            string_type text_copy;
            struct GroupInfo {
                std::ptrdiff_t pos;
                std::ptrdiff_t len;
            };
            std::vector<GroupInfo> groups_;
            bool                   success;

        public:
            Match() : success(false) {}
            Match(string_type t, const match_type &m) :
                text_copy(std::move(t)), success(true) {
                for (size_t i = 0; i < m.size(); ++i)
                    groups_.push_back({m.position(i), m.length(i)});
            }

            string_type group(int n = 0) const {
                if (!success || n < 0 || n >= int(groups_.size()))
                    return string_type{};
                return text_copy.substr(groups_[n].pos, groups_[n].len);
            }

            std::vector<string_type> groups() const {
                std::vector<string_type> result;
                if (!success) return result;
                for (size_t i = 1; i < groups_.size(); ++i)
                    result.push_back(
                        text_copy.substr(groups_[i].pos, groups_[i].len));
                return result;
            }

            int start(int n = 0) const {
                if (!success || n < 0 || n >= int(groups_.size())) return -1;
                return int(groups_[n].pos);
            }

            int end(int n = 0) const {
                if (!success || n < 0 || n >= int(groups_.size())) return -1;
                return int(groups_[n].pos + groups_[n].len);
            }

            std::pair<int, int> span(int n = 0) const {
                return {start(n), end(n)};
            }

            explicit operator bool() const { return success; }
        };

        /**
         * @brief 搜索第一个匹配。
         */
        Match search(string_type text) const {
            match_type m;
            auto       copy = text;
            if (std::regex_search(copy, m, pattern))
                return Match(std::move(copy), m);
            return Match();
        }

        /**
         * @brief 从开头匹配整个模式。
         */
        Match match(string_type text) const {
            match_type m;
            auto       copy = text;
            if (std::regex_match(copy, m, pattern))
                return Match(std::move(copy), m);
            return Match();
        }

        Match fullmatch(const string_type &text) const { return match(text); }

        /**
         * @brief 查找所有不重叠的匹配。
         */
        std::vector<string_type> findall(const string_type &text) const {
            std::vector<string_type> result;
            auto begin = iterator_type(text.begin(), text.end(), pattern);
            auto end   = iterator_type();

            for (auto it = begin; it != end; ++it) result.push_back(it->str());
            return result;
        }

        /**
         * @brief 使用正则表达式分割字符串。
         */
        std::vector<string_type>
        split(const string_type &text, int maxsplit = 0) const {
            std::vector<string_type> result;
            token_iterator           it(text.begin(), text.end(), pattern, -1);
            token_iterator           end;

            int count = 0;
            for (; it != end && (maxsplit <= 0 || count < maxsplit);
                 ++it, ++count)
                result.push_back(*it);

            if (maxsplit > 0 && count == maxsplit && it != end) {
                string_type remaining;
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
         * @brief 替换匹配的子串。
         */
        string_type sub(const string_type &repl,
            const string_type             &text,
            int                            count = 0) const {
            if (count <= 0) return std::regex_replace(text, pattern, repl);

            string_type result;
            auto begin    = iterator_type(text.begin(), text.end(), pattern);
            auto end      = iterator_type();
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
         */
        std::pair<string_type, int> subn(const string_type &repl,
            const string_type                              &text,
            int                                             count = 0) const {
            if (count <= 0) {
                auto result = std::regex_replace(text, pattern, repl);
                auto begin  = iterator_type(text.begin(), text.end(), pattern);
                auto end    = iterator_type();
                int  cnt    = int(std::distance(begin, end));
                return {result, cnt};
            }

            string_type result;
            auto begin    = iterator_type(text.begin(), text.end(), pattern);
            auto end      = iterator_type();
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

    using Regex  = BasicRegex<char>;
    using WRegex = BasicRegex<wchar_t>;

    namespace re {

        inline Regex compile(const std::string &pattern,
            std::regex::flag_type flags = std::regex::ECMAScript) {
            return Regex(pattern, flags);
        }

        inline WRegex compile(const std::wstring &pattern,
            std::wregex::flag_type flags = std::wregex::ECMAScript) {
            return WRegex(pattern, flags);
        }

        inline Regex::Match
        search(const std::string &pattern, const std::string &text) {
            return Regex(pattern).search(text);
        }

        inline WRegex::Match
        search(const std::wstring &pattern, const std::wstring &text) {
            return WRegex(pattern).search(text);
        }

        inline Regex::Match
        match(const std::string &pattern, const std::string &text) {
            return Regex(pattern).match(text);
        }

        inline WRegex::Match
        match(const std::wstring &pattern, const std::wstring &text) {
            return WRegex(pattern).match(text);
        }

        inline std::vector<std::string>
        findall(const std::string &pattern, const std::string &text) {
            return Regex(pattern).findall(text);
        }

        inline std::vector<std::wstring>
        findall(const std::wstring &pattern, const std::wstring &text) {
            return WRegex(pattern).findall(text);
        }

        inline std::vector<std::string> split(const std::string &pattern,
            const std::string                                   &text,
            int                                                  maxsplit = 0) {
            return Regex(pattern).split(text, maxsplit);
        }

        inline std::vector<std::wstring> split(const std::wstring &pattern,
            const std::wstring                                    &text,
            int maxsplit = 0) {
            return WRegex(pattern).split(text, maxsplit);
        }

        inline std::string sub(const std::string &pattern,
            const std::string                    &repl,
            const std::string                    &text,
            int                                   count = 0) {
            return Regex(pattern).sub(repl, text, count);
        }

        inline std::wstring sub(const std::wstring &pattern,
            const std::wstring                     &repl,
            const std::wstring                     &text,
            int                                     count = 0) {
            return WRegex(pattern).sub(repl, text, count);
        }

        inline std::string escape(const std::string &s) {
            static std::regex special(R"([.^$*+?()\[\]{}|\\])");
            return std::regex_replace(s, special, R"(\$&)");
        }

        inline std::wstring escape(const std::wstring &s) {
            static std::wregex special(LR"([.^$*+?()\[\]{}|\\])");
            return std::regex_replace(s, special, LR"(\$&)");
        }
    }
}
