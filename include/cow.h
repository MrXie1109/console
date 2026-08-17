/**
 * @file cow.h
 * @brief 提供 Cow 类实现写时复制操作。
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
#include <memory>
#include <type_traits>
#include <utility>

namespace console {
    /**
     * @class Cow
     * @brief 写时复制数据类。
     * @tparam T 数据类型。
     */
    template <class T>
    class Cow {
        std::shared_ptr<T> data_; ///< 内部数据指针。

    public:
        /**
         * @brief 构造函数，使用可变参数构造内部数据。
         * @tparam Args 构造函数参数类型。
         * @param args 构造函数参数。
         */
        template <class... Args,
            class = typename std::enable_if<
                std::is_constructible<T, Args...>::value>::type>
        explicit Cow(Args &&...args) :
            data_(std::make_shared<T>(std::forward<Args>(args)...)) {}

        /// @brief 默认构造函数。
        explicit Cow() : data_(std::make_shared<T>()) {}

        /// @brief 默认的拷贝构造函数。
        Cow(const Cow &) = default;
        /// @brief 默认的移动构造函数。
        Cow(Cow &&) = default;
        /// @brief 默认的拷贝赋值函数。
        Cow &operator=(const Cow &) = default;
        /// @brief 默认的移动赋值函数。
        Cow &operator=(Cow &&) = default;

        /**
         * @brief 从已有对象拷贝构造 Cow。
         * @param t 已有对象。
         */
        Cow(const T &t) : data_(std::make_shared<T>(t)) {}

        /**
         * @brief 从已有对象移动构造 Cow。
         * @param t 已有对象。
         */
        Cow(T &&t) : data_(std::make_shared<T>(std::move(t))) {}

        /**
         * @brief 赋值运算符。
         * @param t 已有对象。
         * @return 自身引用。
         */
        Cow &operator=(const T &t) {
            data_ = std::make_shared<T>(t);
            return *this;
        }

        /**
         * @brief 移动赋值运算符。
         * @param t 已有对象。
         * @return 自身引用。
         */
        Cow &operator=(T &&t) {
            data_ = std::make_shared<T>(std::move(t));
            return *this;
        }

        /**
         * @brief 分离内部数据，确保只有一个共享指针。
         * @note 若已独立，则不复制任何东西。
         */
        void detach() {
            if (data_.unique()) return;
            std::make_shared<T>(*data_).swap(data_);
        }

        /**
         * @brief 读取数据的常量引用。
         * @return 数据的常量引用。
         */
        const T &reader() const { return *data_; }
        /// @copydoc reader()
        const T &r() const { return *data_; }

        /**
         * @brief 获取数据的引用，用于写入操作。
         * @return 数据的引用。
         * @note 若可能，不要保留引用。
         */
        T &writer() {
            detach();
            return *data_;
        }
        /// @copydoc writer()
        T &w() { return writer(); }

        /**
         * @brief 读取数据的通用函数。
         * @tparam F 函数类型。
         * @param f 函数对象。
         * @return 函数的返回值。
         */
        template <class F>
        auto read(F &&f) const -> decltype(f(reader())) {
            return f(reader());
        }
        /// @copydoc read()
        template <class F>
        auto r(F &&f) const -> decltype(f(r())) {
            return f(r());
        }

        /**
         * @brief 写入数据的通用函数。
         * @tparam F 函数类型。
         * @param f 函数对象。
         * @return 函数的返回值。
         */
        template <class F>
        auto write(F &&f) -> decltype(f(writer())) {
            return f(writer());
        }
        /// @copydoc write()
        template <class F>
        auto w(F &&f) -> decltype(f(w())) {
            return f(w());
        }

        /**
         * @brief 获取数据的共享指针。
         * @return 数据的共享指针。
         * @note 只读。
         */
        const std::shared_ptr<T> &data() const { return data_; }
    };
}
