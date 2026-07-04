/**
 * @file multiarray.h
 * @brief 提供编译期维度固定的多维数组容器
 * MultiArray，支持任意维度和元素级算术/逻辑/位运算。
 * @details MultiArray 继承自 std::array 的递归嵌套，支持 operator()
 * 多维下标访问、扁平化迭代器、
 *          元素级运算（包括数组与数组、数组与标量）、统计函数（sum/min/max/all/any）等。
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
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <initializer_list>
#include <iostream>
#include <stdexcept>
#include <type_traits>

#include "csexc.h"
#include "repr.h"

namespace console {
    /**
     * @class MultiArray
     * @brief 编译期固定维度的多维数组。
     * @tparam T 元素类型。
     * @tparam Dims 各维度大小（可变参数，至少一个维度）。
     * @details 递归继承自 std::array，提供 operator()
     * 进行多维下标访问（带边界检查）， 提供 flatten 迭代器
     * fbegin/fend，支持元素级运算和常用统计函数。
     * @note 维度信息在编译期确定，所有元素连续存储于内存中。
     */
    template <class T, size_t... Dims> class MultiArray;

    /**
     * @brief 一维特化。
     * @tparam T 元素类型。
     * @tparam D 维度大小。
     */
    template <class T, size_t D>
    class MultiArray<T, D> : public std::array<T, D> {
        using base_type = std::array<T, D>;

    public:
        using value_type      = T;
        using reference       = T &;
        using const_reference = const T &;

        /// @brief 默认构造，元素未初始化（基本类型为随机值）。
        MultiArray() = default;

        /**
         * @brief 用相同值填充所有元素。
         * @param value 填充值。
         */
        explicit MultiArray(const T &value) { fill(value); }

        /// @brief 从 std::array 拷贝构造。
        MultiArray(const base_type &other) : base_type(other) {}

        /**
         * @brief 从初始化列表构造（仅一维）。
         * @param init 初始化列表，长度不应超过 D。
         */
        MultiArray(std::initializer_list<T> init) {
            std::copy(init.begin(), init.end(), this->begin());
        }

        /// @brief 返回数组的维数（一维时为 1）。
        static constexpr size_t rank() { return 1; }

        /// @brief 返回元素总数。
        static constexpr size_t fsize() { return D; }

        /// @brief 用给定值填充所有元素。
        void fill(const T &value) {
            for (auto &item : *this) item = value;
        }

        /**
         * @brief 遍历所有元素并应用函数（非常量版本）。
         * @tparam F 可调用对象，接受 T&。
         * @param visit 函数对象。
         */
        template <class F> void for_each(F &&visit) {
            for (auto &item : *this) visit(item);
        }

        /**
         * @brief 遍历所有元素并应用函数（常量版本）。
         * @tparam F 可调用对象，接受 const T&。
         * @param visit 函数对象。
         */
        template <class F> void for_each(F &&visit) const {
            for (const auto &item : *this) visit(item);
        }

        /**
         * @brief 带边界检查的下标访问（一维）。
         * @param i 索引。
         * @return T& 元素引用。
         * @throw MultiArrayError 若索引越界。
         */
        reference operator()(size_t i) {
            if (i >= D)
                throw MultiArrayError("index " + std::to_string(i)
                                      + " out of range [0, " + std::to_string(D)
                                      + ')');
            return (*this)[i];
        }

        /// @brief 常量版本。
        const_reference operator()(size_t i) const {
            if (i >= D)
                throw MultiArrayError("index " + std::to_string(i)
                                      + " out of range [0, " + std::to_string(D)
                                      + ')');
            return (*this)[i];
        }

        /// @brief 输出 MultiArray 到流，格式为嵌套的方括号。
        friend std::ostream &
        operator<<(std::ostream &os, const MultiArray &ma) {
            if (ma.begin() == ma.end()) return os << "[]";
            auto it = ma.begin();
            os << '[';
            repr(*it, os);
            while (++it != ma.end()) {
                os << ", ";
                repr(*it, os);
            }
            return os << ']';
        }

        /// @brief 返回指向扁平化数据起始的指针（非常量）。
        T *fbegin() { return this->data(); }

        /// @brief 常量版本。
        const T *fbegin() const { return this->data(); }

        /// @brief 返回指向扁平化数据末尾的指针（非常量）。
        T *fend() { return this->data() + fsize(); }

        /// @brief 常量版本。
        const T *fend() const { return this->data() + fsize(); }

        /**
         * @brief 扁平化视图（可变）。
         * @note 其实就是它自己。
         */
        MultiArray &flatten() { return *this; }

        /**
         * @brief 扁平化视图（常量）。
         * @note 其实就是它自己。
         */
        const MultiArray &flatten() const { return *this; }

        /**
         * @brief 返回包含维度信息的数组。
         * @return std::array 包含数组各维度的大小，长度为 1。
         */
        static constexpr std::array<size_t, 1> dims() { return {D}; }
    };

    /**
     * @brief 多维特化（维度 >= 2）。
     * @tparam T 元素类型。
     * @tparam First 第一维大小。
     * @tparam Rest 剩余维度大小。
     */
    template <class T, size_t First, size_t... Rest>
    class MultiArray<T, First, Rest...>
        : public std::array<MultiArray<T, Rest...>, First> {
        using base_type = std::array<MultiArray<T, Rest...>, First>;

    public:
        using value_type      = T;
        using reference       = MultiArray<T, Rest...> &;
        using const_reference = const MultiArray<T, Rest...> &;

        /// @brief 默认构造。
        MultiArray() = default;

        /**
         * @brief 用相同值递归填充所有元素。
         * @param value 填充值。
         */
        explicit MultiArray(const T &value) { fill(value); }

        /// @brief 从 std::array 拷贝构造。
        MultiArray(const base_type &other) : base_type(other) {}

        /**
         * @brief 从初始化列表构造（多维）。
         * @param init 初始化列表，每个元素为子数组。
         */
        MultiArray(std::initializer_list<MultiArray<T, Rest...>> init) {
            std::copy(init.begin(), init.end(), this->begin());
        }

        /// @brief 返回数组维数。
        static constexpr size_t rank() { return 1 + sizeof...(Rest); }

        /// @brief 返回元素总数。
        static constexpr size_t fsize() {
            return First * MultiArray<T, Rest...>::fsize();
        }

        /// @brief 递归填充所有元素。
        void fill(const T &value) {
            for (auto &sub : *this) sub.fill(value);
        }

        /**
         * @brief 遍历所有元素（非常量版本）。
         * @tparam F 可调用对象，接受 T&。
         */
        template <class F> void for_each(F &&visit) {
            for (auto &sub : *this) sub.for_each(visit);
        }

        /**
         * @brief 遍历所有元素（常量版本）。
         * @tparam F 可调用对象，接受 const T&。
         */
        template <class F> void for_each(F &&visit) const {
            for (const auto &sub : *this) sub.for_each(visit);
        }

        /**
         * @brief 单下标访问，返回子数组引用。
         * @param idx 第一维索引。
         * @return reference 子数组引用。
         * @throw MultiArrayError 若索引越界。
         */
        reference operator()(size_t idx) {
            if (idx >= First)
                throw MultiArrayError("index " + std::to_string(idx)
                                      + " out of range [0, "
                                      + std::to_string(First) + ')');
            return base_type::operator[](idx);
        }

        /// @brief 常量版本。
        const_reference operator()(size_t idx) const {
            if (idx >= First)
                throw MultiArrayError("index " + std::to_string(idx)
                                      + " out of range [0, "
                                      + std::to_string(First) + ')');
            return base_type::operator[](idx);
        }

        /**
         * @brief 多维下标访问。
         * @tparam Indices 剩余索引类型包。
         * @param idx 第一维索引。
         * @param rest 剩余维度索引。
         * @return 最终元素的引用（若索引数量等于维数）或子数组引用（若不足）。
         */
        template <class... Indices>
        auto operator()(size_t idx,
            Indices... rest) -> decltype(base_type::operator[](idx)(rest...)) {
            static_assert(sizeof...(Indices) < rank(), "Too Many Arguments!");
            if (idx >= First)
                throw MultiArrayError("index " + std::to_string(idx)
                                      + " out of range [0, "
                                      + std::to_string(First) + ')');
            return base_type::operator[](idx)(rest...);
        }

        /// @brief 常量版本。
        template <class... Indices>
        auto operator()(size_t idx, Indices... rest) const
            -> decltype(base_type::operator[](idx)(rest...)) {
            static_assert(sizeof...(Indices) < rank(), "Too Many Arguments!");
            if (idx >= First)
                throw MultiArrayError("index " + std::to_string(idx)
                                      + " out of range [0, "
                                      + std::to_string(First) + ')');
            return base_type::operator[](idx)(rest...);
        }

        /// @brief 输出 MultiArray 到流，递归输出嵌套方括号。
        friend std::ostream &
        operator<<(std::ostream &os, const MultiArray &ma) {
            if (ma.begin() == ma.end()) return os << "[]";
            auto it = ma.begin();
            os << '[' << *it;
            while (++it != ma.end()) {
                os << ", " << *it;
            }
            return os << ']';
        }

        /// @brief 扁平化起始迭代器（非常量）。
        T *fbegin() { return this->data()->fbegin(); }

        /// @brief 常量版本。
        const T *fbegin() const { return this->data()->fbegin(); }

        /// @brief 扁平化结束迭代器（非常量）。
        T *fend() { return this->fbegin() + this->fsize(); }

        /// @brief 常量版本。
        const T *fend() const { return this->fbegin() + this->fsize(); }

        /**
         * @brief 扁平化视图（可变）。
         * @note 这并非不安全，可以自行思考。
         */
        MultiArray<T, fsize()> &flatten() {
            return reinterpret_cast<MultiArray<T, fsize()> &>(*this);
        }

        /**
         * @brief 扁平化视图（常量）。
         * @note 这并非不安全，可以自行思考。
         */
        const MultiArray<T, fsize()> &flatten() const {

            return reinterpret_cast<const MultiArray<T, fsize()> &>(*this);
        }

        /**
         * @brief 返回包含维度信息的数组。
         * @return std::array 包含数组各维度的大小，长度为 rank()。
         */
        static constexpr std::array<size_t, rank()> dims() {
            return {First, Rest...};
        }
    };

    // ========================== 运算符重载（数组与数组）
    // ==========================
    /**
     * @defgroup multiarray_ops 运算符重载
     * @brief MultiArray
     * 支持元素级的算术、比较、逻辑、位运算，支持数组与数组、数组与标量的混合运算。
     * @{
     */

    /**
     * @brief 元素加法（数组 + 数组）。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param a 左操作数。
     * @param b 右操作数。
     * @return MultiArray<T, Dims...> 逐元素和。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator+(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<T, Dims...> c;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        auto                   ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ + *bi++;
        return c;
    }

    /**
     * @brief 元素减法（数组 - 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator-(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<T, Dims...> c;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        auto                   ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ - *bi++;
        return c;
    }

    /**
     * @brief 元素乘法（数组 * 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator*(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<T, Dims...> c;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        auto                   ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ * *bi++;
        return c;
    }

    /**
     * @brief 元素除法（数组 / 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator/(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<T, Dims...> c;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        auto                   ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ / *bi++;
        return c;
    }

    /**
     * @brief 元素取模（数组 % 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator%(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<T, Dims...> c;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        auto                   ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ % *bi++;
        return c;
    }

    /**
     * @brief 元素加法赋值（数组 += 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> &
    operator+=(MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        auto ai = a.fbegin();
        auto bi = b.fbegin();
        while (ai != a.fend()) *ai++ += *bi++;
        return a;
    }

    /**
     * @brief 元素减法赋值（数组 -= 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> &
    operator-=(MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        auto ai = a.fbegin();
        auto bi = b.fbegin();
        while (ai != a.fend()) *ai++ -= *bi++;
        return a;
    }

    /**
     * @brief 元素乘法赋值（数组 *= 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> &
    operator*=(MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        auto ai = a.fbegin();
        auto bi = b.fbegin();
        while (ai != a.fend()) *ai++ *= *bi++;
        return a;
    }

    /**
     * @brief 元素除法赋值（数组 /= 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> &
    operator/=(MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        auto ai = a.fbegin();
        auto bi = b.fbegin();
        while (ai != a.fend()) *ai++ /= *bi++;
        return a;
    }

    /**
     * @brief 元素取模赋值（数组 %= 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> &
    operator%=(MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        auto ai = a.fbegin();
        auto bi = b.fbegin();
        while (ai != a.fend()) *ai++ %= *bi++;
        return a;
    }

    /**
     * @brief 元素相等比较（数组 == 数组），返回 bool 数组。
     */
    template <class T, size_t... Dims>
    MultiArray<bool, Dims...> operator==(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<bool, Dims...> c;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        auto                      ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ == *bi++;
        return c;
    }

    /**
     * @brief 元素不等比较（数组 != 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<bool, Dims...> operator!=(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<bool, Dims...> c;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        auto                      ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ != *bi++;
        return c;
    }

    /**
     * @brief 元素小于比较（数组 < 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<bool, Dims...> operator<(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<bool, Dims...> c;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        auto                      ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ < *bi++;
        return c;
    }

    /**
     * @brief 元素大于比较（数组 > 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<bool, Dims...> operator>(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<bool, Dims...> c;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        auto                      ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ > *bi++;
        return c;
    }

    /**
     * @brief 元素小于等于比较（数组 <= 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<bool, Dims...> operator<=(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<bool, Dims...> c;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        auto                      ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ <= *bi++;
        return c;
    }

    /**
     * @brief 元素大于等于比较（数组 >= 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<bool, Dims...> operator>=(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<bool, Dims...> c;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        auto                      ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ >= *bi++;
        return c;
    }

    // 算术运算符（数组与标量）
    /**
     * @brief 数组 + 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator+(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ + value;
        return b;
    }

    /**
     * @brief 数组 - 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator-(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ - value;
        return b;
    }

    /**
     * @brief 数组 * 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator*(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ * value;
        return b;
    }

    /**
     * @brief 数组 / 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator/(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ / value;
        return b;
    }

    /**
     * @brief 数组 % 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator%(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ % value;
        return b;
    }

    /**
     * @brief 数组 += 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...> &
    operator+=(MultiArray<T, Dims...> &a, const U &value) {
        a.for_each([&](T &ref) { ref += value; });
        return a;
    }

    /**
     * @brief 数组 -= 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...> &
    operator-=(MultiArray<T, Dims...> &a, const U &value) {
        a.for_each([&](T &ref) { ref -= value; });
        return a;
    }

    /**
     * @brief 数组 *= 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...> &
    operator*=(MultiArray<T, Dims...> &a, const U &value) {
        a.for_each([&](T &ref) { ref *= value; });
        return a;
    }

    /**
     * @brief 数组 /= 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...> &
    operator/=(MultiArray<T, Dims...> &a, const U &value) {
        a.for_each([&](T &ref) { ref /= value; });
        return a;
    }

    /**
     * @brief 数组 %= 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...> &
    operator%=(MultiArray<T, Dims...> &a, const U &value) {
        a.for_each([&](T &ref) { ref %= value; });
        return a;
    }

    // 算术运算符（标量与数组）
    /**
     * @brief 标量 + 数组。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator+(const U &value, const MultiArray<T, Dims...> &a) {
        return a + value;
    }

    /**
     * @brief 标量 - 数组。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator-(const U &value, const MultiArray<T, Dims...> &a) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = value - *ai++;
        return b;
    }

    /**
     * @brief 标量 * 数组。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator*(const U &value, const MultiArray<T, Dims...> &a) {
        return a * value;
    }

    /**
     * @brief 标量 / 数组。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator/(const U &value, const MultiArray<T, Dims...> &a) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = value / *ai++;
        return b;
    }

    /**
     * @brief 标量 % 数组。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator%(const U &value, const MultiArray<T, Dims...> &a) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = value % *ai++;
        return b;
    }

    // 一元运算符
    /**
     * @brief 一元正号（数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator+(const MultiArray<T, Dims...> &a) {
        return a;
    }

    /**
     * @brief 一元负号（数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator-(const MultiArray<T, Dims...> &a) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = -*ai++;
        return b;
    }

    /**
     * @brief 逻辑非（数组），返回 bool 数组。
     */
    template <class T, size_t... Dims>
    MultiArray<bool, Dims...> operator!(const MultiArray<T, Dims...> &a) {
        MultiArray<bool, Dims...> b;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        while (bi != b.fend()) *bi++ = !*ai++;
        return b;
    }

    /**
     * @brief 按位取反（数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator~(const MultiArray<T, Dims...> &a) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = ~*ai++;
        return b;
    }

    // 比较运算符（数组与标量）
    /**
     * @brief 数组 == 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator==(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<bool, Dims...> b;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ == value;
        return b;
    }

    /**
     * @brief 数组 != 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator!=(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<bool, Dims...> b;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ != value;
        return b;
    }

    /**
     * @brief 数组 < 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator<(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<bool, Dims...> b;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ < value;
        return b;
    }

    /**
     * @brief 数组 > 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator>(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<bool, Dims...> b;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ > value;
        return b;
    }

    /**
     * @brief 数组 <= 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator<=(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<bool, Dims...> b;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ <= value;
        return b;
    }

    /**
     * @brief 数组 >= 标量。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator>=(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<bool, Dims...> b;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ >= value;
        return b;
    }

    // 比较运算符（标量与数组）
    /**
     * @brief 标量 == 数组。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator==(const U &value, const MultiArray<T, Dims...> &a) {
        return a == value;
    }

    /**
     * @brief 标量 != 数组。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator!=(const U &value, const MultiArray<T, Dims...> &a) {
        return a != value;
    }

    /**
     * @brief 标量 < 数组。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator<(const U &value, const MultiArray<T, Dims...> &a) {
        return a > value;
    }

    /**
     * @brief 标量 > 数组。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator>(const U &value, const MultiArray<T, Dims...> &a) {
        return a < value;
    }

    /**
     * @brief 标量 <= 数组。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator<=(const U &value, const MultiArray<T, Dims...> &a) {
        return a >= value;
    }

    /**
     * @brief 标量 >= 数组。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator>=(const U &value, const MultiArray<T, Dims...> &a) {
        return a <= value;
    }

    // 逻辑运算符（数组与数组）
    /**
     * @brief 逻辑与（数组 && 数组），返回 bool 数组。
     */
    template <class T, size_t... Dims>
    MultiArray<bool, Dims...> operator&&(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<bool, Dims...> c;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        auto                      ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ && *bi++;
        return c;
    }

    /**
     * @brief 逻辑或（数组 || 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<bool, Dims...> operator||(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<bool, Dims...> c;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        auto                      ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ || *bi++;
        return c;
    }

    // 逻辑运算符（数组与标量）
    /**
     * @brief 逻辑与（数组 && 标量）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator&&(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<bool, Dims...> b;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ && value;
        return b;
    }

    /**
     * @brief 逻辑或（数组 || 标量）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator||(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<bool, Dims...> b;
        auto                      ai = a.fbegin();
        auto                      bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ || value;
        return b;
    }

    /**
     * @brief 逻辑与（标量 && 数组）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator&&(const U &value, const MultiArray<T, Dims...> &a) {
        return a && value;
    }

    /**
     * @brief 逻辑或（标量 || 数组）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<bool, Dims...>
    operator||(const U &value, const MultiArray<T, Dims...> &a) {
        return a || value;
    }

    // 位运算符（数组与数组）
    /**
     * @brief 按位与（数组 & 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator&(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<T, Dims...> c;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        auto                   ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ & *bi++;
        return c;
    }

    /**
     * @brief 按位或（数组 | 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator|(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<T, Dims...> c;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        auto                   ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ | *bi++;
        return c;
    }

    /**
     * @brief 按位异或（数组 ^ 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator^(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<T, Dims...> c;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        auto                   ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ ^ *bi++;
        return c;
    }

    /**
     * @brief 左移（数组 << 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator<<(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<T, Dims...> c;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        auto                   ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ << *bi++;
        return c;
    }

    /**
     * @brief 右移（数组 >> 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> operator>>(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        MultiArray<T, Dims...> c;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        auto                   ci = c.fbegin();
        while (ci != c.fend()) *ci++ = *ai++ >> *bi++;
        return c;
    }

    /**
     * @brief 按位与赋值（数组 &= 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> &
    operator&=(MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        auto ai = a.fbegin();
        auto bi = b.fbegin();
        while (ai != a.fend()) *ai++ &= *bi++;
        return a;
    }

    /**
     * @brief 按位或赋值（数组 |= 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> &
    operator|=(MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        auto ai = a.fbegin();
        auto bi = b.fbegin();
        while (ai != a.fend()) *ai++ |= *bi++;
        return a;
    }

    /**
     * @brief 按位异或赋值（数组 ^= 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> &
    operator^=(MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        auto ai = a.fbegin();
        auto bi = b.fbegin();
        while (ai != a.fend()) *ai++ ^= *bi++;
        return a;
    }

    /**
     * @brief 左移赋值（数组 <<= 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> &
    operator<<=(MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        auto ai = a.fbegin();
        auto bi = b.fbegin();
        while (ai != a.fend()) *ai++ <<= *bi++;
        return a;
    }

    /**
     * @brief 右移赋值（数组 >>= 数组）。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> &
    operator>>=(MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        auto ai = a.fbegin();
        auto bi = b.fbegin();
        while (ai != a.fend()) *ai++ >>= *bi++;
        return a;
    }

    // 位运算符（数组与标量）
    /**
     * @brief 数组 & 标量（按位与）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator&(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ & value;
        return b;
    }

    /**
     * @brief 数组 | 标量（按位或）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator|(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ | value;
        return b;
    }

    /**
     * @brief 数组 ^ 标量（按位异或）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator^(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ ^ value;
        return b;
    }

    /**
     * @brief 数组 << 标量（左移）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator<<(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ << value;
        return b;
    }

    /**
     * @brief 数组 >> 标量（右移）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator>>(const MultiArray<T, Dims...> &a, const U &value) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = *ai++ >> value;
        return b;
    }

    /**
     * @brief 标量 & 数组（按位与，对称调用）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator&(const U &value, const MultiArray<T, Dims...> &a) {
        return a & value;
    }

    /**
     * @brief 标量 | 数组（按位或，对称调用）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator|(const U &value, const MultiArray<T, Dims...> &a) {
        return a | value;
    }

    /**
     * @brief 标量 ^ 数组（按位异或，对称调用）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator^(const U &value, const MultiArray<T, Dims...> &a) {
        return a ^ value;
    }

    /**
     * @brief 标量 << 数组（左移，标量左移数组每位）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator<<(const U &value, const MultiArray<T, Dims...> &a) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = value << *ai++;
        return b;
    }

    /**
     * @brief 标量 >> 数组（右移，标量右移数组每位）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...>
    operator>>(const U &value, const MultiArray<T, Dims...> &a) {
        MultiArray<T, Dims...> b;
        auto                   ai = a.fbegin();
        auto                   bi = b.fbegin();
        while (bi != b.fend()) *bi++ = value >> *ai++;
        return b;
    }

    /**
     * @brief 数组 &= 标量（按位与赋值）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...> &
    operator&=(MultiArray<T, Dims...> &a, const U &value) {
        a.for_each([&](T &ref) { ref &= value; });
        return a;
    }

    /**
     * @brief 数组 |= 标量（按位或赋值）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...> &
    operator|=(MultiArray<T, Dims...> &a, const U &value) {
        a.for_each([&](T &ref) { ref |= value; });
        return a;
    }

    /**
     * @brief 数组 ^= 标量（按位异或赋值）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...> &
    operator^=(MultiArray<T, Dims...> &a, const U &value) {
        a.for_each([&](T &ref) { ref ^= value; });
        return a;
    }

    /**
     * @brief 数组 <<= 标量（左移赋值）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...> &
    operator<<=(MultiArray<T, Dims...> &a, const U &value) {
        a.for_each([&](T &ref) { ref <<= value; });
        return a;
    }

    /**
     * @brief 数组 >>= 标量（右移赋值）。
     */
    template <class T, size_t... Dims, class U>
    MultiArray<T, Dims...> &
    operator>>=(MultiArray<T, Dims...> &a, const U &value) {
        a.for_each([&](T &ref) { ref >>= value; });
        return a;
    }

    /** @} */ // end of multiarray_ops

    // ========================== 统计函数 ==========================
    /**
     * @defgroup multiarray_stats 统计函数
     * @brief 对 MultiArray 进行归约操作。
     * @{
     */

    /**
     * @brief 求和所有元素。
     * @tparam T 元素类型。
     * @tparam Dims 维度。
     * @param a 输入数组。
     * @return T 和。
     */
    template <class T, size_t... Dims> T sum(const MultiArray<T, Dims...> &a) {
        T result{};
        a.for_each([&](const T &ref) { result += ref; });
        return result;
    }

    /**
     * @brief 求最小值。
     * @return T 最小值。
     */
    template <class T, size_t... Dims> T min(const MultiArray<T, Dims...> &a) {
        return *std::min_element(a.fbegin(), a.fend());
    }

    /**
     * @brief 求最大值。
     * @return T 最大值。
     */
    template <class T, size_t... Dims> T max(const MultiArray<T, Dims...> &a) {
        return *std::max_element(a.fbegin(), a.fend());
    }

    /**
     * @brief 检查所有元素是否都为 true（逻辑与归约）。
     * @return bool 若所有元素均为真则返回 true。
     */
    template <class T, size_t... Dims>
    bool all(const MultiArray<T, Dims...> &a) {
        for (auto it = a.fbegin(); it != a.fend(); ++it)
            if (!*it) return false;
        return true;
    }

    /**
     * @brief 检查是否存在至少一个 true 元素（逻辑或归约）。
     * @return bool 若至少一个元素为真则返回 true。
     */
    template <class T, size_t... Dims>
    bool any(const MultiArray<T, Dims...> &a) {
        for (auto it = a.fbegin(); it != a.fend(); ++it)
            if (*it) return true;
        return false;
    }

    /** @} */ // end of multiarray_stats

    // ========================== 类型转换 ==========================
    /**
     * @defgroup multiarray_cast 类型转换
     * @brief 在不同维度形状或元素类型之间转换 MultiArray。
     * @{
     */

    /**
     * @brief 改变 MultiArray 的维度形状（元素类型不变），要求总元素数相同。
     * @tparam OutArrDims 目标维度包。
     * @tparam VarType 元素类型。
     * @tparam InArrDims 源维度包。
     * @param inputArr 源数组。
     * @return MultiArray<VarType, OutArrDims...> 转换后的数组（拷贝元素）。
     * @note 编译期检查元素总数是否一致。
     */
    template <size_t... OutArrDims, class VarType, size_t... InArrDims>
    MultiArray<VarType, OutArrDims...>
    multiarray_cast(const MultiArray<VarType, InArrDims...> &inputArr) {
        static_assert(MultiArray<VarType, OutArrDims...>::fsize()
                          == MultiArray<VarType, InArrDims...>::fsize(),
            "Bad multiarray_cast: Mismatch Size");
        MultiArray<VarType, OutArrDims...> outputArr;
        std::copy(inputArr.fbegin(), inputArr.fend(), outputArr.fbegin());
        return outputArr;
    }

    /**
     * @brief 不安全地改变 MultiArray 的元素类型和维度（内存拷贝）。
     * @tparam OutType 目标元素类型。
     * @tparam OutArrDims 目标维度包。
     * @tparam InType 源元素类型。
     * @tparam InArrDims 源维度包。
     * @param inputArr 源数组。
     * @return MultiArray<InType, OutArrDims...> 重新解释内存的结果。
     * @warning 此函数使用 memcpy
     * 直接复制内存，要求源和目标总字节数相同，且类型是平凡可复制的。
     *          不进行任何类型检查，可能导致未定义行为。
     */
    template <class OutType,
        size_t... OutArrDims,
        class InType,
        size_t... InArrDims>
    MultiArray<OutType, OutArrDims...>
    unsafe_multiarray_cast(const MultiArray<InType, InArrDims...> &inputArr) {
        MultiArray<OutType, OutArrDims...> outputArr;
        memcpy(&outputArr, &inputArr, sizeof(outputArr));
        return outputArr;
    }

    /**
     * @brief @brief 就地版本的 multiarray_cast，仅视图，不拷贝（可变版本）
     * @tparam OutArrDims 目标维度包。
     * @tparam VarType 元素类型。
     * @tparam InArrDims 源维度包。
     * @param inputArr 源数组。
     * @return MultiArray<VarType, OutArrDims...> & 转换后的数组引用。
     * @note 编译期检查元素总数是否一致。
     */
    template <size_t... OutArrDims, class VarType, size_t... InArrDims>
    MultiArray<VarType, OutArrDims...> &
    inplace_multiarray_cast(MultiArray<VarType, InArrDims...> &inputArr) {
        static_assert(MultiArray<VarType, OutArrDims...>::fsize()
                          == MultiArray<VarType, InArrDims...>::fsize(),
            "Bad inplace_multiarray_cast: Mismatch Size");
        auto p
            = reinterpret_cast<MultiArray<VarType, OutArrDims...> &>(inputArr);
        return *p;
    }

    /**
     * @brief @brief 就地版本的 multiarray_cast，仅视图，不拷贝（常量版本）
     * @tparam OutArrDims 目标维度包。
     * @tparam VarType 元素类型。
     * @tparam InArrDims 源维度包。
     * @param inputArr 源数组。
     * @return const MultiArray<VarType, OutArrDims...> & 转换后的数组引用。
     * @note 编译期检查元素总数是否一致。
     */
    template <size_t... OutArrDims, class VarType, size_t... InArrDims>
    const MultiArray<VarType, OutArrDims...> &
    inplace_multiarray_cast(const MultiArray<VarType, InArrDims...> &inputArr) {
        static_assert(MultiArray<VarType, OutArrDims...>::fsize()
                          == MultiArray<VarType, InArrDims...>::fsize(),
            "Bad inplace_multiarray_cast: Mismatch Size");
        auto p = reinterpret_cast<const MultiArray<VarType, OutArrDims...> &>(
            inputArr);
        return *p;
    }

    /** @} */ // end of multiarray_cast
}
