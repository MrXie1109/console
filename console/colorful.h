/**
 * @file colorful.h
 * @brief 提供控制台颜色和样式修饰的 ANSI 转义序列常量。
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
#include <iostream>

namespace console {
    /**
     * @namespace console::color
     * @brief 包含控制台颜色、背景色和文本样式的 ANSI 转义序列常量。
     * @details 使用方式：`std::cout << color::Red << "Hello" << color::Reset;`
     */
    namespace color {
        // ------------------------------ 前景色 ------------------------------
        /// @brief 黑色前景
        static constexpr char Black[] = "\033[30m";
        /// @brief 红色前景
        static constexpr char Red[] = "\033[31m";
        /// @brief 绿色前景
        static constexpr char Green[] = "\033[32m";
        /// @brief 黄色前景
        static constexpr char Yellow[] = "\033[33m";
        /// @brief 蓝色前景
        static constexpr char Blue[] = "\033[34m";
        /// @brief 品红前景
        static constexpr char Magenta[] = "\033[35m";
        /// @brief 青色前景
        static constexpr char Cyan[] = "\033[36m";
        /// @brief 白色前景
        static constexpr char White[] = "\033[37m";
        /// @brief 亮黑色（灰）前景
        static constexpr char BrightBlack[] = "\033[90m";
        /// @brief 亮红色前景
        static constexpr char BrightRed[] = "\033[91m";
        /// @brief 亮绿色前景
        static constexpr char BrightGreen[] = "\033[92m";
        /// @brief 亮黄色前景
        static constexpr char BrightYellow[] = "\033[93m";
        /// @brief 亮蓝色前景
        static constexpr char BrightBlue[] = "\033[94m";
        /// @brief 亮品红前景
        static constexpr char BrightMagenta[] = "\033[95m";
        /// @brief 亮青色前景
        static constexpr char BrightCyan[] = "\033[96m";
        /// @brief 亮白色前景
        static constexpr char BrightWhite[] = "\033[97m";
        /// @brief 重置所有颜色/样式
        static constexpr char Reset[] = "\033[0m";

        // ------------------------------ 背景色 ------------------------------
        /// @brief 黑色背景
        static constexpr char BgBlack[] = "\033[40m";
        /// @brief 红色背景
        static constexpr char BgRed[] = "\033[41m";
        /// @brief 绿色背景
        static constexpr char BgGreen[] = "\033[42m";
        /// @brief 黄色背景
        static constexpr char BgYellow[] = "\033[43m";
        /// @brief 蓝色背景
        static constexpr char BgBlue[] = "\033[44m";
        /// @brief 品红背景
        static constexpr char BgMagenta[] = "\033[45m";
        /// @brief 青色背景
        static constexpr char BgCyan[] = "\033[46m";
        /// @brief 白色背景
        static constexpr char BgWhite[] = "\033[47m";
        /// @brief 亮黑色背景
        static constexpr char BgBrightBlack[] = "\033[100m";
        /// @brief 亮红色背景
        static constexpr char BgBrightRed[] = "\033[101m";
        /// @brief 亮绿色背景
        static constexpr char BgBrightGreen[] = "\033[102m";
        /// @brief 亮黄色背景
        static constexpr char BgBrightYellow[] = "\033[103m";
        /// @brief 亮蓝色背景
        static constexpr char BgBrightBlue[] = "\033[104m";
        /// @brief 亮品红背景
        static constexpr char BgBrightMagenta[] = "\033[105m";
        /// @brief 亮青色背景
        static constexpr char BgBrightCyan[] = "\033[106m";
        /// @brief 亮白色背景
        static constexpr char BgBrightWhite[] = "\033[107m";

        // ------------------------------ 文本样式
        // ------------------------------
        /// @brief 粗体
        static constexpr char Bold[] = "\033[1m";
        /// @brief 暗淡（降低亮度）
        static constexpr char Dim[] = "\033[2m";
        /// @brief 斜体
        static constexpr char Italic[] = "\033[3m";
        /// @brief 下划线
        static constexpr char Underline[] = "\033[4m";
        /// @brief 闪烁
        static constexpr char Blink[] = "\033[5m";
        /// @brief 反转（交换前景和背景）
        static constexpr char Reverse[] = "\033[7m";
        /// @brief 隐藏（不可见）
        static constexpr char Hidden[] = "\033[8m";
        /// @brief 删除线
        static constexpr char Strikethrough[] = "\033[9m";
    }

    /**
     * @brief 修改颜色。
     * @param clr 颜色，默认 Reset。
     * @param os 输出流，默认 std::cout。
     * @note cc = change color
     */
    inline void
    cc(const char *clr = color::Reset, std::ostream &os = std::cout) {
        os << clr;
    }

    /**
     * @class ColorGuard
     * @brief RAII颜色守卫，构造时设置颜色，析构时自动恢复。
     */
    class ColorGuard {
        std::ostream &os_; ///< 输出流引用

    public:
        /**
         * @brief 构造函数，应用指定颜色。
         * @param color ANSI颜色序列（如 color::Red）
         * @param os 输出流，默认为 std::cout
         */
        explicit ColorGuard(const char *color, std::ostream &os = std::cout) :
            os_(os) {
            os << color;
        }

        /**
         * @brief 析构函数，自动恢复默认颜色。
         */
        ~ColorGuard() { os_ << color::Reset; }

        // 禁止拷贝
        ColorGuard(const ColorGuard &)            = delete;
        ColorGuard &operator=(const ColorGuard &) = delete;
    };
}
