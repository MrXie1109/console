/**
 * @file rational.h
 * @brief 提供运行时的分数运算。
 * @details 包含模板类 BasicRational 和数个预设的别名。
 *          几乎所有操作都有可能导致整数溢出，请使用较大的整数类型或自行检查。
 *          为了数值计算的效率，BasicRational 内部不做检查。
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
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <type_traits>

#include "csexc.h"

namespace console {
    /**
     * @class BasicRational
     * @brief 用于表示分数的基础模板类。
     * @tparam T 存储整数和浮点数的整数类型。
     */
    template <class T> class BasicRational {
        static_assert(std::is_integral<T>::value && std::is_signed<T>::value,
            "T must be signed integral type"); ///< T 必须是有符号整数

        T num; ///< 分子
        T den; ///< 分母

        /**
         * @brief 求两个整数的最大公因数。
         * @param a 一个整数。
         * @param b 另一个整数。
         * @return T 数字 a 和数字 b 的最大公因数。
         */
        static T gcd(T a, T b) {
            a = std::abs(a);
            b = std::abs(b);
            while (b != 0) {
                T temp = b;
                b      = a % b;
                a      = temp;
            }
            return a;
        }

        /**
         * @brief 辅助函数，用于约分一个分数。
         * @throw console::DomainError 若分母为 0。
         */
        void reduce() {
            if (den == 0) {
                throw DomainError("Denominator cannot be zero");
            }
            if (den < 0) {
                num = -num;
                den = -den;
            }
            T g = gcd(num, den);
            num /= g;
            den /= g;
        }

    public:
        /// @brief 默认构造函数，构造一个值为 0/1 的分数。
        constexpr BasicRational() : num(0), den(1) {}

        /**
         * @brief 接受单参的构造函数，构造一个值为 n/1 的分数。
         * @param n 分子。
         */
        constexpr BasicRational(T n) : num(n), den(1) {}

        /**
         * @brief 接受双参的构造函数，构造一个值为 n/d 的分数。
         * @details 在内部应用 reduce() 进行约分。
         * @param n 分子。
         * @param d 分母。
         * @throw console::DomainError 若 d 为 0。
         */
        BasicRational(T n, T d) : num(n), den(d) { reduce(); }

        /**
         * @brief 取得分数的分子。
         * @return T 分子。
         */
        T numerator() const { return num; }

        /**
         * @brief 取得分数的分母。
         * @return T 分子。
         */
        T denominator() const { return den; }

        /**
         * @brief 显式的将一个分数转化为一个指定的数值类型。
         * @tparam U 目标类型
         * @return U 转化后的值。
         */
        template <class U> explicit operator U() const { return U(num) / den; }

        /**
         * @brief 计算两个分数的和。
         * @param lhs 加数。
         * @param rhs 加数。
         * @return BasicRational 和。
         * @warning 整数可能溢出。
         */
        friend BasicRational
        operator+(const BasicRational &lhs, const BasicRational &rhs) {
            return BasicRational(
                lhs.num * rhs.den + rhs.num * lhs.den, lhs.den * rhs.den);
        }

        /**
         * @brief 计算两个分数的差。
         * @param lhs 被减数。
         * @param rhs 减数。
         * @return BasicRational 差。
         * @warning 整数可能溢出。
         */
        friend BasicRational
        operator-(const BasicRational &lhs, const BasicRational &rhs) {
            return BasicRational(
                lhs.num * rhs.den - rhs.num * lhs.den, lhs.den * rhs.den);
        }

        /**
         * @brief 计算两个分数的积。
         * @param lhs 乘数。
         * @param rhs 乘数。
         * @return BasicRational 积。
         * @warning 整数可能溢出。
         */
        friend BasicRational
        operator*(const BasicRational &lhs, const BasicRational &rhs) {
            return BasicRational(lhs.num * rhs.num, lhs.den * rhs.den);
        }

        /**
         * @brief 计算两个分数的商。
         * @param lhs 被除数。
         * @param rhs 除数。
         * @return BasicRational 商。
         * @warning 整数可能溢出。
         * @throw console::DomainError 如果除以零。
         */
        friend BasicRational
        operator/(const BasicRational &lhs, const BasicRational &rhs) {
            if (rhs.num == 0) throw DomainError("Division by zero");
            return BasicRational(lhs.num * rhs.den, lhs.den * rhs.num);
        }

        /**
         * @brief 就地版本，等价于 a = a + b。
         * @param other 加数。
         * @return BasicRational & 即 *this。
         */
        BasicRational &operator+=(const BasicRational &other) {
            *this = *this + other;
            return *this;
        }

        /**
         * @brief 就地版本，等价于 a = a - b。
         * @param other 减数。
         * @return BasicRational & 即 *this。
         */
        BasicRational &operator-=(const BasicRational &other) {
            *this = *this - other;
            return *this;
        }

        /**
         * @brief 就地版本，等价于 a = a * b。
         * @param other 乘数。
         * @return BasicRational & 即 *this。
         */
        BasicRational &operator*=(const BasicRational &other) {
            *this = *this * other;
            return *this;
        }

        /**
         * @brief 就地版本，等价于 a = a / b。
         * @param other 除数。
         * @return BasicRational & 即 *this。
         * @throw console::DomainError 如果除以零。
         */
        BasicRational &operator/=(const BasicRational &other) {
            *this = *this / other;
            return *this;
        }

        /**
         * @brief 返回相反数。
         * @return BasicRational -*this
         */
        BasicRational operator-() const { return BasicRational(-num, den); }

        /**
         * @brief 返回自身。
         * @return BasicRational *this
         */
        BasicRational operator+() const { return *this; }

        /**
         * @brief 比较两个分数是否相等（eq）。
         * @param lhs 左操作数。
         * @param rhs 右操作数。
         * @return bool 如果相等 true 否则 false。
         */
        friend bool
        operator==(const BasicRational &lhs, const BasicRational &rhs) {
            return lhs.num == rhs.num && lhs.den == rhs.den;
        }

        /**
         * @brief 比较两个分数是否不等（ne）。
         * @param lhs 左操作数。
         * @param rhs 右操作数。
         * @return bool 如果不等 true 否则 false。
         */
        friend bool
        operator!=(const BasicRational &lhs, const BasicRational &rhs) {
            return !(lhs == rhs);
        }

        /**
         * @brief 比较左操作数是否小于右操作数（lt）。
         * @param lhs 左操作数。
         * @param rhs 右操作数。
         * @return bool 如果左 < 右 true 否则 false。
         */
        friend bool
        operator<(const BasicRational &lhs, const BasicRational &rhs) {
            return lhs.num * rhs.den < rhs.num * lhs.den;
        }

        /**
         * @brief 比较左操作数是否大于右操作数（gt）。
         * @param lhs 左操作数。
         * @param rhs 右操作数。
         * @return bool 如果左 > 右 true 否则 false。
         */
        friend bool
        operator>(const BasicRational &lhs, const BasicRational &rhs) {
            return rhs < lhs;
        }

        /**
         * @brief 比较左操作数是否小于等于右操作数（le）。
         * @param lhs 左操作数。
         * @param rhs 右操作数。
         * @return bool 如果左 <= 右 true 否则 false。
         */
        friend bool
        operator<=(const BasicRational &lhs, const BasicRational &rhs) {
            return !(lhs > rhs);
        }

        /**
         * @brief 比较左操作数是否大于等于右操作数（ge）。
         * @param lhs 左操作数。
         * @param rhs 右操作数。
         * @return bool 如果左 >= 右 true 否则 false。
         */
        friend bool
        operator>=(const BasicRational &lhs, const BasicRational &rhs) {
            return !(lhs < rhs);
        }

        /**
         * @brief 将一个分数打印到指定输出流。
         * @param os 输出流。
         * @param r 分数。
         * @return std::ostream & 原输出流 os 的引用。
         */
        friend std::ostream &
        operator<<(std::ostream &os, const BasicRational &r) {
            os << intmax_t(r.num);
            if (r.den != 1) os << '/' << intmax_t(r.den);
            return os;
        }

        /**
         * @brief 从指定输入流读取一个分数。
         * @details 会尽可能的在内部做一些操作，使得读取操作正常。
         * @param is 输入流。
         * @param r 分数。
         * @return std::istream & 原输出流 is 的引用。
         */
        friend std::istream &operator>>(std::istream &is, BasicRational &r) {
            intmax_t n, d;
            char     slash;
            is >> n;
            if (is >> std::ws, is.peek() == '/') {
                is >> slash >> d;
                r = BasicRational(intmax_t(n), intmax_t(d));
            } else
                r = BasicRational(intmax_t(n));
            return is;
        }
    };

    using Rational = BasicRational<intmax_t>; ///< 以 intmax_t 为基的分数。
    using Rational_8 = BasicRational<int8_t>; ///< 以 int8_t 为基的分数。
    using Rational_16 = BasicRational<int16_t>; ///< 以 int16_t 为基的分数。
    using Rational_32 = BasicRational<int32_t>; ///< 以 int32_t 为基的分数。
    using Rational_64 = BasicRational<int64_t>; ///< 以 int64_t 为基的分数。
}
