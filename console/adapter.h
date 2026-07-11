/**
 * @file adapter.h
 * @brief 提供带有值返回 pop 操作的容器适配器包装类。
 * @details 该模块对 STL 容器适配器（stack、queue、priority_queue）进行扩展，
 *          重载了 pop() 方法，使其在弹出元素的同时返回该元素的值。
 *          适用于需要在弹出元素时获取其值的场景。
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
#include <string>
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
}
