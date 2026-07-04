/**
 * @file input.h
 * @brief
 * 提供丰富的控制台输入函数，支持类型安全输入、范围检查、行读取、确认输入等。
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
#include <cfloat>
#include <cstdint>
#include <iostream>
#include <limits>
#include <string>

#include "strpp.h"

namespace console {
    /**
     * @struct InputSettings
     * @brief 输入/输出流设置，用于自定义 input 函数的输入输出目标。
     * @details 默认使用 std::cout 和 std::cin，可修改以实现重定向或测试。
     */
    struct InputSettings {
        std::ostream &os; ///< 输出提示信息的流
        std::istream &is; ///< 读取输入的流
    };

    static InputSettings inputSettings{
        std::cout, std::cin}; ///< 全局默认输入设置

    /**
     * @brief 从标准输入读取一个值，支持类型模板。
     * @tparam T 要读取的类型，默认为 std::string。
     * @param prompt 显示给用户的提示字符串。
     * @param is 输入设置（默认使用全局 inputSettings）。
     * @return T 读取的值。
     * @note 若输入失败（如类型不匹配），会清空错误状态并重新提示，直到成功。
     */
    template <class T = std::string>
    T input(const std::string &prompt = "",
        const InputSettings   &is     = inputSettings) {
        T           tmp;
        std::string message;
        while (true) {
            is.os << prompt << std::flush;
            is.is >> tmp;
            if (!is.is) {
                is.is.clear();
                is.is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                is.os << "StreamError!\n";
                continue;
            }
            is.is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            return tmp;
        }
    }

    /**
     * @brief 读取一个 long double 类型的数字。
     * @param prompt 提示字符串（默认为 "Type a number: "）。
     * @param is 输入设置。
     * @return long double 读取的数字。
     */
    inline long double
    inputNumber(const std::string &prompt = "Type a number: ",
        const InputSettings       &is     = inputSettings) {
        return input<long double>(prompt, is);
    }

    /**
     * @brief 读取一整行字符串（包含空格）。
     * @param prompt 提示字符串（默认为 "Type a line string: "）。
     * @param is 输入设置。
     * @return std::string 读取的行（不含换行符）。
     */
    inline std::string
    inputLine(const std::string &prompt = "Type a line string: ",
        const InputSettings     &is     = inputSettings) {
        std::string tmp;
        is.os << prompt << std::flush;
        if (is.is.peek() == '\n') is.is.get();
        std::getline(is.is, tmp);
        return tmp;
    }

    /**
     * @brief 读取一个在指定范围内的数字。
     * @tparam T 输入类型，默认为 long double。
     * @param min 最小值（包含）。
     * @param max 最大值（包含）。
     * @param prompt 提示字符串（默认为 "Type a number: "）。
     * @param is 输入设置。
     * @return T 验证后的数字。
     * @note 若输入超出范围，会输出错误信息并重新提示。
     */
    template <class T>
    T inputWithRange(T       min,
        T                    max,
        const std::string   &prompt = "Type a number: ",
        const InputSettings &is     = inputSettings) {
        T tmp;
        while (true) {
            tmp = input<T>(prompt, is);
            if (tmp < min) {
                is.os << "less than the minimum value of " << min << std::endl;
                continue;
            }
            if (tmp > max) {
                is.os << "Greater than the maximum value of " << max
                      << std::endl;
                continue;
            }
            return tmp;
        }
    }

    /**
     * @brief 读取一个字符。
     * @param prompt 提示字符串（默认为 "Type a character: "）。
     * @param is 输入设置。
     * @return char 读取的第一个字符。
     * @note 此函数使用 is.is.get()，不会跳过空白字符，注意与格式化输入的区别。
     */
    inline char inputChar(const std::string &prompt = "Type a character: ",
        const InputSettings                 &is     = inputSettings) {
        is.os << prompt << std::flush;
        char tmp = is.is.get();
        is.is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        return tmp;
    }

    /**
     * @brief 读取一个 y/n 确认，返回布尔值。
     * @param prompt 提示字符串（默认为 "Type yes or no: "）。
     * @param is 输入设置。
     * @return bool true 若输入 'Y' 或 'y'，false 若输入 'N' 或 'n'。
     * @note 若输入其他字符，会提示重新输入。
     */
    inline bool inputYesOrNo(const std::string &prompt = "Type yes or no: ",
        const InputSettings                    &is     = inputSettings) {
        std::string tmp;
        while (true) {
            char tmp = inputChar(prompt, is);
            if (tmp == 'Y' || tmp == 'y')
                return true;
            else if (tmp == 'N' || tmp == 'n')
                return false;
            else
                is.os << "Please type yes or no." << std::endl;
        }
    }

    /**
     * @brief 读取输入流中剩余的全部内容（直到 EOF）。
     * @param prompt 提示字符串（默认为空）。
     * @param is 输入设置。
     * @return std::string 从当前位置到流末尾的所有字符。
     * @note 常用于读取多行输入，直到用户输入 EOF（Ctrl+Z/Ctrl+D）。
     */
    inline std::string inputAll(const std::string &prompt = "",
        const InputSettings                       &is     = inputSettings) {
        is.os << prompt;
        return {std::istreambuf_iterator<char>(is.is),
            std::istreambuf_iterator<char>()};
    }
}
