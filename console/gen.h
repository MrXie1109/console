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
#include <vector>
#include <utility>
#include <iterator>
#include <cstdlib>
#include <cmath>
#include <type_traits>
#include <vector>
#include "csexc.h"

namespace console
{
    /**
     * @brief 生成器模板类，用于创建可迭代的序列生成器。
     * @tparam Derived 派生类类型，用于CRTP模式。
     * @tparam T 生成器产生的值类型。
     */
    template <class Derived, class T>
    class Generator
    {
    public:
        /**
         * @brief 生成器迭代器类，实现输入迭代器接口。
         */
        class iterator
        {
            Derived &d;

        public:
            using value_type = T;
            using reference = T;
            using pointer = void;
            using difference_type = std::ptrdiff_t;
            using iterator_category = std::input_iterator_tag;

            /**
             * @brief 构造函数，使用派生类引用初始化迭代器。
             * @param d 派生类对象的引用。
             */
            iterator(Derived &d) : d(d) {}

            /**
             * @brief 解引用操作符，获取当前值。
             * @return 当前值。
             * @throws StopIteration 如果生成器已完成。
             */
            reference operator*()
            {
                if (done())
                    throw StopIteration("StopIteration");
                return d.current();
            }

            /**
             * @brief 前缀自增操作符，移动到下一个元素。
             * @return 自增后的迭代器引用。
             */
            iterator &operator++()
            {
                if (done())
                    return *this;
                d.advance();
                return *this;
            }

            /**
             * @brief 后缀自增操作符，移动到下一个元素。
             * @return 自增前的迭代器副本。
             */
            const iterator operator++(int)
            {
                if (done())
                    return *this;
                auto old = *this;
                d.advance();
                return old;
            }

            /**
             * @brief 相等比较操作符。
             * @return 如果生成器已完成则返回true，否则返回false。
             */
            bool operator==(const iterator &) const { return d.done(); }

            /**
             * @brief 不等比较操作符。
             * @return 如果生成器未完成则返回true，否则返回false。
             */
            bool operator!=(const iterator &) const { return !d.done(); }

            /**
             * @brief 获取当前值并移动到下一个位置。
             * @return 当前值。
             * @throws StopIteration 如果生成器已完成。
             */
            T next()
            {
                if (done())
                    throw StopIteration("StopIteration");
                auto current = d.current();
                d.advance();
                return current;
            }

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果已完成返回true，否则返回false。
             */
            bool done()
            {
                return d.done();
            }
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
    typename Generator<Derived, T>::iterator iter(Generator<Derived, T> &g)
    {
        return g.iter();
    }

    /**
     * @brief 从迭代器获取下一个值的辅助函数。
     * @param it 生成器迭代器的引用。
     * @return 下一个值。
     */
    template <class Derived, class T>
    T next(typename Generator<Derived, T>::iterator &it)
    {
        return it.next();
    }

    namespace gen
    {
        /**
         * @brief 迭代器范围生成器，生成从begin到end的值。
         * @tparam Iter 迭代器类型。
         */
        template <class Iter>
        class Views : public Generator<Views<Iter>, typename std::iterator_traits<Iter>::value_type>
        {
            Iter curr;
            Iter end;

        public:
            using value_type = typename std::iterator_traits<Iter>::value_type;

            /**
             * @brief 构造函数，使用迭代器范围初始化。
             * @param begin 起始迭代器。
             * @param end 结束迭代器。
             */
            Views(Iter begin, Iter end)
                : curr(begin), end(end) {}

            /**
             * @brief 构造函数，使用容器引用初始化。
             * @tparam Container 容器类型。
             * @param container 容器引用。
             */
            template <class Container>
            Views(const Container &container)
                : curr(std::begin(container)), end(std::end(container)) {}

            /**
             * @brief 构造函数，使用初始化列表。
             * @param list 初始化列表。
             */
            Views(std::initializer_list<value_type> list)
                : curr(list.begin()), end(list.end()) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果已到达结束迭代器则返回true。
             */
            bool done() { return curr == end; }

            /**
             * @brief 获取当前值。
             * @return 当前迭代器指向的值。
             */
            value_type current() { return *curr; }

            /**
             * @brief 向前移动一步。
             */
            void advance()
            {
                if (curr != end)
                    ++curr;
            }
        };

        /**
         * @brief 范围生成器，生成从start到end的等差数列。
         * @tparam T 数值类型。
         */
        template <class T>
        class Range : public Generator<Range<T>, T>
        {
            T start, curr, end_;
            T step;

        public:
            /**
             * @brief 构造函数。
             * @param begin 起始值。
             * @param end 结束值（不包含）。
             * @param step 步长，默认为1。
             */
            Range(T begin, T end, T step = 1)
                : start(begin), curr(begin), end_(end), step(step) {}

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
        class Repeat : public Generator<Repeat<T>, T>
        {
            T value;
            int count;
            int index = 0;

        public:
            /**
             * @brief 构造函数。
             * @param val 要重复的值。
             * @param n 重复次数。
             */
            Repeat(T val, int n) : value(val), count(n) {}

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
        class Cycle : public Generator<Cycle<T>, T>
        {
            std::vector<T> data;
            size_t index = 0;

        public:
            /**
             * @brief 构造函数，使用初始化列表。
             * @param list 要循环的列表。
             */
            Cycle(std::initializer_list<T> list) : data(list) {}

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
        class Counter : public Generator<Counter, int>
        {
            int curr;
            int step;
            int max_count;
            int count;

        public:
            /**
             * @brief 构造函数，创建无限计数器。
             * @param start 起始值，默认为0。
             * @param step 步长，默认为1。
             */
            Counter(int start = 0, int step = 1)
                : curr(start), step(step), max_count(-1), count(0) {}

            /**
             * @brief 构造函数，创建有限计数器。
             * @param start 起始值。
             * @param times 输出次数。
             * @param step 步长，默认为1。
             */
            Counter(int start, int times, int step)
                : curr(start), step(step), max_count(times), count(0) {}

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
            int current() { return curr; }

            /**
             * @brief 向前移动一步。
             * @note 该函数会更新当前计数值和已输出次数。
             */
            void advance()
            {
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
                               decltype(std::declval<Func>()(
                                   std::declval<typename Gen::iterator::value_type>()))>
        {
            Gen gen;
            Func func;
            using ResultType = decltype(func(gen.current()));

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
        template <class Gen, typename Pred>
        class Filter : public Generator<Filter<Gen, Pred>,
                                        typename Gen::iterator::value_type>
        {
            Gen gen;
            Pred pred;
            bool started = false;

            /**
             * @brief 跳过所有不满足谓词的元素。
             */
            void
            skip_to_next_valid()
            {
                while (!gen.done() && !pred(gen.current()))
                {
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
            auto current() -> decltype(gen.current())
            {
                if (!started)
                    skip_to_next_valid();
                return gen.current();
            }

            /**
             * @brief 向前移动一步。
             */
            void advance()
            {
                gen.advance();
                skip_to_next_valid();
            }
        };

        /**
         * @brief 取前n个元素的生成器适配器。
         * @tparam Gen 源生成器类型。
         */
        template <class Gen>
        class Take : public Generator<Take<Gen>,
                                      typename Gen::value_type>
        {
            Gen gen;
            int count;
            int taken = 0;

        public:
            /**
             * @brief 构造函数。
             * @param g 源生成器。
             * @param n 要取的元素个数。
             */
            Take(Gen g, int n) : gen(g), count(n) {}

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
            void advance()
            {
                gen.advance();
                taken++;
            }
        };

        /**
         * @brief 跳过前n个元素的生成器适配器。
         * @tparam Gen 源生成器类型。
         */
        template <class Gen>
        class Skip : public Generator<Skip<Gen>,
                                      typename Gen::iterator::value_type>
        {
            Gen gen;
            int count;
            bool skipped = false;

            /**
             * @brief 执行跳过操作。
             */
            void do_skip()
            {
                for (int i = 0; i < count && !gen.done(); i++)
                {
                    gen.advance();
                }
                skipped = true;
            }

        public:
            /**
             * @brief 构造函数。
             * @param g 源生成器。
             * @param n 要跳过的元素个数。
             */
            Skip(Gen g, int n) : gen(g), count(n) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果源生成器已完成则返回true。
             */
            bool done()
            {
                if (!skipped)
                    do_skip();
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
                               std::pair<int, typename Gen::iterator::value_type>>
        {
            Gen gen;
            int index = 0;

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
            auto current() -> decltype(std::make_pair(index, gen.current()))
            {
                return std::make_pair(index, gen.current());
            }

            /**
             * @brief 向前移动一步。
             */
            void advance()
            {
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
                                          decltype(std::declval<Func>()())>
        {
            Func func;

        public:
            /**
             * @brief 构造函数。
             * @param f 生成函数，每次调用返回一个值。
             */
            Generate(Func f) : func(f) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 始终返回false（无限生成）。
             */
            bool done() { return false; }

            /**
             * @brief 获取当前值。
             * @return 调用生成函数返回的值。
             */
            auto current() -> decltype(func()) { return func(); }

            /**
             * @brief 向前移动一步（无操作）。
             */
            void advance() {}
        };

        /**
         * @brief 压缩生成器，将两个生成器的元素配对输出。
         * @tparam Gen1 第一个生成器类型。
         * @tparam Gen2 第二个生成器类型。
         */
        template <class Gen1, class Gen2>
        class Zip : public Generator<Zip<Gen1, Gen2>,
                                     std::pair<typename Gen1::value_type,
                                               typename Gen2::value_type>>
        {
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
            current()
            {
                return std::make_pair(gen1.current(), gen2.current());
            }

            /**
             * @brief 向前移动一步。
             */
            void advance()
            {
                gen1.advance();
                gen2.advance();
            }
        };

        /**
         * @brief 从迭代器范围创建视图生成器。
         * @tparam Iter 迭代器类型。
         * @param begin 起始迭代器。
         * @param end 结束迭代器。
         * @return Views<Iter> 视图生成器。
         */
        template <class Iter>
        Views<Iter> views(Iter begin, Iter end)
        {
            return Views<Iter>(begin, end);
        }

        /**
         * @brief 从容器引用创建视图生成器。
         * @tparam Container 容器类型（自动推导）。
         * @param container 容器引用。
         * @return Views<typename Container::iterator> 视图生成器。
         */
        template <class Container>
        Views<typename Container::iterator>
        views(Container &container)
        {
            return {std::begin(container), std::end(container)};
        }

        /**
         * @brief 从初始化列表创建视图生成器。
         * @tparam T 初始化列表元素类型。
         * @param list 初始化列表。
         * @return Views<typename std::initializer_list<T>::iterator> 视图生成器。
         */
        template <class T>
        Views<typename std::initializer_list<T>::iterator>
        views(std::initializer_list<T> list)
        {
            return {list.begin(), list.end()};
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
        Range<T> range(T begin, T end, T step = 1)
        {
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
        Repeat<T> repeat(T val, int n)
        {
            return Repeat<T>(val, n);
        }

        /**
         * @brief 创建循环生成器。
         * @tparam T 元素类型。
         * @param list 要循环的初始化列表。
         * @return Cycle<T> 循环生成器。
         */
        template <class T>
        Cycle<T> cycle(std::initializer_list<T> list)
        {
            return Cycle<T>(list);
        }

        /**
         * @brief 创建计数器生成器。
         * @param start 起始值，默认为0。
         * @param step 步长，默认为1。
         * @return Counter 无限计数器。
         */
        inline Counter counter(int start = 0, int step = 1)
        {
            return Counter(start, step);
        }

        /**
         * @brief 创建有限计数器生成器。
         * @param start 起始值。
         * @param times 输出次数。
         * @param step 步长，默认为1。
         * @return Counter 有限计数器。
         */
        inline Counter counter(int start, int times, int step = 1)
        {
            return Counter(start, times, step);
        }

        /**
         * @brief 映射适配器（用于管道操作符）。
         * @tparam Func 映射函数类型。
         */
        template <class Func>
        class map_t
        {
            Func func;

        public:
            explicit map_t(Func f) : func(f) {}

            template <class Gen>
            friend Map<Gen, Func> operator|(Gen g, map_t m)
            {
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
        map_t<Func> map(Func f)
        {
            return map_t<Func>(f);
        }

        /**
         * @brief 过滤适配器（用于管道操作符）。
         * @tparam Pred 谓词函数类型。
         */
        template <class Pred>
        class filter_t
        {
            Pred pred;

        public:
            explicit filter_t(Pred p) : pred(p) {}

            template <class Gen>
            friend Filter<Gen, Pred> operator|(Gen g, filter_t f)
            {
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
        filter_t<Pred> filter(Pred pred)
        {
            return filter_t<Pred>(pred);
        }

        /**
         * @brief 取前n个元素适配器（用于管道操作符）。
         */
        class take_t
        {
            int count;

        public:
            explicit take_t(int n) : count(n) {}

            template <class Gen>
            friend Take<Gen> operator|(Gen g, take_t t)
            {
                return Take<Gen>(g, t.count);
            }
        };

        /**
         * @brief 创建取前n个元素适配器。
         * @param n 要取的元素个数。
         * @return take_t 取元素适配器。
         */
        inline take_t take(int n)
        {
            return take_t(n);
        }

        /**
         * @brief 跳过前n个元素适配器（用于管道操作符）。
         */
        class skip_t
        {
            int count;

        public:
            explicit skip_t(int n) : count(n) {}

            template <class Gen>
            friend Skip<Gen> operator|(Gen g, skip_t s)
            {
                return Skip<Gen>(g, s.count);
            }
        };

        /**
         * @brief 创建跳过前n个元素适配器。
         * @param n 要跳过的元素个数。
         * @return skip_t 跳过元素适配器。
         */
        inline skip_t skip(int n)
        {
            return skip_t(n);
        }

        /**
         * @brief 枚举生成器，为每个元素附加索引。
         */
        class enumerate_t
        {
        public:
            explicit enumerate_t() {}

            template <class Gen>
            friend Enumerate<Gen> operator|(Gen g, enumerate_t)
            {
                return Enumerate<Gen>(g);
            }

            template <class Gen>
            Enumerate<Gen> operator()(Gen g)
            {
                return Enumerate<Gen>(g);
            }
        } enumerate;

        /**
         * @brief 创建生成器适配器。
         * @tparam Func 生成函数类型。
         * @param func 生成函数。
         * @return Generate<Func> 生成器适配器。
         */
        template <class Func>
        Generate<Func> generate(Func func)
        {
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
        Zip<typename std::decay<Gen1>::type,
            typename std::decay<Gen2>::type>
        zip(Gen1 &&g1, Gen2 &&g2)
        {
            return Zip<typename std::decay<Gen1>::type,
                       typename std::decay<Gen2>::type>(
                std::forward<Gen1>(g1),
                std::forward<Gen2>(g2));
        }

        /**
         * @brief 容器收集适配器。
         * @tparam Container 目标容器类型，默认为void。
         * @details 将范围中的元素收集到指定容器中。
         *          当Container为void时，自动使用std::vector作为目标容器。
         */
        template <class Container = void>
        class collect
        {
        private:
            /**
             * @brief 非void版本的实现，将范围元素拷贝到指定容器。
             * @tparam Range 范围类型。
             * @tparam C 容器类型。
             * @param r 输入范围。
             * @return C 包含收集到的元素的容器。
             */
            template <class Range, class C>
            static C impl(Range r, std::false_type)
            {
                return C(r.begin(), r.end());
            }

            /**
             * @brief void版本的实现，将范围元素拷贝到std::vector。
             * @tparam Range 范围类型。
             * @param r 输入范围。
             * @return std::vector<typename Range::value_type> 包含收集到的元素的vector。
             */
            template <class Range, class>
            static std::vector<typename Range::value_type> impl(Range r, std::true_type)
            {
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
                    r, std::is_same<Container, void>{}))
            {
                return collect<Container>::template impl<Range, Container>(
                    r, std::is_same<Container, void>{});
            }
        };

        /**
         * @brief 按位与操作符重载，用于压缩两个生成器。
         * @tparam Gen1 第一个生成器类型。
         * @tparam Gen2 第二个生成器类型。
         * @param g1 第一个生成器。
         * @param g2 第二个生成器。
         * @return Zip<Gen1, Gen2> 压缩生成器。
         */
        template <class Gen1, class Gen2>
        auto operator&(Gen1 &&g1, Gen2 &&g2)
            -> decltype(gen::zip(std::forward<Gen1>(g1), std::forward<Gen2>(g2)))
        {
            return gen::zip(std::forward<Gen1>(g1), std::forward<Gen2>(g2));
        }
    }

    namespace ops
    {
        /**
         * @brief 加固定值变换器。
         * @tparam T 固定值类型。
         */
        template <class T>
        struct Add
        {
            T value;

            /**
             * @brief 对输入值加上固定值。
             * @param x 输入值。
             * @return x + value。
             */
            template <class U>
            auto operator()(U x) const -> decltype(x + value)
            {
                return x + value;
            }
        };

        /**
         * @brief 减固定值变换器。
         * @tparam T 固定值类型。
         */
        template <class T>
        struct Sub
        {
            T value;

            /**
             * @brief 对输入值减去固定值。
             * @param x 输入值。
             * @return x - value。
             */
            template <class U>
            auto operator()(U x) const -> decltype(x - value)
            {
                return x - value;
            }
        };

        /**
         * @brief 乘固定值变换器。
         * @tparam T 固定值类型。
         */
        template <class T>
        struct Mul
        {
            T value;

            /**
             * @brief 对输入值乘以固定值。
             * @param x 输入值。
             * @return x * value。
             */
            template <class U>
            auto operator()(U x) const -> decltype(x * value)
            {
                return x * value;
            }
        };

        /**
         * @brief 除固定值变换器。
         * @tparam T 固定值类型。
         */
        template <class T>
        struct Div
        {
            T value;

            /**
             * @brief 对输入值除以固定值。
             * @param x 输入值。
             * @return x / value。
             */
            template <class U>
            auto operator()(U x) const -> decltype(x / value)
            {
                return x / value;
            }
        };

        /**
         * @brief 取模固定值变换器。
         * @tparam T 固定值类型。
         */
        template <class T>
        struct Mod
        {
            T value;

            /**
             * @brief 对输入值取模固定值。
             * @param x 输入值。
             * @return x % value。
             */
            template <class U>
            auto operator()(U x) const -> decltype(x % value)
            {
                return x % value;
            }
        };

        /**
         * @brief 自增变换器。
         */
        struct inc_t
        {
            /**
             * @brief 对输入值自增1。
             * @param x 输入值。
             * @return ++x。
             */
            template <class T>
            auto operator()(T x) const -> typename std::decay<decltype(++x)>::type
            {
                return ++x;
            }
        } inc;

        /**
         * @brief 自减变换器。
         */
        struct dec_t
        {
            /**
             * @brief 对输入值自减1。
             * @param x 输入值。
             * @return --x。
             */
            template <class T>
            auto operator()(T x) const -> typename std::decay<decltype(--x)>::type
            {
                return --x;
            }
        } dec;

        /**
         * @brief 取负变换器。
         */
        struct negate_t
        {
            /**
             * @brief 对输入值取负。
             * @param x 输入值。
             * @return -x。
             */
            template <class T>
            auto operator()(T x) const -> decltype(-x)
            {
                return -x;
            }
        } negate;

        /**
         * @brief 平方变换器。
         */
        struct square_t
        {
            /**
             * @brief 计算输入值的平方。
             * @param x 输入值。
             * @return x * x。
             */
            template <class T>
            auto operator()(T x) const -> decltype(x * x)
            {
                return x * x;
            }
        } square;

        /**
         * @brief 立方变换器。
         */
        struct cube_t
        {
            /**
             * @brief 计算输入值的立方。
             * @param x 输入值。
             * @return x * x * x。
             */
            template <class T>
            auto operator()(T x) const -> decltype(x * x * x)
            {
                return x * x * x;
            }
        } cube;

        /**
         * @brief 平方根变换器。
         */
        struct sqrt_t
        {
            /**
             * @brief 计算输入值的平方根。
             * @param x 输入值。
             * @return std::sqrt(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::sqrt(x))
            {
                return std::sqrt(x);
            }
        } sqrt;

        /**
         * @brief 倒数变换器。
         */
        struct reciprocal_t
        {
            /**
             * @brief 计算输入值的倒数。
             * @param x 输入值。
             * @return 1 / x。
             */
            template <class T>
            auto operator()(T x) const -> decltype(1 / x)
            {
                return 1 / x;
            }
        } reciprocal;

        /**
         * @brief 绝对值变换器。
         */
        struct abs_t
        {
            /**
             * @brief 计算输入值的绝对值。
             * @param x 输入值。
             * @return std::abs(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::abs(x))
            {
                return std::abs(x);
            }
        } abs;

        /**
         * @brief 转为字符串变换器。
         */
        struct to_string_t
        {
            /**
             * @brief 将输入值转换为字符串。
             * @param x 输入值。
             * @return std::to_string(x)。
             */
            template <class T>
            std::string operator()(const T &x) const
            {
                return std::to_string(x);
            }
        } to_string;

        /**
         * @brief 大于谓词。
         * @tparam T 阈值类型。
         */
        template <class T>
        struct Gt
        {
            T threshold;

            /**
             * @brief 判断输入值是否大于阈值。
             * @param x 输入值。
             * @return x > threshold。
             */
            template <class U>
            bool operator()(const U &x) const
            {
                return x > threshold;
            }
        };

        /**
         * @brief 大于等于谓词。
         * @tparam T 阈值类型。
         */
        template <class T>
        struct Ge
        {
            T threshold;

            /**
             * @brief 判断输入值是否大于等于阈值。
             * @param x 输入值。
             * @return x >= threshold。
             */
            template <class U>
            bool operator()(const U &x) const
            {
                return x >= threshold;
            }
        };

        /**
         * @brief 小于谓词。
         * @tparam T 阈值类型。
         */
        template <class T>
        struct Lt
        {
            T threshold;

            /**
             * @brief 判断输入值是否小于阈值。
             * @param x 输入值。
             * @return x < threshold。
             */
            template <class U>
            bool operator()(const U &x) const
            {
                return x < threshold;
            }
        };

        /**
         * @brief 小于等于谓词。
         * @tparam T 阈值类型。
         */
        template <class T>
        struct Le
        {
            T threshold;

            /**
             * @brief 判断输入值是否小于等于阈值。
             * @param x 输入值。
             * @return x <= threshold。
             */
            template <class U>
            bool operator()(const U &x) const
            {
                return x <= threshold;
            }
        };

        /**
         * @brief 等于谓词。
         * @tparam T 比较值类型。
         */
        template <class T>
        struct Eq
        {
            T value;

            /**
             * @brief 判断输入值是否等于指定值。
             * @param x 输入值。
             * @return x == value。
             */
            template <class U>
            bool operator()(const U &x) const
            {
                return x == value;
            }
        };

        /**
         * @brief 不等于谓词。
         * @tparam T 比较值类型。
         */
        template <class T>
        struct Ne
        {
            T value;

            /**
             * @brief 判断输入值是否不等于指定值。
             * @param x 输入值。
             * @return x != value。
             */
            template <class U>
            bool operator()(const U &x) const
            {
                return x != value;
            }
        };

        /**
         * @brief 偶数谓词。
         */
        struct even_t
        {
            /**
             * @brief 判断输入值是否为偶数。
             * @param x 输入值。
             * @return x % 2 == 0。
             */
            template <class T>
            bool operator()(T x) const
            {
                return x % 2 == 0;
            }
        } even;

        /**
         * @brief 奇数谓词。
         */
        struct odd_t
        {
            /**
             * @brief 判断输入值是否为奇数。
             * @param x 输入值。
             * @return x % 2 != 0。
             */
            template <class T>
            bool operator()(T x) const
            {
                return x % 2 != 0;
            }
        } odd;

        /**
         * @brief 正数谓词。
         */
        struct positive_t
        {
            /**
             * @brief 判断输入值是否为正数。
             * @param x 输入值。
             * @return x > 0。
             */
            template <class T>
            bool operator()(T x) const
            {
                return x > 0;
            }
        } positive;

        /**
         * @brief 负数谓词。
         */
        struct negative_t
        {
            /**
             * @brief 判断输入值是否为负数。
             * @param x 输入值。
             * @return x < 0。
             */
            template <class T>
            bool operator()(T x) const
            {
                return x < 0;
            }
        } negative;

        /**
         * @brief 零谓词。
         */
        struct zero_t
        {
            /**
             * @brief 判断输入值是否为零。
             * @param x 输入值。
             * @return x == 0。
             */
            template <class T>
            bool operator()(T x) const
            {
                return x == 0;
            }
        } zero;

        /**
         * @brief 素数谓词。
         */
        struct prime_t
        {
            /**
             * @brief 判断输入值是否为素数。
             * @param n 输入值。
             * @return 是否为素数。
             */
            template <class Int>
            bool operator()(Int n) const
            {
                if (n <= 1)
                    return false;
                for (int i = 2; i * i <= n; ++i)
                    if (n % i == 0)
                        return false;
                return true;
            }
        } prime;

        /**
         * @brief 逻辑与组合器。
         * @tparam P 第一个谓词类型。
         * @tparam Q 第二个谓词类型。
         */
        template <class P, typename Q>
        struct And
        {
            P p;
            Q q;

            /**
             * @brief 对输入值同时应用两个谓词并返回逻辑与结果。
             * @param x 输入值。
             * @return p(x) && q(x)。
             */
            template <class T>
            bool operator()(const T &x) const
            {
                return p(x) && q(x);
            }
        };

        /**
         * @brief 逻辑或组合器。
         * @tparam P 第一个谓词类型。
         * @tparam Q 第二个谓词类型。
         */
        template <class P, typename Q>
        struct Or
        {
            P p;
            Q q;

            /**
             * @brief 对输入值同时应用两个谓词并返回逻辑或结果。
             * @param x 输入值。
             * @return p(x) || q(x)。
             */
            template <class T>
            bool operator()(const T &x) const
            {
                return p(x) || q(x);
            }
        };

        /**
         * @brief 逻辑非组合器。
         * @tparam P 谓词类型。
         */
        template <class P>
        struct Not
        {
            P p;

            /**
             * @brief 对输入值应用谓词并返回逻辑非结果。
             * @param x 输入值。
             * @return !p(x)。
             */
            template <class T>
            bool operator()(const T &x) const
            {
                return !p(x);
            }
        };

        /**
         * @brief 取成员变量变换器。
         * @tparam Class 类类型。
         * @tparam Member 成员类型。
         */
        template <class Class, typename Mem>
        struct Member
        {
            Mem Class::*ptr;

            /**
             * @brief 获取对象的成员变量。
             * @param obj 对象。
             * @return obj.*ptr。
             */
            template <class T>
            auto operator()(const T &obj) const -> decltype(obj.*ptr)
            {
                return obj.*ptr;
            }
        };

        /**
         * @brief 取 pair 的 first 变换器。
         */
        struct first_t
        {
            /**
             * @brief 获取 pair 的第一个元素。
             * @param p pair 对象。
             * @return p.first。
             */
            template <class Pair>
            auto operator()(const Pair &p) const -> decltype(p.first)
            {
                return p.first;
            }
        } first;

        /**
         * @brief 取 pair 的 second 变换器。
         */
        struct second_t
        {
            /**
             * @brief 获取 pair 的第二个元素。
             * @param p pair 对象。
             * @return p.second。
             */
            template <class Pair>
            auto operator()(const Pair &p) const -> decltype(p.second)
            {
                return p.second;
            }
        } second;

        /**
         * @brief 相邻去重过滤器。
         * @tparam T 元素类型。
         */
        template <class T>
        struct unique
        {
            bool first = true;
            T last;

            /**
             * @brief 判断当前元素是否与前一个元素不同。
             * @param x 当前元素。
             * @return 是否保留该元素。
             */
            bool operator()(const T &x)
            {
                if (first)
                {
                    first = false;
                    last = x;
                    return true;
                }
                if (last == x)
                    return false;
                last = x;
                return true;
            }
        };

        /**
         * @brief 恒等变换器。
         */
        struct identity_t
        {
            /**
             * @brief 返回输入值本身。
             * @param x 输入值。
             * @return x。
             */
            template <class T>
            T operator()(T x) const
            {
                return x;
            }
        } identity;

        /**
         * @brief 转型变化器。
         * @tparam T 目标元素类型。
         */
        template <class T>
        struct as
        {
            /**
             * @brief 返回转型后的值。
             * @tparam U 输入元素类型。
             * @param x 输入值。
             * @return T 转化后的值。
             */
            template <class U>
            T operator()(U &&x)
            {
                return static_cast<T>(std::forward<U>(x));
            }
        };

        /**
         * @brief 创建加固定值变换器。
         * @tparam T 值类型（自动推导）。
         * @param v 要加的值。
         */
        template <class T>
        Add<T> add(T v) { return {v}; }

        /**
         * @brief 创建减固定值变换器。
         * @param v 要减的值。
         */
        template <class T>
        Sub<T> sub(T v) { return {v}; }

        /**
         * @brief 创建乘固定值变换器。
         * @param v 要乘的值。
         */
        template <class T>
        Mul<T> mul(T v) { return {v}; }

        /**
         * @brief 创建除固定值变换器。
         * @param v 要除的值。
         */
        template <class T>
        Div<T> div(T v) { return {v}; }

        /**
         * @brief 创建取模固定值变换器。
         * @param v 要取模的值。
         */
        template <class T>
        Mod<T> mod(T v) { return {v}; }

        /**
         * @brief 创建大于谓词。
         */
        template <class T>
        Gt<T> gt(T t) { return {t}; }

        /**
         * @brief 创建大于等于谓词。
         */
        template <class T>
        Ge<T> ge(T t) { return {t}; }

        /**
         * @brief 创建小于谓词。
         */
        template <class T>
        Lt<T> lt(T t) { return {t}; }

        /**
         * @brief 创建小于等于谓词。
         */
        template <class T>
        Le<T> le(T t) { return {t}; }

        /**
         * @brief 创建等于谓词。
         */
        template <class T>
        Eq<T> eq(T v) { return {v}; }

        /**
         * @brief 创建不等于谓词。
         */
        template <class T>
        Ne<T> ne(T v) { return {v}; }

        /**
         * @brief 创建逻辑与组合谓词。
         */
        template <class P, typename Q>
        And<P, Q> and_(P p, Q q) { return {p, q}; }

        /**
         * @brief 创建逻辑或组合谓词。
         */
        template <class P, typename Q>
        Or<P, Q> or_(P p, Q q) { return {p, q}; }

        /**
         * @brief 创建逻辑非组合谓词。
         */
        template <class P>
        Not<P> not_(P p) { return {p}; }

        /**
         * @brief 创建成员变量访问器。
         */
        template <class Class, typename Mem>
        Member<Class, Mem> member(Mem Class::*p) { return {p}; }
    }
}
