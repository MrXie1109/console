/**
 * @file file.h
 * @brief 提供跨平台的文件路径封装和文件 I/O 操作。
 * @details 包含 BasicPath 类，支持路径拼接、文本/二进制读写、按行读写、POD
 * 类型读写等。
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
#include <cstdio>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
#include <vector>

#include "csexc.h"
#include "strpp.h"

namespace console {
    /**
     * @class BasicPath
     * @brief 文件路径封装类，提供便捷的文件读写和路径操作。
     * @tparam CharT 字符类型(char 或 wchar_t)。
     */
    template <class CharT = char>
    class BasicPath {
        using string_type = std::basic_string<CharT>; ///< 字符串类型

        string_type path; ///< 路径字符串

    public:
        using Bytes = std::vector<unsigned char>; ///< 字节向量类型

        /**
         * @brief 构造函数，接受一个字符指针作为路径字符串。
         * @param str 路径字符串。
         */
        BasicPath(const CharT *str) : path(string_type(str)) {}

        /**
         * @brief 构造函数，接受一个字符串作为路径字符串。
         * @param str 路径字符串。
         */
        BasicPath(const string_type &str) : path(str) {
#ifdef _WIN32
            for (auto &ch : path) {
                if (ch == static_cast<CharT>('/'))
                    ch = static_cast<CharT>('\\');
            }
#endif
        }

        /**
         * @brief 重载运算符，用于路径拼接。
         * @param p1 路径字符串。
         * @param p2 路径字符串。
         * @return 拼接后的路径字符串。
         */
        friend BasicPath operator/(const BasicPath &p1, const BasicPath &p2) {
            return BasicPath(p1.path + static_cast<CharT>('/') + p2.path);
        }

        /**
         * @brief 重载运算符，用于路径拼接。
         * @param p 路径字符串。
         * @return 拼接后的路径字符串。
         */
        BasicPath &operator/=(const BasicPath &p) {
            *this = *this / p;
            return *this;
        }

    private:
        /**
         * @brief 将宽字符路径转换为窄字符路径。
         * @return 窄字符路径字符串。
         */
        std::string narrow() const {
            return std::string(path.begin(), path.end());
        }

        /**
         * @brief 根据字符类型选择宽字符或窄字符转换函数。
         * @param s 字符串。
         * @return 转换后的字符串。
         */
        template <class T = CharT>
        static typename std::enable_if<std::is_same<T, char>::value,
            string_type>::type
        widen_or_pass(const std::string &s) {
            return s;
        }

        /**
         * @brief 根据字符类型选择宽字符或窄字符转换函数。
         * @param s 字符串。
         * @return 转换后的字符串。
         */
        template <class T = CharT>
        static typename std::enable_if<std::is_same<T, wchar_t>::value,
            string_type>::type
        widen_or_pass(const std::string &s) {
            return string_type(s.begin(), s.end());
        }

        /**
         * @brief 根据字符类型选择宽字符或窄字符转换函数。
         * @param fout 输出流。
         * @param text 字符串。
         */
        template <class T = CharT>
        static typename std::enable_if<std::is_same<T, char>::value, void>::type
        write_to_stream(std::ofstream &fout, const string_type &text) {
            fout << text;
        }

        /**
         * @brief 根据字符类型选择宽字符或窄字符转换函数。
         * @param fout 输出流。
         * @param text 字符串。
         */
        template <class T = CharT>
        static
            typename std::enable_if<std::is_same<T, wchar_t>::value, void>::type
            write_to_stream(std::ofstream &fout, const string_type &text) {
            fout << std::string(text.begin(), text.end());
        }

    public:
        /**
         * @brief 读取文本文件内容。
         * @return 文本内容。
         */
        string_type read_text() const {
            std::ifstream fin(narrow());
            if (!fin.is_open())
                throw FileError("Cannot Open File \"" + narrow() + '"');
            std::string buf{std::istreambuf_iterator<char>(fin),
                std::istreambuf_iterator<char>()};
            if (fin.fail() && !fin.eof())
                throw FileError(
                    "The Stream of \"" + narrow() + "\" is Not Good");
            return widen_or_pass(buf);
        }

        /**
         * @brief 读取二进制文件内容。
         * @return 二进制内容。
         */
        Bytes read_binary() const {
            std::ifstream fin(narrow(), std::ios::binary);
            if (!fin.is_open())
                throw FileError("Cannot Open File \"" + narrow() + '"');
            Bytes bytes{std::istreambuf_iterator<char>(fin),
                std::istreambuf_iterator<char>()};
            if (fin.fail() && !fin.eof())
                throw FileError(
                    "The Stream of \"" + narrow() + "\" is Not Good");
            return bytes;
        }

        /**
         * @brief 读取文本文件的行内容。
         * @return 行内容列表。
         */
        std::vector<string_type> read_lines() const {
            string_type text = read_text();
            if (text.empty()) return {};
            return split(text, string_type(1, static_cast<CharT>('\n')));
        }

        /**
         * @brief 读取 POD 类型数据。
         * @return 数据。
         */
        template <class T>
        T read_POD() const {
            static_assert(std::is_trivially_copyable<T>::value,
                "This Type is Not POD Type!");
            std::ifstream fin(narrow(), std::ios::binary);
            if (!fin.is_open())
                throw FileError("Cannot Open File \"" + narrow() + '"');
            T data;
            fin.read((char *)(&data), sizeof(data));
            if (!fin.good())
                throw FileError(
                    "The Stream of \"" + narrow() + "\" is Not Good");
            return data;
        }

        /**
         * @brief 读取 POD 类型数据(不安全)。
         * @return 数据。
         */
        template <class T>
        T unsafe_read_POD() const {
            std::ifstream fin(narrow(), std::ios::binary);
            if (!fin.is_open())
                throw FileError("Cannot Open File \"" + narrow() + '"');
            T data;
            fin.read((char *)(&data), sizeof(data));
            if (!fin.good())
                throw FileError(
                    "The Stream of \"" + narrow() + "\" is Not Good");
            return data;
        }

        /**
         * @brief 写入文本内容。
         * @param text 文本内容。
         */
        void write_text(const string_type &text) const {
            std::ofstream fout(narrow());
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + narrow() + '"');
            write_to_stream(fout, text);
            if (fout.fail())
                throw FileError(
                    "The Stream of \"" + narrow() + "\" is Not Good");
        }

        /**
         * @brief 写入二进制内容。
         * @param bts 二进制内容。
         */
        void write_binary(const Bytes &bts) const {
            std::ofstream fout(narrow(), std::ios::binary);
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + narrow() + '"');
            fout.write((const char *)(bts.data()), bts.size());
            if (fout.fail())
                throw FileError(
                    "The Stream of \"" + narrow() + "\" is Not Good");
        }

        /**
         * @brief 写入行内容。
         * @param lines 行内容列表。
         */
        void write_lines(const std::vector<string_type> &lines) const {
            std::ofstream fout(narrow(), std::ios::binary);
            if (lines.empty()) return;
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + narrow() + '"');
            write_to_stream(fout, lines[0]);
            for (size_t i = 1; i < lines.size(); ++i) {
                fout << '\n';
                write_to_stream(fout, lines[i]);
            }
            if (fout.fail())
                throw FileError(
                    "The Stream of \"" + narrow() + "\" is Not Good");
        }

        /**
         * @brief 写入 POD 类型数据。
         * @param data 数据。
         */
        template <class T>
        void write_POD(const T &data) const {
            static_assert(std::is_trivially_copyable<T>::value,
                "This Type is Not POD Type!");
            std::ofstream fout(narrow(), std::ios::binary);
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + narrow() + '"');
            fout.write((const char *)(&data), sizeof(data));
            if (fout.fail())
                throw FileError(
                    "The Stream of \"" + narrow() + "\" is Not Good");
        }

        /**
         * @brief 写入 POD 类型数据(不安全)。
         * @param data 数据。
         */
        template <class T>
        void unsafe_write_POD(const T &data) const {
            std::ofstream fout(narrow(), std::ios::binary);
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + narrow() + '"');
            fout.write((const char *)(&data), sizeof(data));
            if (fout.fail())
                throw FileError(
                    "The Stream of \"" + narrow() + "\" is Not Good");
        }

        /**
         * @brief 判断文件是否存在。
         * @return 存在返回 true，否则返回 false。
         * @note 若是目录，返回 fasle。
         */
        bool exists() const { return std::ifstream{narrow()}.is_open(); }

        /**
         * @brief 创建文件(若不存在)。
         * @note 若文件已存在，不会覆盖原有内容。
         */
        void touch() const { std::ofstream{narrow()}; }

        /**
         * @brief 确保文件存在(若不存在则创建)。
         * @note 若文件已存在，不会覆盖原有内容。
         */
        void ensure() const { std::ofstream{narrow(), std::ios::app}; }

        /**
         * @brief 删除文件(若存在)。
         */
        void remove() const {
            if (std::remove(narrow().c_str()) != 0)
                throw FileError("Cannot Remove File \"" + narrow() + '"');
        }

        /**
         * @brief 获取文件路径字符串。
         */
        const string_type &str() const { return path; }

        /**
         * @brief 以指定模式打开文件流。
         * @param mode 打开模式，默认为 std::ios_base::in | std::ios_base::out。
         * @return 打开的文件流。
         */
        std::fstream stream(std::ios_base::openmode mode
                            = std::ios_base::in | std::ios_base::out) const {
            return std::fstream(narrow(), mode);
        }
    };

    /// @brief 窄字符路径类型。
    using Path = BasicPath<char>;
    /// @brief 宽字符路径类型。
    using WPath = BasicPath<wchar_t>;
}
