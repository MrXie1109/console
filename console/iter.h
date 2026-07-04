/**
 * @file iter.h
 * @brief 提供些许关于迭代器的拓展功能。
 * @details 本文件提供了两个主要工具：IteratorPair
 * 用于将迭代器对包装为可范围遍历的对象， 以及 zip
 * 函数族用于同时遍历两个容器，并在较短者结束时自动停止。
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
#include <iterator>
#include <type_traits>
#include <utility>

namespace console {
    /**
     * @struct IteratorPair
     * @brief 存储迭代器对，可以直接范围 for。
     * @details 公开继承自 std::pair<Iter, Iter>。
     * @tparam Iter 迭代器类型。
     */
    template <class Iter> struct IteratorPair : public std::pair<Iter, Iter> {
        /**
         * @brief 接受迭代器对的构造函数。
         * @param beg 首迭代器。
         * @param end 超尾迭代器。
         */
        IteratorPair(Iter beg, Iter end) : std::pair<Iter, Iter>(beg, end) {}

        typedef Iter iterator; ///< 迭代器类型别名。
        using value_type =
            typename std::iterator_traits<Iter>::value_type; ///< 值类型别名。
        using reference =
            typename std::iterator_traits<Iter>::reference; ///< 引用类型别名。
        using pointer =
            typename std::iterator_traits<Iter>::pointer; ///< 指针类型别名。

        /**
         * @brief 返回首迭代器。
         * @return iterator first
         */
        iterator begin() { return this->first; }
        /**
         * @brief 返回超尾迭代器。
         * @return iterator second
         */
        iterator end() { return this->second; }
    };

    /**
     * @brief 工厂函数，构建 IteratorPair。
     * @tparam Iter 迭代器类型。
     * @param beg 首迭代器。
     * @param end 超尾迭代器。
     */
    template <class Iter> IteratorPair<Iter> iterpair(Iter beg, Iter end) {
        return {beg, end};
    }

    /**
     * @class TiedIterators
     * @brief 绑定迭代器对，一齐迭代。
     * @tparam It1 迭代器类型。
     * @tparam It2 另一个迭代器类型。
     */
    template <class It1, class It2> class TiedIterators {
        It1 it1; ///< 迭代器A
        It2 it2; ///< 迭代器B

    public:
        typedef std::pair<typename It1::value_type,
            typename It2::value_type>
                     value_type; ///< 类型别名
        typedef void pointer; ///< 类型别名（不实际使用）
        typedef std::pair<typename It1::reference,
            typename It2::reference>
                                          reference; ///< 类型别名
        typedef std::forward_iterator_tag iterator_category; ///< 类型别名
        typedef std::ptrdiff_t            difference_type; ///< 类型别名

        /**
         * @brief 接受迭代器组的构造函数。
         * @param beg 迭代器A。
         * @param end 迭代器B。
         */
        TiedIterators(It1 i1, It2 i2) : it1(i1), it2(i2) {}

        /**
         * @brief 将两个迭代器均前进一步。
         * @return TiedIterators & 返回 *this。
         */
        TiedIterators &operator++() {
            ++it1;
            ++it2;
            return *this;
        }
        /**
         * @brief 对迭代器组解引用。
         * @return reference 返回由两个迭代器解引用得到的对。
         */
        reference operator*() const { return {*it1, *it2}; }

        /**
         * @brief 比较是否与另一迭代器相等。
         * @details 任一迭代器相同即视作相等。
         * @note 为了实现在较短的序列结束时结束。
         * @warning 可能不是传统语义。
         * @return bool 相等如果 true 否则 false
         */
        bool operator==(const TiedIterators &other) const {
            return it1 == other.it1 || it2 == other.it2;
        }

        /**
         * @brief 比较是否与另一迭代器不等。
         * @details 所有迭代器不等才视作不等。
         * @note 为了实现在较短的序列结束时结束。
         * @warning 可能不是传统语义。
         * @return bool 相等如果 false 否则 true
         */
        bool operator!=(const TiedIterators &other) const {
            return !(*this == other);
        }
    };

    /**
     * @brief 将两个容器“拉链”式地绑定在一起，以便同时迭代。
     * @details 返回一个 IteratorPair，其中包含一个特殊的 TiedIterators
     * 迭代器对。 当使用范围 for
     * 循环遍历时，每次迭代将同时从两个容器中各取出一个元素， 并以 std::pair
     * 的形式返回。 循环会在较短的容器遍历完毕后自动结束。
     * @tparam C1 第一个容器的类型（自动推导）。
     * @tparam C2 第二个容器的类型（自动推导）。
     * @param c1 第一个容器（左值，非常量）。
     * @param c2 第二个容器（左值，非常量）。
     * @return IteratorPair<TiedIterators<C1::iterator, C2::iterator>>
     *         一个可用于范围 for 循环的迭代器对。
     * @note 等效操作类似 Python 的 zip 函数。
     * @warning
     * 返回的迭代器对中存储的是原始容器的引用，请确保容器的生命周期长于迭代器的使用。
     */
    template <class C1, class C2>
    IteratorPair<TiedIterators<typename C1::iterator, typename C2::iterator>>
    zip(C1 &c1, C2 &c2) {
        return {{std::begin(c1), std::begin(c2)}, {std::end(c1), std::end(c2)}};
    }

    /**
     * @overload
     * @brief 针对两个常量容器的 zip 版本。
     * @param c1 第一个常量容器。
     * @param c2 第二个常量容器。
     */
    template <class C1, class C2>
    IteratorPair<
        TiedIterators<typename C1::const_iterator, typename C2::const_iterator>>
    zip(const C1 &c1, const C2 &c2) {
        return {{std::begin(c1), std::begin(c2)}, {std::end(c1), std::end(c2)}};
    }

    /**
     * @overload
     * @brief 针对第一个容器为常量、第二个容器为非常量的 zip 版本。
     * @param c1 第一个常量容器。
     * @param c2 第二个非常量容器。
     */
    template <class C1, class C2>
    IteratorPair<
        TiedIterators<typename C1::const_iterator, typename C2::iterator>>
    zip(const C1 &c1, C2 &c2) {
        return {{std::begin(c1), std::begin(c2)}, {std::end(c1), std::end(c2)}};
    }

    /**
     * @overload
     * @brief 针对第一个容器为非常量、第二个容器为常量的 zip 版本。
     * @param c1 第一个非常量容器。
     * @param c2 第二个常量容器。
     */
    template <class C1, class C2>
    IteratorPair<
        TiedIterators<typename C1::iterator, typename C2::const_iterator>>
    zip(C1 &c1, const C2 &c2) {
        return {{std::begin(c1), std::begin(c2)}, {std::end(c1), std::end(c2)}};
    }
}
