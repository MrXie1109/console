/**
 * @file config.h
 * @brief INI 配置文件解析与操作类
 * @details 提供 INI 格式配置文件的读取、写入、查询和修改功能
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
#include <map>

#include "file.h"
#include "maybe.h"
#include "strpp.h"

namespace console {
    /**
     * @brief INI 配置文件操作类。
     * @tparam CharT 字符类型，支持 char 和 wchar_t。
     * @details 支持加载、保存、读取、修改 INI 格式的配置文件，本类的 INI
     * 变种支持空节头和空键。
     */
    template <class CharT = char>
    class BasicINIConfig {
        using string_type = std::basic_string<CharT>;

        std::map<string_type, std::map<string_type, string_type>> data_;

    public:
        /// @brief 默认构造函数，创建空的配置对象。
        BasicINIConfig() = default;

        /**
         * @brief 从指定文件加载配置的构造函数。
         * @param filename 配置文件路径。
         */
        BasicINIConfig(const std::string &filename) { load(Path(filename)); }

        /**
         * @brief 从指定文件加载配置的构造函数。
         * @param filename 配置文件路径。
         */
        BasicINIConfig(const Path &filename) { load(filename); }

        /**
         * @brief 从文件加载 INI 配置。
         * @param filename 配置文件路径。
         * @details 支持行首注释（; 或 #），支持节头 [section]。
         */
        void load(const Path &filename) {
            std::fstream fs = filename.stream();
            fs >> *this;
        }

        /**
         * @brief 将配置保存到文件。
         * @param filename 目标文件路径。
         * @throw FileError 文件打开失败或写入流异常时抛出。
         */
        void save(const std::string &filename) { save(Path(filename)); }

        /**
         * @brief 将配置保存到文件。
         * @param filename 目标文件路径。
         * @throw FileError 文件打开失败或写入流异常时抛出。
         */
        void save(const Path &filename) const {
            std::ofstream fout(filename.str(), std::ios::trunc);
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + filename.str() + '"');
            // 用 narrow 流输出，通过窄字符串中转
            std::ostringstream oss;
            write_to_stream(oss);
            fout << oss.str();
            if (!fout.good())
                throw FileError(
                    "The Stream of \"" + filename.str() + "\" is Not Good");
        }

    private:
        void write_to_stream(std::ostream &os) const {
            for (const auto &p : data_) {
                if (!p.first.empty())
                    os << '[' << to_narrow(p.first) << ']' << '\n';
                for (const auto &kv : p.second)
                    os << to_narrow(kv.first) << " = " << to_narrow(kv.second)
                       << '\n';
                os << '\n';
            }
        }

        static std::string to_narrow(const string_type &s) {
            return std::string(s.begin(), s.end());
        }

        void read_from_stream(std::istream &is) {
            std::string line;
            string_type current_section;
            while (std::getline(is, line)) {
                string_type trimmed_line
                    = trim(string_type(line.begin(), line.end()));
                if (trimmed_line.empty() || trimmed_line[0] == CharT(';')
                    || trimmed_line[0] == CharT('#'))
                    continue;
                if (trimmed_line.front() == CharT('[')
                    && trimmed_line.back() == CharT(']'))
                    current_section
                        = trim(trimmed_line.substr(1, trimmed_line.size() - 2));
                else {
                    auto pos = trimmed_line.find(CharT('='));
                    if (pos != string_type::npos) {
                        string_type key   = trim(trimmed_line.substr(0, pos));
                        string_type value = trim(trimmed_line.substr(pos + 1));
                        data_[current_section][key] = value;
                    }
                }
            }
        }

    public:
        /**
         * @brief 配置对象的输出流运算符，按 INI 格式输出配置内容。
         */
        friend std::basic_ostream<CharT> &operator<<(
            std::basic_ostream<CharT> &os, const BasicINIConfig &config) {
            for (const auto &p : config.data_) {
                if (!p.first.empty())
                    os << CharT('[') << p.first << CharT(']') << CharT('\n');
                for (const auto &kv : p.second)
                    os << kv.first << " = " << kv.second << CharT('\n');
                os << CharT('\n');
            }
            return os;
        }

        /**
         * @brief 配置对象的输入流运算符，按 INI 格式输入配置内容。
         */
        friend std::basic_istream<CharT> &
        operator>>(std::basic_istream<CharT> &is, BasicINIConfig &config) {
            string_type line;
            string_type current_section;
            while (std::getline(is, line)) {
                string_type trimmed_line = trim(line);
                if (trimmed_line.empty() || trimmed_line[0] == CharT(';')
                    || trimmed_line[0] == CharT('#'))
                    continue;
                if (trimmed_line.front() == CharT('[')
                    && trimmed_line.back() == CharT(']'))
                    current_section
                        = trim(trimmed_line.substr(1, trimmed_line.size() - 2));
                else {
                    auto pos = trimmed_line.find(CharT('='));
                    if (pos != string_type::npos) {
                        string_type key   = trim(trimmed_line.substr(0, pos));
                        string_type value = trim(trimmed_line.substr(pos + 1));
                        config.data_[current_section][key] = value;
                    }
                }
            }
            return is;
        }

        /**
         * @brief 配置项代理类，支持隐式类型转换。
         */
        class Item {
            string_type str_;

        public:
            Item(const string_type &s) : str_(s) {}

            operator string_type() const { return str_; }

            operator bool() const {
                string_type lower = str_;
                for (auto &c : lower) c = std::tolower(c, std::locale{});
                if (lower
                    == string_type{//
                        CharT('t'),
                        CharT('r'),
                        CharT('u'),
                        CharT('e')}
                    || lower == string_type{CharT('1')}
                    || lower == string_type{CharT('y'), CharT('e'), CharT('s')}
                    || lower == string_type{CharT('o'), CharT('n')})
                    return true;
                if (lower
                    == string_type{//
                        CharT('f'),
                        CharT('a'),
                        CharT('l'),
                        CharT('s'),
                        CharT('e')}
                    || lower == string_type{CharT('0')}
                    || lower == string_type{CharT('n'), CharT('o')}
                    || lower == string_type{CharT('o'), CharT('f'), CharT('f')})
                    return false;
                throw TypeError(
                    "Failed to Convert \"" + to_narrow(str_) + "\" to bool");
            }

            template <class T>
            operator T() const {
                std::basic_istringstream<CharT> iss(str_);
                T                               value;
                iss >> value;
                if (iss.fail())
                    throw TypeError("Failed to Convert \"" + to_narrow(str_)
                                    + "\" to Target Type");
                return value;
            }
        };

        /**
         * @brief 获取配置项的值。
         * @param section_and_key 节和键，格式为 "节名.键名"。
         * @return Item 配置项代理对象，可隐式转换为目标类型。
         * @throw IndexError 格式错误、节不存在或键不存在时抛出。
         */
        Item get(const string_type &section_and_key) const {
            auto pr = partition(section_and_key, string_type(1, CharT('.')));
            if (pr.middle.empty())
                throw IndexError("Invalid Section and Key Format: \""
                                 + to_narrow(section_and_key) + '"');
            auto section = pr.left;
            auto key     = pr.right;
            auto sec_it  = data_.find(section);
            if (sec_it == data_.end())
                throw IndexError(
                    "Section \"" + to_narrow(section) + "\" Not Found");
            auto key_it = sec_it->second.find(key);
            if (key_it == sec_it->second.end())
                throw IndexError("Key \"" + to_narrow(key)
                                 + "\" Not Found in Section \""
                                 + to_narrow(section) + '"');
            return Item(key_it->second);
        }

        /**
         * @brief 获取配置项的值，若不存在则返回默认值。
         * @tparam T 目标类型。
         * @param section_and_key 节和键，格式为 "节名.键名"。
         * @param default_value 默认值，当配置项不存在时返回。
         * @return T 配置值或默认值。
         */
        template <class T>
        T
        get(const string_type &section_and_key, const T &default_value) const {
            auto pr = partition(section_and_key, string_type(1, CharT('.')));
            if (pr.middle.empty()) return default_value;
            auto section = pr.left;
            auto key     = pr.right;
            auto sec_it  = data_.find(section);
            if (sec_it == data_.end()) return default_value;
            auto key_it = sec_it->second.find(key);
            if (key_it == sec_it->second.end()) return default_value;
            return T(Item(key_it->second));
        }

        /**
         * @brief 设置配置项的值。
         * @param section_and_key 节和键，格式为 "节名.键名"。
         * @param value 要设置的值。
         * @throw IndexError 格式错误时抛出。
         * @note 若节或键不存在，会自动创建。
         */
        void set(const string_type &section_and_key, const string_type &value) {
            auto pr = partition(section_and_key, string_type(1, CharT('.')));
            if (pr.middle.empty())
                throw IndexError("Invalid Section and Key Format: \""
                                 + to_narrow(section_and_key) + '"');
            auto section        = pr.left;
            auto key            = pr.right;
            data_[section][key] = value;
        }

        /**
         * @brief 检查配置项或节是否存在。
         * @param section_and_key 节和键，格式为 "节名.键名" 或仅 "节名"。
         * @return bool 存在返回 true，否则返回 false。
         */
        bool has(const string_type &section_and_key) const {
            auto pr = partition(section_and_key, string_type(1, CharT('.')));
            if (pr.middle.empty()) {
                auto section = pr.left;
                return data_.find(section) != data_.end();
            } else {
                auto section = pr.left;
                auto key     = pr.right;
                auto sec_it  = data_.find(section);
                if (sec_it == data_.end()) return false;
                return sec_it->second.find(key) != sec_it->second.end();
            }
        }

        /**
         * @brief 删除配置项或节。
         * @param section_and_key 节和键，格式为 "节名.键名" 或仅 "节名"。
         * @return bool 成功删除返回 true，未找到返回 false。
         */
        bool remove(const string_type &section_and_key) {
            auto pr = partition(section_and_key, string_type(1, CharT('.')));
            if (pr.middle.empty()) {
                auto section = pr.left;
                return data_.erase(section) > 0;
            } else {
                auto section = pr.left;
                auto key     = pr.right;
                auto sec_it  = data_.find(section);
                if (sec_it == data_.end()) return false;
                return sec_it->second.erase(key) > 0;
            }
        }

        /**
         * @brief 获取原始数据结构的只读引用。
         */
        const std::map<string_type, std::map<string_type, string_type>> &
        data() const {
            return data_;
        }
    };

    /** @brief BasicINIConfig<char> 的类型别名。 */
    using INIConfig = BasicINIConfig<char>;

    /** @brief BasicINIConfig<wchar_t> 的类型别名。 */
    using WINIConfig = BasicINIConfig<wchar_t>;

}
