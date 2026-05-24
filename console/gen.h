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

    // ===== 预定义的生成器和迭代器 =====

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
        Views(Container &&container)
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
        bool done() const { return curr == end; }

        /**
         * @brief 获取当前值。
         * @return 当前迭代器指向的值。
         */
        value_type current() const { return *curr; }

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
        T start, curr, end;
        T step;

    public:
        /**
         * @brief 构造函数。
         * @param begin 起始值。
         * @param end 结束值（不包含）。
         * @param step 步长，默认为1。
         */
        Range(T begin, T end, T step = 1)
            : start(begin), curr(begin), end(end), step(step) {}

        /**
         * @brief 检查生成器是否已完成。
         * @return 如果已到达结束值则返回true。
         */
        bool done() const { return step > 0 ? curr >= end : curr <= end; }

        /**
         * @brief 获取当前值。
         * @return 当前值。
         */
        T current() const { return curr; }

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
        bool done() const { return index >= count; }

        /**
         * @brief 获取当前值。
         * @return 重复的值。
         */
        T current() const { return value; }

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
        bool done() const { return false; }

        /**
         * @brief 获取当前值。
         * @return 当前循环位置的值。
         */
        T current() const { return data[index % data.size()]; }

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
        bool done() const { return max_count != -1 && count >= max_count; }

        /**
         * @brief 获取当前值。
         * @return 当前计数值。
         */
        int current() const { return curr; }

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
        bool done() const { return gen.done(); }

        /**
         * @brief 获取当前值。
         * @return 映射后的值。
         */
        ResultType current() const { return func(gen.current()); }

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
    template <typename Gen, typename Pred>
    class Filter : public Generator<Filter<Gen, Pred>,
                                    typename Gen::iterator::value_type>
    {
        Gen gen;
        Pred pred;

        /**
         * @brief 跳过所有不满足谓词的元素。
         */
        void skip_to_next_valid()
        {
            while (!gen.done() && !pred(gen.current()))
            {
                gen.advance();
            }
        }

    public:
        /**
         * @brief 构造函数。
         * @param g 源生成器。
         * @param p 谓词函数。
         */
        Filter(Gen g, Pred p) : gen(g), pred(p)
        {
            skip_to_next_valid();
        }

        /**
         * @brief 检查生成器是否已完成。
         * @return 如果源生成器已完成则返回true。
         */
        bool done() const { return gen.done(); }

        /**
         * @brief 获取当前值。
         * @return 当前满足谓词的值。
         */
        auto current() const { return gen.current(); }

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
                                  typename Gen::iterator::value_type>
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
        bool done() const { return taken >= count || gen.done(); }

        /**
         * @brief 获取当前值。
         * @return 当前值。
         */
        auto current() const { return gen.current(); }

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
        bool done() const
        {
            if (!skipped)
                const_cast<Skip *>(this)->do_skip();
            return gen.done();
        }

        /**
         * @brief 获取当前值。
         * @return 当前值。
         */
        auto current() const { return gen.current(); }

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
        bool done() const { return gen.done(); }

        /**
         * @brief 获取当前值。
         * @return 包含索引和值的键值对。
         */
        auto current() const { return std::make_pair(index, gen.current()); }

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
        bool done() const { return false; }

        /**
         * @brief 获取当前值。
         * @return 调用生成函数返回的值。
         */
        auto current() const { return func(); }

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
        bool done() const { return gen1.done() || gen2.done(); }

        /**
         * @brief 获取当前值。
         * @return 包含两个生成器当前值的键值对。
         */
        std::pair<typename Gen1::value_type, typename Gen2::value_type>
        current() const
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

    namespace gen
    {
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
        Views<typename Container::iterator> views(Container &&container)
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
         * @brief 创建等差数列生成器。
         * @tparam T 数值类型。
         * @param begin 起始值（包含）。
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
         * @brief 创建重复值生成器。
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
         * @brief 创建无限循环生成器。
         * @tparam T 元素类型。
         * @param list 要循环的列表。
         * @return Cycle<T> 循环生成器。
         */
        template <class T>
        Cycle<T> cycle(std::initializer_list<T> list)
        {
            return Cycle<T>(list);
        }

        /**
         * @brief 创建无限计数器生成器。
         * @param start 起始值，默认为0。
         * @param step 步长，默认为1。
         * @return Counter 计数器生成器。
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
         * @return Counter 计数器生成器。
         */
        inline Counter counter(int start, int times, int step = 1)
        {
            return Counter(start, times, step);
        }

        /**
         * @brief 创建映射适配器，对每个元素应用函数。
         * @tparam Gen 源生成器类型。
         * @tparam Func 映射函数类型。
         * @param gen 源生成器。
         * @param func 映射函数。
         * @return Map<Gen, Func> 映射生成器。
         */
        template <class Gen, class Func>
        Map<Gen, Func> map(Gen gen, Func func)
        {
            return Map<Gen, Func>(gen, func);
        }

        /**
         * @brief 创建过滤适配器，保留满足条件的元素。
         * @tparam Gen 源生成器类型。
         * @tparam Pred 谓词函数类型。
         * @param gen 源生成器。
         * @param pred 谓词函数。
         * @return Filter<Gen, Pred> 过滤生成器。
         */
        template <class Gen, class Pred>
        Filter<Gen, Pred> filter(Gen gen, Pred pred)
        {
            return Filter<Gen, Pred>(gen, pred);
        }

        /**
         * @brief 创建取前n个元素的适配器。
         * @tparam Gen 源生成器类型。
         * @param gen 源生成器。
         * @param n 要取的元素个数。
         * @return Take<Gen> 截取生成器。
         */
        template <class Gen>
        Take<Gen> take(Gen gen, int n)
        {
            return Take<Gen>(gen, n);
        }

        /**
         * @brief 创建跳过前n个元素的适配器。
         * @tparam Gen 源生成器类型。
         * @param gen 源生成器。
         * @param n 要跳过的元素个数。
         * @return Skip<Gen> 跳过生成器。
         */
        template <class Gen>
        Skip<Gen> skip(Gen gen, int n)
        {
            return Skip<Gen>(gen, n);
        }

        /**
         * @brief 创建枚举适配器，为每个元素附加索引。
         * @tparam Gen 源生成器类型。
         * @param gen 源生成器。
         * @return Enumerate<Gen> 枚举生成器。
         */
        template <class Gen>
        Enumerate<Gen> enumerate(Gen gen)
        {
            return Enumerate<Gen>(gen);
        }

        /**
         * @brief 创建函数生成器，无限调用函数生成值。
         * @tparam Func 生成函数类型。
         * @param func 生成函数，每次调用返回一个值。
         * @return Generate<Func> 函数生成器。
         */
        template <class Func>
        Generate<Func> generate(Func func)
        {
            return Generate<Func>(func);
        }

        /**
         * @brief 创建压缩适配器，将两个生成器的元素配对输出。
         * @tparam Gen1 第一个生成器类型。
         * @tparam Gen2 第二个生成器类型。
         * @param gen1 第一个生成器。
         * @param gen2 第二个生成器。
         * @return Zip<Gen1, Gen2> 压缩生成器。
         */
        template <class Gen1, class Gen2>
        Zip<Gen1, Gen2> zip(Gen1 gen1, Gen2 gen2)
        {
            return Zip<Gen1, Gen2>(gen1, gen2);
        }
    }
}
