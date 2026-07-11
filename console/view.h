/**
 * @file view.h
 * @brief 提供容器的非拥有视图（View），支持区间切片和便捷的工厂函数。
 * @details View 类是一个轻量级的容器视图，不复制数据，仅保存一对迭代器。
 *          支持范围 for 循环、下标访问、边界检查的 at() 方法，以及 collect()
 * 复制数据到新容器。 同时提供工厂函数 make_view 简化 View 对象的创建。
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
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

#include "csexc.h"
#include "output.h"

namespace console {
    /**
     * @class View
     * @brief 容器的非拥有视图（可变版本）。
     * @tparam Container 容器类型，必须支持迭代器。
     * @details View
     * 不拥有数据，仅保存一对迭代器，可以高效地表示容器的一个连续子区间。
     *          支持迭代器、下标访问、边界检查的 at() 方法，以及 collect()
     * 复制数据到新容器。
     * @note 需要确保底层容器在视图生命周期内有效。
     */
    template <class Container>
    class View {
        using Iterator  = typename Container::iterator;
        using cIterator = typename Container::const_iterator;
        Iterator begin_, end_;

    public:
        typedef Iterator  iterator;
        typedef cIterator const_iterator;

        /// @brief 返回起始迭代器。
        Iterator begin() const { return begin_; }
        /// @brief 返回结束迭代器。
        Iterator end() const { return end_; }
        /// @brief 返回常量起始迭代器。
        cIterator cbegin() const { return begin_; }
        /// @brief 返回常量结束迭代器。
        cIterator cend() const { return end_; }

        /**
         * @brief 构造整个容器的视图。
         * @param container 底层容器（非 const 左值引用）。
         */
        View(Container &container) :
            begin_(std::begin(container)), end_(std::end(container)) {}

        /**
         * @brief 构造容器子区间的视图。
         * @param container 底层容器。
         * @param start_pos 起始索引（包含）。
         * @param end_pos 结束索引（不包含）。
         */
        View(Container &container, size_t start_pos, size_t end_pos) :
            begin_(std::next(std::begin(container), start_pos)),
            end_(std::next(std::begin(container), end_pos)) {}

        /**
         * @brief 从迭代器对构造视图。
         * @param begin 起始迭代器。
         * @param end 结束迭代器。
         */
        View(Iterator begin, Iterator end) : begin_(begin), end_(end) {}

        /// @brief 返回视图中的元素个数。
        size_t size() const { return std::distance(begin_, end_); }

        /**
         * @brief 无边界检查的下标访问。
         * @param pos 索引。
         * @return 元素的引用。
         */
        auto operator[](size_t pos) const -> decltype(*begin_) {
            return *std::next(begin_, pos);
        }

        /**
         * @brief 带边界检查的下标访问。
         * @param pos 索引。
         * @return 元素的引用。
         * @throw IndexError 若索引超出范围。
         */
        auto at(size_t pos) const -> decltype(*begin_) {
            if (pos >= size())
                throw IndexError(std::to_string(pos) + " out of 0 ~ "
                                 + std::to_string(size() - 1));
            return (*this)[pos];
        }

        /**
         * @brief 将视图中的元素复制到一个新的容器中。
         * @return Container 包含视图元素的新容器。
         */
        Container collect() const { return Container(begin_, end_); }
    };

    /**
     * @class View<const Container>
     * @brief 容器的非拥有视图（常量版本）。
     * @tparam Container 容器类型。
     */
    template <class Container>
    class View<const Container> {
        using cIterator = typename Container::const_iterator;
        cIterator begin_, end_;

    public:
        typedef cIterator iterator;
        typedef cIterator const_iterator;

        cIterator begin() const { return begin_; }
        cIterator end() const { return end_; }
        cIterator cbegin() const { return begin_; }
        cIterator cend() const { return end_; }

        /**
         * @brief 构造整个常量容器的视图。
         * @param container 常量容器引用。
         */
        View(const Container &container) :
            begin_(std::begin(container)), end_(std::end(container)) {}

        /**
         * @brief 构造常量容器子区间的视图。
         * @param container 常量容器引用。
         * @param start_pos 起始索引。
         * @param end_pos 结束索引。
         */
        View(const Container &container, size_t start_pos, size_t end_pos) :
            begin_(std::next(std::begin(container), start_pos)),
            end_(std::next(std::begin(container), end_pos)) {}

        /**
         * @brief 从常量迭代器对构造视图。
         * @param begin 起始迭代器。
         * @param end 结束迭代器。
         */
        View(cIterator begin, cIterator end) : begin_(begin), end_(end) {}

        size_t size() const { return std::distance(begin_, end_); }

        auto operator[](size_t pos) const -> decltype(*begin_) {
            return *std::next(begin_, pos);
        }

        auto at(size_t pos) const -> decltype(*begin_) {
            if (pos >= size())
                throw IndexError(std::to_string(pos) + " out of 0 ~ "
                                 + std::to_string(size() - 1));
            return (*this)[pos];
        }

        /**
         * @brief 将常量视图中的元素复制到新容器。
         * @return Container 新容器。
         */
        Container collect() const { return Container(begin_, end_); }
    };

    /**
     * @class View<T[]>
     * @brief T[] 的特化，包装原生数组指针。
     * @tparam T 元素类型。
     */
    template <class T>
    class View<T[]> {
        T *begin_, *end_;

    public:
        typedef T       *iterator;
        typedef const T *const_iterator;

        T       *begin() const { return begin_; }
        T       *end() const { return end_; }
        const T *cbegin() const { return begin_; }
        const T *cend() const { return end_; }

        View(T *begin, T *end) : begin_(begin), end_(end) {}
        View(T *data, size_t count) : begin_(data), end_(data + count) {}

        size_t size() const { return end_ - begin_; }

        T &operator[](size_t pos) const { return begin_[pos]; }

        T &at(size_t pos) const {
            if (pos >= size())
                throw IndexError(std::to_string(pos) + " out of 0 ~ "
                                 + std::to_string(size() - 1));
            return begin_[pos];
        }

        std::vector<T> collect() const { return {begin_, end_}; }
    };

    /**
     * @class View<const T[]>
     * @brief const T[] 的特化，包装指向 const 的原生数组指针。
     * @tparam T 元素类型。
     */
    template <class T>
    class View<const T[]> {
        const T *begin_, *end_;

    public:
        typedef const T *iterator;
        typedef const T *const_iterator;

        const T *begin() const { return begin_; }
        const T *end() const { return end_; }
        const T *cbegin() const { return begin_; }
        const T *cend() const { return end_; }

        View(const T *begin, const T *end) : begin_(begin), end_(end) {}
        View(const T *data, size_t count) : begin_(data), end_(data + count) {}

        size_t size() const { return end_ - begin_; }

        const T &operator[](size_t pos) const { return begin_[pos]; }

        const T &at(size_t pos) const {
            if (pos >= size())
                throw IndexError(std::to_string(pos) + " out of 0 ~ "
                                 + std::to_string(size() - 1));
            return begin_[pos];
        }

        std::vector<T> collect() const { return {begin_, end_}; }
    };

    /**
     * @brief 输出 std::string 视图的内容。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @param os 输出流。
     * @param sv 字符串视图。
     * @return std::basic_ostream<CharT, Traits>& 流引用。
     */
    template <class CharT, class Traits>
    inline std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &os, const View<std::string> &sv) {
        for (auto it = sv.begin(); it != sv.end(); ++it) os << *it;
        return os;
    }

    /**
     * @brief 输出任意视图的内容。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam T 容器类型。
     * @param os 输出流。
     * @param v 视图对象。
     * @return std::basic_ostream<CharT, Traits>& 流引用。
     */
    template <class CharT, class Traits, class Container>
    std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &os, const View<Container> &v) {
        return os << v.collect();
    }

    // ========================== 工厂函数 ==========================

    /**
     * @defgroup view_factories 视图工厂函数
     * @brief 用于方便创建 View 对象的辅助函数。
     * @{
     */

    /**
     * @brief 创建整个容器的视图（可变版本）。
     * @tparam Container 容器类型。
     * @param cont 容器引用。
     * @return View<Container> 视图对象。
     */
    template <class Container>
    View<Container> make_view(Container &cont) {
        return {cont};
    }

    /**
     * @brief 创建整个容器的视图（常量版本）。
     * @tparam Container 容器类型。
     * @param cont 常量容器引用。
     * @return View<const Container> 视图对象。
     */
    template <class Container>
    View<const Container> make_view(const Container &cont) {
        return {cont};
    }

    /**
     * @brief 创建容器子区间的视图（可变版本）。
     * @tparam Container 容器类型。
     * @param cont 容器引用。
     * @param start 起始索引（包含）。
     * @param end 结束索引（不包含）。
     * @return View<Container> 视图对象。
     */
    template <class Container>
    View<Container> make_view(Container &cont, size_t start, size_t end) {
        return {cont, start, end};
    }

    /**
     * @brief 创建容器子区间的视图（常量版本）。
     * @tparam Container 容器类型。
     * @param cont 常量容器引用。
     * @param start 起始索引。
     * @param end 结束索引。
     * @return View<const Container> 视图对象。
     */
    template <class Container>
    View<const Container>
    make_view(const Container &cont, size_t start, size_t end) {
        return {cont, start, end};
    }

    /**
     * @brief 从迭代器对创建视图（可变版本）。
     * @tparam Container 迭代器类型。
     * @param begin 起始迭代器。
     * @param end 结束迭代器。
     * @return View<Container> 视图对象。
     */
    template <class Container>
    View<Container> make_view(
        typename Container::iterator begin, typename Container::iterator end) {
        return {begin, end};
    }

    /**
     * @brief 从迭代器对创建视图（常量版本）。
     * @tparam Container 迭代器类型。
     * @param begin 起始迭代器。
     * @param end 结束迭代器。
     * @return View<const Container> 视图对象。
     */
    template <class Container>
    View<const Container> make_view(typename Container::const_iterator begin,
        typename Container::const_iterator                             end) {
        return {begin, end};
    }

    /**
     * @brief 从指针对创建视图（可变版本）。
     * @tparam T 元素类型。
     * @param begin 起始指针。
     * @param end 结束指针。
     * @return View<T[]> 视图对象。
     */
    template <class T>
    View<T[]> make_view(T *begin, T *end) {
        return {begin, end};
    }

    /**
     * @brief 从指针对创建视图（常量版本）。
     * @tparam T 元素类型。
     * @param begin 起始指针。
     * @param end 结束指针。
     * @return View<const T[]> 视图对象。
     */
    template <class T>
    View<const T[]> make_view(const T *begin, const T *end) {
        return {begin, end};
    }

    /**
     * @brief 从指针和长度创建视图（可变版本）。
     * @tparam T 元素类型。
     * @param data 数据指针。
     * @param count 元素个数。
     * @return View<T[]> 视图对象。
     */
    template <class T>
    View<T[]> make_view(T *data, size_t count) {
        return {data, count};
    }

    /**
     * @brief 从指针和长度创建视图（常量版本）。
     * @tparam T 元素类型。
     * @param data 数据指针。
     * @param count 元素个数。
     * @return View<const T[]> 视图对象。
     */
    template <class T>
    View<const T[]> make_view(const T *data, size_t count) {
        return {data, count};
    }

    /** @} */ // end of view_factories
}
