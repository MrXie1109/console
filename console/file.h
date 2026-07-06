/**
 * @file file.h
 * @brief 提供跨平台的文件路径封装和文件 I/O 操作。
 * @details 包含 Path 类，支持路径拼接、文本/二进制读写、按行读写、POD
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
     * @class Path
     * @brief 文件路径封装类，提供便捷的文件读写和路径操作。
     * @details 在 Windows 平台上自动将 '/' 转换为 '\\'，支持路径拼接运算符 /。
     *          所有文件读写操作均会抛出 FileError
     * 异常（若文件无法打开或流状态异常）。
     */
    class Path {
        std::string path; ///< 存储的路径字符串

    public:
        /// @brief 字节类型别名，表示二进制数据的容器（unsigned char 的
        /// vector）。
        using Bytes = std::vector<unsigned char>;

        /**
         * @brief 从字符串构造 Path 对象。
         * @param str 路径字符串。
         * @note 在 Windows 上会将 '/' 自动转换为 '\\'。
         */
        Path(const std::string &str) : path(str) {
#ifdef _WIN32
            for (char &ch : path) {
                if (ch == '/') ch = '\\';
            }
#endif
        }

        /**
         * @brief 路径拼接运算符。
         * @param p1 左路径。
         * @param p2 右路径。
         * @return Path 拼接后的新路径，格式为 "p1/p2" 或 "p1\\p2"。
         * @note 在 Windows 拼接后，构造函数会自动将 '/' 变成 '\\'。
         */
        friend Path operator/(const Path &p1, const Path &p2) {
            return p1.path + '/' + p2.path;
        }

        /**
         * @brief 以文本模式读取文件全部内容。
         * @return std::string 文件内容。
         * @throw FileError 若文件无法打开或读取过程中流状态出错。
         */
        std::string read_text() const {
            std::ifstream fin(path);
            if (!fin.is_open())
                throw FileError("Cannot Open File \"" + path + '"');
            std::string text{std::istreambuf_iterator<char>(fin),
                std::istreambuf_iterator<char>()};
            if (!fin.good())
                throw FileError("The Stream of \"" + path + "\" is Not Good");
            return text;
        }

        /**
         * @brief 以二进制模式读取文件全部内容。
         * @return Bytes 无符号字节向量。
         * @throw FileError 若文件无法打开或读取过程中流状态出错。
         */
        Bytes read_binary() const {
            std::ifstream fin(path, std::ios::binary);
            if (!fin.is_open())
                throw FileError("Cannot Open File \"" + path + '"');
            Bytes bytes{std::istreambuf_iterator<char>(fin),
                std::istreambuf_iterator<char>()};
            if (!fin.good())
                throw FileError("The Stream of \"" + path + "\" is Not Good");
            return bytes;
        }

        /**
         * @brief 按行读取文本文件，返回每行字符串的 vector。
         * @return std::vector<std::string> 各行内容（不包含换行符）。
         * @throw FileError 若文件无法打开或读取过程中出错。
         */
        std::vector<std::string> read_lines() const {
            return split(read_text(), "\n");
        }

        /**
         * @brief 从二进制文件读取一个 POD 类型对象（类型安全版本）。
         * @tparam T 必须是平凡可复制（trivially copyable）的类型。
         * @return T 读取到的对象。
         * @throw FileError 若文件无法打开或读取失败。
         * @note 编译期检查 T 是否为 POD 类型，否则触发 static_assert。
         */
        template <class T>
        T read_POD() const {
            static_assert(std::is_trivially_copyable<T>::value,
                "This Type is Not POD Type!");
            std::ifstream fin(path, std::ios::binary);
            if (!fin.is_open())
                throw FileError("Cannot Open File \"" + path + '"');
            T data;
            fin.read((char *)(&data), sizeof(data));
            if (!fin.good())
                throw FileError("The Stream of \"" + path + "\" is Not Good");
            return data;
        }

        /**
         * @brief 从二进制文件读取一个 POD 类型对象（不安全版本）。
         * @tparam T 任何类型（无编译期检查）。
         * @return T 读取到的对象。
         * @throw FileError 若文件无法打开或读取失败。
         * @warning 不检查 T 是否为 POD 类型，可能因类型不匹配导致未定义行为。
         */
        template <class T>
        T unsafe_read_POD() const {
            std::ifstream fin(path, std::ios::binary);
            if (!fin.is_open())
                throw FileError("Cannot Open File \"" + path + '"');
            T data;
            fin.read((char *)(&data), sizeof(data));
            if (!fin.good())
                throw FileError("The Stream of \"" + path + "\" is Not Good");
            return data;
        }

        /**
         * @brief 以文本模式写入字符串到文件（覆盖模式）。
         * @param text 要写入的文本。
         * @throw FileError 若文件无法打开或写入过程中流状态出错。
         */
        void write_text(const std::string &text) const {
            std::ofstream fout(path);
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + path + '"');
            fout << text;
            if (!fout.good())
                throw FileError("The Stream of \"" + path + "\" is Not Good");
        }

        /**
         * @brief 以二进制模式写入字节数据到文件（覆盖模式）。
         * @param bts 要写入的字节向量。
         * @throw FileError 若文件无法打开或写入过程中流状态出错。
         */
        void write_binary(const Bytes &bts) const {
            std::ofstream fout(path, std::ios::binary);
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + path + '"');
            fout.write((const char *)(bts.data()), bts.size());
            if (!fout.good())
                throw FileError("The Stream of \"" + path + "\" is Not Good");
        }

        /**
         * @brief 将多行字符串写入文件，每行之间用换行符分隔。
         * @param lines 字符串向量。
         * @throw FileError 若文件无法打开或写入过程中出错。
         * @note 若 lines 为空，则不做任何操作（不创建文件）。
         */
        void write_lines(const std::vector<std::string> &lines) const {
            std::ofstream fout(path, std::ios::binary);
            if (lines.empty()) return;
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + path + '"');
            auto it = lines.begin();
            fout << *it;
            while (++it != lines.end()) fout << '\n' << *it;
            if (!fout.good())
                throw FileError("The Stream of \"" + path + "\" is Not Good");
        }

        /**
         * @brief 写入一个 POD 类型对象到二进制文件（类型安全版本）。
         * @tparam T 必须是平凡可复制类型。
         * @param data 要写入的对象。
         * @throw FileError 若文件无法打开或写入过程中出错。
         * @note 编译期检查 T 是否为 POD 类型。
         */
        template <class T>
        void write_POD(const T &data) const {
            static_assert(std::is_trivially_copyable<T>::value,
                "This Type is Not POD Type!");
            std::ofstream fout(path, std::ios::binary);
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + path + '"');
            fout.write((const char *)(&data), sizeof(data));
            if (!fout.good())
                throw FileError("The Stream of \"" + path + "\" is Not Good");
        }

        /**
         * @brief 写入一个对象到二进制文件（不安全版本）。
         * @tparam T 任何类型。
         * @param data 要写入的对象。
         * @throw FileError 若文件无法打开或写入过程中出错。
         * @warning 不检查 T 是否为 POD
         * 类型，直接以二进制写入内存表示，可能导致不可移植。
         */
        template <class T>
        void unsafe_write_POD(const T &data) const {
            std::ofstream fout(path, std::ios::binary);
            if (!fout.is_open())
                throw FileError("Cannot Open File \"" + path + '"');
            fout.write((const char *)(&data), sizeof(data));
            if (!fout.good())
                throw FileError("The Stream of \"" + path + "\" is Not Good");
        }

        /**
         * @brief 检查文件是否存在。
         * @return bool 若文件存在且可打开则返回 true，否则 false。
         */
        bool exists() const { return std::ifstream{path}.is_open(); }

        /**
         * @brief 创建空文件（若已存在则更新访问和修改时间）。
         * @details 相当于 Unix 的 touch
         * 命令，若文件不存在则创建，若存在则仅更新时间戳。
         */
        void touch() const { std::ofstream{path}; }

        /**
         * @brief 确保文件存在，若不存在则创建空文件。
         * @details 使用追加模式打开文件，不会清空已有内容。
         */
        void ensure() const { std::ofstream{path, std::ios::app}; }

        /**
         * @brief 删除文件。
         * @details 使用 std::remove 删除文件，若删除失败（如文件不存在）
         */
        void remove() const {
            if (std::remove(path.c_str()) != 0)
                throw FileError("Cannot Remove File \"" + path + '"');
        }

        /**
         * @brief 获取路径字符串。
         * @return const std::string& 路径字符串。
         */
        const std::string &str() const { return path; }

        /**
         * @brief 获取文件流对象。
         * @return std::fstream 文件流对象。
         * @note 文件流关闭前其它操作可能会导致流状态异常，
         *       使用前请确保正确管理流的生命周期和状态。
         */
        std::fstream stream(std::ios_base::openmode mode
                            = std::ios_base::in | std::ios_base::out) const {
            return std::fstream(path, mode);
        }
    };
}
