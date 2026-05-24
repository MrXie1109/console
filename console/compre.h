/**
 * @file compre.h
 * @brief 提供类似 Python 列表推导式的容器适配器，支持 map/filter 等函数式操作。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @warning 此头文件已被弃用。
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

namespace console
{
    /**
     * @class Comprehension
     * @brief 函数式风格的数据流处理容器。
     * @tparam T 容器中元素的类型。
     * @details 该类包装一个 std::vector<T>，提供 map、filter 等惰性求值风格的操作（实际是立即求值），
     *          并支持从多种来源构造，如容器、迭代器对、初始化列表等。
     * @note 所有转换操作都会创建新的 Comprehension 对象，不会修改原对象。
     */
    template <class T>
    class Comprehension
    {
    protected:
        std::vector<T> vec; ///< 内部存储的 std::vector

    public:
        /// @brief 默认构造函数，创建一个空 Comprehension。
        Comprehension() : vec() {}

        /**
         * @brief 从容器（左值）构造，拷贝所有元素。
         * @tparam Cont 容器类型，必须支持 std::begin/end。
         * @param cont 源容器（左值）。
         */
        template <class Cont>
        Comprehension(const Cont &cont)
            : vec(std::begin(cont), std::end(cont)) {}

        /**
         * @brief 从容器（右值）构造，移动所有元素。
         * @tparam Cont 容器类型，必须支持 std::begin/end。
         * @param cont 源容器（右值）。
         */
        template <class Cont>
        Comprehension(Cont &&cont)
            : vec(
                  std::make_move_iterator(std::begin(cont)),
                  std::make_move_iterator(std::end(cont))) {}

        /**
         * @brief 从容器的子区间（左值）构造，拷贝指定范围的元素。
         * @tparam Cont 容器类型。
         * @param cont 源容器（左值）。
         * @param start_pos 起始索引（包含）。
         * @param end_pos 结束索引（不包含）。
         * @note 区间为 [start_pos, end_pos)。
         */
        template <class Cont>
        Comprehension(const Cont &cont, size_t start_pos, size_t end_pos)
            : vec(std::next(std::begin(cont), start_pos),
                  std::next(std::begin(cont), end_pos)) {}

        /**
         * @brief 从容器的子区间（右值）构造，移动指定范围的元素。
         * @tparam Cont 容器类型。
         * @param cont 源容器（右值）。
         * @param start_pos 起始索引（包含）。
         * @param end_pos 结束索引（不包含）。
         * @note 区间为 [start_pos, end_pos)。
         */
        template <class Cont>
        Comprehension(Cont &&cont, size_t start_pos, size_t end_pos)
            : vec(std::make_move_iterator(
                      std::next(std::begin(cont), start_pos)),
                  std::make_move_iterator(
                      std::next(std::begin(cont), end_pos))) {}

        /**
         * @brief 从迭代器对构造，拷贝 [beg, end) 范围内的元素。
         * @tparam Iter 迭代器类型。
         * @param beg 起始迭代器。
         * @param end 结束迭代器。
         */
        template <class Iter>
        Comprehension(Iter beg, Iter end) : vec(beg, end) {}

        /**
         * @brief 从 std::initializer_list 构造。
         * @param init 初始化列表。
         */
        Comprehension(std::initializer_list<T> init) : vec(init) {}

        /**
         * @brief 获取内部 vector 的引用。
         * @return std::vector<T>& 内部 vector 的引用。
         */
        std::vector<T> &get_vec()
        {
            return vec;
        }

        /**
         * @brief 对每个元素应用函数 f，返回新 Comprehension。
         * @tparam F 可调用对象类型，接受 T 并返回某个类型 U。
         * @param f 映射函数。
         * @return Comprehension<decltype(f(vec[0]))> 包含映射结果的 Comprehension。
         * @note 该操作立即求值，生成新容器。
         */
        template <class F>
        auto map(F &&f) const -> Comprehension<decltype(f(vec[0]))>
        {
            Comprehension<decltype(f(vec[0]))> tmp;
            for (const T &item : vec)
                tmp.get_vec().push_back(f(item));
            return tmp;
        }

        /**
         * @brief 筛选满足谓词 f 的元素，返回新 Comprehension。
         * @tparam F 可调用对象类型，接受 T 并返回 bool。
         * @param f 谓词函数。
         * @return Comprehension<T> 包含满足条件的元素的 Comprehension。
         */
        template <class F>
        Comprehension<T> filter(F &&f) const
        {
            Comprehension<T> tmp;
            for (const T &item : vec)
                if (f(item))
                    tmp.vec.push_back(item);
            return tmp;
        }

        /**
         * @brief 将内部元素移动构造到目标容器 Cont 中。
         * @tparam Cont 目标容器类型，必须支持从移动迭代器构造。
         * @return Cont 构造完成的目标容器。
         * @note 调用后当前 Comprehension 仍有效但内部元素已被移走。
         */
        template <class Cont>
        Cont to()
        {
            return Cont(
                std::make_move_iterator(vec.begin()),
                std::make_move_iterator(vec.end()));
        }

        /**
         * @brief 将内部元素拷贝构造到目标容器 Cont 中。
         * @tparam Cont 目标容器类型，必须支持从迭代器构造。
         * @return Cont 构造完成的目标容器。
         * @note 不修改当前 Comprehension。
         */
        template <class Cont>
        Cont make() const
        {
            return Cont(vec.begin(), vec.end());
        }
    };

    /**
     * @brief 从容器（左值）创建 Comprehension。
     * @tparam Cont 容器类型。
     * @param cont 源容器（左值）。
     * @return Comprehension<typename Cont::value_type> 新构造的 Comprehension。
     */
    template <class Cont>
    auto compre(const Cont &cont)
        -> Comprehension<typename Cont::value_type>
    {
        return Comprehension<typename Cont::value_type>(cont);
    }

    /**
     * @brief 从容器（右值）创建 Comprehension（移动元素）。
     * @tparam Cont 容器类型。
     * @param cont 源容器（右值）。
     * @return Comprehension<typename Cont::value_type> 新构造的 Comprehension。
     */
    template <class Cont>
    auto compre(Cont &&cont)
        -> Comprehension<typename Cont::value_type>
    {
        return Comprehension<typename Cont::value_type>(std::move(cont));
    }

    /**
     * @brief 从容器的子区间（左值）创建 Comprehension。
     * @tparam Cont 容器类型。
     * @param cont 源容器（左值）。
     * @param start_pos 起始索引。
     * @param end_pos 结束索引。
     * @return Comprehension<typename Cont::value_type> 新构造的 Comprehension。
     */
    template <class Cont>
    auto compre(const Cont &cont, size_t start_pos, size_t end_pos)
        -> Comprehension<typename Cont::value_type>
    {
        return Comprehension<typename Cont::value_type>(
            cont, start_pos, end_pos);
    }

    /**
     * @brief 从容器的子区间（右值）创建 Comprehension（移动元素）。
     * @tparam Cont 容器类型。
     * @param cont 源容器（右值）。
     * @param start_pos 起始索引。
     * @param end_pos 结束索引。
     * @return Comprehension<typename Cont::value_type> 新构造的 Comprehension。
     */
    template <class Cont>
    auto compre(Cont &&cont, size_t start_pos, size_t end_pos)
        -> Comprehension<typename Cont::value_type>
    {
        return Comprehension<typename Cont::value_type>(
            std::move(cont), start_pos, end_pos);
    }

    /**
     * @brief 从迭代器对创建 Comprehension。
     * @tparam Iter 迭代器类型。
     * @param beg 起始迭代器。
     * @param end 结束迭代器。
     * @return Comprehension<decltype(*beg)> 新构造的 Comprehension。
     */
    template <class Iter>
    auto compre(Iter beg, Iter end) -> Comprehension<decltype(*beg)>
    {
        return Comprehension<decltype(*beg)>(beg, end);
    }

    /**
     * @brief 从初始化列表创建 Comprehension。
     * @tparam T 元素类型。
     * @param init 初始化列表。
     * @return Comprehension<T> 新构造的 Comprehension。
     */
    template <class T>
    Comprehension<T> compre(std::initializer_list<T> init)
    {
        return Comprehension<T>(init);
    }
}
