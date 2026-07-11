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
     * @tparam CharT 字符类型（char 或 wchar_t）。
     */
    template <class CharT = char>
    class BasicPath {
        using string_type = std::basic_string<CharT>;

        string_type path;

    public:
        using Bytes = std::vector<unsigned char>;

        BasicPath(const string_type &str) : path(str) {
#ifdef _WIN32
            for (auto &ch : path) {
                if (ch == static_cast<CharT>('/'))
                    ch = static_cast<CharT>('\\');
            }
#endif
        }

        friend BasicPath operator/(const BasicPath &p1, const BasicPath &p2) {
            return BasicPath(p1.path + static_cast<CharT>('/') + p2.path);
        }

    private:
        std::string narrow() const {
            return std::string(path.begin(), path.end());
        }

        template <class T = CharT>
        static typename std::enable_if<std::is_same<T, char>::value,
            string_type>::type
        widen_or_pass(const std::string &s) {
            return s;
        }

        template <class T = CharT>
        static typename std::enable_if<std::is_same<T, wchar_t>::value,
            string_type>::type
        widen_or_pass(const std::string &s) {
            return string_type(s.begin(), s.end());
        }

        template <class T = CharT>
        static typename std::enable_if<std::is_same<T, char>::value, void>::type
        write_to_stream(std::ofstream &fout, const string_type &text) {
            fout << text;
        }

        template <class T = CharT>
        static
            typename std::enable_if<std::is_same<T, wchar_t>::value, void>::type
            write_to_stream(std::ofstream &fout, const string_type &text) {
            fout << std::string(text.begin(), text.end());
        }

    public:
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

        std::vector<string_type> read_lines() const {
            return split(read_text(), string_type(1, static_cast<CharT>('\n')));
        }

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

        void write_text(const string_type &text) const {
            std::ofstream fout(narrow());
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + narrow() + '"');
            write_to_stream(fout, text);
            if (fout.fail())
                throw FileError(
                    "The Stream of \"" + narrow() + "\" is Not Good");
        }

        void write_binary(const Bytes &bts) const {
            std::ofstream fout(narrow(), std::ios::binary);
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + narrow() + '"');
            fout.write((const char *)(bts.data()), bts.size());
            if (fout.fail())
                throw FileError(
                    "The Stream of \"" + narrow() + "\" is Not Good");
        }

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

        bool exists() const { return std::ifstream{narrow()}.is_open(); }

        void touch() const { std::ofstream{narrow()}; }

        void ensure() const { std::ofstream{narrow(), std::ios::app}; }

        void remove() const {
            if (std::remove(narrow().c_str()) != 0)
                throw FileError("Cannot Remove File \"" + narrow() + '"');
        }

        const string_type &str() const { return path; }

        std::fstream stream(std::ios_base::openmode mode
                            = std::ios_base::in | std::ios_base::out) const {
            return std::fstream(narrow(), mode);
        }
    };

    using Path  = BasicPath<char>;
    using WPath = BasicPath<wchar_t>;
}
