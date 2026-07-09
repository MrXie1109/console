/**
 * @file gen.h
 * @brief 生成器模式头文件。
 * @details 提供生成器模式的模板类实现，包含多种预定义的生成器和迭代器适配器。
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
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <type_traits>
#include <utility>
#include <vector>

#include "csexc.h"
#include "iter.h"

namespace console {
    /**
     * @brief 生成器模板类，用于创建可迭代的序列生成器。
     * @tparam Derived 派生类类型，用于CRTP模式。
     * @tparam T 生成器产生的值类型。
     */
    template <class Derived, class T>
    class Generator {
    public:
        /**
         * @brief 生成器迭代器类，实现输入迭代器接口。
         */
        class iterator {
            Derived d;

        public:
            using value_type        = T;
            using reference         = T;
            using pointer           = void;
            using difference_type   = std::ptrdiff_t;
            using iterator_category = std::input_iterator_tag;

            /**
             * @brief 构造函数，使用派生类引用初始化迭代器。
             * @param d 派生类对象的引用。
             */
            iterator(const Derived &d) : d(d) {}

            /**
             * @brief 解引用操作符，获取当前值。
             * @return 当前值。
             * @throws StopIteration 如果生成器已完成。
             */
            reference operator*() {
                if (done()) throw StopIteration("StopIteration");
                return d.current();
            }

            /**
             * @brief 前缀自增操作符，移动到下一个元素。
             * @return 自增后的迭代器引用。
             */
            iterator &operator++() {
                if (done()) return *this;
                d.advance();
                return *this;
            }

            /**
             * @brief 后缀自增操作符，移动到下一个元素。
             * @return 自增前的迭代器副本。
             */
            const iterator operator++(int) {
                if (done()) return *this;
                auto old = *this;
                d.advance();
                return old;
            }

            /**
             * @brief 相等比较操作符。
             * @return 如果生成器已完成则返回true，否则返回false。
             */
            bool operator==(const iterator &) { return d.done(); }

            /**
             * @brief 不等比较操作符。
             * @return 如果生成器未完成则返回true，否则返回false。
             */
            bool operator!=(const iterator &) { return !d.done(); }

            /**
             * @brief 获取当前值并移动到下一个位置。
             * @return 当前值。
             * @throws StopIteration 如果生成器已完成。
             */
            T next() {
                if (done()) throw StopIteration("StopIteration");
                auto current = d.current();
                d.advance();
                return current;
            }

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果已完成返回true，否则返回false。
             */
            bool done() { return d.done(); }
        };

        /**
         * @brief 获取生成器的迭代器对象。
         * @return 生成器迭代器。
         */
        iterator iter() { return iterator(static_cast<Derived &>(*this)); }

        /**
         * @brief 获取开始迭代器。
         * @return 生成器的起始迭代器。
         */
        iterator begin() { return iter(); }

        /**
         * @brief 获取结束迭代器。
         * @return 生成器的结束迭代器。
         */
        iterator end() { return iter(); }

        typedef T value_type; ///< 生成器产生的值类型。
    };

    /**
     * @brief 获取生成器迭代器的辅助函数。
     * @param g 生成器对象的引用。
     * @return 生成器迭代器。
     */
    template <class Derived, class T>
    typename Generator<Derived, T>::iterator iter(Generator<Derived, T> &g) {
        return g.iter();
    }

    /**
     * @brief 从迭代器获取下一个值的辅助函数。
     * @param it 生成器迭代器的引用。
     * @return 下一个值。
     */
    template <class Derived, class T>
    T next(typename Generator<Derived, T>::iterator &it) {
        return it.next();
    }

    namespace gen {
        /**
         * @brief 迭代器范围生成器，生成从begin到end的值。
         * @tparam Iter 迭代器类型。
         */
        template <class Iter>
        class Views : public Generator<Views<Iter>,
                          typename std::iterator_traits<Iter>::value_type> {
            Iter curr;
            Iter end_;

        public:
            using value_type = typename std::iterator_traits<Iter>::value_type;

            /**
             * @brief 构造函数，使用迭代器范围初始化。
             * @param begin 起始迭代器。
             * @param end 结束迭代器。
             */
            Views(Iter begin, Iter end) : curr(begin), end_(end) {}

            /**
             * @brief 构造函数，使用容器引用初始化。
             * @tparam Container 容器类型。
             * @param container 容器引用。
             */
            template <class Container>
            Views(const Container &container) :
                curr(std::begin(container)), end_(std::end(container)) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果已到达结束迭代器则返回true。
             */
            bool done() { return curr == end_; }

            /**
             * @brief 获取当前值。
             * @return 当前迭代器指向的值。
             */
            value_type current() { return *curr; }

            /**
             * @brief 向前移动一步。
             */
            void advance() {
                if (curr != end_) ++curr;
            }
        };

        /**
         * @brief 用书数据的序列，从初始化列表构造。
         * @tparam T 元素类型。
         */
        template <class T>
        class List : public Generator<List<T>, T> {
            std::vector<T> data;
            size_t         idx = 0;

        public:
            /**
             * @brief 构造函数，使用初始化列表初始化。
             * @param list 初始化列表。
             */
            List(std::initializer_list<T> list) : data(list) {}

            /**
             * @brief 构造函数，使用迭代器范围初始化。
             * @tparam Iterator 迭代器类型。
             * @param begin 迭代器范围的起始位置。
             * @param end 迭代器范围的结束位置。
             */
            template <class Iterator>
            List(Iterator begin, Iterator end) : data(begin, end) {}

            /**
             * @brief 构造函数，使用容器初始化。
             * @tparam Container 容器类型。
             * @param container 容器。
             */
            template <class Container>
            List(const Container &container) :
                data(std::begin(container), std::end(container)) {}

            /**
             * @brief 检查生成器是否已完成。
             */
            bool done() { return idx >= data.size(); }

            /**
             * @brief 获取当前值。
             */
            T current() { return data[idx]; }

            /**
             * @brief 向前移动一步。
             */
            void advance() { idx++; }
        };
        /**
         * @brief 范围生成器，生成从start到end的等差数列。
         * @tparam T 数值类型。
         */
        template <class T>
        class Range : public Generator<Range<T>, T> {
            T start, curr, end_;
            T step;

        public:
            /**
             * @brief 构造函数。
             * @param begin 起始值。
             * @param end 结束值（不包含）。
             * @param step 步长，默认为1。
             */
            Range(T begin, T end, T step = 1) :
                start(begin), curr(begin), end_(end), step(step) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果已到达结束值则返回true。
             */
            bool done() { return step > 0 ? curr >= end_ : curr <= end_; }

            /**
             * @brief 获取当前值。
             * @return 当前值。
             */
            T current() { return curr; }

            /**
             * @brief 向前移动一步。
             */
            void advance() { curr += step; }
        };

        /**
         * @brief 重复生成器，重复生成同一个值指定次数。
         * @tparam T 值类型。
         */
        template <class T>
        class Repeat : public Generator<Repeat<T>, T> {
            T      value;
            size_t count;
            size_t index = 0;

        public:
            /**
             * @brief 构造函数。
             * @param val 要重复的值。
             * @param n 重复次数。
             */
            Repeat(T val, size_t n) : value(val), count(n) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果已达到指定次数则返回true。
             */
            bool done() { return index >= count; }

            /**
             * @brief 获取当前值。
             * @return 重复的值。
             */
            T current() { return value; }

            /**
             * @brief 向前移动一步。
             */
            void advance() { index++; }
        };

        /**
         * @brief 循环生成器，无限循环遍历给定列表。
         * @tparam T 元素类型。
         */
        template <class T>
        class Cycle : public Generator<Cycle<T>, T> {
            std::vector<T> data;
            size_t         index = 0;

        public:
            /**
             * @brief 构造函数，使用初始化列表。
             * @param list 要循环的列表。
             */
            Cycle(std::initializer_list<T> list) : data(list) {}

            /**
             * @brief 构造函数，使用迭代器范围初始化。
             * @tparam Iterator 迭代器类型。
             * @param begin 迭代器范围的起始位置。
             * @param end 迭代器范围的结束位置。
             */
            template <class Iterator>
            Cycle(Iterator begin, Iterator end) : data(begin, end) {}

            /**
             * @brief 构造函数，使用容器初始化。
             * @tparam Container 容器类型。
             * @param container 容器。
             */
            template <class Container>
            Cycle(const Container &container) :
                data(std::begin(container), std::end(container)) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 始终返回false（无限循环）。
             */
            bool done() { return false; }

            /**
             * @brief 获取当前值。
             * @return 当前循环位置的值。
             */
            T current() { return data[index % data.size()]; }

            /**
             * @brief 向前移动一步。
             */
            void advance() { index++; }
        };

        /**
         * @brief 计数器生成器，生成递增或递减的整数序列。
         */
        template <class T>
        class Counter : public Generator<Counter<T>, T> {
            T curr;
            T step;
            T max_count;
            T count;

        public:
            /**
             * @brief 构造函数，创建无限计数器。
             * @param start 起始值，默认为0。
             * @param step 步长，默认为1。
             */
            Counter(T start = 0, T step = 1) :
                curr(start), step(step), max_count(-1), count(0) {}

            /**
             * @brief 构造函数，创建有限计数器。
             * @param start 起始值。
             * @param times 输出次数。
             * @param step 步长，默认为1。
             */
            Counter(T start, T times, T step) :
                curr(start), step(step), max_count(times), count(0) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果已达到最大计数则返回true。
             * @note 无限计数器永远返回false。
             */
            bool done() { return max_count != -1 && count >= max_count; }

            /**
             * @brief 获取当前值。
             * @return 当前计数值。
             */
            T current() { return curr; }

            /**
             * @brief 向前移动一步。
             * @note 该函数会更新当前计数值和已输出次数。
             */
            void advance() {
                curr += step;
                count++;
            }
        };

        /**
         * @brief 映射生成器，对源生成器的每个元素应用函数。
         * @tparam Gen 源生成器类型。
         * @tparam Func 映射函数类型。
         */
        template <class Gen, class Func>
        class Map
            : public Generator<Map<Gen, Func>,
                  typename std::decay<decltype(std::declval<Func>()(
                      std::declval<typename Gen::iterator::value_type>()))>::
                      type> {
            Gen  gen;
            Func func;
            using ResultType =
                typename std::decay<decltype(func(gen.current()))>::type;

        public:
            /**
             * @brief 构造函数。
             * @param g 源生成器。
             * @param f 映射函数。
             */
            Map(Gen g, Func f) : gen(g), func(f) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果源生成器已完成则返回true。
             */
            bool done() { return gen.done(); }

            /**
             * @brief 获取当前值。
             * @return 映射后的值。
             */
            ResultType current() { return func(gen.current()); }

            /**
             * @brief 向前移动一步。
             */
            void advance() { gen.advance(); }
        };

        /**
         * @brief 过滤生成器，只保留满足谓词的元素。
         * @tparam Gen 源生成器类型。
         * @tparam Pred 谓词函数类型。
         */
        template <class Gen, class Pred>
        class Filter : public Generator<Filter<Gen, Pred>,
                           typename Gen::iterator::value_type> {
            Gen  gen;
            Pred pred;
            bool started = false;

            /**
             * @brief 跳过所有不满足谓词的元素。
             */
            void skip_to_next_valid() {
                while (!gen.done() && !pred(gen.current())) {
                    gen.advance();
                }
                started = true;
            }

        public:
            /**
             * @brief 构造函数。
             * @param g 源生成器。
             * @param p 谓词函数。
             */
            Filter(Gen g, Pred p) : gen(g), pred(p) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果源生成器已完成则返回true。
             */
            bool done() { return gen.done(); }

            /**
             * @brief 获取当前值。
             * @return 当前满足谓词的值。
             */
            auto current() -> decltype(gen.current()) {
                if (!started) skip_to_next_valid();
                return gen.current();
            }

            /**
             * @brief 向前移动一步。
             */
            void advance() {
                gen.advance();
                skip_to_next_valid();
            }
        };

        /**
         * @brief 取前n个元素的生成器适配器。
         * @tparam Gen 源生成器类型。
         */
        template <class Gen>
        class Take : public Generator<Take<Gen>, typename Gen::value_type> {
            Gen    gen;
            size_t count;
            size_t taken = 0;

        public:
            /**
             * @brief 构造函数。
             * @param g 源生成器。
             * @param n 要取的元素个数。
             */
            Take(Gen g, size_t n) : gen(g), count(n) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果已取够数量或源生成器已完成则返回true。
             */
            bool done() { return taken >= count || gen.done(); }

            /**
             * @brief 获取当前值。
             * @return 当前值。
             */
            auto current() -> decltype(gen.current()) { return gen.current(); }

            /**
             * @brief 向前移动一步。
             */
            void advance() {
                gen.advance();
                taken++;
            }
        };

        /**
         * @brief 跳过前n个元素的生成器适配器。
         * @tparam Gen 源生成器类型。
         */
        template <class Gen>
        class Drop
            : public Generator<Drop<Gen>, typename Gen::iterator::value_type> {
            Gen    gen;
            size_t count;
            bool   droped = false;

            /**
             * @brief 执行跳过操作。
             */
            void do_drop() {
                for (size_t i = 0; i < count && !gen.done(); i++) gen.advance();
                droped = true;
            }

        public:
            /**
             * @brief 构造函数。
             * @param g 源生成器。
             * @param n 要跳过的元素个数。
             */
            Drop(Gen g, size_t n) : gen(g), count(n) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果源生成器已完成则返回true。
             */
            bool done() {
                if (!droped) do_drop();
                return gen.done();
            }

            /**
             * @brief 获取当前值。
             * @return 当前值。
             */
            auto current() -> decltype(gen.current()) { return gen.current(); }

            /**
             * @brief 向前移动一步。
             */
            void advance() { gen.advance(); }
        };

        /**
         * @brief 枚举生成器，为每个元素附加索引。
         * @tparam Gen 源生成器类型。
         */
        template <class Gen>
        class Enumerate
            : public Generator<Enumerate<Gen>,
                  std::pair<size_t, typename Gen::iterator::value_type>> {
            Gen    gen;
            size_t index = 0;

        public:
            /**
             * @brief 构造函数。
             * @param g 源生成器。
             */
            Enumerate(Gen g) : gen(g) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果源生成器已完成则返回true。
             */
            bool done() { return gen.done(); }

            /**
             * @brief 获取当前值。
             * @return 包含索引和值的键值对。
             */
            auto current() -> decltype(std::make_pair(index, gen.current())) {
                return std::make_pair(index, gen.current());
            }

            /**
             * @brief 向前移动一步。
             */
            void advance() {
                gen.advance();
                index++;
            }
        };

        /**
         * @brief 生成器，通过反复调用函数生成无限序列。
         * @tparam Func 生成函数类型。
         */
        template <class Func>
        class Generate : public Generator<Generate<Func>,
                             decltype(std::declval<Func>()())> {
            Func             func;
            decltype(func()) curr;

        public:
            /**
             * @brief 构造函数。
             * @param f 生成函数，每次调用返回一个值。
             */
            Generate(Func f) : func(f), curr(func()) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 始终返回false（无限生成）。
             */
            bool done() { return false; }

            /**
             * @brief 获取当前值。
             * @return 调用生成函数返回的值。
             */
            decltype(curr) current() { return curr; }

            /**
             * @brief 向前移动一步。
             */
            void advance() { curr = func(); }
        };

        /**
         * @brief 压缩生成器，将两个生成器的元素配对输出。
         * @tparam Gen1 第一个生成器类型。
         * @tparam Gen2 第二个生成器类型。
         */
        template <class Gen1, class Gen2>
        class Zip : public Generator<Zip<Gen1, Gen2>,
                        std::pair<typename Gen1::value_type,
                            typename Gen2::value_type>> {
            Gen1 gen1;
            Gen2 gen2;

        public:
            /**
             * @brief 构造函数。
             * @param g1 第一个生成器。
             * @param g2 第二个生成器。
             */
            Zip(Gen1 g1, Gen2 g2) : gen1(g1), gen2(g2) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果任一源生成器已完成则返回true。
             */
            bool done() { return gen1.done() || gen2.done(); }

            /**
             * @brief 获取当前值。
             * @return 包含两个生成器当前值的键值对。
             */
            std::pair<typename Gen1::value_type, typename Gen2::value_type>
            current() {
                return std::make_pair(gen1.current(), gen2.current());
            }

            /**
             * @brief 向前移动一步。
             */
            void advance() {
                gen1.advance();
                gen2.advance();
            }
        };

        /**
         * @brief 取得前若干个元素直到谓词不成立。
         * @tparam Gen 源生成器类型。
         * @tparam Pred 谓词类型。
         */
        template <class Gen, class Pred>
        class TakeWhile
            : public Generator<TakeWhile<Gen, Pred>, typename Gen::value_type> {
            Gen  gen;
            Pred pred;
            bool stopped = false;

            void try_advance() {
                if (stopped || gen.done()) return;
                if (!pred(gen.current())) {
                    stopped = true;
                }
            }

        public:
            /**
             * @brief 构造函数。
             * @param g 生成器。
             * @param p 谓词。
             */
            TakeWhile(Gen g, Pred p) : gen(g), pred(p) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果源生成器已完成或已停止则返回true。
             */
            bool done() { return gen.done() || stopped; }

            /**
             * @brief 获取当前值。
             * @return 当前值。
             */
            auto current() -> decltype(gen.current()) { return gen.current(); }

            /**
             * @brief 向前移动一步。
             */
            void advance() {
                gen.advance();
                try_advance();
            }
        };

        /**
         * @brief 跳过前若干个元素直到谓词不成立。
         * @tparam Gen 源生成器类型。
         * @tparam Pred 谓词类型。
         */
        template <class Gen, class Pred>
        class DropWhile
            : public Generator<DropWhile<Gen, Pred>, typename Gen::value_type> {
            Gen  gen;
            Pred pred;
            bool droped = false;

            /**
             * @brief 执行跳过操作。
             */
            void do_drop() {
                while (!gen.done() && pred(gen.current())) gen.advance();
                droped = true;
            }

        public:
            DropWhile(Gen g, Pred p) : gen(g), pred(p) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果源生成器已完成则返回true。
             */
            bool done() {
                if (!droped) do_drop();
                return gen.done();
            }

            /**
             * @brief 获取当前值。
             */
            auto current() -> decltype(gen.current()) { return gen.current(); }

            /**
             * @brief 向前移动一步。
             */
            void advance() { gen.advance(); }
        };

        /**
         * @brief 从迭代器范围创建视图生成器。
         * @tparam Iter 迭代器类型。
         * @param begin 起始迭代器。
         * @param end 结束迭代器。
         * @return Views<Iter> 视图生成器。
         */
        template <class Iter>
        Views<Iter> views(Iter begin, Iter end) {
            return Views<Iter>(begin, end);
        }

        /**
         * @brief 从容器引用创建视图生成器。
         * @tparam Container 容器类型（自动推导）。
         * @param container 容器引用。
         * @return Views<typename Container::iterator> 视图生成器。
         */
        template <class Container>
        Views<typename Container::iterator> views(Container &container) {
            return {std::begin(container), std::end(container)};
        }

        /**
         * @brief 从初始化列表创建列表生成器。
         * @tparam T 元素类型。
         * @param list 初始化列表。
         * @return List<T> 列表生成器。
         */
        template <class T>
        List<T> list(std::initializer_list<T> list) {
            return List<T>(list);
        }

        /**
         * @brief 从迭代器范围创建列表生成器。
         * @tparam Iterator 迭代器类型（自动推导）。
         * @param begin 起始迭代器。
         * @param end 结束迭代器。
         * @return List<typename std::decay<typename Iterator::value_type>::type> 列表生成器。
         */
        template <class Iterator>
        List<typename std::decay<typename Iterator::value_type>::type>
        list(Iterator begin, Iterator end) {
            return List<
                typename std::decay<typename Iterator::value_type>::type>(
                begin, end);
        }

        /**
         * @brief 从容器引用创建列表生成器。
         * @tparam Container 容器类型（自动推导）。
         * @param container 容器引用。
         * @return List<typename Container::value_type> 列表生成器。
         */
        template <class Container>
        List<typename Container::value_type> list(Container &container) {
            return List<typename Container::value_type>(
                std::begin(container), std::end(container));
        }

        /**
         * @brief 创建范围生成器。
         * @tparam T 数值类型。
         * @param begin 起始值。
         * @param end 结束值（不包含）。
         * @param step 步长，默认为1。
         * @return Range<T> 范围生成器。
         */
        template <class T>
        Range<T> range(T begin, T end, T step = 1) {
            return Range<T>(begin, end, step);
        }

        /**
         * @brief 创建重复生成器。
         * @tparam T 值类型。
         * @param val 要重复的值。
         * @param n 重复次数。
         * @return Repeat<T> 重复生成器。
         */
        template <class T>
        Repeat<T> repeat(T val, size_t n) {
            return Repeat<T>(val, n);
        }

        /**
         * @brief 创建循环生成器。
         * @tparam T 元素类型。
         * @param list 要循环的初始化列表。
         * @return Cycle<T> 循环生成器。
         */
        template <class T>
        Cycle<T> cycle(std::initializer_list<T> list) {
            return Cycle<T>(list);
        }

        /**
         * @brief 创建循环生成器。
         * @tparam Iterator 迭代器类型。
         * @param begin 起始迭代器。
         * @param end 结束迭代器。
         * @return Cycle<typename std::decay<typename Iterator::value_type>::type> 循环生成器。
         */
        template <class Iterator>
        Cycle<typename std::decay<typename Iterator::value_type>::type>
        cycle(Iterator begin, Iterator end) {
            return Cycle<
                typename std::decay<typename Iterator::value_type>::type>(
                begin, end);
        }

        /**
         * @brief 创建循环生成器。
         * @tparam Container 容器类型。
         * @param container 容器。
         * @return Cycle<typename Container::value_type> 循环生成器。
         */
        template <class Container>
        Cycle<typename Container::value_type>
        cycle(const Container &container) {
            return Cycle<
                typename std::decay<typename Container::value_type>::type>(
                container.begin(), container.end());
        }

        /**
         * @brief 创建计数器生成器。
         * @tparam T 值类型。
         * @param start 起始值，默认为0。
         * @param step 步长，默认为1。
         * @return Counter<T> 无限计数器。
         */
        template <class T>
        inline Counter<T> counter(T start = 0, T step = 1) {
            return Counter<T>(start, step);
        }

        /**
         * @brief 创建有限计数器生成器。
         * @tparam T 值类型。
         * @param start 起始值。
         * @param times 输出次数。
         * @param step 步长，默认为1。
         * @return Counter<T> 有限计数器。
         */
        template <class T>
        inline Counter<T> counter(T start, T times, T step) {
            return Counter<T>(start, times, step);
        }

        /**
         * @brief 映射适配器（用于管道操作符）。
         * @tparam Func 映射函数类型。
         */
        template <class Func>
        class map_t {
            Func func;

        public:
            explicit map_t(Func f) : func(f) {}

            template <class Gen>
            friend Map<Gen, Func> operator|(Gen g, map_t m) {
                return Map<Gen, Func>(g, m.func);
            }
        };

        /**
         * @brief 创建映射适配器。
         * @tparam Func 映射函数类型。
         * @param f 映射函数。
         * @return map_t<Func> 映射适配器。
         */
        template <class Func>
        map_t<Func> map(Func f) {
            return map_t<Func>(f);
        }

        /**
         * @brief 过滤适配器（用于管道操作符）。
         * @tparam Pred 谓词函数类型。
         */
        template <class Pred>
        class filter_t {
            Pred pred;

        public:
            explicit filter_t(Pred p) : pred(p) {}

            template <class Gen>
            friend Filter<Gen, Pred> operator|(Gen g, filter_t f) {
                return Filter<Gen, Pred>(g, f.pred);
            }
        };

        /**
         * @brief 创建过滤适配器。
         * @tparam Pred 谓词函数类型。
         * @param pred 谓词函数。
         * @return filter_t<Pred> 过滤适配器。
         */
        template <class Pred>
        filter_t<Pred> filter(Pred pred) {
            return filter_t<Pred>(pred);
        }

        /**
         * @brief 取前n个元素适配器（用于管道操作符）。
         */
        class take_t {
            size_t count;

        public:
            explicit take_t(size_t n) : count(n) {}

            template <class Gen>
            friend Take<Gen> operator|(Gen g, take_t t) {
                return Take<Gen>(g, t.count);
            }
        };

        /**
         * @brief 创建取前n个元素适配器。
         * @param n 要取的元素个数。
         * @return take_t 取元素适配器。
         */
        inline take_t take(size_t n) {
            return take_t(n);
        }

        /**
         * @brief 跳过前n个元素适配器（用于管道操作符）。
         */
        class drop_t {
            size_t count;

        public:
            explicit drop_t(size_t n) : count(n) {}

            template <class Gen>
            friend Drop<Gen> operator|(Gen g, drop_t s) {
                return Drop<Gen>(g, s.count);
            }
        };

        /**
         * @brief 创建跳过前n个元素适配器。
         * @param n 要跳过的元素个数。
         * @return drop_t 跳过元素适配器。
         */
        inline drop_t drop(size_t n) {
            return drop_t(n);
        }

        /**
         * @brief 枚举生成器，为每个元素附加索引。
         */
        class enumerate_t {
        public:
            explicit enumerate_t() = default;

            template <class Gen>
            friend Enumerate<Gen> operator|(Gen g, enumerate_t) {
                return Enumerate<Gen>(g);
            }

            template <class Gen>
            Enumerate<Gen> operator()(Gen g) {
                return Enumerate<Gen>(g);
            }
        };

        static constexpr enumerate_t enumerate;

        /**
         * @brief 创建生成器适配器。
         * @tparam Func 生成函数类型。
         * @param func 生成函数。
         * @return Generate<Func> 生成器适配器。
         */
        template <class Func>
        Generate<Func> generate(Func func) {
            return Generate<Func>(func);
        }

        /**
         * @brief 压缩两个生成器。
         * @tparam Gen1 第一个生成器类型。
         * @tparam Gen2 第二个生成器类型。
         * @param g1 第一个生成器。
         * @param g2 第二个生成器。
         * @return Zip<Gen1, Gen2> 压缩生成器。
         */
        template <class Gen1, class Gen2>
        Zip<typename std::decay<Gen1>::type, typename std::decay<Gen2>::type>
        zip(Gen1 &&g1, Gen2 &&g2) {
            return Zip<typename std::decay<Gen1>::type,
                typename std::decay<Gen2>::type>(
                std::forward<Gen1>(g1), std::forward<Gen2>(g2));
        }

        /**
         * @brief 取前若干个元素适配器（用于管道操作符）。
         */
        template <class Pred>
        class take_while_t {
            Pred pred;

        public:
            explicit take_while_t(Pred p) : pred(p) {}

            template <class Gen>
            friend TakeWhile<Gen, Pred> operator|(Gen g, take_while_t tw) {
                return TakeWhile<Gen, Pred>(g, tw.pred);
            }
        };

        /**
         * @brief 创建取前若干个元素适配器。
         * @param p 谓词。
         * @return take_while_t 取元素适配器。
         */
        template <class Pred>
        inline take_while_t<Pred> take_while(Pred pred) {
            return take_while_t<Pred>(pred);
        }

        /**
         * @brief 跳过前若干个元素适配器（用于管道操作符）。
         */
        template <class Pred>
        class drop_while_t {
            Pred pred;

        public:
            explicit drop_while_t(Pred p) : pred(p) {}

            template <class Gen>
            friend DropWhile<Gen, Pred> operator|(Gen g, drop_while_t dw) {
                return DropWhile<Gen, Pred>(g, dw.pred);
            }
        };

        /**
         * @brief 创建跳过前若干个元素适配器。
         * @param p 谓词。
         * @return drop_while_t 取元素适配器。
         */
        template <class Pred>
        inline drop_while_t<Pred> drop_while(Pred pred) {
            return drop_while_t<Pred>(pred);
        }

        /**
         * @brief 容器收集适配器。
         * @tparam Container 目标容器类型，默认为void。
         * @details 将范围中的元素收集到指定容器中。
         *          当Container为void时，自动使用std::vector作为目标容器。
         */
        template <class Container = void>
        class collect {
        private:
            /**
             * @brief 非void版本的实现，将范围元素拷贝到指定容器。
             * @tparam Range 范围类型。
             * @tparam C 容器类型。
             * @param r 输入范围。
             * @return C 包含收集到的元素的容器。
             */
            template <class Range, class C>
            static C impl(Range r, std::false_type) {
                return C(r.begin(), r.end());
            }

            /**
             * @brief void版本的实现，将范围元素拷贝到std::vector。
             * @tparam Range 范围类型。
             * @param r 输入范围。
             * @return std::vector<typename Range::value_type>
             * 包含收集到的元素的vector。
             */
            template <class Range, class>
            static std::vector<
                typename std::decay<typename Range::value_type>::type>
            impl(Range r, std::true_type) {
                return {r.begin(), r.end()};
            }

        public:
            /**
             * @brief 管道操作符，将范围元素收集到容器中。
             * @tparam Range 范围类型。
             * @param r 输入范围。
             * @return Container或std::vector 包含收集到的元素的容器。
             */
            template <class Range>
            friend auto operator|(Range r, collect<Container>)
                -> decltype(collect<Container>::template impl<Range, Container>(
                    r, std::is_same<Container, void>{})) {
                return collect<Container>::template impl<Range, Container>(
                    r, std::is_same<Container, void>{});
            }
        };

        /**
         * @brief 创建目标迭代器适配器。
         * @tparam Iterator 目标迭代器类型。
         */
        template <class Iterator>
        class to_t {
            Iterator it;

        public:
            explicit to_t(Iterator i) : it(i) {}

            template <class Range>
            friend void operator|(Range r, to_t m) {
                std::copy(r.begin(), r.end(), m.it);
            }
        };

        /**
         * @brief 创建目标迭代器适配器。
         * @tparam Iterator 目标迭代器类型。
         * @param it 目标迭代器。
         * @return to_t<Iterator> 目标迭代器适配器。
         */
        template <class Iterator>
        to_t<Iterator> to(Iterator it) {
            return to_t<Iterator>(it);
        }

        /**
         * @brief 按位与操作符重载，用于压缩两个生成器。
         * @tparam Gen1 第一个生成器类型。
         * @tparam Gen2 第二个生成器类型。
         * @param g1 第一个生成器。
         * @param g2 第二个生成器。
         * @return Zip<Gen1, Gen2> 压缩生成器。
         */
        template <class Gen1, class Gen2>
        auto operator&(
            Gen1 &&g1, Gen2 &&g2) -> decltype(gen::zip(std::forward<Gen1>(g1),
                                      std::forward<Gen2>(g2))) {
            return gen::zip(std::forward<Gen1>(g1), std::forward<Gen2>(g2));
        }
    }

    namespace ops {
        /**
         * @brief 加固定值变换器。
         * @tparam T 固定值类型。
         */
        template <class T>
        struct Add {
            T value;

            /**
             * @brief 对输入值加上固定值。
             * @param x 输入值。
             * @return x + value。
             */
            template <class U>
            auto operator()(U x) const -> decltype(x + value) {
                return x + value;
            }
        };

        /**
         * @brief 减固定值变换器。
         * @tparam T 固定值类型。
         */
        template <class T>
        struct Sub {
            T value;

            /**
             * @brief 对输入值减去固定值。
             * @param x 输入值。
             * @return x - value。
             */
            template <class U>
            auto operator()(U x) const -> decltype(x - value) {
                return x - value;
            }
        };

        /**
         * @brief 乘固定值变换器。
         * @tparam T 固定值类型。
         */
        template <class T>
        struct Mul {
            T value;

            /**
             * @brief 对输入值乘以固定值。
             * @param x 输入值。
             * @return x * value。
             */
            template <class U>
            auto operator()(U x) const -> decltype(x * value) {
                return x * value;
            }
        };

        /**
         * @brief 除固定值变换器。
         * @tparam T 固定值类型。
         */
        template <class T>
        struct Div {
            T value;

            /**
             * @brief 对输入值除以固定值。
             * @param x 输入值。
             * @return x / value。
             */
            template <class U>
            auto operator()(U x) const -> decltype(x / value) {
                return x / value;
            }
        };

        /**
         * @brief 取模固定值变换器。
         * @tparam T 固定值类型。
         */
        template <class T>
        struct Mod {
            T value;

            /**
             * @brief 对输入值取模固定值。
             * @param x 输入值。
             * @return x % value。
             */
            template <class U>
            auto operator()(U x) const -> decltype(x % value) {
                return x % value;
            }
        };

        /**
         * @brief 自增变换器。
         */
        struct inc_t {
            /**
             * @brief 对输入值自增1。
             * @param x 输入值。
             * @return ++x。
             */
            template <class T>
            auto
            operator()(T x) const -> typename std::decay<decltype(++x)>::type {
                return ++x;
            }
        };

        static constexpr inc_t inc;

        /**
         * @brief 自减变换器。
         */
        struct dec_t {
            /**
             * @brief 对输入值自减1。
             * @param x 输入值。
             * @return --x。
             */
            template <class T>
            auto
            operator()(T x) const -> typename std::decay<decltype(--x)>::type {
                return --x;
            }
        };

        static constexpr dec_t dec;

        /**
         * @brief 取负变换器。
         */
        struct negate_t {
            /**
             * @brief 对输入值取负。
             * @param x 输入值。
             * @return -x。
             */
            template <class T>
            auto operator()(T x) const -> decltype(-x) {
                return -x;
            }
        };

        static constexpr negate_t negate;

        /**
         * @brief 平方变换器。
         */
        struct square_t {
            /**
             * @brief 计算输入值的平方。
             * @param x 输入值。
             * @return x * x。
             */
            template <class T>
            auto operator()(T x) const -> decltype(x * x) {
                return x * x;
            }
        };

        static constexpr square_t square;

        /**
         * @brief 立方变换器。
         */
        struct cube_t {
            /**
             * @brief 计算输入值的立方。
             * @param x 输入值。
             * @return x * x * x。
             */
            template <class T>
            auto operator()(T x) const -> decltype(x * x * x) {
                return x * x * x;
            }
        };

        static constexpr cube_t cube;

        /**
         * @brief 平方根变换器。
         */
        struct sqrt_t {
            /**
             * @brief 计算输入值的平方根。
             * @param x 输入值。
             * @return std::sqrt(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::sqrt(x)) {
                return std::sqrt(x);
            }
        };

        static constexpr sqrt_t sqrt;

        /**
         * @brief 倒数变换器。
         */
        struct reciprocal_t {
            /**
             * @brief 计算输入值的倒数。
             * @param x 输入值。
             * @return 1 / x。
             */
            template <class T>
            auto operator()(T x) const -> decltype(1 / x) {
                return 1 / x;
            }
        };

        static constexpr reciprocal_t reciprocal;

        /**
         * @brief 绝对值变换器。
         */
        struct abs_t {
            /**
             * @brief 计算输入值的绝对值。
             * @param x 输入值。
             * @return std::abs(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::abs(x)) {
                return std::abs(x);
            }
        };

        static constexpr abs_t abs;

        /**
         * @brief 转为字符串变换器。
         */
        struct to_string_t {
            /**
             * @brief 将输入值转换为字符串。
             * @param x 输入值。
             * @return std::to_string(x)。
             */
            template <class T>
            std::string operator()(const T &x) const {
                return std::to_string(x);
            }
        };

        static constexpr to_string_t to_string;

        /**
         * @brief 大于谓词。
         * @tparam T 阈值类型。
         */
        template <class T>
        struct Gt {
            T threshold;

            /**
             * @brief 判断输入值是否大于阈值。
             * @param x 输入值。
             * @return x > threshold。
             */
            template <class U>
            bool operator()(const U &x) const {
                return x > threshold;
            }
        };

        /**
         * @brief 大于等于谓词。
         * @tparam T 阈值类型。
         */
        template <class T>
        struct Ge {
            T threshold;

            /**
             * @brief 判断输入值是否大于等于阈值。
             * @param x 输入值。
             * @return x >= threshold。
             */
            template <class U>
            bool operator()(const U &x) const {
                return x >= threshold;
            }
        };

        /**
         * @brief 小于谓词。
         * @tparam T 阈值类型。
         */
        template <class T>
        struct Lt {
            T threshold;

            /**
             * @brief 判断输入值是否小于阈值。
             * @param x 输入值。
             * @return x < threshold。
             */
            template <class U>
            bool operator()(const U &x) const {
                return x < threshold;
            }
        };

        /**
         * @brief 小于等于谓词。
         * @tparam T 阈值类型。
         */
        template <class T>
        struct Le {
            T threshold;

            /**
             * @brief 判断输入值是否小于等于阈值。
             * @param x 输入值。
             * @return x <= threshold。
             */
            template <class U>
            bool operator()(const U &x) const {
                return x <= threshold;
            }
        };

        /**
         * @brief 等于谓词。
         * @tparam T 比较值类型。
         */
        template <class T>
        struct Eq {
            T value;

            /**
             * @brief 判断输入值是否等于指定值。
             * @param x 输入值。
             * @return x == value。
             */
            template <class U>
            bool operator()(const U &x) const {
                return x == value;
            }
        };

        /**
         * @brief 不等于谓词。
         * @tparam T 比较值类型。
         */
        template <class T>
        struct Ne {
            T value;

            /**
             * @brief 判断输入值是否不等于指定值。
             * @param x 输入值。
             * @return x != value。
             */
            template <class U>
            bool operator()(const U &x) const {
                return x != value;
            }
        };

        /**
         * @brief 偶数谓词。
         */
        struct even_t {
            /**
             * @brief 判断输入值是否为偶数。
             * @param x 输入值。
             * @return x % 2 == 0。
             */
            template <class T>
            bool operator()(T x) const {
                return x % 2 == 0;
            }
        };

        static constexpr even_t even;

        /**
         * @brief 奇数谓词。
         */
        struct odd_t {
            /**
             * @brief 判断输入值是否为奇数。
             * @param x 输入值。
             * @return x % 2 != 0。
             */
            template <class T>
            bool operator()(T x) const {
                return x % 2 != 0;
            }
        };

        static constexpr odd_t odd;

        /**
         * @brief 正数谓词。
         */
        struct positive_t {
            /**
             * @brief 判断输入值是否为正数。
             * @param x 输入值。
             * @return x > 0。
             */
            template <class T>
            bool operator()(T x) const {
                return x > 0;
            }
        };

        static constexpr positive_t positive;

        /**
         * @brief 负数谓词。
         */
        struct negative_t {
            /**
             * @brief 判断输入值是否为负数。
             * @param x 输入值。
             * @return x < 0。
             */
            template <class T>
            bool operator()(T x) const {
                return x < 0;
            }
        };

        static constexpr negative_t negative;

        /**
         * @brief 零谓词。
         */
        struct zero_t {
            /**
             * @brief 判断输入值是否为零。
             * @param x 输入值。
             * @return x == 0。
             */
            template <class T>
            bool operator()(T x) const {
                return x == 0;
            }
        };

        static constexpr zero_t zero;

        /**
         * @brief 素数谓词。
         */
        struct prime_t {
            /**
             * @brief 判断输入值是否为素数。
             * @param n 输入值。
             * @return 是否为素数。
             */
            template <class Int>
            bool operator()(Int n) const {
                if (n <= 1) return false;
                for (size_t i = 2; i * i <= n; ++i)
                    if (n % i == 0) return false;
                return true;
            }
        };

        static constexpr prime_t prime;

        /**
         * @brief 逻辑与组合器。
         * @tparam P 第一个谓词类型。
         * @tparam Q 第二个谓词类型。
         */
        template <class P, typename Q>
        struct And {
            P p;
            Q q;

            /**
             * @brief 对输入值同时应用两个谓词并返回逻辑与结果。
             * @param x 输入值。
             * @return p(x) && q(x)。
             */
            template <class T>
            bool operator()(const T &x) const {
                return p(x) && q(x);
            }
        };

        /**
         * @brief 逻辑或组合器。
         * @tparam P 第一个谓词类型。
         * @tparam Q 第二个谓词类型。
         */
        template <class P, typename Q>
        struct Or {
            P p;
            Q q;

            /**
             * @brief 对输入值同时应用两个谓词并返回逻辑或结果。
             * @param x 输入值。
             * @return p(x) || q(x)。
             */
            template <class T>
            bool operator()(const T &x) const {
                return p(x) || q(x);
            }
        };

        /**
         * @brief 逻辑非组合器。
         * @tparam P 谓词类型。
         */
        template <class P>
        struct Not {
            P p;

            /**
             * @brief 对输入值应用谓词并返回逻辑非结果。
             * @param x 输入值。
             * @return !p(x)。
             */
            template <class T>
            bool operator()(const T &x) const {
                return !p(x);
            }
        };

        /**
         * @brief 取成员变量变换器。
         * @tparam Class 类类型。
         * @tparam Member 成员类型。
         */
        template <class Class, typename Mem>
        struct Member {
            Mem Class::*ptr;

            /**
             * @brief 获取对象的成员变量。
             * @param obj 对象。
             * @return obj.*ptr。
             */
            template <class T>
            auto operator()(const T &obj) const ->
                typename std::decay<decltype(obj.*ptr)>::type {
                return obj.*ptr;
            }
        };

        /**
         * @brief 取 pair 的 first 变换器。
         */
        struct first_t {
            /**
             * @brief 获取 pair 的第一个元素。
             * @param p pair 对象。
             * @return p.first。
             */
            template <class Pair>
            typename Pair::first_type operator()(const Pair &p) const {
                return p.first;
            }
        };

        static constexpr first_t first;

        /**
         * @brief 取 pair 的 second 变换器。
         */
        struct second_t {
            /**
             * @brief 获取 pair 的第二个元素。
             * @param p pair 对象。
             * @return p.second。
             */
            template <class Pair>
            typename Pair::second_type operator()(const Pair &p) const {
                return p.second;
            }
        };

        static constexpr second_t second;

        /**
         * @brief 相邻去重过滤器。
         * @tparam T 元素类型。
         */
        template <class T>
        struct unique {
            bool first = true;
            T    last;

            /**
             * @brief 判断当前元素是否与前一个元素不同。
             * @param x 当前元素。
             * @return 是否保留该元素。
             */
            bool operator()(const T &x) {
                if (first) {
                    first = false;
                    last  = x;
                    return true;
                }
                if (last == x) return false;
                last = x;
                return true;
            }
        };

        /**
         * @brief 恒等变换器。
         */
        struct identity_t {
            /**
             * @brief 返回输入值本身。
             * @param x 输入值。
             * @return x。
             */
            template <class T>
            T operator()(T x) const {
                return x;
            }
        };

        static constexpr identity_t identity;

        /**
         * @brief 转型变化器。
         * @tparam T 目标元素类型。
         */
        template <class T>
        struct as {
            /**
             * @brief 返回转型后的值。
             * @tparam U 输入元素类型。
             * @param x 输入值。
             * @return T 转化后的值。
             */
            template <class U>
            T operator()(U &&x) {
                return static_cast<T>(std::forward<U>(x));
            }
        };

        /**
         * @brief 创建加固定值变换器。
         * @tparam T 值类型（自动推导）。
         * @param v 要加的值。
         */
        template <class T>
        Add<T> add(T v) {
            return {v};
        }

        /**
         * @brief 创建减固定值变换器。
         * @param v 要减的值。
         */
        template <class T>
        Sub<T> sub(T v) {
            return {v};
        }

        /**
         * @brief 创建乘固定值变换器。
         * @param v 要乘的值。
         */
        template <class T>
        Mul<T> mul(T v) {
            return {v};
        }

        /**
         * @brief 创建除固定值变换器。
         * @param v 要除的值。
         */
        template <class T>
        Div<T> div(T v) {
            return {v};
        }

        /**
         * @brief 创建取模固定值变换器。
         * @param v 要取模的值。
         */
        template <class T>
        Mod<T> mod(T v) {
            return {v};
        }

        /**
         * @brief 创建大于谓词。
         */
        template <class T>
        Gt<T> gt(T t) {
            return {t};
        }

        /**
         * @brief 创建大于等于谓词。
         */
        template <class T>
        Ge<T> ge(T t) {
            return {t};
        }

        /**
         * @brief 创建小于谓词。
         */
        template <class T>
        Lt<T> lt(T t) {
            return {t};
        }

        /**
         * @brief 创建小于等于谓词。
         */
        template <class T>
        Le<T> le(T t) {
            return {t};
        }

        /**
         * @brief 创建等于谓词。
         */
        template <class T>
        Eq<T> eq(T v) {
            return {v};
        }

        /**
         * @brief 创建不等于谓词。
         */
        template <class T>
        Ne<T> ne(T v) {
            return {v};
        }

        /**
         * @brief 创建逻辑与组合谓词。
         */
        template <class P, typename Q>
        And<P, Q> and_(P p, Q q) {
            return {p, q};
        }

        /**
         * @brief 创建逻辑或组合谓词。
         */
        template <class P, typename Q>
        Or<P, Q> or_(P p, Q q) {
            return {p, q};
        }

        /**
         * @brief 创建逻辑非组合谓词。
         */
        template <class P>
        Not<P> not_(P p) {
            return {p};
        }

        /**
         * @brief 创建成员变量访问器。
         */
        template <class Class, typename Mem>
        Member<Class, Mem> member(Mem Class::*p) {
            return {p};
        }

        /**
         * @brief 逻辑与运算符，等价于 and_(p, q)。
         * @note 小范围内的重载，依赖 ADL，作用域可控。
         */
        template <class P, class Q>
        And<P, Q> operator&&(P p, Q q) {
            return {p, q};
        }

        /**
         * @brief 逻辑或运算符，等价于 or_(p, q)。
         * @note 小范围内的重载，依赖 ADL，作用域可控。
         */
        template <class P, class Q>
        Or<P, Q> operator||(P p, Q q) {
            return {p, q};
        }

        /**
         * @brief 逻辑非运算符，等价于 not_(p)。
         * @note 小范围内的重载，依赖 ADL，作用域可控。
         */
        template <class P>
        Not<P> operator!(P p) {
            return {p};
        }

        /**
         * @brief 函数组合器，将两个函数组合为一个函数。
         * @tparam F 第一个函数类型。
         * @tparam G 第二个函数类型。
         */
        template <class F, class G>
        struct Compose {
            F f;
            G g;

            /**
             * @brief 对输入值应用两个函数的组合。
             * @tparam T 输入值类型。
             * @param x 输入值。
             * @return g(f(x))。
             */
            template <class T>
            auto operator()(T x) const -> decltype(g(f(x))) {
                return g(f(x));
            }
        };

        /**
         * @brief 将两个函数组合为一个函数。
         * @tparam F 第一个函数类型。
         * @tparam G 第二个函数类型。
         * @param f 第一个函数。
         * @param g 第二个函数。
         * @return Compose<F, G> 组合后的函数。
         */
        template <class F, class G>
        Compose<F, G> compose(F f, G g) {
            return {f, g};
        }

        /**
         * @brief 将多个函数组合为一个函数。
         * @tparam F 第一个函数类型。
         * @tparam ...Rest 其余函数类型。
         * @param f 第一个函数。
         * @param rest 其余函数。
         * @return Compose<F, Compose<Rest...>> 组合后的函数。
         */
        template <class F, class... Rest>
        auto
        compose(F f, Rest... rest) -> Compose<F, decltype(compose(rest...))> {
            return {f, compose(rest...)};
        }

        /**
         * @brief 函数组合运算符，将两个函数组合为一个函数。
         * @tparam F 第一个函数类型。
         * @tparam G 第二个函数类型。
         * @param f 第一个函数。
         * @param g 第二个函数。
         * @return Compose<F, G> 组合后的函数。
         */
        template <class F, class G>
        Compose<F, G> operator>>(F f, G g) {
            return {f, g};
        }
    }
}
