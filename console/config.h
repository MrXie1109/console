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
#include "strpp.h"
#include "maybe.h"
#include "file.h"

namespace console
{
    /**
     * @brief INI 配置文件操作类。
     * @details 支持加载、保存、读取、修改 INI 格式的配置文件，本类的 INI 变种支持空节头和空键。
     */
    class INIConfig
    {
        std::map<std::string, std::map<std::string, std::string>> data_;

    public:
        /**
         * @brief 默认构造函数，创建空的配置对象。
         */
        INIConfig() = default;

        /**
         * @brief 从指定文件加载配置的构造函数。
         * @param filename 配置文件路径。
         */
        INIConfig(const std::string &filename)
        {
            load(Path(filename));
        }

        /**
         * @brief 从指定文件加载配置的构造函数。
         * @param filename 配置文件路径。
         */
        INIConfig(const Path &filename)
        {
            load(filename);
        }

        /**
         * @brief 从文件加载 INI 配置。
         * @param filename 配置文件路径。
         * @details 支持行首注释（; 或 #），支持节头 [section]。
         */
        void load(const Path &filename)
        {
            filename.stream() >> *this;
        }

        /**
         * @brief 将配置保存到文件。
         * @param filename 目标文件路径。
         * @throw FileError 文件打开失败或写入流异常时抛出。
         */
        void save(const std::string &filename)
        {
            save(Path(filename));
        }

        /**
         * @brief 将配置保存到文件。
         * @param filename 目标文件路径。
         * @throw FileError 文件打开失败或写入流异常时抛出。
         */
        void save(const Path &filename) const
        {
            std::ofstream fout(filename.str(), std::ios::trunc);
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + filename.str() + '"');
            fout << *this;
            if (!fout.good())
                throw FileError("The Stream of \"" + filename.str() + "\" is Not Good");
        }

        /**
         * @brief 配置对象的输出流运算符，按 INI 格式输出配置内容。
         * @param os 输出流。
         * @param config 配置对象。
         * @return std::ostream& 输出流引用。
         */
        friend std::ostream &operator<<(std::ostream &os, const INIConfig &config)
        {
            for (const auto &p : config.data_)
            {
                if (!p.first.empty())
                    os << '[' << p.first << ']' << '\n';
                for (const auto &kv : p.second)
                    os << kv.first << " = " << kv.second << '\n';
                os << '\n';
            }
            return os;
        }

        /**
         * @brief 配置对象的输入流运算符，按 INI 格式输入配置内容。
         * @param is 输入流。
         * @param config 配置对象。
         * @return std::istream& 输入流引用。
         */
        friend std::istream &operator>>(std::istream &is, INIConfig &config)
        {
            std::string line;
            std::string current_section;
            while (std::getline(is, line))
            {
                std::string trimmed_line = trim(line);
                if (trimmed_line.empty() ||
                    trimmed_line[0] == ';' || trimmed_line[0] == '#')
                    continue;
                if (trimmed_line.front() == '[' && trimmed_line.back() == ']')
                    current_section = trim(
                        trimmed_line.substr(1, trimmed_line.size() - 2));
                else
                {
                    auto pos = trimmed_line.find('=');
                    if (pos != std::string::npos)
                    {
                        std::string key = trim(trimmed_line.substr(0, pos));
                        std::string value = trim(trimmed_line.substr(pos + 1));
                        config.data_[current_section][key] = value;
                    }
                }
            }
            return is;
        }

        /**
         * @brief 配置项代理类，支持隐式类型转换。
         * @details 用于封装配置值，支持自动转换为 string 或算术类型。
         */
        class Item
        {
            std::string str_;

        public:
            /**
             * @brief 用字符串构造配置项。
             * @param s 配置值的字符串形式。
             */
            Item(const std::string &s) : str_(s) {}

            /**
             * @brief 隐式转换为 std::string。
             */
            operator std::string() const
            {
                return str_;
            }

            /**
             * @brief 隐式转换为 bool。
             */
            operator bool() const
            {
                std::string lower = str_;
                for (auto &c : lower)
                    c = std::tolower(c);
                if (lower == "true" || lower == "1" || lower == "yes" || lower == "on")
                    return true;
                if (lower == "false" || lower == "0" || lower == "no" || lower == "off")
                    return false;
                throw TypeError("Failed to Convert \"" + str_ + "\" to bool");
            }

            /**
             * @brief 隐式转换为目标类型 T。
             * @tparam T 目标类型（如 int、float、double 等）。
             * @throw TypeError 类型转换失败时抛出。
             */
            template <class T>
            operator T() const
            {
                std::istringstream iss(str_);
                T value;
                iss >> value;
                if (iss.fail())
                    throw TypeError("Failed to Convert \"" + str_ + "\" to Target Type");
                return value;
            }
        };

        /**
         * @brief 获取配置项的值。
         * @param section_and_key 节和键，格式为 "节名.键名"。
         * @return Item 配置项代理对象，可隐式转换为目标类型。
         * @throw IndexError 格式错误、节不存在或键不存在时抛出。
         */
        Item get(const std::string &section_and_key) const
        {
            auto pos = section_and_key.find('.');
            auto pr = partition(section_and_key, ".");
            if (pr.middle.empty())
                throw IndexError("Invalid Section and Key Format: \"" + section_and_key + '"');
            auto section = pr.left;
            auto key = pr.right;
            auto sec_it = data_.find(section);
            if (sec_it == data_.end())
                throw IndexError("Section \"" + section + "\" Not Found");
            auto key_it = sec_it->second.find(key);
            if (key_it == sec_it->second.end())
                throw IndexError("Key \"" + key + "\" Not Found in Section \"" + section + '"');
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
        T get(const std::string &section_and_key, const T &default_value) const
        {
            auto pos = section_and_key.find('.');
            auto pr = partition(section_and_key, ".");
            if (pr.middle.empty())
                return default_value;
            auto section = pr.left;
            auto key = pr.right;
            auto sec_it = data_.find(section);
            if (sec_it == data_.end())
                return default_value;
            auto key_it = sec_it->second.find(key);
            if (key_it == sec_it->second.end())
                return default_value;
            return T(Item(key_it->second));
        }

        /**
         * @brief 设置配置项的值。
         * @param section_and_key 节和键，格式为 "节名.键名"。
         * @param value 要设置的值（字符串形式）。
         * @throw IndexError 格式错误时抛出。
         * @note 若节或键不存在，会自动创建。
         */
        void set(const std::string &section_and_key, const std::string &value)
        {
            auto pos = section_and_key.find('.');
            auto pr = partition(section_and_key, ".");
            if (pr.middle.empty())
                throw IndexError("Invalid Section and Key Format: \"" + section_and_key + '"');
            auto section = pr.left;
            auto key = pr.right;
            data_[section][key] = value;
        }

        /**
         * @brief 检查配置项或节是否存在。
         * @param section_and_key 节和键，格式为 "节名.键名" 或仅 "节名"。
         * @return bool 存在返回 true，否则返回 false。
         */
        bool has(const std::string &section_and_key) const
        {
            auto pr = partition(section_and_key, ".");
            if (pr.middle.empty())
            {
                auto section = pr.left;
                return data_.find(section) != data_.end();
            }
            else
            {
                auto section = pr.left;
                auto key = pr.right;
                auto sec_it = data_.find(section);
                if (sec_it == data_.end())
                    return false;
                return sec_it->second.find(key) != sec_it->second.end();
            }
        }

        /**
         * @brief 删除配置项或节。
         * @param section_and_key 节和键，格式为 "节名.键名" 或仅 "节名"。
         * @return bool 成功删除返回 true，未找到返回 false。
         */
        bool remove(const std::string &section_and_key)
        {
            auto pr = partition(section_and_key, ".");
            if (pr.middle.empty())
            {
                auto section = pr.left;
                return data_.erase(section) > 0;
            }
            else
            {
                auto section = pr.left;
                auto key = pr.right;
                auto sec_it = data_.find(section);
                if (sec_it == data_.end())
                    return false;
                return sec_it->second.erase(key) > 0;
            }
        }

        /**
         * @brief 获取原始数据结构的只读引用。
         * @return const std::map<std::string, std::map<std::string, std::string>>&
         *         内层 map 为 [节名] -> [键名] -> [值]
         */
        const std::map<std::string, std::map<std::string, std::string>> &data() const
        {
            return data_;
        }
    };
}
