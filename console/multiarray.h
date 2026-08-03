/**
 * @file multiarray.h
 * @brief 提供编译期维度固定的多维数组容器
 * MultiArray，支持任意维度和元素级算术/逻辑/位运算。
 * @details MultiArray 使用堆内存存储，支持视图语义、
 *          扁平化迭代器 fbegin/fend、元素级运算(包括数组与数组、数组与标量)、
 *          统计函数(sum/min/max/all/any)等。
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
#include <memory>
#include <stdexcept>
#include <type_traits>

#include "csexc.h"
#include "repr.h"

namespace console {
#ifdef CONSOLE_MULTIARRAY_V1
    inline
#endif
        namespace _V1 {
        /**
         * @class MultiArray
         * @brief 编译期固定维度的多维数组。
         * @tparam T 元素类型。
         * @tparam Dims 各维度大小(可变参数，至少一个维度)。
         * @details 递归继承自 std::array，
         *          提供 operator() 进行多维下标访问(带边界检查)，
         *          提供 flatten 迭代器 fbegin/fend，
         *          支持元素级运算和常用统计函数。
         * @note 维度信息在编译期确定，所有元素连续存储于内存中。
         */
        template <class T, size_t... Dims>
        class MultiArray;

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

            /// @brief 默认构造，元素未初始化(基本类型为随机值)。
            MultiArray() = default;

            /**
             * @brief 用相同值填充所有元素。
             * @param value 填充值。
             */
            explicit MultiArray(const T &value) { fill(value); }

            /// @brief 从 std::array 拷贝构造。
            MultiArray(const base_type &other) : base_type(other) {}

            /**
             * @brief 从初始化列表构造(仅一维)。
             * @param init 初始化列表，长度不应超过 D。
             */
            MultiArray(std::initializer_list<T> init) {
                std::copy(init.begin(), init.end(), this->begin());
            }

            /// @brief 返回数组的维数(一维时为 1)。
            static constexpr size_t rank() { return 1; }

            /// @brief 返回元素总数。
            static constexpr size_t fsize() { return D; }

            /// @brief 用给定值填充所有元素。
            void fill(const T &value) {
                for (auto &item : *this) item = value;
            }

            /**
             * @brief 遍历所有元素并应用函数(非常量版本)。
             * @tparam F 可调用对象，接受 T&。
             * @param visit 函数对象。
             */
            template <class F>
            void for_each(F &&visit) {
                for (auto &item : *this) visit(item);
            }

            /**
             * @brief 遍历所有元素并应用函数(常量版本)。
             * @tparam F 可调用对象，接受 const T&。
             * @param visit 函数对象。
             */
            template <class F>
            void for_each(F &&visit) const {
                for (const auto &item : *this) visit(item);
            }

            /**
             * @brief 带边界检查的下标访问(一维)。
             * @param i 索引。
             * @return T& 元素引用。
             * @throw MultiArrayError 若索引越界。
             */
            reference operator()(size_t i) {
                if (i >= D)
                    throw MultiArrayError("index " + std::to_string(i)
                                          + " out of range [0, "
                                          + std::to_string(D) + ')');
                return (*this)[i];
            }

            /// @brief 常量版本。
            const_reference operator()(size_t i) const {
                if (i >= D)
                    throw MultiArrayError("index " + std::to_string(i)
                                          + " out of range [0, "
                                          + std::to_string(D) + ')');
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

            /// @brief 返回指向扁平化数据起始的指针(非常量)。
            T *fbegin() { return this->data(); }

            /// @brief 常量版本。
            const T *fbegin() const { return this->data(); }

            /// @brief 返回指向扁平化数据末尾的指针(非常量)。
            T *fend() { return this->data() + fsize(); }

            /// @brief 常量版本。
            const T *fend() const { return this->data() + fsize(); }

            /**
             * @brief 扁平化视图(可变)。
             * @note 其实就是它自己。
             */
            MultiArray &flatten() { return *this; }

            /**
             * @brief 扁平化视图(常量)。
             * @note 其实就是它自己。
             */
            const MultiArray &flatten() const { return *this; }

            /**
             * @brief 返回包含维度信息的数组。
             * @return std::array 包含数组各维度的大小，长度为 1。
             */
            static constexpr std::array<size_t, 1> dims() { return {D}; }

            /**
             * @brief 重新调整数组的形状。
             * @tparam Dims 新的维度大小。
             * @return 重新调整形状后的数组。
             */
            template <size_t... Dims>
            MultiArray<T, Dims...> &reshape() {
                static_assert(MultiArray<T, Dims...>::fsize() == fsize(),
                    "reshape: total element count must match");
                return *reinterpret_cast<MultiArray<T, Dims...> *>(this);
            }

            /**
             * @brief 重新调整数组的形状(const版本)。
             * @tparam Dims 新的维度大小。
             * @return 重新调整形状后的数组。
             */
            template <size_t... Dims>
            const MultiArray<T, Dims...> &reshape() const {
                static_assert(MultiArray<T, Dims...>::fsize() == fsize(),
                    "reshape: total element count must match");
                return *reinterpret_cast<const MultiArray<T, Dims...> *>(this);
            }
        };

        /**
         * @brief 多维特化(维度 >= 2)。
         * @tparam T 元素类型。
         * @tparam First 第一维大小。
         * @tparam Rest 剩余维度大小。
         */
        template <class T, size_t First, size_t... Rest>
        class MultiArray<T, First, Rest...>
            : public std::array<MultiArray<T, Rest...>, First> {
            using base_type = std::array<MultiArray<T, Rest...>, First>;

        public:
            using value_type      = MultiArray<T, Rest...>;
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
             * @brief 从初始化列表构造(多维)。
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
             * @brief 遍历所有元素(非常量版本)。
             * @tparam F 可调用对象，接受 T&。
             */
            template <class F>
            void for_each(F &&visit) {
                for (auto &sub : *this) sub.for_each(visit);
            }

            /**
             * @brief 遍历所有元素(常量版本)。
             * @tparam F 可调用对象，接受 const T&。
             */
            template <class F>
            void for_each(F &&visit) const {
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
             * @return 最终元素的引用(若索引数量等于维数)或子数组引用(若不足)。
             */
            template <class... Indices>
            auto operator()(size_t idx, Indices... rest)
                -> decltype(base_type::operator[](idx)(rest...)) {
                static_assert(
                    sizeof...(Indices) < rank(), "Too Many Arguments!");
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
                static_assert(
                    sizeof...(Indices) < rank(), "Too Many Arguments!");
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

            /// @brief 扁平化起始迭代器(非常量)。
            T *fbegin() { return this->data()->fbegin(); }

            /// @brief 常量版本。
            const T *fbegin() const { return this->data()->fbegin(); }

            /// @brief 扁平化结束迭代器(非常量)。
            T *fend() { return this->fbegin() + this->fsize(); }

            /// @brief 常量版本。
            const T *fend() const { return this->fbegin() + this->fsize(); }

            /**
             * @brief 扁平化视图(可变)。
             * @note 这并非不安全，可以自行思考。
             */
            MultiArray<T, fsize()> &flatten() {
                return reinterpret_cast<MultiArray<T, fsize()> &>(*this);
            }

            /**
             * @brief 扁平化视图(常量)。
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

            /**
             * @brief 重新调整数组的形状。
             * @tparam Dims 新的维度大小。
             * @return 重新调整形状后的数组。
             */
            template <size_t... Dims>
            MultiArray<T, Dims...> &reshape() {
                static_assert(MultiArray<T, Dims...>::fsize() == fsize(),
                    "reshape: total element count must match");
                return *reinterpret_cast<MultiArray<T, Dims...> *>(this);
            }

            /**
             * @brief 重新调整数组的形状(const版本)。
             * @tparam Dims 新的维度大小。
             * @return 重新调整形状后的数组。
             */
            template <size_t... Dims>
            const MultiArray<T, Dims...> &reshape() const {
                static_assert(MultiArray<T, Dims...>::fsize() == fsize(),
                    "reshape: total element count must match");
                return *reinterpret_cast<const MultiArray<T, Dims...> *>(this);
            }
        };
    }

#ifndef CONSOLE_MULTIARRAY_V1
    inline
#endif
        namespace _V2 {
        /**
         * @class MultiArray
         * @brief 编译期固定维度的多维数组(堆版本)。
         * @tparam T 元素类型。
         * @tparam Dims 各维度大小(可变参数，至少一个维度)。
         * @details
         * MultiArray 使用堆内存存储，支持视图语义：
         * - 默认构造分配堆内存，对象拥有数据；
         * - 子数组访问返回视图(浅拷贝)，共享同一份数据；
         * - 拷贝构造深拷贝，返回独立拥有者；
         * - 移动构造转移所有权，源对象变为空视图。
         * @note 维度信息在编译期确定，所有元素连续存储于堆内存中。
         *       视图是轻量级对象(仅包含指针和布尔标志)，可高效传递。
         */
        template <class T, size_t... Dims>
        class MultiArray;

        /**
         * @brief 一维特化。
         * @tparam T 元素类型。
         * @tparam D 维度大小。
         * @note 一维数组不产生子数组视图，operator[] 直接返回元素引用。
         */
        template <class T, size_t D>
        class MultiArray<T, D> {
            T   *data_;     ///< 指向堆内存数据的指针
            bool is_views_; ///< true 表示视图(不拥有数据)，false 表示拥有者

            static constexpr size_t SIZE = D; ///< 元素总数

            /// @brief 私有构造函数，仅供视图创建使用。
            MultiArray(T *data, bool is_views) :
                data_(data), is_views_(is_views) {}
            /// @brief 私有构造函数，const 版本，仅供 const 视图使用。
            MultiArray(const T *data, bool is_views) :
                data_(const_cast<T *>(data)), is_views_(is_views) {}

            template <class U, size_t... Dims>
            friend class MultiArray;

        public:
            using value_type       = T;
            using size_type        = size_t;
            using difference_type  = ptrdiff_t;
            using pointer          = value_type *;
            using const_pointer    = const value_type *;
            using reference        = value_type &;
            using const_reference  = const value_type &;
            using iterator         = value_type *;
            using const_iterator   = const value_type *;
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator
                = std::reverse_iterator<const_iterator>;

            /// @brief 默认构造，分配堆内存，元素未初始化(基本类型为随机值)。
            MultiArray() : data_(new T[SIZE]), is_views_(false) {}

            /**
             * @brief 用相同值填充所有元素。
             * @param value 填充值。
             */
            MultiArray(const T &value) : data_(new T[SIZE]), is_views_(false) {
                std::fill(data_, data_ + SIZE, value);
            }

            /**
             * @brief 从初始化列表构造(一维)。
             * @param init 初始化列表，长度必须等于 D。
             */
            MultiArray(std::initializer_list<T> init) :
                data_(new T[SIZE]), is_views_(false) {
                std::copy(init.begin(), init.end(), data_);
            }

            /**
             * @brief 拷贝构造，深拷贝，产生独立拥有者。
             * @param other 源数组。
             */
            MultiArray(const MultiArray &other) :
                data_(new T[SIZE]), is_views_(false) {
                std::copy(other.data_, other.data_ + SIZE, data_);
            }

            /**
             * @brief 移动构造，窃取资源，源对象变为视图。
             * @param other 源数组。
             * @note 移动后 other 处于"有效但未指定"状态，仅可析构或赋值。
             */
            MultiArray(MultiArray &&other) noexcept :
                data_(other.data_), is_views_(other.is_views_) {
                other.is_views_ = true;
            }

            /**
             * @brief 拷贝赋值，深拷贝，目标变为拥有者。
             * @param other 源数组。
             * @return *this。
             */
            MultiArray &operator=(const MultiArray &other) {
                if (this != &other) {
                    if (!is_views_) delete[] data_;
                    data_ = new T[SIZE];
                    std::copy(other.data_, other.data_ + SIZE, data_);
                }
                return *this;
            }

            /**
             * @brief 移动赋值，窃取资源，目标变为拥有者。
             * @param other 源数组。
             * @return *this。
             * @note 移动后 other 处于"有效但未指定"状态。
             */
            MultiArray &operator=(MultiArray &&other) noexcept {
                if (this != &other) {
                    if (!is_views_) delete[] data_;
                    data_       = other.data_;
                    is_views_   = other.is_views_;
                    other.data_ = nullptr;
                }
                return *this;
            }

            /**
             * @brief 显式深拷贝，返回独立拥有者。
             * @return MultiArray 独立副本。
             */
            MultiArray clone() const { return MultiArray(*this); }

            /// @brief 析构，拥有者释放堆内存，视图不释放。
            ~MultiArray() {
                if (!is_views_) delete[] data_;
            }

            /// @brief 迭代器 begin，返回指向首元素的迭代器。
            iterator begin() { //
                return iterator(data_);
            }
            /// @brief 迭代器 end，返回指向尾元素的迭代器。
            iterator end() { //
                return iterator(data_ + SIZE);
            }
            /// @brief const迭代器 begin，返回指向首元素的 const 迭代器。
            const_iterator begin() const { //
                return const_iterator(data_);
            }
            /// @brief const迭代器 end，返回指向尾元素的 const 迭代器。
            const_iterator end() const { //
                return const_iterator(data_ + SIZE);
            }
            /// @brief 反向迭代器 rbegin，返回指向尾元素的反向迭代器。
            reverse_iterator rbegin() { //
                return reverse_iterator(data_ + SIZE);
            }
            /// @brief 反向迭代器 rend，返回指向首元素的反向迭代器。
            reverse_iterator rend() { //
                return reverse_iterator(data_);
            }
            /// @brief const 反向迭代器 rbegin，返回指向尾元素的 const 反向迭代器。
            const_reverse_iterator rbegin() const {
                return const_reverse_iterator(data_ + SIZE);
            }
            /// @brief const 反向迭代器 rend，返回指向首元素的 const 反向迭代器。
            const_reverse_iterator rend() const {
                return const_reverse_iterator(data_);
            }
            /// @brief const 迭代器 cbegin，返回指向首元素的 const 迭代器。
            const_iterator cbegin() const { //
                return const_iterator(data_);
            }
            /// @brief const 迭代器 cend，返回指向尾元素的 const 迭代器。
            const_iterator cend() const { //
                return const_iterator(data_ + SIZE);
            }
            /// @brief const 反向迭代器 crbegin，返回指向尾元素的 const 反向迭代器。
            const_reverse_iterator crbegin() const {
                return const_reverse_iterator(data_ + SIZE);
            }
            /// @brief const 反向迭代器 crend，返回指向首元素的 const 反向迭代器。
            const_reverse_iterator crend() const {
                return const_reverse_iterator(data_);
            }

            /// @brief 扁平化迭代器，返回指向首元素的指针。
            T *fbegin() { return data_; }
            /// @brief 扁平化迭代器，返回指向首元素的 const 指针。
            const T *fbegin() const { return data_; }
            /// @brief 扁平化迭代器，返回指向尾元素的指针。
            T *fend() { return data_ + SIZE; }
            /// @brief 扁平化迭代器，返回指向尾元素的 const 指针。
            const T *fend() const { return data_ + SIZE; }

            /// @brief 维度，返回 1。
            static constexpr size_t rank() { return 1; }
            /// @brief 维度数组，返回 {D}。
            static constexpr std::array<size_t, rank()> dims() { return {D}; }
            /// @brief 扁平化大小，返回 SIZE。
            static constexpr size_t fsize() { return SIZE; }
            /// @brief 扁平化是否为空，返回 SIZE == 0。
            static constexpr bool fempty() { return SIZE == 0; }
            /// @brief 扁平化最大大小，返回 SIZE。
            static constexpr size_t fmax_size() { return SIZE; }

            /// @brief 扁平化视图，一维数组返回自身。
            MultiArray<T, SIZE> &flatten() { return *this; }

            /// @brief 扁平化视图，一维数组返回自身。
            const MultiArray<T, SIZE> &flatten() const { return *this; }

            /// @brief 下标运算符，返回指向 index 位置的引用。
            reference operator[](size_t index) { //
                return data_[index];
            }
            /// @brief 下标运算符，返回指向 index 位置的 const 引用。
            const_reference operator[](size_t index) const {
                return data_[index];
            }

            /**
             * @brief 带边界检查的下标访问。
             * @param index 索引。
             * @return T& 元素引用。
             * @throw MultiArrayError 若索引越界。
             */
            reference at(size_t index) {
                if (index >= D)
                    throw MultiArrayError("index " + std::to_string(index)
                                          + " out of range [0, "
                                          + std::to_string(D) + ')');
                return data_[index];
            }
            /**
             * @brief 带边界检查的下标访问。
             * @param index 索引。
             * @return const T& 元素引用。
             * @throw MultiArrayError 若索引越界。
             */
            const_reference at(size_t index) const {
                if (index >= D)
                    throw MultiArrayError("index " + std::to_string(index)
                                          + " out of range [0, "
                                          + std::to_string(D) + ')');
                return data_[index];
            }

            /**
             * @brief 带边界检查的下标访问。
             * @param index 索引。
             * @return T& 元素引用。
             * @throw MultiArrayError 若索引越界。
             */
            reference operator()(size_t index) { return at(index); }
            /**
             * @brief 带边界检查的下标访问。
             * @param index 索引。
             * @return const T& 元素引用。
             * @throw MultiArrayError 若索引越界。
             */
            const_reference operator()(size_t index) const { return at(index); }

            /// @brief 访问第一个元素。
            reference front() { return operator[](0); }
            /// @brief 访问第一个元素。
            const_reference front() const { return operator[](0); }
            /// @brief 访问最后一个元素。
            reference back() { return operator[](D - 1); }
            /// @brief 访问最后一个元素。
            const_reference back() const { return operator[](D - 1); }

            /// @brief 第一维元素数量，返回 D。
            static size_t size() { return D; }
            /// @brief 第一维是否为空，返回 D == 0。
            static bool empty() { return D == 0; }
            /// @brief 第一维最大大小，返回 D。
            static size_t max_size() { return D; }

            /**
             * @brief 流输出，格式为嵌套方括号。
             * @tparam CharT 字符类型。
             * @tparam Traits 字符特征。
             */
            template <class CharT, class Traits>
            friend std::basic_ostream<CharT, Traits> &operator<<(
                std::basic_ostream<CharT, Traits> &os, const MultiArray &ma) {
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

            /**
             * @brief 用给定值填充所有元素。
             * @param value 填充值。
             */
            void fill(const T &value) { std::fill(data_, data_ + SIZE, value); }

            /**
             * @brief 遍历所有元素并应用函数。
             * @tparam F 可调用对象，接受 T&。
             * @param f 函数对象。
             */
            template <class F>
            void for_each(F f) {
                std::for_each(data_, data_ + SIZE, f);
            }

            /**
             * @brief 遍历所有元素并应用函数(const 版本)。
             * @tparam F 可调用对象，接受 const T&。
             * @param f 函数对象。
             */
            template <class F>
            void for_each(F f) const {
                std::for_each(data_, data_ + SIZE, f);
            }

            /**
             * @brief 交换两个数组的内容。
             * @param other 要交换的数组。
             */
            void swap(MultiArray &other) noexcept {
                std::swap(data_, other.data_);
                std::swap(is_views_, other.is_views_);
            }

            /**
             * @brief 交换两个数组的内容。
             * @param other 要交换的数组。
             */
            friend void swap(MultiArray &a, MultiArray &b) noexcept {
                a.swap(b);
            }

            /**
             * @brief 重新调整数组的形状。
             * @tparam Dims 新的维度大小。
             * @return 重新调整形状后的数组。
             */
            template <size_t... Dims>
            MultiArray<T, Dims...> &reshape() {
                static_assert(MultiArray<T, Dims...>::fsize() == fsize(),
                    "reshape: total element count must match");
                return *reinterpret_cast<MultiArray<T, Dims...> *>(this);
            }

            /**
             * @brief 重新调整数组的形状(const版本)。
             * @tparam Dims 新的维度大小。
             * @return 重新调整形状后的数组。
             */
            template <size_t... Dims>
            const MultiArray<T, Dims...> &reshape() const {
                static_assert(MultiArray<T, Dims...>::fsize() == fsize(),
                    "reshape: total element count must match");
                return *reinterpret_cast<const MultiArray<T, Dims...> *>(this);
            }
        };

        /**
         * @brief 多维特化(维度 >= 2)。
         * @tparam T 元素类型。
         * @tparam First 第一维大小。
         * @tparam Rest 剩余维度大小。
         * @details 多维数组的子数组访问返回视图。
         */
        template <class T, size_t First, size_t... Rest>
        class MultiArray<T, First, Rest...> {
            T   *data_;     ///< 指向堆内存数据的指针
            bool is_views_; ///< true 表示视图(不拥有数据)，false 表示拥有者

            /// @brief 编译期计算总元素数。
            template <size_t... Is>
            struct prod;

            template <size_t I>
            struct prod<I> {
                static constexpr size_t value = I;
            };

            template <size_t I, size_t... Is>
            struct prod<I, Is...> {
                static constexpr size_t value = I * prod<Is...>::value;
            };

            static constexpr size_t SIZE = prod<First, Rest...>::value;

            /// @brief 私有构造函数，仅供视图创建使用。
            MultiArray(T *data, bool is_views) :
                data_(data), is_views_(is_views) {}
            /// @brief 私有构造函数，const 版本。
            MultiArray(const T *data, bool is_views) :
                data_(const_cast<T *>(data)), is_views_(is_views) {}

            template <class U, size_t... Dims>
            friend class MultiArray;

            friend class iterator;
            friend class const_iterator;

        public:
            /**
             * @brief 随机访问迭代器，遍历第一维的子数组。
             * @details 迭代器解引用时构造临时视图，不指向真实存在的对象。
             *          operator-> 返回 shared_ptr<value_type> 以确保生命周期。
             */
            class iterator {
                T *ptr_;

            public:
                using iterator_category = std::random_access_iterator_tag;
                using value_type        = MultiArray<T, Rest...>;
                using difference_type   = ptrdiff_t;
                using pointer           = std::shared_ptr<value_type>;
                using reference         = value_type;
                explicit iterator(T *ptr) : ptr_(ptr) {}
                reference operator*() const { return reference(ptr_, true); }
                pointer   operator->() const {
                    return pointer(new value_type(ptr_, true));
                }
                iterator &operator++() {
                    ptr_ += SIZE / First;
                    return *this;
                }
                iterator operator++(int) {
                    auto tmp = *this;
                    ++*this;
                    return tmp;
                }
                iterator &operator--() {
                    ptr_ -= SIZE / First;
                    return *this;
                }
                iterator operator--(int) {
                    auto tmp = *this;
                    --*this;
                    return tmp;
                }
                reference operator[](difference_type n) const {
                    return reference(ptr_ + SIZE / First * n, true);
                }
                friend bool operator==(const iterator &a, const iterator &b) {
                    return a.ptr_ == b.ptr_;
                }
                friend bool operator!=(const iterator &a, const iterator &b) {
                    return a.ptr_ != b.ptr_;
                }
                friend bool operator<(const iterator &a, const iterator &b) {
                    return a.ptr_ < b.ptr_;
                }
                friend difference_type
                operator-(const iterator &a, const iterator &b) {
                    return (a.ptr_ - b.ptr_) / (SIZE / First);
                }
                friend iterator operator+(iterator it, difference_type n) {
                    it.ptr_ += n * (SIZE / First);
                    return it;
                }
                friend iterator operator+(difference_type n, iterator it) {
                    it.ptr_ += n * (SIZE / First);
                    return it;
                }
                iterator &operator+=(difference_type n) {
                    ptr_ += n * (SIZE / First);
                    return *this;
                }
                iterator &operator-=(difference_type n) {
                    ptr_ -= n * (SIZE / First);
                    return *this;
                }
            };

            /**
             * @brief const 随机访问迭代器。
             * @details 与 iterator 类似，但解引用返回 const 视图。
             */
            class const_iterator {
                const T *ptr_;

            public:
                using iterator_category = std::random_access_iterator_tag;
                using value_type        = const MultiArray<T, Rest...>;
                using difference_type   = ptrdiff_t;
                using pointer           = std::shared_ptr<value_type>;
                using reference         = value_type;
                explicit const_iterator(const T *ptr) : ptr_(ptr) {}
                reference operator*() const { return reference(ptr_, true); }
                pointer   operator->() const {
                    return pointer(new value_type(ptr_, true));
                }
                const_iterator &operator++() {
                    ptr_ += SIZE / First;
                    return *this;
                }
                const_iterator operator++(int) {
                    auto tmp = *this;
                    ++*this;
                    return tmp;
                }
                const_iterator &operator--() {
                    ptr_ -= SIZE / First;
                    return *this;
                }
                const_iterator operator--(int) {
                    auto tmp = *this;
                    --*this;
                    return tmp;
                }
                reference operator[](difference_type n) const {
                    return reference(ptr_ + SIZE / First * n, true);
                }
                friend bool
                operator==(const const_iterator &a, const const_iterator &b) {
                    return a.ptr_ == b.ptr_;
                }
                friend bool
                operator!=(const const_iterator &a, const const_iterator &b) {
                    return a.ptr_ != b.ptr_;
                }
                friend bool
                operator<(const const_iterator &a, const const_iterator &b) {
                    return a.ptr_ < b.ptr_;
                }
                friend difference_type
                operator-(const const_iterator &a, const const_iterator &b) {
                    return (a.ptr_ - b.ptr_) / (SIZE / First);
                }
                friend const_iterator
                operator+(const_iterator it, difference_type n) {
                    it.ptr_ += n * (SIZE / First);
                    return it;
                }
                friend const_iterator
                operator+(difference_type n, iterator it) {
                    it.ptr_ += n * (SIZE / First);
                    return it;
                }
                const_iterator &operator+=(difference_type n) {
                    ptr_ += n * (SIZE / First);
                    return *this;
                }
                const_iterator &operator-=(difference_type n) {
                    ptr_ -= n * (SIZE / First);
                    return *this;
                }
            };

            using value_type       = MultiArray<T, Rest...>;
            using size_type        = size_t;
            using difference_type  = ptrdiff_t;
            using pointer          = value_type *;
            using const_pointer    = const value_type *;
            using reference        = value_type;       ///< 视图(值类型)
            using const_reference  = const value_type; ///< const 视图
            using reverse_iterator = std::reverse_iterator<iterator>;
            using const_reverse_iterator
                = std::reverse_iterator<const_iterator>;

            /// @brief 默认构造，分配堆内存，元素未初始化。
            MultiArray() : data_(new T[SIZE]), is_views_(false) {}

            /**
             * @brief 用相同值填充所有元素。
             * @param value 填充值。
             */
            MultiArray(const T &value) : data_(new T[SIZE]), is_views_(false) {
                std::fill(data_, data_ + SIZE, value);
            }

            /**
             * @brief 从一维初始化列表构造(总元素数必须匹配)。
             * @param init 初始化列表。
             */
            MultiArray(std::initializer_list<T> init) :
                data_(new T[SIZE]), is_views_(false) {
                std::copy(init.begin(), init.end(), data_);
            }

            /**
             * @brief 从嵌套初始化列表构造(多维)。
             * @param init 初始化列表，每个元素为子数组。
             */
            MultiArray(std::initializer_list<MultiArray<T, Rest...>> init) :
                data_(new T[SIZE]), is_views_(false) {
                size_t offset = 0;
                for (const auto &sub : init) {
                    std::copy(sub.fbegin(), sub.fend(), data_ + offset);
                    offset += sub.fsize();
                }
            }

            /**
             * @brief 拷贝构造，深拷贝，产生独立拥有者。
             * @param other 源数组。
             */
            MultiArray(const MultiArray &other) :
                data_(new T[SIZE]), is_views_(false) {
                std::copy(other.data_, other.data_ + SIZE, data_);
            }

            /**
             * @brief 移动构造，窃取资源，源对象变为视图。
             * @param other 源数组。
             * @note 移动后 other 处于"有效但未指定"状态。
             */
            MultiArray(MultiArray &&other) noexcept :
                data_(other.data_), is_views_(other.is_views_) {
                other.is_views_ = true;
            }

            /**
             * @brief 拷贝赋值，深拷贝。
             * @param other 源数组。
             * @return *this。
             */
            MultiArray &operator=(const MultiArray &other) {
                if (this != &other) {
                    if (!is_views_) delete[] data_;
                    data_ = new T[SIZE];
                    std::copy(other.data_, other.data_ + SIZE, data_);
                }
                return *this;
            }

            /**
             * @brief 移动赋值，窃取资源。
             * @param other 源数组。
             * @return *this。
             */
            MultiArray &operator=(MultiArray &&other) noexcept {
                if (this != &other) {
                    if (!is_views_) delete[] data_;
                    data_       = other.data_;
                    is_views_   = other.is_views_;
                    other.data_ = nullptr;
                }
                return *this;
            }

            /**
             * @brief 显式深拷贝，返回独立拥有者。
             * @return MultiArray 独立副本。
             */
            MultiArray clone() const { return MultiArray(*this); }

            /// @brief 析构，拥有者释放堆内存，视图不释放。
            ~MultiArray() {
                if (!is_views_) delete[] data_;
            }

            /// @brief 迭代器 begin，返回指向首元素的迭代器。
            iterator begin() { //
                return iterator(data_);
            }
            /// @brief 迭代器 end，返回指向尾元素的迭代器。
            iterator end() { //
                return iterator(data_ + SIZE);
            }
            /// @brief const迭代器 begin，返回指向首元素的 const 迭代器。
            const_iterator begin() const { //
                return const_iterator(data_);
            }
            /// @brief const迭代器 end，返回指向尾元素的 const 迭代器。
            const_iterator end() const { //
                return const_iterator(data_ + SIZE);
            }
            /// @brief 反向迭代器 rbegin，返回指向尾元素的反向迭代器。
            reverse_iterator rbegin() { //
                return reverse_iterator(data_ + SIZE);
            }
            /// @brief 反向迭代器 rend，返回指向首元素的反向迭代器。
            reverse_iterator rend() { //
                return reverse_iterator(data_);
            }
            /// @brief const 反向迭代器 rbegin，返回指向尾元素的 const 反向迭代器。
            const_reverse_iterator rbegin() const {
                return const_reverse_iterator(data_ + SIZE);
            }
            /// @brief const 反向迭代器 rend，返回指向首元素的 const 反向迭代器。
            const_reverse_iterator rend() const {
                return const_reverse_iterator(data_);
            }
            /// @brief const 迭代器 cbegin，返回指向首元素的 const 迭代器。
            const_iterator cbegin() const { //
                return const_iterator(data_);
            }
            /// @brief const 迭代器 cend，返回指向尾元素的 const 迭代器。
            const_iterator cend() const { //
                return const_iterator(data_ + SIZE);
            }
            /// @brief const 反向迭代器 crbegin，返回指向尾元素的 const 反向迭代器。
            const_reverse_iterator crbegin() const {
                return const_reverse_iterator(data_ + SIZE);
            }
            /// @brief const 反向迭代器 crend，返回指向首元素的 const 反向迭代器。
            const_reverse_iterator crend() const {
                return const_reverse_iterator(data_);
            }

            /// @brief 扁平迭代器 fbegin，返回指向首元素的指针。
            T *fbegin() { return data_; }
            /// @brief const 扁平迭代器 fbegin，返回指向首元素的 const 指针。
            const T *fbegin() const { return data_; }
            /// @brief 扁平迭代器 fend，返回指向尾元素的指针。
            T *fend() { return data_ + SIZE; }
            /// @brief const 扁平迭代器 fend，返回指向尾元素的 const 指针。
            const T *fend() const { return data_ + SIZE; }

            /// @brief 静态成员函数 rank，返回数组的维度。
            static constexpr size_t rank() { return 1 + sizeof...(Rest); }
            /// @brief 静态成员函数 dims，返回数组的维度数组。
            static constexpr std::array<size_t, rank()> dims() {
                return {First, Rest...};
            }
            /// @brief 静态成员函数 fsize，返回数组的元素个数。
            static constexpr size_t fsize() { return SIZE; }
            /// @brief 静态成员函数 fempty，返回数组是否为空。
            static constexpr bool fempty() { return SIZE == 0; }
            /// @brief 静态成员函数 fmax_size，返回数组的最大元素个数。
            static constexpr size_t fmax_size() { return SIZE; }

            /**
             * @brief 扁平化视图，将多维数组视为一维数组。
             * @return MultiArray<T, SIZE>& 一维视图。
             * @note 零开销，仅 reinterpret_cast。
             */
            MultiArray<T, SIZE> &flatten() {
                return *reinterpret_cast<MultiArray<T, SIZE> *>(this);
            }
            /**
             * @brief const 扁平化视图，将多维数组视为一维数组。
             * @return const MultiArray<T, SIZE>& 一维视图。
             * @note 零开销，仅 reinterpret_cast。
             */
            const MultiArray<T, SIZE> &flatten() const {
                return *reinterpret_cast<const MultiArray<T, SIZE> *>(this);
            }

            /**
             * @brief 访问第一维的子数组。
             * @param index 索引。
             * @return reference 子数组视图。
             * @note 不进行边界检查。
             */
            reference operator[](size_t index) {
                return reference(data_ + SIZE / First * index, true);
            }
            /**
             * @brief 访问第一维的子数组。
             * @param index 索引。
             * @return const_reference 子数组视图。
             * @note 不进行边界检查。
             */
            const_reference operator[](size_t index) const {
                return const_reference(data_ + SIZE / First * index, true);
            }

            /**
             * @brief 带边界检查的子数组访问。
             * @param index 索引。
             * @return reference 子数组视图。
             * @throw MultiArrayError 若索引越界。
             */
            reference at(size_t index) {
                if (index >= First)
                    throw MultiArrayError("index " + std::to_string(index)
                                          + " out of range [0, "
                                          + std::to_string(First) + ')');
                return operator[](index);
            }
            /**
             * @brief 带边界检查的子数组访问。
             * @param index 索引。
             * @return const_reference 子数组视图。
             * @throw MultiArrayError 若索引越界。
             */
            const_reference at(size_t index) const {
                if (index >= First)
                    throw MultiArrayError("index " + std::to_string(index)
                                          + " out of range [0, "
                                          + std::to_string(First) + ')');
                return operator[](index);
            }

            /**
             * @brief 带边界检查的子数组访问。
             * @param index 索引。
             * @return reference 子数组视图。
             * @throw MultiArrayError 若索引越界。
             */
            reference operator()(size_t index) { return at(index); }
            /**
             * @brief 带边界检查的子数组访问。
             * @param index 索引。
             * @return const_reference 子数组视图。
             * @throw MultiArrayError 若索引越界。
             */
            const_reference operator()(size_t index) const { return at(index); }

            /**
             * @brief 多维下标访问(带边界检查)。
             * @tparam Indices 剩余索引类型包。
             * @param idx 第一维索引。
             * @param rest 剩余维度索引。
             * @return 最终元素的引用(若索引数等于维数)或子数组视图(若不足)。
             * @throw MultiArrayError 若任意索引越界。
             */
            template <class... Indices>
            auto operator()(size_t idx, Indices... rest) //
                -> decltype(at(idx)(rest...)) {
                static_assert(
                    sizeof...(Indices) < rank(), "Too Many Arguments!");
                return at(idx)(rest...);
            }
            template <class... Indices>
            auto operator()(size_t idx, Indices... rest) const //
                -> decltype(at(idx)(rest...)) {
                static_assert(
                    sizeof...(Indices) < rank(), "Too Many Arguments!");
                return at(idx)(rest...);
            }

            /// @brief 访问第一个子数组。
            reference front() { return operator[](0); }
            /// @brief 访问第一个子数组。
            const_reference front() const { return operator[](0); }
            /// @brief 访问最后一个子数组。
            reference back() { return operator[](First - 1); }
            /// @brief 访问最后一个子数组。
            const_reference back() const { return operator[](First - 1); }

            /// @brief 第一维元素数量，返回 First。
            static size_t size() { return First; }
            /// @brief 第一维是否为空，返回 First == 0。
            static bool empty() { return First == 0; }
            /// @brief 第一维最大大小，返回 First。
            static size_t max_size() { return First; }

            /**
             * @brief 流输出，递归输出嵌套方括号。
             * @tparam CharT 字符类型。
             * @tparam Traits 字符特征。
             */
            template <class CharT, class Traits>
            friend std::basic_ostream<CharT, Traits> &operator<<(
                std::basic_ostream<CharT, Traits> &os, const MultiArray &ma) {
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

            /**
             * @brief 用给定值填充所有元素。
             * @param value 填充值。
             */
            void fill(const T &value) { std::fill(data_, data_ + SIZE, value); }

            /**
             * @brief 遍历所有元素并应用函数。
             * @tparam F 可调用对象，接受 T&。
             * @param f 函数对象。
             */
            template <class F>
            void for_each(F f) {
                std::for_each(data_, data_ + SIZE, f);
            }

            /**
             * @brief 遍历所有元素并应用函数(const 版本)。
             * @tparam F 可调用对象，接受 const T&。
             * @param f 函数对象。
             */
            template <class F>
            void for_each(F f) const {
                std::for_each(data_, data_ + SIZE, f);
            }

            /**
             * @brief 交换两个数组的内容。
             * @param other 要交换的数组。
             */
            void swap(MultiArray &other) noexcept {
                std::swap(data_, other.data_);
                std::swap(is_views_, other.is_views_);
            }

            /**
             * @brief 交换两个数组的内容。
             * @param other 要交换的数组。
             */
            friend void swap(MultiArray &a, MultiArray &b) noexcept {
                a.swap(b);
            }

            /**
             * @brief 重新调整数组的形状。
             * @tparam Dims 新的维度大小。
             * @return 重新调整形状后的数组。
             */
            template <size_t... Dims>
            MultiArray<T, Dims...> &reshape() {
                static_assert(MultiArray<T, Dims...>::fsize() == fsize(),
                    "reshape: total element count must match");
                return *reinterpret_cast<MultiArray<T, Dims...> *>(this);
            }

            /**
             * @brief 重新调整数组的形状(const版本)。
             * @tparam Dims 新的维度大小。
             * @return 重新调整形状后的数组。
             */
            template <size_t... Dims>
            const MultiArray<T, Dims...> &reshape() const {
                static_assert(MultiArray<T, Dims...>::fsize() == fsize(),
                    "reshape: total element count must match");
                return *reinterpret_cast<const MultiArray<T, Dims...> *>(this);
            }
        };
    }

    // clang-format off
#define ARRAY_AND_ARRAY(OP)                                                    \
    template <class T, class U, size_t... Dims>                                \
    MultiArray<decltype(std::declval<T>() OP std::declval<U>()), Dims...>      \
    operator OP(const MultiArray<T, Dims...> &a,                               \
                const MultiArray<U, Dims...> &b) {                             \
        MultiArray<decltype(std::declval<T>() OP std::declval<U>()), Dims...>  \
            c;                                                                 \
        auto ai = a.fbegin();                                                  \
        auto bi = b.fbegin();                                                  \
        auto ci = c.fbegin();                                                  \
        while (ci != c.fend()) *ci++ = *ai++ OP *bi++;                         \
        return c;                                                              \
    }

#define ARRAY_AND_SCALAR(OP)                                                   \
    template <class T, class U, size_t... Dims>                                \
    MultiArray<decltype(std::declval<T>() OP std::declval<U>()), Dims...>      \
    operator OP(const MultiArray<T, Dims...> &a, const U &b) {                 \
        MultiArray<decltype(std::declval<T>() OP std::declval<U>()), Dims...>  \
            c;                                                                 \
        auto ai = a.fbegin();                                                  \
        auto ci = c.fbegin();                                                  \
        while (ci != c.fend()) *ci++ = *ai++ OP b;                             \
        return c;                                                              \
    }

#define SCALAR_AND_ARRAY(OP)                                                   \
    template <class T, class U, size_t... Dims>                                \
    MultiArray<decltype(std::declval<T>() OP std::declval<U>()), Dims...>      \
    operator OP(const U &a, const MultiArray<T, Dims...> &b) {                 \
        MultiArray<decltype(std::declval<T>() OP std::declval<U>()), Dims...>  \
            c;                                                                 \
        auto bi = b.fbegin();                                                  \
        auto ci = c.fbegin();                                                  \
        while (ci != c.fend()) *ci++ = a OP *bi++;                             \
        return c;                                                              \
    }

#define ARRAY_AND_ARRAY_INPLACE(OP)                                            \
    template <class T, class U, size_t... Dims>                                \
    MultiArray<T, Dims...> & operator OP(                                      \
            MultiArray<T, Dims...> &a,                                         \
            const MultiArray<U, Dims...> &b) {                                 \
        auto ai = a.fbegin();                                                  \
        auto bi = b.fbegin();                                                  \
        while (ai != a.fend()) *ai++ OP *bi++;                                 \
        return a;\
    }

#define ARRAY_AND_SCALAR_INPLACE(OP)                                           \
    template <class T, class U, size_t... Dims>                                \
    MultiArray<T, Dims...> & operator OP(MultiArray<T, Dims...> &a,            \
                                         const U &b) {                         \
        auto ai = a.fbegin();                                                  \
        while (ai != a.fend()) *ai++ OP b;                                     \
        return a;                                                              \
    }

#define ARRAY_AND_ARRAY_BOOL(OP)                                               \
    template <class T, class U, size_t... Dims>                                \
    MultiArray<bool, Dims...>                                                  \
    operator OP(const MultiArray<T, Dims...> &a,                               \
                const MultiArray<U, Dims...> &b) {                             \
        MultiArray<bool, Dims...> c;                                           \
        auto ai = a.fbegin();                                                  \
        auto bi = b.fbegin();                                                  \
        auto ci = c.fbegin();                                                  \
        while (ci != c.fend()) *ci++ = *ai++ OP *bi++;                         \
        return c;                                                              \
    }

#define ARRAY_AND_SCALAR_BOOL(OP)                                              \
    template <class T, class U, size_t... Dims>                                \
    MultiArray<bool, Dims...>                                                  \
    operator OP(const MultiArray<T, Dims...> &a, const U &b) {                 \
        MultiArray<bool, Dims...> c;                                           \
        auto ai = a.fbegin();                                                  \
        auto ci = c.fbegin();                                                  \
        while (ci != c.fend()) *ci++ = *ai++ OP b;                             \
        return c;                                                              \
    }

#define SCALAR_AND_ARRAY_BOOL(OP)                                              \
    template <class T, class U, size_t... Dims>                                \
    MultiArray<bool, Dims...>                                                  \
    operator OP(const U &a, const MultiArray<T, Dims...> &b) {                 \
        MultiArray<bool, Dims...> c;                                           \
        auto bi = b.fbegin();                                                  \
        auto ci = c.fbegin();                                                  \
        while (ci != c.fend()) *ci++ = a OP *bi++;                             \
        return c;                                                              \
    }

#define UNARY_ARRAY(OP)                                                        \
    template <class T, size_t... Dims>                                         \
    MultiArray<decltype(OP std::declval<T>()), Dims...>                        \
    operator OP(const MultiArray<T, Dims...> &a) {                             \
        MultiArray<decltype(OP std::declval<T>()), Dims...> c;                 \
        auto ai = a.fbegin();                                                  \
        auto ci = c.fbegin();                                                  \
        while (ci != c.fend()) *ci++ = OP *ai++;                               \
        return c;                                                              \
    }

#define UNARY_ARRAY_BOOL(OP)                                                   \
    template <class T, size_t... Dims>                                         \
    MultiArray<bool, Dims...>                                                  \
    operator OP(const MultiArray<T, Dims...> &a) {                             \
        MultiArray<bool, Dims...> c;                                           \
        auto ai = a.fbegin();                                                  \
        auto ci = c.fbegin();                                                  \
        while (ci != c.fend()) *ci++ = OP *ai++;                               \
        return c;                                                              \
    }
    // clang-format on

    ARRAY_AND_ARRAY(+)
    ARRAY_AND_ARRAY(-)
    ARRAY_AND_ARRAY(*)
    ARRAY_AND_ARRAY(/)
    ARRAY_AND_ARRAY(%)

    ARRAY_AND_ARRAY_INPLACE(+=)
    ARRAY_AND_ARRAY_INPLACE(-=)
    ARRAY_AND_ARRAY_INPLACE(*=)
    ARRAY_AND_ARRAY_INPLACE(/=)
    ARRAY_AND_ARRAY_INPLACE(%=)

    ARRAY_AND_SCALAR_INPLACE(+=)
    ARRAY_AND_SCALAR_INPLACE(-=)
    ARRAY_AND_SCALAR_INPLACE(*=)
    ARRAY_AND_SCALAR_INPLACE(/=)
    ARRAY_AND_SCALAR_INPLACE(%=)

    ARRAY_AND_ARRAY_BOOL(==)
    ARRAY_AND_ARRAY_BOOL(!=)
    ARRAY_AND_ARRAY_BOOL(<)
    ARRAY_AND_ARRAY_BOOL(>)
    ARRAY_AND_ARRAY_BOOL(<=)
    ARRAY_AND_ARRAY_BOOL(>=)

    ARRAY_AND_SCALAR(+)
    ARRAY_AND_SCALAR(-)
    ARRAY_AND_SCALAR(*)
    ARRAY_AND_SCALAR(/)
    ARRAY_AND_SCALAR(%)

    SCALAR_AND_ARRAY(+)
    SCALAR_AND_ARRAY(-)
    SCALAR_AND_ARRAY(*)
    SCALAR_AND_ARRAY(/)
    SCALAR_AND_ARRAY(%)

    UNARY_ARRAY(+)
    UNARY_ARRAY(-)
    UNARY_ARRAY(~)

    UNARY_ARRAY_BOOL(!)

    ARRAY_AND_SCALAR_BOOL(==)
    ARRAY_AND_SCALAR_BOOL(!=)
    ARRAY_AND_SCALAR_BOOL(<)
    ARRAY_AND_SCALAR_BOOL(>)
    ARRAY_AND_SCALAR_BOOL(<=)
    ARRAY_AND_SCALAR_BOOL(>=)

    SCALAR_AND_ARRAY_BOOL(==)
    SCALAR_AND_ARRAY_BOOL(!=)
    SCALAR_AND_ARRAY_BOOL(<)
    SCALAR_AND_ARRAY_BOOL(>)
    SCALAR_AND_ARRAY_BOOL(<=)
    SCALAR_AND_ARRAY_BOOL(>=)

    ARRAY_AND_ARRAY_BOOL(&&)
    ARRAY_AND_ARRAY_BOOL(||)

    ARRAY_AND_SCALAR_BOOL(&&)
    ARRAY_AND_SCALAR_BOOL(||)

    SCALAR_AND_ARRAY_BOOL(&&)
    SCALAR_AND_ARRAY_BOOL(||)

    ARRAY_AND_ARRAY(&)
    ARRAY_AND_ARRAY(|)
    ARRAY_AND_ARRAY(^)
    ARRAY_AND_ARRAY(<<)
    ARRAY_AND_ARRAY(>>)

    ARRAY_AND_ARRAY_INPLACE(&=)
    ARRAY_AND_ARRAY_INPLACE(|=)
    ARRAY_AND_ARRAY_INPLACE(^=)
    ARRAY_AND_ARRAY_INPLACE(<<=)
    ARRAY_AND_ARRAY_INPLACE(>>=)

    ARRAY_AND_SCALAR(&)
    ARRAY_AND_SCALAR(|)
    ARRAY_AND_SCALAR(^)
    ARRAY_AND_SCALAR(<<)
    ARRAY_AND_SCALAR(>>)

    SCALAR_AND_ARRAY(&)
    SCALAR_AND_ARRAY(|)
    SCALAR_AND_ARRAY(^)
    SCALAR_AND_ARRAY(>>)

    // SCALAR_AND_ARRAY(<<)
    template <class T, class U, size_t... Dims>
    typename std::enable_if<!std::is_base_of<std::ios_base, U>::value,
        MultiArray<decltype(std::declval<U>() << std::declval<T>()),
            Dims...>>::type
    operator<<(const U &value, const MultiArray<T, Dims...> &a) {
        MultiArray<decltype(std::declval<U>() << std::declval<T>()), Dims...> b;
        auto ai = a.fbegin();
        auto bi = b.fbegin();
        while (bi != b.fend()) *bi++ = value << *ai++;
        return b;
    }

    ARRAY_AND_SCALAR_INPLACE(&=)
    ARRAY_AND_SCALAR_INPLACE(|=)
    ARRAY_AND_SCALAR_INPLACE(^=)
    ARRAY_AND_SCALAR_INPLACE(<<=)
    ARRAY_AND_SCALAR_INPLACE(>>=)

#undef ARRAY_AND_ARRAY
#undef ARRAY_AND_SCALAR
#undef SCALAR_AND_ARRAY
#undef ARRAY_AND_ARRAY_INPLACE
#undef ARRAY_AND_SCALAR_INPLACE
#undef ARRAY_AND_ARRAY_BOOL
#undef ARRAY_AND_SCALAR_BOOL
#undef SCALAR_AND_ARRAY_BOOL
#undef UNARY_ARRAY
#undef UNARY_ARRAY_BOOL

    /**
     * @brief 求和所有元素。
     * @tparam T 元素类型。
     * @tparam Dims 维度。
     * @param a 输入数组。
     * @return T 和。
     */
    template <class T, size_t... Dims>
    T sum(const MultiArray<T, Dims...> &a) {
        T result{};
        a.for_each([&](const T &ref) { result += ref; });
        return result;
    }

    /**
     * @brief 求最小值。
     * @return T 最小值。
     */
    template <class T, size_t... Dims>
    T min(const MultiArray<T, Dims...> &a) {
        return *std::min_element(a.fbegin(), a.fend());
    }

    /**
     * @brief 求最大值。
     * @return T 最大值。
     */
    template <class T, size_t... Dims>
    T max(const MultiArray<T, Dims...> &a) {
        return *std::max_element(a.fbegin(), a.fend());
    }

    /**
     * @brief 检查所有元素是否都为 true(逻辑与归约)。
     * @return bool 若所有元素均为真则返回 true。
     */
    template <class T, size_t... Dims>
    bool all(const MultiArray<T, Dims...> &a) {
        for (auto it = a.fbegin(); it != a.fend(); ++it)
            if (!*it) return false;
        return true;
    }

    /**
     * @brief 检查是否存在至少一个 true 元素(逻辑或归约)。
     * @return bool 若至少一个元素为真则返回 true。
     */
    template <class T, size_t... Dims>
    bool any(const MultiArray<T, Dims...> &a) {
        for (auto it = a.fbegin(); it != a.fend(); ++it)
            if (*it) return true;
        return false;
    }

    /**
     * @brief 检查两个 MultiArray 是否相等。
     * @return bool 若两个 MultiArray 相等则返回 true。
     */
    template <class T, class U, size_t... Dims>
    bool
    equals(const MultiArray<T, Dims...> &a, const MultiArray<U, Dims...> &b) {
        return std::equal(a.fbegin(), a.fend(), b.fbegin(), b.fend());
    }

    /**
     * @brief 检查两个 MultiArray 是否相等。
     * @return 恒假。
     */
    template <class T, class U, size_t... Dims1, size_t... Dims2>
    constexpr bool
    equals(const MultiArray<T, Dims1...> &, const MultiArray<U, Dims2...> &) {
        return false;
    }

    /**
     * @brief 检查两个 MultiArray 是否相等。
     * @return bool 若两个 MultiArray 相等则返回 true。
     */
    template <class T, class U, size_t... Dims, class Op>
    bool equals(const MultiArray<T, Dims...> &a,
        const MultiArray<U, Dims...>         &b,
        Op                                  &&op) {
        return std::equal(
            a.fbegin(), a.fend(), b.fbegin(), b.fend(), std::forward<Op>(op));
    }

    /**
     * @brief 检查两个 MultiArray 是否相等。
     * @return 恒假。
     */
    template <class T, class U, size_t... Dims1, size_t... Dims2, class Op>
    constexpr bool equals(const MultiArray<T, Dims1...> &,
        const MultiArray<U, Dims2...> &,
        Op &&) {
        return false;
    }

    /**
     * @brief 比较两个 MultiArray 的大小。
     * @return 若 a < b (字典序) 则返回 -1，若 a > b 则返回 1，否则返回 0。
     */
    template <class T, class U, size_t... Dims>
    int
    compare(const MultiArray<T, Dims...> &a, const MultiArray<U, Dims...> &b) {
        for (auto it1 = a.fbegin(), it2 = b.fbegin(); //
            it1 != a.fend();                          //
            ++it1, ++it2) {
            if (*it1 < *it2) return -1;
            if (*it2 < *it1) return 1;
        }
        return 0;
    }
}
