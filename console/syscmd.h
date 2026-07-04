/**
 * @file syscmd.h
 * @brief 跨平台系统命令执行模块。
 * @details 封装 popen/_popen 函数，支持命令行参数和关键字参数，自动合并 stderr
 * 到 stdout。
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
#include <array>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

#ifdef _WIN32
#define POPEN _popen
#define PCLOSE _pclose
#else
#define POPEN popen
#define PCLOSE pclose
#endif

namespace console {
    /**
     * @class CmdResult
     * @brief 命令执行结果封装类。
     * @details 存储命令执行状态码和标准输出内容，对象只能通过 syscmd 函数创建。
     */
    class CmdResult {
        int status_; ///< 命令退出状态码（0 成功，-1 管道打开失败）
        std::unique_ptr<std::string> ptr_; ///< 命令输出内容（含 stderr）

        /**
         * @brief 私有默认构造函数。
         */
        CmdResult() {}

        /**
         * @brief 友元声明，仅允许 syscmd 创建实例。
         */
        friend CmdResult syscmd(const std::string &,
            const std::vector<std::string> &,
            const std::map<std::string, std::string> &);

    public:
        /**
         * @brief 检查命令是否成功执行。
         * @return true  执行成功（管道打开且退出码为 0）
         * @return false 执行失败
         */
        bool ok() const { return ptr_ && status_ == 0; }

        /**
         * @brief 检查命令是否执行但返回错误码。
         * @return true  管道打开成功但退出码非 0
         * @return false 管道打开失败或退出码为 0
         */
        bool error() const { return ptr_ && status_ != 0; }

        /**
         * @brief 检查命令执行是否完全失败。
         * @return true  管道打开失败
         * @return false 管道成功打开
         */
        bool fail() const { return !ptr_; }

        /**
         * @brief 获取命令退出状态码。
         * @return int 退出码（0 成功，非 0 错误，-1 管道打开失败）
         */
        int status() const { return status_; }

        /**
         * @brief 获取命令的标准输出内容。
         * @return const std::string& 输出内容（含 stderr）
         * @warning 请先使用 ok() 或 fail() || error()
         * 检查合法性，否则行为未定义
         */
        const std::string &output() const { return *ptr_; }
    };

    /**
     * @brief 执行系统命令并捕获输出。
     * @param exe     可执行文件路径或命令名。
     * @param args    命令行参数列表。
     * @param kwargs  关键字参数字典（格式：key value）。
     * @return CmdResult 包含执行结果的对象。
     * @details 自动合并 stderr 到 stdout，使用 256 字节缓冲区读取输出。
     * @code
     *     auto result = syscmd("ls", {"-la", "/tmp"}, {});
     *     if (result.ok()) {
     *         std::cout << result.output();
     *     }
     * @endcode
     * @warning 函数阻塞直到命令执行完毕。
     */
    inline CmdResult syscmd(const std::string    &exe,
        const std::vector<std::string>           &args   = {},
        const std::map<std::string, std::string> &kwargs = {}) {
        std::string cmd = exe;
        for (const std::string &arg : args) cmd += " " + arg;
        for (const auto &kwarg : kwargs)
            cmd += " " + kwarg.first + " " + kwarg.second;
        cmd += " 2>&1";
        FILE *pipe = POPEN(cmd.c_str(), "r");
        if (!pipe) {
            CmdResult cr;
            cr.status_ = -1;
            cr.ptr_    = nullptr;
            return cr;
        }
        std::array<char, 256>        buffer;
        std::unique_ptr<std::string> output(new std::string);
        while (fgets(buffer.data(), buffer.size(), pipe) != nullptr)
            *output += buffer.data();
        int       status = PCLOSE(pipe);
        CmdResult cr;
        cr.status_ = status;
        cr.ptr_    = std::move(output);
        return cr;
    }
}

#undef POPEN
#undef PCLOSE
