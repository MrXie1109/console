/**
 * @file box.h
 * @brief 提供可存储任意类型的异构容器（Box）和类型擦除包装器（Item）。
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
#include <cstdlib>
#include <memory>
#include <ostream>
#include <typeinfo>
#include <utility>
#include <vector>

#include "csexc.h"
#include "literals.h"
#include "strpp.h"

namespace console {
    /**
     * @class Item
     * @brief 可存储任意类型单个对象的类型擦除包装器。
     * @details 采用经典的继承 + 虚函数方式实现类型擦除，不进行小对象优化。
     *          支持拷贝、移动、类型安全访问和不安全访问。
     */
    class Item {
    private:
        /// @brief 虚基类，定义了所有派生类必须实现的接口。
        struct Base {
            virtual ~Base() {}
            virtual Base       *clone() const                               = 0;
            virtual void        print(std::ostream &) const                 = 0;
            virtual void        print(std::wostream &) const                = 0;
            virtual void        print(std::basic_ostream<char16_t> &) const = 0;
            virtual void        print(std::basic_ostream<char32_t> &) const = 0;
            virtual std::string str() const                                 = 0;
            virtual std::wstring          wstr() const                      = 0;
            virtual std::u16string        u16str() const                    = 0;
            virtual std::u32string        u32str() const                    = 0;
            virtual const std::type_info &type() const                      = 0;
        };

        /**
         * @brief 模板派生类，具体存储类型 T 的值。
         * @tparam T 实际存储的数据类型。
         */
        template <typename T>
        struct Derived : Base {
            T value; ///< 实际存储的值

            /**
             * @brief 从左值构造 Derived。
             * @param v 源左值。
             */
            Derived(const T &v) : value(v) {}

            /**
             * @brief 从右值构造 Derived。
             * @param v 源右值。
             */
            Derived(T &&v) : value(std::move(v)) {}

            /**
             * @brief 克隆当前对象（堆上分配）。
             * @return 指向新拷贝的 Base 指针。
             */
            Base *clone() const override { return new Derived(value); }

            /**
             * @brief 将存储的值输出到流（使用 repr 函数）。
             * @param os 目标输出流。
             */
            void print(std::ostream &os) const override { repr(value, os); }
            void print(std::wostream &os) const override {
                auto s = str();
                os << std::wstring(s.begin(), s.end());
            }
            void print(std::basic_ostream<char16_t> &os) const override {
                auto s = str();
                os << std::u16string(s.begin(), s.end());
            }
            void print(std::basic_ostream<char32_t> &os) const override {
                auto s = str();
                os << std::u32string(s.begin(), s.end());
            }

            /**
             * @brief 返回存储值的字符串表示。
             * @return 值的字符串形式。
             */
            std::string str() const override {
                std::ostringstream oss;
                repr(value, oss);
                return oss.str();
            }

            std::wstring wstr() const override {
                auto s = str();
                return std::wstring(s.begin(), s.end());
            }
            std::u16string u16str() const override {
                auto s = str();
                return std::u16string(s.begin(), s.end());
            }
            std::u32string u32str() const override {
                auto s = str();
                return std::u32string(s.begin(), s.end());
            }

            /**
             * @brief 获取存储值的类型信息。
             * @return typeid(T)。
             */
            const std::type_info &type() const override { return typeid(T); }
        };

        Base *ptr; ///< 指向实际存储数据的指针（堆上对象）。

    public:
        /// @brief 默认构造一个空 Item（ptr 为 nullptr）。
        Item() : ptr(nullptr) {}

        /**
         * @brief 从任意类型构造 Item（万能引用）。
         * @tparam T 推导出的数据类型（自动去除引用和 cv 限定）。
         * @param value 要存储的值（左值或右值）。
         */
        template <typename T>
        Item(T &&value) :
            ptr(new Derived<typename std::decay<T>::type>(
                std::forward<T>(value))) {}

        /**
         * @brief 拷贝构造函数。
         * @param other 源 Item。
         */
        Item(const Item &other) :
            ptr(other.ptr ? other.ptr->clone() : nullptr) {}

        /**
         * @brief 移动构造函数。
         * @param other 源 Item（将被置于空状态）。
         */
        Item(Item &&other) noexcept : ptr(other.ptr) { other.ptr = nullptr; }

        /**
         * @brief 类型安全地获取存储值的引用（非常量版本）。
         * @tparam T 期望的类型。
         * @return T& 存储值的引用。
         * @throw console::TypeError 若 Item 为空或实际类型与 T 不匹配。
         */
        template <typename T>
        T &get() {
            if (ptr == nullptr) throw TypeError("empty item");
            if (typeid(T) != ptr->type())
                throw TypeError(std::string("type mismatch: ")
                                + tiname(typeid(T)) + " and "
                                + tiname(ptr->type()));
            return ((Derived<T> *)ptr)->value;
        }

        /**
         * @brief 不安全地获取存储值的引用（非常量版本）。
         * @tparam T 期望的类型（必须与实际类型完全匹配，否则 UB）。
         * @return T& 存储值的引用。
         * @warning 不进行任何类型检查或空指针检查，可能导致未定义行为。
         */
        template <typename T>
        T &unsafe_get() {
            return ((Derived<T> *)ptr)->value;
        }

        /**
         * @brief 类型安全地获取存储值的引用（常量版本）。
         * @tparam T 期望的类型。
         * @return const T& 存储值的常量引用。
         * @throw console::TypeError 若 Item 为空或实际类型与 T 不匹配。
         */
        template <typename T>
        const T &get() const {
            if (ptr == nullptr) throw TypeError("empty item");
            if (typeid(T) != ptr->type())
                throw TypeError(std::string("type mismatch: ")
                                + typeid(T).name() + " and "
                                + ptr->type().name());
            return ((Derived<T> *)ptr)->value;
        }

        /**
         * @brief 不安全地获取存储值的引用（常量版本）。
         * @tparam T 期望的类型（必须与实际类型完全匹配，否则 UB）。
         * @return const T& 存储值的常量引用。
         * @warning 不进行任何类型检查或空指针检查，可能导致未定义行为。
         */
        template <typename T>
        const T &unsafe_get() const {
            return ((Derived<T> *)ptr)->value;
        }

        /**
         * @brief 拷贝赋值运算符。
         * @param other 源 Item。
         * @return const Item& 返回 *this 的常量引用。
         */
        const Item &operator=(const Item &other) {
            if (this != &other) {
                Base *new_ptr = other.ptr ? other.ptr->clone() : nullptr;
                delete ptr;
                ptr = new_ptr;
            }
            return *this;
        }

        /**
         * @brief 移动赋值运算符。
         * @param other 源 Item（将被置于空状态）。
         * @return const Item& 返回 *this 的常量引用。
         */
        const Item &operator=(Item &&other) noexcept {
            if (this != &other) {
                delete ptr;
                ptr       = other.ptr;
                other.ptr = nullptr;
            }
            return *this;
        }

        /**
         * @brief 将 Item 输出到流。
         * @param os 输出流。
         * @param item 要输出的 Item。
         * @return std::ostream& 返回 os 以便链式调用。
         */
        friend std::ostream &operator<<(std::ostream &os, const Item &item) {
            item.ptr->print(os);
            return os;
        }

        /**
         * @brief 将 Item 输出到宽字符流。
         * @param os 宽字符输出流。
         * @param item 要输出的 Item。
         * @return std::wostream& 返回 os 以便链式调用。
         */
        friend std::wostream &operator<<(std::wostream &os, const Item &item) {
            item.ptr->print(os);
            return os;
        }

        /**
         * @brief 将 Item 输出到 UTF-16 流。
         */
        friend std::basic_ostream<char16_t> &
        operator<<(std::basic_ostream<char16_t> &os, const Item &item) {
            item.ptr->print(os);
            return os;
        }

        /**
         * @brief 将 Item 输出到 UTF-32 流。
         */
        friend std::basic_ostream<char32_t> &
        operator<<(std::basic_ostream<char32_t> &os, const Item &item) {
            item.ptr->print(os);
            return os;
        }

        /**
         * @brief 返回 Item 的字符串表示。
         * @return std::string 字符串形式。
         */
        std::string    str() const { return ptr->str(); }
        std::wstring   wstr() const { return ptr->wstr(); }
        std::u16string u16str() const { return ptr->u16str(); }
        std::u32string u32str() const { return ptr->u32str(); }

        /// @brief 析构函数，释放内部堆内存。
        ~Item() { delete ptr; }
    };

    /**
     * @class Box
     * @brief 存储多个 Item 的异构容器。
     * @details 公有继承自 std::vector<Item>，支持从多个任意类型的值直接构造，
     *          并提供类型安全的元素访问和解包功能。
     */
    class Box : public std::vector<Item> {
    public:
        /**
         * @brief 从任意数量、任意类型的值构造 Box。
         * @tparam Args 参数包类型。
         * @param args 要存储的值（每个值都会被隐式转换为 Item）。
         */
        template <class... Args>
        Box(Args &&...args) :
            std::vector<Item>({Item(std::forward<Args>(args))...}) {}

        /**
         * @brief 类型安全地获取指定索引处的元素（非常量版本）。
         * @tparam T 期望的类型。
         * @param index 元素索引（从 0 开始）。
         * @return T& 元素的引用。
         * @throw console::TypeError 若索引越界或实际类型与 T 不匹配。
         */
        template <class T>
        T &get(size_t index) {
            return std::vector<Item>::at(index).get<T>();
        }

        /**
         * @brief 不安全地获取指定索引处的元素（非常量版本）。
         * @tparam T 期望的类型（必须与实际类型匹配，否则 UB）。
         * @param index 元素索引（从 0 开始）。
         * @return T& 元素的引用。
         * @warning 不检查索引越界和类型匹配，可能导致未定义行为。
         */
        template <typename T>
        T &unsafe_get(size_t index) {
            return std::vector<Item>::operator[](index).unsafe_get<T>();
        }

        /**
         * @brief 将 Box 中的元素按顺序解包到多个变量中（类型安全）。
         * @tparam Args 变量包类型。
         * @param args 要赋值的变量（按引用传递）。
         * @note 若元素数量或类型不匹配会抛出 TypeError 异常。
         */
        template <class... Args>
        void unpack(Args &...args) {
            size_t i   = 0;
            int    _[] = {0, ((args = get<Args>(i++)), 0)...};
            (void)_;
        }

        /**
         * @brief 将 Box 中的元素按顺序解包到多个变量中（不安全版本）。
         * @tparam Args 变量包类型。
         * @param args 要赋值的变量（按引用传递）。
         * @warning 不进行任何类型或边界检查，需确保 Box
         * 大小和类型与参数包完全匹配。
         */
        template <class... Args>
        void unsafe_unpack(Args &...args) {
            size_t i   = 0;
            int    _[] = {0, ((args = unsafe_get<Args>(i++)), 0)...};
            (void)_;
        }

        /**
         * @brief 将 Box 输出到流，格式类似于元组 (elem1, elem2, ...)。
         * @param os 输出流。
         * @param box 要输出的 Box。
         * @return std::ostream& 返回 os。
         */
        friend std::ostream &operator<<(std::ostream &os, const Box &box) {
            if (box.empty()) return os << "()";
            auto it = box.begin();
            os << '(' << *it;
            while (++it != box.end()) os << ", " << *it;
            return os << ')';
        }

        /**
         * @brief 将 Box 输出到宽字符流。
         * @param os 宽字符输出流。
         * @param box 要输出的 Box。
         * @return std::wostream& 返回 os。
         */
        friend std::wostream &operator<<(std::wostream &os, const Box &box) {
            if (box.empty()) return os << L"()";
            auto it = box.begin();
            os << L'(' << *it;
            while (++it != box.end()) os << L", " << *it;
            return os << L')';
        }

        /**
         * @brief 将 Box 输出到 UTF-16 流。
         */
        friend std::basic_ostream<char16_t> &
        operator<<(std::basic_ostream<char16_t> &os, const Box &box) {
            if (box.empty()) return os << u"()";
            auto it = box.begin();
            os << u'(' << *it;
            while (++it != box.end()) os << u", " << *it;
            return os << u')';
        }

        /**
         * @brief 将 Box 输出到 UTF-32 流。
         */
        friend std::basic_ostream<char32_t> &
        operator<<(std::basic_ostream<char32_t> &os, const Box &box) {
            if (box.empty()) return os << U"()";
            auto it = box.begin();
            os << U'(' << *it;
            while (++it != box.end()) os << U", " << *it;
            return os << U')';
        }
    };
}
