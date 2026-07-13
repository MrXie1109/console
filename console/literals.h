/**
 * @file literals.h
 * @brief
 * 提供自定义字面量运算符，包括字符串字面量、时间字面量和格式化字符串字面量。
 * @details 提供纳秒、微秒、毫秒、秒、分钟、小时的
 * `_ns`、`_us`、`_ms`、`_s`、`_min`、`_hr` 后缀； 以及用于格式化字符串的 `_f`
 * 后缀。
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
#include "strpp.h"
#include "time.h"

/**
 * @namespace console::literals
 * @brief 包含自定义字面量运算符的命名空间。
 * @details 使用时需要 `using namespace console::literals;`
 * 或直接引入所需字面量。
 */
namespace console {
    namespace literals {
        // ----------------------------
        // 时间字面量（整数版本）----------------------------
        /**
         * @brief 纳秒字面量（整数）。
         * @param ns 纳秒数。
         * @return Time 表示指定纳秒数的 Time 对象。
         */
        inline Time operator""_ns(unsigned long long ns) {
            return Time(ns);
        }

        /**
         * @brief 微秒字面量（整数）。
         * @param us 微秒数。
         * @return Time 表示指定微秒数的 Time 对象（转换为纳秒）。
         */
        inline Time operator""_us(unsigned long long us) {
            return Time(us * 1e3);
        }

        /**
         * @brief 毫秒字面量（整数）。
         * @param ms 毫秒数。
         * @return Time 表示指定毫秒数的 Time 对象（转换为纳秒）。
         */
        inline Time operator""_ms(unsigned long long ms) {
            return Time(ms * 1e6);
        }

        /**
         * @brief 秒字面量（整数）。
         * @param s 秒数。
         * @return Time 表示指定秒数的 Time 对象（转换为纳秒）。
         */
        inline Time operator""_s(unsigned long long s) {
            return Time(s * 1e9);
        }

        /**
         * @brief 分钟字面量（整数）。
         * @param min 分钟数。
         * @return Time 表示指定分钟数的 Time 对象（转换为纳秒）。
         */
        inline Time operator""_min(unsigned long long min) {
            return Time(min * 6e10);
        }

        /**
         * @brief 小时字面量（整数）。
         * @param hr 小时数。
         * @return Time 表示指定小时数的 Time 对象（转换为纳秒）。
         */
        inline Time operator""_hr(unsigned long long hr) {
            return Time(hr * 3.6e12);
        }

        // ----------------------------
        // 时间字面量（浮点数版本）----------------------------
        /**
         * @brief 纳秒字面量（浮点数）。
         * @param ns 纳秒数（可为小数）。
         * @return Time 表示指定纳秒数的 Time 对象。
         */
        inline Time operator""_ns(long double ns) {
            return Time(ns);
        }

        /**
         * @brief 微秒字面量（浮点数）。
         * @param us 微秒数。
         * @return Time 表示指定微秒数的 Time 对象。
         */
        inline Time operator""_us(long double us) {
            return Time(us * 1e3);
        }

        /**
         * @brief 毫秒字面量（浮点数）。
         * @param ms 毫秒数。
         * @return Time 表示指定毫秒数的 Time 对象。
         */
        inline Time operator""_ms(long double ms) {
            return Time(ms * 1e6);
        }

        /**
         * @brief 秒字面量（浮点数）。
         * @param s 秒数。
         * @return Time 表示指定秒数的 Time 对象。
         */
        inline Time operator""_s(long double s) {
            return Time(s * 1e9);
        }

        /**
         * @brief 分钟字面量（浮点数）。
         * @param min 分钟数。
         * @return Time 表示指定分钟数的 Time 对象。
         */
        inline Time operator""_min(long double min) {
            return Time(min * 6e10);
        }

        /**
         * @brief 小时字面量（浮点数）。
         * @param hr 小时数。
         * @return Time 表示指定小时数的 Time 对象。
         */
        inline Time operator""_hr(long double hr) {
            return Time(hr * 3.6e12);
        }

        /**
         * @brief 格式化字符串字面量，返回 FString 对象，支持延迟格式化。
         * @param str 字符串字面量。
         * @param 长度参数（未使用）。
         * @return FString 可进行格式化绑定的对象。
         */
        inline FString operator""_f(const char *str, size_t) {
            return str;
        }
        /**
         * @brief 宽字符格式化字符串字面量，返回 WFString 对象。
         */
        inline WFString operator""_f(const wchar_t *str, size_t) {
            return str;
        }
    }
}
