/**
 * @file result.h
 * @brief 提供类似 Rust 的 Result 类型，用于表示成功(Ok）或失败(Err）的结果。
 * @details Result<T, E> 可包含一个成功值 T 或一个错误值 E。提供安全的访问、
 *          链式操作、值提取等功能。使用联合存储(union-like）方式节省内存。
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
#include <cassert>
#include <cstring>
#include <new>
#include <type_traits>
#include <utility>

#include "csexc.h"

#if __cplusplus >= 201703L
#include <optional>
#endif

namespace console {
    /**
     * @class Result
     * @brief 表示可能成功或失败的结果，类似于 Rust 的 Result 类型。
     * @tparam T 成功值的类型。
     * @tparam E 错误值的类型。
     * @details Result 要么包含一个 T 类型的成功值(Ok），要么包含一个 E
     *          类型的错误值(Err）。使用对齐的字节缓冲区存储两者之一，
     *          避免了额外的堆分配。访问错误状态的成功值会抛出异常。
     */
    template <class T, class E>
    class Result {
        static_assert(
            !std::is_same<T, E>::value, "T and E cannot be the same type");

        /// @brief 缓冲区大小，取 T 和 E 中较大者。
        static constexpr size_t buffer_size
            = sizeof(T) > sizeof(E) ? sizeof(T) : sizeof(E);
        /// @brief 缓冲区对齐要求，取 T 和 E 中较大者。
        static constexpr size_t buffer_align
            = alignof(T) > alignof(E) ? alignof(T) : alignof(E);

        /// @brief 对齐的原始字节缓冲区，用于存储 T 或 E。
        alignas(buffer_align) mutable unsigned char buffer[buffer_size];
        /// @brief 标记当前是否为 Ok 状态(true 表示 Ok，false 表示 Err）。
        bool is_ok_;

        /// @brief 获取指向成功值 T 的指针(常量）。
        T *get_ok_ptr() const { return reinterpret_cast<T *>(buffer); }
        /// @brief 获取指向错误值 E 的指针(常量）。
        E *get_err_ptr() const { return reinterpret_cast<E *>(buffer); }

    public:
        /// @brief 成功值类型别名。
        typedef T value_type;
        /// @brief 错误值类型别名。
        typedef E error_type;

        /**
         * @brief 从成功值构造 Result(拷贝）。
         * @param value 成功值。
         */
        Result(const T &value) : is_ok_(true) { new (get_ok_ptr()) T(value); }

        /**
         * @brief 从错误值构造 Result(拷贝）。
         * @param error 错误值。
         */
        Result(const E &error) : is_ok_(false) { new (get_err_ptr()) E(error); }

        /**
         * @brief 从成功值构造 Result(移动）。
         * @param value 成功值右值。
         */
        Result(T &&value) : is_ok_(true) {
            new (get_ok_ptr()) T(std::move(value));
        }

        /**
         * @brief 从错误值构造 Result(移动）。
         * @param error 错误值右值。
         */
        Result(E &&error) : is_ok_(false) {
            new (get_err_ptr()) E(std::move(error));
        }

        /**
         * @brief 拷贝构造，深拷贝内部值。
         * @param other 源 Result。
         * @note 根据 other 的状态拷贝对应的 T 或 E。
         */
        Result(const Result &other) : is_ok_(other.is_ok_) {
            if (is_ok_) {
                new (get_ok_ptr()) T(*other.get_ok_ptr());
            } else {
                new (get_err_ptr()) E(*other.get_err_ptr());
            }
        }

        /**
         * @brief 移动构造，转移内部值。
         * @param other 源 Result，移动后仍保持原状态(值被移动）。
         */
        Result(Result &&other) noexcept : is_ok_(other.is_ok_) {
            if (is_ok_)
                new (get_ok_ptr()) T(std::move(*other.get_ok_ptr()));
            else
                new (get_err_ptr()) E(std::move(*other.get_err_ptr()));
        }

        /**
         * @brief 拷贝赋值(使用 copy-and-swap 惯用法）。
         * @param other 源 Result。
         * @return Result& *this。
         */
        Result &operator=(const Result &other) {
            if (this != &other) Result(other).swap(*this);
            return *this;
        }

        /**
         * @brief 移动赋值(使用 copy-and-swap 惯用法）。
         * @param other 源 Result。
         * @return Result& *this。
         */
        Result &operator=(Result &&other) noexcept {
            if (this != &other) Result(std::move(other)).swap(*this);
            return *this;
        }

        /**
         * @brief 析构函数，正确销毁当前存储的值。
         */
        ~Result() {
            if (is_ok_)
                get_ok_ptr()->~T();
            else
                get_err_ptr()->~E();
        }

        /**
         * @brief 获取成功值的副本(左值引用版本）。
         * @return T 成功值的副本。
         * @note 仅当 is_ok() 为 true 时调用，
         *       否则触发 assert (DEBUG) 或未定义行为 (RELEASE)。
         */
        T ok() const & {
            assert(is_ok_ && "Called get_ok on error");
            return *get_ok_ptr();
        }

        /**
         * @brief 获取错误值的副本(左值引用版本）。
         * @return E 错误值的副本。
         * @note 仅当 is_err() 为 true 时调用，
         *       否则触发 assert (DEBUG) 或未定义行为 (RELEASE)。
         */
        E err() const & {
            assert(!is_ok_ && "Called get_err on success");
            return *get_err_ptr();
        }

        /**
         * @brief 移动获取成功值(右值引用版本）。
         * @return T 成功值的右值引用。
         * @note 仅当 is_ok() 为 true 时调用，
         *       否则触发 assert (DEBUG) 或未定义行为 (RELEASE)。
         */
        T ok() && {
            assert(is_ok_ && "Called get_ok on error");
            return std::move(*get_ok_ptr());
        }

        /**
         * @brief 移动获取错误值(右值引用版本）。
         * @return E 错误值的右值引用。
         * @note 仅当 is_err() 为 true 时调用，
         *       否则触发 assert (DEBUG) 或未定义行为 (RELEASE)。
         */
        E err() && {
            assert(!is_ok_ && "Called get_err on success");
            return std::move(*get_err_ptr());
        }

        /**
         * @brief 解包成功值，若为错误则抛出异常。
         * @return T 成功值的副本。
         * @throw ValueError 若 Result 为 Err 状态。
         * @note 相当于 Rust 的 unwrap()。
         */
        T unwrap() const {
            if (!is_ok_) throw ValueError("Called unwrap on error");
            return *get_ok_ptr();
        }

        /**
         * @brief 带自定义消息的解包，若为错误则抛出异常。
         * @param msg 异常消息。
         * @return T 成功值的副本。
         * @throw ValueError 若 Result 为 Err 状态，携带 msg。
         * @note 相当于 Rust 的 expect()。
         */
        T expect(const std::string &msg) const {
            if (!is_ok_) throw ValueError(msg);
            return *get_ok_ptr();
        }

        /**
         * @brief 检查是否为 Ok(成功）状态。
         * @return true 若为 Ok，false 若为 Err。
         */
        bool is_ok() const { return is_ok_; }

        /**
         * @brief 检查是否为 Err(错误）状态。
         * @return true 若为 Err，false 若为 Ok。
         */
        bool is_err() const { return !is_ok_; }

        /**
         * @brief 访问 Result 的内容，根据状态调用不同的函数。
         * @tparam F Ok 处理函数类型。
         * @tparam G Err 处理函数类型。
         * @param f 若为 Ok，用成功值调用 f。
         * @param g 若为 Err，用错误值调用 g。
         * @details 类似于模式匹配，根据状态分发到对应的处理函数。
         */
        template <class F, class G>
        void visit(F &&f, G &&g) const {
            if (is_ok_)
                f(ok());
            else
                g(err());
        }

        /**
         * @brief 链式操作：若为 Ok 则应用 f，否则传递 Err(左值引用版本）。
         * @tparam F 映射函数类型，接受 T 返回 Result<U, E>。
         * @param f 应用到成功值的函数。
         * @return Result<U, E> f 返回的结果，或当前的 Err。
         */
        template <class F>
        auto and_then(F &&f) const & -> Result< //
            typename std::decay<decltype(f(
                std::declval<T>()))>::type::value_type,
            E> {
            if (is_ok_) return f(*get_ok_ptr());
            return *get_err_ptr();
        }

        /**
         * @brief 链式操作：若为 Ok 则应用 f，否则传递 Err(右值引用版本）。
         * @tparam F 映射函数类型，接受 T 返回 Result<U, E>。
         * @param f 应用到成功值的函数。
         * @return Result<U, E> f 返回的结果，或移动的 Err。
         */
        template <class F>
        auto and_then(F &&f) && -> Result< //
            typename std::decay<decltype(f(
                std::declval<T>()))>::type::value_type,
            E> {
            if (is_ok_) return f(std::move(*get_ok_ptr()));
            return std::move(*get_err_ptr());
        }

        /**
         * @brief 链式操作：若为 Err 则应用 f，否则传递 Ok(左值引用版本）。
         * @tparam F 映射函数类型，接受 E 返回 Result<T, U>。
         * @param f 应用到错误值的函数。
         * @return Result<T, U> f 返回的结果，或当前的 Ok。
         * @note 与 Rust 的 or_else 语义不同，注意区分。
         */
        template <class F>
        auto or_else(F &&f) const & -> Result<T,
            typename std::decay<decltype(f(
                std::declval<E>()))>::type::error_type> {
            if (is_ok_) return ok();
            return f(err());
        }

        /**
         * @brief 链式操作：若为 Err 则应用 f，否则传递 Ok(右值引用版本）。
         * @tparam F 映射函数类型，接受 E 返回 Result<T, U>。
         * @param f 应用到错误值的函数。
         * @return Result<T, U> f 返回的结果，或移动的 Ok。
         * @note 与 Rust 的 or_else 语义不同，注意区分。
         */
        template <class F>
        auto or_else(F &&f) && -> Result<T,
            typename std::decay<decltype(f(
                std::declval<E>()))>::type::error_type> {
            if (is_ok_) return std::move(*get_ok_ptr());
            return f(std::move(*get_err_ptr()));
        }

        /**
         * @brief 交换两个 Result 的内容。
         * @param other 要交换的 Result。
         */
        void swap(Result &other) noexcept {
            if (this == &other) return;
            if (is_ok_ && other.is_ok_) {
                T temp              = std::move(*get_ok_ptr());
                *get_ok_ptr()       = std::move(*other.get_ok_ptr());
                *other.get_ok_ptr() = std::move(temp);
            } else if (!is_ok_ && !other.is_ok_) {
                E temp               = std::move(*get_err_ptr());
                *get_err_ptr()       = std::move(*other.get_err_ptr());
                *other.get_err_ptr() = std::move(temp);
            } else if (is_ok_) {
                T temp_t = std::move(*get_ok_ptr());
                get_ok_ptr()->~T();
                new (get_err_ptr()) E(std::move(*other.get_err_ptr()));
                other.get_err_ptr()->~E();
                new (other.get_ok_ptr()) T(std::move(temp_t));
            } else {
                E temp_e = std::move(*get_err_ptr());
                get_err_ptr()->~E();
                new (get_ok_ptr()) T(std::move(*other.get_ok_ptr()));
                other.get_ok_ptr()->~T();
                new (other.get_err_ptr()) E(std::move(temp_e));
            }
            std::swap(is_ok_, other.is_ok_);
        }

        /**
         * @brief 交换两个 Result 的内容(友元函数）。
         * @param a 第一个 Result。
         * @param b 第二个 Result。
         */
        friend void swap(Result &a, Result &b) noexcept { a.swap(b); }

        /**
         * @brief 布尔转换，检查是否为 Ok。
         * @return true 若为 Ok，false 若为 Err。
         */
        explicit operator bool() const { return is_ok_; }

        /**
         * @brief 逻辑非，检查是否为 Err。
         * @return true 若为 Err，false 若为 Ok。
         */
        bool operator!() const { return !is_ok_; }

        /**
         * @brief 如果是Ok则返回值的引用，否则返回默认值的引用。
         * @param default_value 默认值引用。
         * @return const T& 值的引用。
         */
        const T &unwrap_or(const T &default_value) const & {
            return is_ok_ ? *get_ok_ptr() : default_value;
        }

        /**
         * @brief 如果是Ok则移动返回值，否则返回默认值。
         * @param default_value 默认值。
         * @return T 值。
         */
        T unwrap_or(T &&default_value) && {
            return is_ok_ ? std::move(*get_ok_ptr()) : std::move(default_value);
        }

        /**
         * @brief 如果是Ok则返回Ok值，否则通过函数生成默认值。
         * @param fn 生成默认值的函数。
         * @return T 值。
         */
        template <class F>
        T unwrap_or_else(F &&fn) const & {
            return is_ok_ ? *get_ok_ptr() : fn();
        }

        /**
         * @brief 如果是Ok则移动返回值，否则通过函数生成默认值。
         * @param fn 生成默认值的函数。
         * @return T 值。
         */
        template <class F>
        T unwrap_or_else(F &&fn) && {
            return is_ok_ ? std::move(*get_ok_ptr()) : fn();
        }

        /**
         * @brief 获取Ok值的指针(可能为空）。
         * @return const T* 指向Ok值的指针，如果是Err则返回nullptr。
         */
        const T *ok_ptr() const { return is_ok_ ? get_ok_ptr() : nullptr; }

        /**
         * @brief 获取Ok值的指针(可能为空）。
         * @return T* 指向Ok值的指针，如果是Err则返回nullptr。
         */
        T *ok_ptr() { return is_ok_ ? get_ok_ptr() : nullptr; }

        /**
         * @brief 获取Err值的指针(可能为空)。
         * @return const E* 指向Err值的指针，如果是Ok则返回nullptr。
         */
        const E *err_ptr() const { return !is_ok_ ? get_err_ptr() : nullptr; }

        /**
         * @brief 获取Err值的指针(可能为空）。
         * @return E* 指向Err值的指针，如果是Ok则返回nullptr。
         */
        E *err_ptr() { return !is_ok_ ? get_err_ptr() : nullptr; }

        /**
         * @brief 相等性比较。
         * @return bool 两个Result对象是否相等。
         */
        bool operator==(const Result &other) const {
            if (is_ok_ != other.is_ok_) return false;
            if (is_ok_) return *get_ok_ptr() == *other.get_ok_ptr();
            return *get_err_ptr() == *other.get_err_ptr();
        }

        /**
         * @brief 不等性比较。
         * @return bool 两个Result对象是否不等。
         */
        bool operator!=(const Result &other) const { return !(*this == other); }

#if __cplusplus >= 201703L
        /**
         * @brief 将Ok值转换为std::optional。
         * @return std::optional<T> 包含Ok值的std::optional，
         *                          如果是Err则返回std::nullopt。
         */
        std::optional<T> as_optional() const & {
            return is_ok_ ? std::optional<T>(*get_ok_ptr()) : std::nullopt;
        }

        /**
         * @brief 将Ok值转换为std::optional。
         * @return std::optional<T> 包含Ok值的std::optional，
         *                          如果是Err则返回std::nullopt。
         */
        std::optional<T> as_optional() && {
            return is_ok_ ? std::optional<T>(std::move(*get_ok_ptr()))
                          : std::nullopt;
        }
#endif
    };

    /**
     * 关于 template <class E> class Result<void, E> 特化，
     * 也许下个版本就有了呢？(只是也许)
     */
}
