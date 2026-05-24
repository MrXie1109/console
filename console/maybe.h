/**
 * @file maybe.h
 * @brief 提供类似 std::optional 的可选值容器 Maybe，支持空状态（nothing）。
 * @details Maybe<T> 可包含一个 T 类型的值或为空（nothing）。提供安全的访问、值提取、默认值等操作。
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
#include <utility>
#include <memory>
#include <iostream>
#include "csexc.h"

namespace console
{
    /**
     * @brief 空状态标记对象，用于显式构造空的 Maybe。
     * @details 使用 `nothing` 可以清空 Maybe 或构造一个空 Maybe。
     */
    struct
    {
    } nothing;

    /**
     * @class Maybe
     * @brief 可选值容器，可包含一个值或为空。
     * @tparam T 存储的值类型。
     * @details 基于 std::unique_ptr 实现，支持拷贝、移动、流输入输出。
     *          访问空 Maybe 会抛出 TypeError 异常。
     */
    template <class T>
    class Maybe
    {
        std::unique_ptr<T> ptr; ///< 实际存储的指针，为空表示 nothing。

    public:
        /// @brief 默认构造一个空 Maybe。
        Maybe() : ptr(nullptr) {}

        /**
         * @brief 从参数包直接构造一个包含值的 Maybe。
         * @tparam Args 构造 T 所需的参数类型。
         * @param args 转发给 T 的构造函数。
         */
        template <class... Args>
        Maybe(Args &&...args)
            : ptr(new T(std::forward<Args>(args)...)) {}

        /**
         * @brief 拷贝构造，深拷贝内部值。
         * @param other 源 Maybe。
         * @note 若 other 为空，则新对象也为空。
         */
        Maybe(const Maybe &other)
            : ptr(other.ptr ? new T(other.value()) : nullptr) {}

        /**
         * @brief 移动构造，转移所有权。
         * @param other 源 Maybe，移动后为空。
         */
        Maybe(Maybe &&other)
            : ptr(std::move(other.ptr)) {}

        /**
         * @brief 从 nothing 标记构造空 Maybe。
         * @param  nothing 标记。
         */
        Maybe(decltype(nothing)) : ptr(nullptr) {}

        /**
         * @brief 获取内部值的引用（非常量）。
         * @return T& 值的引用。
         * @throw TypeError 若 Maybe 为空。
         */
        T &value()
        {
            if (ptr)
                return *ptr;
            throw TypeError("Nothing");
        }

        /**
         * @brief 获取内部值的引用（常量）。
         * @return const T& 值的常量引用。
         * @throw TypeError 若 Maybe 为空。
         */
        const T &value() const
        {
            if (ptr)
                return *ptr;
            throw TypeError("Nothing");
        }

        /**
         * @brief 从 T 值赋值（拷贝）。
         * @param value 要赋值的值。
         * @return const Maybe& *this。
         */
        const Maybe &operator=(const T &value)
        {
            ptr.reset(new T(value));
            return *this;
        }

        /**
         * @brief 从 T 值赋值（移动）。
         * @param value 要赋值的右值。
         * @return const Maybe& *this。
         */
        const Maybe &operator=(T &&value)
        {
            ptr.reset(new T(std::move(value)));
            return *this;
        }

        /**
         * @brief 拷贝赋值。
         * @param other 源 Maybe。
         * @return Maybe& *this。
         */
        Maybe &operator=(const Maybe &other)
        {
            if (this != &other)
                ptr.reset(other.ptr ? new T(*other.ptr) : nullptr);
            return *this;
        }

        /**
         * @brief 移动赋值。
         * @param other 源 Maybe，移动后为空。
         * @return Maybe& *this。
         */
        Maybe &operator=(Maybe &&other) noexcept
        {
            if (this != &other)
                ptr = std::move(other.ptr);
            return *this;
        }

        /**
         * @brief 赋值为空（nothing）。
         * @param nothing 标记。
         * @return const Maybe& *this。
         */
        const Maybe &operator=(decltype(nothing))
        {
            ptr = nullptr;
            return *this;
        }

        /**
         * @brief 清空 Maybe，使其为空。
         */
        void reset()
        {
            ptr.reset();
        }

        /**
         * @brief 重置为新的值（从参数包构造）。
         * @tparam Args 参数类型。
         * @param args 转发给 T 的构造函数。
         */
        template <class... Args>
        void reset(Args &&...args)
        {
            ptr.reset(new T(std::forward<Args>(args)...));
        }

        /**
         * @brief 输出 Maybe 到流。若包含值则输出值，否则输出 "(nothing)"。
         * @param os 输出流。
         * @param maybe Maybe 对象。
         * @return std::ostream& os。
         */
        friend std::ostream &operator<<(std::ostream &os, const Maybe &maybe)
        {
            if (maybe.ptr)
                return os << maybe.value();
            return os << "(nothing)";
        }

        /**
         * @brief 从流读取一个值到 Maybe。
         * @param is 输入流。
         * @param maybe 目标 Maybe。
         * @return std::istream& is。
         * @details 尝试从流读取一个 T 类型的值，若成功则存入 Maybe，否则将 Maybe 置空并清除错误标志。
         */
        friend std::istream &operator>>(std::istream &is, Maybe &maybe)
        {
            T tmp;
            if (is >> tmp)
                maybe.reset(std::move(tmp));
            else
            {
                maybe = nothing;
                is.clear();
            }
            return is;
        }

        /**
         * @brief 检查 Maybe 是否包含值。
         * @return true 若有值，false 若为空。
         */
        explicit operator bool() const noexcept { return ptr; }

        /**
         * @brief 检查 Maybe 是否包含值。
         * @return true 若有值，false 若为空。
         */
        bool has_value() const noexcept { return ptr; }

        /// @brief 解引用获取值（非常量），前置条件：has_value() 为 true。
        T &operator*() { return *ptr; }

        /// @brief 解引用获取值（常量），前置条件：has_value() 为 true。
        const T &operator*() const { return *ptr; }

        /// @brief 成员访问运算符（非常量），前置条件：has_value() 为 true。
        T *operator->() { return ptr.get(); }

        /// @brief 成员访问运算符（常量），前置条件：has_value() 为 true。
        const T *operator->() const { return ptr.get(); }

        /**
         * @brief 返回当前值，若为空则返回提供的默认值。
         * @tparam U 默认值类型。
         * @param default_value 默认值（可转发）。
         * @return T 当前值或转换后的默认值。
         */
        template <class U>
        T value_or(U &&default_value) const
        {
            return ptr ? *ptr : T(std::forward<U>(default_value));
        }

        /**
         * @brief 交换两个 Maybe 的内容。
         * @param other 要交换的 Maybe。
         */
        void swap(Maybe &other) noexcept
        {
            ptr.swap(other.ptr);
        }
    };
}
