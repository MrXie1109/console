/**
 * @file adapter.h
 * @brief 提供带有值返回 pop 操作的容器适配器包装类和 range 函数。
 * @details 该模块对 STL 容器适配器（stack、queue、priority_queue）进行扩展，
 *          重载了 pop()
 * 方法，使其在弹出元素的同时返回该元素的值。适用于需要在弹出元素时获取其值的场景。
 *          提供 range() 函数用于生成数值范围，类似于 Python 的 range() 函数。
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
#include <deque>
#include <queue>
#include <stack>
#include <utility>
#include <vector>

#include "iter.h"

namespace console {
    /**
     * @class Stack
     * @brief 栈容器适配器，扩展自 std::stack。
     * @tparam T 栈中存储的元素类型。
     * @tparam Container 底层容器类型，默认为 std::deque<T>。
     * @details 该适配器继承自 std::stack，提供了返回值版本的 pop() 方法。
     *          使用示例：
     *          @code
     *          console::Stack<int> s;
     *          s.push(1);
     *          s.push(2);
     *          int value = s.pop();  // 返回 2 并弹出
     *          @endcode
     */
    template <class T, class Container = std::deque<T>>
    class Stack : public std::stack<T, Container> {
        using base_type = std::stack<T, Container>;

    public:
        /// 继承 std::stack 的所有构造函数
        using base_type::stack;

        /**
         * @brief 弹出栈顶元素并返回其值。
         * @return typename base_type::value_type
         * 栈顶元素的值（通过移动语义返回）。
         * @details 该方法结合了 top() 和 pop()
         * 操作，返回栈顶元素的副本/移动值，
         *          然后将栈顶元素弹出。使用移动语义避免不必要的拷贝。
         */
        typename base_type::value_type pop() {
            auto top = std::move(base_type::top());
            base_type::pop();
            return top;
        }
    };

    /**
     * @class Queue
     * @brief 队列容器适配器，扩展自 std::queue。
     * @tparam T 队列中存储的元素类型。
     * @tparam Container 底层容器类型，默认为 std::deque<T>。
     * @details 该适配器继承自 std::queue，提供了返回值版本的 pop() 方法。
     *          使用示例：
     *          @code
     *          console::Queue<int> q;
     *          q.push(1);
     *          q.push(2);
     *          int value = q.pop();  // 返回 1 并弹出
     *          @endcode
     */
    template <class T, class Container = std::deque<T>>
    class Queue : public std::queue<T, Container> {
        using base_type = std::queue<T, Container>;

    public:
        /// 继承 std::queue 的所有构造函数
        using base_type::queue;

        /**
         * @brief 弹出队首元素并返回其值。
         * @return typename base_type::value_type
         * 队首元素的值（通过移动语义返回）。
         * @details 该方法结合了 front() 和 pop()
         * 操作，返回队首元素的副本/移动值，
         *          然后将队首元素弹出。使用移动语义避免不必要的拷贝。
         */
        typename base_type::value_type pop() {
            auto top = std::move(base_type::front());
            base_type::pop();
            return top;
        }
    };

    /**
     * @class PriorityQueue
     * @brief 优先队列容器适配器，扩展自 std::priority_queue。
     * @tparam T 优先队列中存储的元素类型。
     * @tparam Container 底层容器类型，默认为 std::vector<T>。
     * @tparam Compare 比较函数对象类型，默认为 std::less<T>（最大堆）。
     * @details 该适配器继承自 std::priority_queue，提供了返回值版本的 pop()
     * 方法。 使用示例：
     *          @code
     *          console::PriorityQueue<int> pq;
     *          pq.push(3);
     *          pq.push(1);
     *          pq.push(4);
     *          int value = pq.pop();  // 返回 4（最大值）并弹出
     *          @endcode
     */
    template <class T,
        class Container = std::vector<T>,
        class Compare   = std::less<T>>
    class PriorityQueue : public std::priority_queue<T, Container, Compare> {
        using base_type = std::priority_queue<T, Container, Compare>;

    public:
        /// 继承 std::priority_queue 的所有构造函数
        using base_type::priority_queue;

        /**
         * @brief 弹出优先级最高的元素并返回其值。
         * @return typename base_type::value_type
         * 优先级最高的元素的值（通过移动语义返回）。
         * @details 该方法结合了 top() 和 pop()
         * 操作，返回堆顶元素的副本/移动值，
         *          然后将堆顶元素弹出。使用移动语义避免不必要的拷贝。
         */
        typename base_type::value_type pop() {
            auto top = std::move(base_type::top());
            base_type::pop();
            return top;
        }
    };

    /**
     * @brief RangeIterator 类模板，用于实现 range() 函数生成的迭代器。
     * @details 该迭代器支持前置和后置递增操作，解引用操作返回当前值。
     *          比较操作符根据步长方向判断是否达到范围边界。
     *          可以直接用于 range-based for 循环中，生成数值范围。
     * @tparam T 值类型。
     */
    template <class T> class RangeIterator {
        T cur_;
        T bounds_;
        T step_;

    public:
        typedef T    value_type; ///< 类型别名
        typedef void pointer; ///< 类型别名（不实际使用）
        typedef T
            reference; ///< 类型别名（由于解引用产出值，所以引用类型实际上不是引用）

        typedef std::forward_iterator_tag iterator_category; ///< 类型别名
        typedef std::ptrdiff_t            difference_type; ///< 类型别名

        /**
         * @brief 构造函数。
         * @param start 范围的起始值，默认为 T 的默认构造值。
         * @param end 范围的结束值，默认为 T 的默认构造
         * @param step 范围的步长，默认为 T 的默认构造值。
         */
        RangeIterator(
            const T &cur = T{}, const T &bounds = T{}, const T &step = T{}) :
            cur_(cur), bounds_(bounds), step_(step) {}

        /**
         * @brief 解引用运算符。
         * @return T 当前迭代器指向的值。
         */
        T operator*() const { return cur_; }

        /**
         * @brief 前置递增运算符。
         * @return RangeIterator& 递增后的迭代器引用。
         */
        RangeIterator &operator++() {
            cur_ += step_;
            return *this;
        }

        /**
         * @brief 后置递增运算符。
         * @return const RangeIterator 递增前的迭代器副本。
         */
        const RangeIterator operator++(int) {
            const RangeIterator old = *this;
            cur_ += step_;
            return old;
        }

        /**
         * @brief 比较运算符。
         * @param 另一个 RangeIterator 对象。
         * @return bool 如果当前迭代器未达到范围边界，则返回 true；否则返回
         * false。
         * @note 可能不是传统语义，但是用于 range-based for 没问题。
         *       第二个迭代器（end）只是一个空壳，比较时不使用其值，仅用于标识范围结束。
         * @see std::istream_iterator 的比较运算符实现，类似地不使用 end
         * 迭代器的值进行比较， 而是根据当前迭代器的状态判断是否达到范围边界。
         */
        bool operator!=(const RangeIterator &) const {
            return step_ > T{} ? cur_ < bounds_ : cur_ > bounds_;
        }

        /**
         * @brief 比较运算符。
         * @param 另一个 RangeIterator 对象。
         * @return bool 如果当前迭代器已达到范围边界，则返回 true；否则返回
         * false。
         * @note 可能不是传统语义，但是用于 range-based for 没问题。
         *       第二个迭代器（end）只是一个空壳，比较时不使用其值，仅用于标识范围结束。
         * @see std::istream_iterator 的比较运算符实现，类似地不使用 end
         * 迭代器的值进行比较， 而是根据当前迭代器的状态判断是否达到范围边界。
         */
        bool operator==(const RangeIterator &) const {
            return step_ > T{} ? cur_ >= bounds_ : cur_ <= bounds_;
        }
    };

    /**
     * @brief 生成一个数值范围的迭代器对，类似于 Python 的 range() 函数。
     * @tparam T 值类型。
     * @param end 结束值。
     * @return IteratorPair<RangeIterator<T>> 可迭代对象。
     */
    template <class T>
    inline IteratorPair<RangeIterator<T>> range(const T &end) {
        return {{T{}, end, T{1}}, {T{}, end, T{1}}};
    }

    /**
     * @brief 生成一个数值范围的迭代器对，类似于 Python 的 range() 函数。
     * @tparam T 值类型。
     * @param start 起始值。
     * @param end 结束值。
     * @return IteratorPair<RangeIterator<T>> 可迭代对象。
     */
    template <class T>
    inline IteratorPair<RangeIterator<T>> range(const T &start, const T &end) {
        return {{start, end, T{1}}, {start, end, T{1}}};
    }

    /**
     * @brief 生成一个数值范围的迭代器对，类似于 Python 的 range() 函数。
     * @tparam T 值类型。
     * @param start 起始值。
     * @param end 结束值。
     * @param step 步长。
     * @return IteratorPair<RangeIterator<T>> 可迭代对象。
     */
    template <class T>
    inline IteratorPair<RangeIterator<T>>
    range(const T &start, const T &end, const T &step) {
        return {{start, end, step}, {start, end, step}};
    }
}
