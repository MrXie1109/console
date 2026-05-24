/**
 * @file csexc.h
 * @brief 定义 console 库使用的自定义异常类层次结构。
 * @details 所有异常都继承自 console::console_error，后者派生自 std::runtime_error。
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
#include <stdexcept>
#include <string>

namespace console
{
    /**
     * @class ConsoleError
     * @brief console 库所有异常的基类。
     * @details 派生自 std::runtime_error，用于捕获所有由 console 库抛出的异常。
     */
    class ConsoleError : public std::runtime_error
    {
    public:
        /**
         * @brief 构造 ConsoleError。
         * @param msg 错误描述信息。
         */
        ConsoleError(const std::string &msg)
            : std::runtime_error(msg) {}
    };

    /**
     * @class FatalLogging
     * @brief 表示致命日志错误，通常会导致程序终止。
     */
    class FatalLogging : public ConsoleError
    {
    public:
        /**
         * @brief 构造 FatalLogging。
         * @param msg 错误描述信息。
         */
        FatalLogging(const std::string &msg)
            : ConsoleError(msg) {}
    };

    /**
     * @class BadFormat
     * @brief 表示格式化字符串错误，例如占位符与参数不匹配。
     */
    class BadFormat : public ConsoleError
    {
    public:
        /**
         * @brief 构造 BadFormat。
         * @param msg 错误描述信息。
         */
        BadFormat(const std::string &msg)
            : ConsoleError(msg) {}
    };

    /**
     * @class FileError
     * @brief 表示文件操作错误，如打开失败、读取失败等。
     */
    class FileError : public ConsoleError
    {
    public:
        /**
         * @brief 构造 FileError。
         * @param msg 错误描述信息。
         */
        FileError(const std::string &msg)
            : ConsoleError(msg) {}
    };

    /**
     * @class TypeError
     * @brief 表示类型错误，例如访问 Maybe 中不存在的值或 Box 中类型不匹配。
     * @see console::Maybe, console::Box
     * @note `空值` 也被视为一种类型错误，因为它表示没有类型可访问。
     */
    class TypeError : public ConsoleError
    {
    public:
        /**
         * @brief 构造 TypeError。
         * @param msg 错误描述信息。
         */
        TypeError(const std::string &msg)
            : ConsoleError(msg) {}
    };

    /**
     * @class MultiArrayError
     * @brief 表示 MultiArray 多维数组操作中的错误，如维度不匹配。
     */
    class MultiArrayError : public ConsoleError
    {
    public:
        /**
         * @brief 构造 MultiArrayError。
         * @param msg 错误描述信息。
         */
        MultiArrayError(const std::string &msg)
            : ConsoleError(msg) {}
    };

    /**
     * @class ContainerError
     * @brief 表示通用容器操作错误。
     */
    class ContainerError : public ConsoleError
    {
    public:
        /**
         * @brief 构造 ContainerError。
         * @param msg 错误描述信息。
         */
        ContainerError(const std::string &msg)
            : ConsoleError(msg) {}
    };

    /**
     * @class IndexError
     * @brief 表示索引越界错误。
     */
    class IndexError : public ConsoleError
    {
    public:
        /**
         * @brief 构造 IndexError。
         * @param msg 错误描述信息。
         */
        IndexError(const std::string &msg)
            : ConsoleError(msg) {}
    };

    /**
     * @class DomainError
     * @brief 表示试图对无效的数学值域进行操作时抛出的异常。
     * @see std::DomainError
     */
    class DomainError : public ConsoleError
    {
    public:
        /**
         * @brief 构造 DomainError。
         * @param msg 错误描述信息。
         */
        DomainError(const std::string &msg)
            : ConsoleError(msg) {}
    };

    /**
     * @class StopIteration
     * @brief 表示试图对已结束的生成器调用 next()。
     */
    class StopIteration : public ConsoleError
    {
    public:
        /**
         * @brief 构造 StopIteration。
         * @param msg 错误描述信息。
         */
        StopIteration(const std::string &msg)
            : ConsoleError(msg) {}
    };
}
