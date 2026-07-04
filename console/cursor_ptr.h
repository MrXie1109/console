/**
 * @file cursor_ptr.h
 * @brief 提供双指针游标智能指针，分离所有权与访问位置。
 * @details 该类维护两个指针：original_ptr 持有内存所有权，current_ptr
 * 表示当前访问位置。 支持指针算术运算，复制时不转移所有权，移动时转移所有权。
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
#include <cstddef>

namespace console {
    /**
     * @class cursor_ptr
     * @brief 双指针游标智能指针（非数组版本）。
     * @tparam T 元素类型（非数组）。
     * @details 通过 original_ptr 管理内存生命周期，current_ptr 支持指针运算。
     *          复制构造/赋值仅复制游标位置而不转移所有权，移动构造/赋值转移所有权。
     * @warning 复制赋值要求源对象的生命周期覆盖目标对象的使用期。
     * @warning 指针运算不进行边界检查。
     * @warning 需确保 delete 与 new 匹配（本版本使用 delete）。
     */
    template <class T> class cursor_ptr {
        T *original_ptr; ///< 持有内存所有权，析构时释放
        T *current_ptr; ///< 当前游标位置，支持算术运算

    public:
        /// @brief 默认构造，两个指针均为 nullptr。
        cursor_ptr() : original_ptr(nullptr), current_ptr(nullptr) {}

        /**
         * @brief 从原始指针构造，获取所有权。
         * @param p 指向动态分配内存的指针（使用 new）。
         * @note 当前游标位置初始化为 p。
         */
        cursor_ptr(T *p) : original_ptr(p), current_ptr(p) {}

        /**
         * @brief 拷贝构造，仅复制游标位置，不转移所有权。
         * @param cp 源对象。
         * @note 新对象的 original_ptr 为 nullptr，不参与内存管理。
         */
        cursor_ptr(const cursor_ptr &cp) :
            original_ptr(nullptr), current_ptr(cp.current_ptr) {}

        /**
         * @brief 移动构造，转移所有权。
         * @param cp 源对象，移动后其 original_ptr 置为 nullptr，current_ptr
         * 保持不变。
         */
        cursor_ptr(cursor_ptr &&cp) :
            original_ptr(cp.original_ptr), current_ptr(cp.current_ptr) {
            cp.original_ptr = nullptr;
        }

        /// @name 解引用与元素访问
        /// @{
        T &operator*() { return *current_ptr; }
        T *operator->() { return current_ptr; }
        T &operator[](size_t i) { return *(current_ptr + i); }
        T *get() { return current_ptr; }
        operator T *() { return current_ptr; }
        /// @}

        /// @name const 版本解引用与元素访问
        /// @{
        const T &operator*() const { return *current_ptr; }
        const T *operator->() const { return current_ptr; }
        const T &operator[](size_t i) const { return *(current_ptr + i); }
        const T *get() const { return current_ptr; }
        operator const T *() const { return current_ptr; }
        /// @}

        /// @brief 析构，释放 original_ptr 指向的内存（使用 delete）。
        ~cursor_ptr() { delete original_ptr; }

        /// @name 自增/自减运算符
        /// @{
        cursor_ptr &operator++() {
            ++current_ptr;
            return *this;
        }
        cursor_ptr operator++(int) {
            cursor_ptr tmp(*this);
            ++current_ptr;
            return tmp;
        }
        cursor_ptr &operator--() {
            --current_ptr;
            return *this;
        }
        cursor_ptr operator--(int) {
            cursor_ptr tmp(*this);
            --current_ptr;
            return tmp;
        }
        /// @}

        /// @name 指针算术运算
        /// @{
        cursor_ptr operator+(int sep) const {
            cursor_ptr cp;
            cp.current_ptr = current_ptr + sep;
            return cp;
        }
        cursor_ptr operator-(int sep) const {
            cursor_ptr cp;
            cp.current_ptr = current_ptr - sep;
            return cp;
        }
        const cursor_ptr &operator+=(int sep) {
            current_ptr += sep;
            return *this;
        }
        const cursor_ptr &operator-=(int sep) {
            current_ptr -= sep;
            return *this;
        }
        int operator-(const cursor_ptr &cp) const {
            return current_ptr - cp.current_ptr;
        }
        /// @}

        /// @name 相等/不等比较
        /// @{
        bool operator==(const cursor_ptr &cp) const {
            return current_ptr == cp.current_ptr;
        }
        bool        operator==(const T *p) const { return current_ptr == p; }
        friend bool operator==(const T *p, const cursor_ptr &cp) {
            return cp == p;
        }
        bool operator!=(const cursor_ptr &cp) const {
            return current_ptr != cp.current_ptr;
        }
        bool        operator!=(const T *p) const { return current_ptr != p; }
        friend bool operator!=(const T *p, const cursor_ptr &cp) {
            return cp != p;
        }
        /// @}

        /// @name 赋值运算符
        /// @{
        /**
         * @brief 从原始指针赋值，释放原有内存并接管新指针。
         * @param p 新指针（使用 new 分配）。
         * @return const cursor_ptr& *this。
         */
        const cursor_ptr &operator=(T *p) {
            if (original_ptr == p) return *this;
            delete original_ptr;
            original_ptr = p;
            current_ptr  = p;
            return *this;
        }

        /**
         * @brief 拷贝赋值，仅复制游标位置，不转移所有权。
         * @param cp 源对象。
         * @return const cursor_ptr& *this。
         */
        const cursor_ptr &operator=(const cursor_ptr &cp) {
            if (this == &cp) return *this;
            delete original_ptr;
            original_ptr = nullptr;
            current_ptr  = cp.current_ptr;
            return *this;
        }

        /**
         * @brief 移动赋值，转移所有权。
         * @param cp 源对象。
         * @return const cursor_ptr& *this。
         */
        const cursor_ptr &operator=(cursor_ptr &&cp) {
            if (this == &cp) return *this;
            delete original_ptr;
            current_ptr     = cp.current_ptr;
            original_ptr    = cp.original_ptr;
            cp.original_ptr = nullptr;
            return *this;
        }
        /// @}

        /**
         * @brief 交换两个 cursor_ptr 的内容（包括所有者和游标）。
         * @param cp 要交换的对象。
         */
        void swap(cursor_ptr &cp) {
            T *p1 = current_ptr, *p2 = original_ptr;
            current_ptr     = cp.current_ptr;
            original_ptr    = cp.original_ptr;
            cp.current_ptr  = p1;
            cp.original_ptr = p2;
        }
    };

    /**
     * @class cursor_ptr<T[]>
     * @brief 双指针游标智能指针（数组版本）。
     * @tparam T 数组元素类型。
     * @details 与非数组版本的区别在于析构使用 delete[]，赋值时使用 delete[]
     * 释放原内存。
     * @warning 需确保与 new[] 匹配。
     */
    template <class T> class cursor_ptr<T[]> {
        T *original_ptr;
        T *current_ptr;

    public:
        /// @brief 默认构造。
        cursor_ptr() : original_ptr(nullptr), current_ptr(nullptr) {}

        /**
         * @brief 从原始数组指针构造，获取所有权。
         * @param p 指向动态分配的数组（使用 new[]）。
         */
        cursor_ptr(T *p) : original_ptr(p), current_ptr(p) {}

        /**
         * @brief 拷贝构造，仅复制游标位置。
         * @param cp 源对象。
         */
        cursor_ptr(const cursor_ptr &cp) :
            original_ptr(nullptr), current_ptr(cp.current_ptr) {}

        /**
         * @brief 移动构造，转移所有权。
         * @param cp 源对象。
         */
        cursor_ptr(cursor_ptr &&cp) :
            original_ptr(cp.original_ptr), current_ptr(cp.current_ptr) {
            cp.original_ptr = nullptr;
        }

        /// @name 解引用与元素访问
        /// @{
        T &operator*() { return *current_ptr; }
        T *operator->() { return current_ptr; }
        T &operator[](size_t i) { return *(current_ptr + i); }
        T *get() { return current_ptr; }
        operator T *() { return current_ptr; }
        /// @}

        /// @name const 版本
        /// @{
        const T &operator*() const { return *current_ptr; }
        const T *operator->() const { return current_ptr; }
        const T &operator[](size_t i) const { return *(current_ptr + i); }
        const T *get() const { return current_ptr; }
        operator const T *() const { return current_ptr; }
        /// @}

        /// @brief 析构，释放数组内存（使用 delete[]）。
        ~cursor_ptr() { delete[] original_ptr; }

        /// @name 自增/自减
        /// @{
        cursor_ptr &operator++() {
            ++current_ptr;
            return *this;
        }
        cursor_ptr operator++(int) {
            cursor_ptr tmp(*this);
            ++current_ptr;
            return tmp;
        }
        cursor_ptr &operator--() {
            --current_ptr;
            return *this;
        }
        cursor_ptr operator--(int) {
            cursor_ptr tmp(*this);
            --current_ptr;
            return tmp;
        }
        /// @}

        /// @name 指针算术
        /// @{
        cursor_ptr operator+(int sep) const {
            cursor_ptr cp;
            cp.current_ptr = current_ptr + sep;
            return cp;
        }
        cursor_ptr operator-(int sep) const {
            cursor_ptr cp;
            cp.current_ptr = current_ptr - sep;
            return cp;
        }
        const cursor_ptr &operator+=(int sep) {
            current_ptr += sep;
            return *this;
        }
        const cursor_ptr &operator-=(int sep) {
            current_ptr -= sep;
            return *this;
        }
        int operator-(const cursor_ptr &cp) const {
            return current_ptr - cp.current_ptr;
        }
        /// @}

        /// @name 比较
        /// @{
        bool operator==(const cursor_ptr &cp) const {
            return current_ptr == cp.current_ptr;
        }
        bool        operator==(const T *p) const { return current_ptr == p; }
        friend bool operator==(const T *p, const cursor_ptr &cp) {
            return cp == p;
        }
        bool operator!=(const cursor_ptr &cp) const {
            return current_ptr != cp.current_ptr;
        }
        bool        operator!=(const T *p) const { return current_ptr != p; }
        friend bool operator!=(const T *p, const cursor_ptr &cp) {
            return cp != p;
        }
        /// @}

        /// @name 赋值
        /// @{
        const cursor_ptr &operator=(T *p) {
            if (original_ptr == p) return *this;
            delete[] original_ptr;
            original_ptr = p;
            current_ptr  = p;
            return *this;
        }
        const cursor_ptr &operator=(const cursor_ptr &cp) {
            if (this == &cp) return *this;
            delete[] original_ptr;
            original_ptr = nullptr;
            current_ptr  = cp.current_ptr;
            return *this;
        }
        const cursor_ptr &operator=(cursor_ptr &&cp) {
            if (this == &cp) return *this;
            delete[] original_ptr;
            current_ptr     = cp.current_ptr;
            original_ptr    = cp.original_ptr;
            cp.original_ptr = nullptr;
            return *this;
        }
        /// @}

        /**
         * @brief 交换两个 cursor_ptr 的内容。
         * @param cp 要交换的对象。
         */
        void swap(cursor_ptr &cp) {
            T *p1 = current_ptr, *p2 = original_ptr;
            current_ptr     = cp.current_ptr;
            original_ptr    = cp.original_ptr;
            cp.current_ptr  = p1;
            cp.original_ptr = p2;
        }
    };
}
