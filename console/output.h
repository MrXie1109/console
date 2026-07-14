/**
 * @file output.h
 * @brief 提供 STL 容器的格式化输出和灵活的输出控制工具。
 * @details 该头文件重载了 `operator<<` 使得标准容器（vector, list, set, map
 * 等）、 pair 和 tuple 可以直接输出到流。同时提供了 `BasicOutput`
 * 类用于灵活控制输出格式， 以及辅助函数 `to_array` 和 `to_vector` 用于将 C
 * 风格数组转换为 STL 容器。
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
#include <iterator>

#include "put.h"

namespace console {
    /**
     * @defgroup output_helpers 输出辅助函数
     * @brief 用于输出不同容器类型的辅助函数。
     * @{
     */

    /**
     * @brief 输出序列容器（如 vector, list）的内容，格式为 [a, b, c]。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam Cont 容器类型，必须支持 begin()/end() 迭代器。
     * @param os 输出流。
     * @param cont 要输出的容器。
     * @return std::basic_ostream<CharT, Traits>& 输出流引用。
     */
    template <class CharT, class Traits, class Cont>
    inline std::basic_ostream<CharT, Traits> &cont_print_sequence(
        std::basic_ostream<CharT, Traits> &os, const Cont &cont) {
        if (begin(cont) == end(cont)) return os << CharT('[') << CharT(']');
        auto it = begin(cont);
        os << CharT('[');
        repr(*it, os);
        while (++it != end(cont)) {
            os << CharT(','), os << CharT(' ');
            repr(*it, os);
        }
        return os << CharT(']');
    }

    /**
     * @brief 输出集合容器（如 set, unordered_set）的内容，格式为 {a, b, c}。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam Cont 容器类型，必须支持 begin()/end() 迭代器。
     * @param os 输出流。
     * @param cont 要输出的容器。
     * @return std::basic_ostream<CharT, Traits>& 输出流引用。
     */
    template <class CharT, class Traits, class Cont>
    inline std::basic_ostream<CharT, Traits> &
    cont_print_set(std::basic_ostream<CharT, Traits> &os, const Cont &cont) {
        if (begin(cont) == end(cont)) return os << CharT('{') << CharT('}');
        auto it = begin(cont);
        os << CharT('{');
        repr(*it, os);
        while (++it != end(cont)) {
            os << CharT(','), os << CharT(' ');
            repr(*it, os);
        }
        return os << CharT('}');
    }

    /**
     * @brief 输出映射容器（如 map, unordered_map）的内容，格式为 {key: value,
     * ...}。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam Cont 容器类型，其元素必须为 pair<const K, V> 且支持 ->first 和
     * ->second。
     * @param os 输出流。
     * @param cont 要输出的容器。
     * @return std::basic_ostream<CharT, Traits>& 输出流引用。
     */
    template <class CharT, class Traits, class Cont>
    inline std::basic_ostream<CharT, Traits> &
    cont_print_map(std::basic_ostream<CharT, Traits> &os, const Cont &cont) {
        if (begin(cont) == end(cont)) return os << CharT('{') << CharT('}');
        auto it = begin(cont);
        os << CharT('{');
        repr(it->first, os);
        os << CharT(':'), os << CharT(' ');
        repr(it->second, os);
        while (++it != end(cont)) {
            os << CharT(','), os << CharT(' ');
            repr(it->first, os);
            os << CharT(':'), os << CharT(' ');
            repr(it->second, os);
        }
        return os << CharT('}');
    }

    /** @} */

    /**
     * @defgroup tuple_printer Tuple 打印辅助
     * @brief 用于递归打印 std::tuple 的辅助模板。
     * @{
     */

    /**
     * @brief 递归打印 tuple 的辅助模板（主模板）。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam Tuple tuple 类型。
     * @tparam N 当前要打印的最后一个元素的索引（从 1 开始）。
     */
    template <class CharT,
        class Traits,
        class Tuple,
        size_t N = std::tuple_size<Tuple>::value>
    struct BasicTuplePrinter {
        /**
         * @brief 递归打印 tuple 元素。
         * @param os 输出流。
         * @param t tuple 对象。
         */
        static void
        print(std::basic_ostream<CharT, Traits> &os, const Tuple &t) {
            BasicTuplePrinter<CharT, Traits, Tuple, N - 1>::print(os, t);
            if (N > 1) os << CharT(','), os << CharT(' ');
            put(os, std::get<N - 1>(t));
        }
    };

    /**
     * @brief 递归打印 tuple 的偏特化：处理单个元素。
     */
    template <class CharT, class Traits, class Tuple>
    struct BasicTuplePrinter<CharT, Traits, Tuple, 1> {
        static void
        print(std::basic_ostream<CharT, Traits> &os, const Tuple &t) {
            put(os, std::get<0>(t));
        }
    };

    /**
     * @brief 递归打印 tuple 的偏特化：处理空 tuple。
     */
    template <class CharT, class Traits, class Tuple>
    struct BasicTuplePrinter<CharT, Traits, Tuple, 0> {
        static void print(std::basic_ostream<CharT, Traits> &, const Tuple &) {}
    };

    /** @brief BasicTuplePrinter<char> 的类型别名。 */
    template <class Tuple, size_t N = std::tuple_size<Tuple>::value>
    using TuplePrinter
        = BasicTuplePrinter<char, std::char_traits<char>, Tuple, N>;

    /** @} */

    // ==================== 标准容器 operator<< 重载 ====================

    /**
     * @defgroup output_std_containers 标准容器输出重载
     * @brief 为各种 STL 容器提供 operator<< 重载。
     * @{
     */

    /// @brief 输出 std::vector。
    template <class CharT, class Traits, class T>
    inline std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &os, const std::vector<T> &vec) {
        return cont_print_sequence<CharT, Traits>(os, vec);
    }

    /// @brief 输出 std::deque。
    template <class CharT, class Traits, class T>
    inline std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &os, const std::deque<T> &deq) {
        return cont_print_sequence<CharT, Traits>(os, deq);
    }

    /// @brief 输出 std::list。
    template <class CharT, class Traits, class T>
    inline std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os, const std::list<T> &lst) {
        return cont_print_sequence<CharT, Traits>(os, lst);
    }

    /// @brief 输出 std::forward_list。
    template <class CharT, class Traits, class T>
    inline std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
        const std::forward_list<T>               &flst) {
        return cont_print_sequence<CharT, Traits>(os, flst);
    }

    /// @brief 输出 std::array。
    template <class CharT, class Traits, class T, size_t n>
    inline std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &os, const std::array<T, n> &arr) {
        return cont_print_sequence<CharT, Traits>(os, arr);
    }

    /// @brief 输出 std::set。
    template <class CharT, class Traits, class T>
    inline std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os, const std::set<T> &s) {
        return cont_print_set<CharT, Traits>(os, s);
    }

    /// @brief 输出 std::map。
    template <class CharT, class Traits, class K, class V>
    inline std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os, const std::map<K, V> &m) {
        return cont_print_map<CharT, Traits>(os, m);
    }

    /// @brief 输出 std::multiset。
    template <class CharT, class Traits, class T>
    inline std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &os, const std::multiset<T> &ms) {
        return cont_print_set<CharT, Traits>(os, ms);
    }

    /// @brief 输出 std::multimap。
    template <class CharT, class Traits, class K, class V>
    inline std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &os, const std::multimap<K, V> &mm) {
        return cont_print_map<CharT, Traits>(os, mm);
    }

    /// @brief 输出 std::unordered_set。
    template <class CharT, class Traits, class T>
    inline std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
        const std::unordered_set<T>              &us) {
        return cont_print_set<CharT, Traits>(os, us);
    }

    /// @brief 输出 std::unordered_map。
    template <class CharT, class Traits, class K, class V>
    inline std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
        const std::unordered_map<K, V>           &um) {
        return cont_print_map<CharT, Traits>(os, um);
    }

    /// @brief 输出 std::unordered_multiset。
    template <class CharT, class Traits, class T>
    inline std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
        const std::unordered_multiset<T>         &ums) {
        return cont_print_set<CharT, Traits>(os, ums);
    }

    /// @brief 输出 std::unordered_multimap。
    template <class CharT, class Traits, class K, class V>
    inline std::basic_ostream<CharT, Traits> &
    operator<<(std::basic_ostream<CharT, Traits> &os,
        const std::unordered_multimap<K, V>      &ump) {
        return cont_print_map<CharT, Traits>(os, ump);
    }

    /// @brief 输出 std::valarray。
    template <class CharT, class Traits, class T>
    inline std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &os, const std::valarray<T> &va) {
        return cont_print_sequence<CharT, Traits>(os, va);
    }

    /**
     * @brief 输出 std::pair，格式为 (first, second)。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam T pair 的第一个类型。
     * @tparam U pair 的第二个类型。
     * @param os 输出流。
     * @param p pair 对象。
     * @return std::basic_ostream<CharT, Traits>& 输出流引用。
     */
    template <class CharT, class Traits, class T, class U>
    inline std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &os, const std::pair<T, U> &p) {
        os << CharT('(');
        put(os, p.first);
        os << CharT(','), os << CharT(' ');
        put(os, p.second);
        return os << CharT(')');
    }

    /**
     * @brief 输出 std::tuple，格式为 (elem1, elem2, ...)。
     * @tparam CharT 流的字符类型。
     * @tparam Traits 流的字符特征类型。
     * @tparam Args tuple 的元素类型。
     * @param os 输出流。
     * @param t tuple 对象。
     * @return std::basic_ostream<CharT, Traits>& 输出流引用。
     */
    template <class CharT, class Traits, class... Args>
    inline std::basic_ostream<CharT, Traits> &operator<<(
        std::basic_ostream<CharT, Traits> &os, const std::tuple<Args...> &t) {
        os << CharT('(');
        BasicTuplePrinter<CharT, Traits, std::tuple<Args...>>::print(os, t);
        return os << CharT(')');
    }

    /** @} */

    /**
     * @defgroup array_conversion 数组转换函数
     * @brief 将 C 风格数组转换为 std::array 或 std::vector。
     * @{
     */

    /**
     * @brief 将 C 风格数组转换为 std::array。
     * @tparam T 数组元素类型。
     * @tparam N 数组大小（由编译器推导）。
     * @param ar 原始数组。
     * @return std::array<T, N> 转换后的 array。
     */
    template <class T, size_t N>
    inline std::array<T, N> to_array(const T (&ar)[N]) {
        std::array<T, N> arr;
        std::copy(ar, ar + N, arr.begin());
        return arr;
    }

    /**
     * @brief 将 C 风格数组转换为 std::vector。
     * @tparam T 数组元素类型。
     * @tparam N 数组大小（由编译器推导）。
     * @param ar 原始数组。
     * @return std::vector<T> 转换后的 vector。
     */
    template <class T, size_t N>
    inline std::vector<T> to_vector(const T (&ar)[N]) {
        std::vector<T> vec(N);
        std::copy(ar, ar + N, vec.begin());
        return vec;
    }

    /** @} */

    /**
     * @class BasicOutput
     * @brief 灵活的输出控制类，支持链式调用和多种参数。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @details 该类允许用户指定分隔符、结尾符和是否立即刷新，并提供
     * `operator()` 重载 来接收任意数量的参数进行格式化输出。 默认全局实例
     * Output 的行为类似于 Python 的 `print` 函数。
     *
     * 使用示例：
     * @code
     * print("Hello", "world");     // 输出 "Hello world\n" 并刷新
     * print();                      // 只输出换行
     * BasicOutput err(std::cerr, "", "\n", true);
     * err("Error:", code);          // 输出到 stderr，无分隔符
     * @endcode
     */
    template <class CharT = char, class Traits = std::char_traits<CharT>>
    class BasicOutput {
        using string_type = std::basic_string<CharT, Traits>;

        std::basic_ostream<CharT, Traits> &os; ///< 输出目标流
        string_type sep;     ///< 多个参数之间的分隔符
        string_type end;     ///< 结尾符（通常是换行符）
        bool        isFlush; ///< 输出后是否立即刷新

    public:
        /**
         * @brief 默认构造，输出到 std::cout，分隔符为空格，结尾为换行，并刷新。
         */
        BasicOutput() :
            os(std::cout), sep(string_type{1, CharT(' ')}),
            end(string_type{1, CharT('\n')}), isFlush(true) {}

        /**
         * @brief 自定义构造。
         * @param o   输出流。
         * @param s   参数之间的分隔符。
         * @param e   结尾符。
         * @param isF 输出后是否刷新。
         */
        BasicOutput(std::basic_ostream<CharT, Traits> &o,
            const string_type                         &s,
            const string_type                         &e,
            bool isF) : os(o), sep(s), end(e), isFlush(isF) {}

        /**
         * @brief 无参数调用：仅输出结尾符（如换行）。
         * @return BasicOutput& 当前对象引用，支持链式调用。
         */
        BasicOutput &operator()() {
            os << end;
            if (isFlush) os << std::flush;
            return *this;
        }

        /**
         * @brief 单参数调用：输出该参数后加结尾符。
         * @tparam T 参数类型（必须支持 operator<<）。
         * @param t 要输出的对象。
         * @return BasicOutput& 当前对象引用。
         */
        template <class T>
        BasicOutput &operator()(const T &t) {
            put(os, t);
            return operator()();
        }

        /**
         * @brief
         * 多参数调用：依次输出每个参数，参数之间插入分隔符，最后加结尾符。
         * @tparam T 第一个参数类型。
         * @tparam Args 其余参数类型包。
         * @param t     第一个参数。
         * @param args  其余参数。
         * @return BasicOutput& 当前对象引用。
         */
        template <class T, class... Args>
        BasicOutput &operator()(const T &t, const Args &...args) {
            put(os, t);
            os << sep;
            return operator()(args...);
        }
    };

    /** @brief BasicOutput<char> 的类型别名。 */
    using Output = BasicOutput<char>;

    /** @brief BasicOutput<wchar_t> 的类型别名。 */
    using WOutput = BasicOutput<wchar_t>;

    // ———— 全局实例 ————

    /**
     * @brief 内部单例（char）。
     */
    inline BasicOutput<> &get_print() {
        static BasicOutput<> instance;
        return instance;
    }

    /**
     * @brief 内部单例（wchar_t）。
     */
    inline BasicOutput<wchar_t> &get_wprint() {
        static BasicOutput<wchar_t> instance(
            std::wcout, std::wstring(1, L' '), std::wstring(1, L'\n'), true);
        return instance;
    }

    static BasicOutput<> &print
        = get_print(); ///< 全局输出对象，模仿 Python 的 print 函数。
    static BasicOutput<wchar_t> &wprint = get_wprint(); ///< 全局宽字符输出。

}
