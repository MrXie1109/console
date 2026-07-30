/**
 * @file params.h
 * @brief 提供参数包包装器(Params)和便捷构造函数(params)，用于将多个参数打包并支持延迟应用。
 * @details 在没有 std::index_sequence 和 std::apply 的 C++11 环境下，提供一个可用的替代品。
 *          采用递归模板结构实现参数包的存储和延迟调用。
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
#include <type_traits>

namespace console {
    /**
     * @class Params
     * @brief 递归参数包包装器，将多个参数存储为值类型，并支持通过 apply 方法延迟调用。
     * @tparam Ts 参数类型包，可以是任意数量、任意类型。
     * @details 采用递归模板结构，每个 Params 实例存储一个值及其余参数。
     *          通过 apply 方法可以将存储的所有参数传递给一个可调用对象。
     *          常用于参数捕获、延迟求值或适配需要多个参数的接口。
     * @note 这是非特化版本的声明，具体实现见特化和递归特化版本。
     */
    template <class... Ts>
    class Params;

    /**
     * @class Params<>
     * @brief 空参数特化版本。
     * @details 用于处理无参数的情况，提供空的 apply 方法。
     */
    template <>
    class Params<> {
    public:
        /**
         * @brief 无参构造函数。
         */
        Params() = default;

        /**
         * @brief 将存储的参数应用于可调用对象。
         * @tparam F 可调用对象的类型，必须能接受零个参数。
         * @param f 要调用的可调用对象。
         * @return decltype(f()) f 的返回值。
         */
        template <class F>
        auto apply(F f) const -> decltype(f()) {
            return f();
        }
    };

    /**
     * @class Params<T>
     * @brief 单参数版本的 Params 特化，递归基类。
     * @tparam T 参数的类型。
     * @details 作为递归链的终止节点，存储单个值并提供 apply 方法。
     */
    template <class T>
    class Params<T> {
        T value; ///< 存储的参数值

    public:
        /**
         * @brief 从给定值构造单参数 Params。
         * @param value 要存储的参数值(按常量引用传入)。
         */
        Params(const T &value) : value(value) {}

        /**
         * @brief 将存储的参数应用于可调用对象。
         * @tparam F 可调用对象的类型，必须能接受类型 T 的参数。
         * @param f 要调用的可调用对象。
         * @return decltype(f(value)) f 的返回值。
         * @note 对于单参数版本，直接使用存储的值调用 f。
         */
        template <class F>
        auto apply(F f) const -> decltype(f(value)) {
            return f(value);
        };
    };

    /**
     * @class Params<T, Rest...>
     * @brief 多参数版本的 Params 递归特化。
     * @tparam T 当前参数的类型。
     * @tparam Rest 剩余参数的类型包。
     * @details 存储当前参数值和一个包含剩余参数的 Params 对象。
     *          apply 方法通过 Helper 辅助对象实现参数累积传递。
     */
    template <class T, class... Rest>
    class Params<T, Rest...> {
        T               value; ///< 当前参数值
        Params<Rest...> rest;  ///< 剩余参数的包装器

        /**
         * @struct Helper
         * @brief 辅助函数对象，用于在 apply 过程中将当前参数与剩余参数合并。
         * @tparam F 原始可调用对象的类型。
         * @details 捕获用户提供的可调用对象和当前参数值，当剩余参数被展开时，
         *          通过 operator() 将当前参数作为第一个参数传递给用户函数。
         */
        template <class F>
        struct Helper {
            F        f;     ///< 用户提供的可调用对象
            const T &value; ///< 当前参数值的引用

            /**
             * @brief 构造 Helper 对象。
             * @param f 用户提供的可调用对象。
             * @param value 当前参数值的引用。
             */
            Helper(F f, const T &value) : f(f), value(value) {}

            /**
             * @brief 调用辅助函数对象。
             * @tparam Args 剩余参数的类型包。
             * @param args 剩余参数值。
             * @return decltype(f(value, args...)) 用户函数的返回值。
             * @note 将当前参数 value 作为第一个参数，args 作为后续参数调用用户函数。
             */
            template <class... Args>
            auto operator()(const Args &...args) const
                -> decltype(f(value, args...)) {
                return f(value, args...);
            }
        };

    public:
        /**
         * @brief 从多个值构造多参数 Params。
         * @param value 当前参数值(按常量引用传入)。
         * @param rest 剩余参数值(按常量引用传入)。
         */
        Params(const T &value, const Rest &...rest) :
            value(value), rest(rest...) {}

        /**
         * @brief 将存储的所有参数应用于可调用对象。
         * @tparam F 可调用对象的类型，必须能接受所有存储的参数。
         * @param f 要调用的可调用对象。
         * @return decltype(rest.apply(Helper<F>(f, value))) f 的返回值。
         * @details 创建一个 Helper 对象捕获当前参数，然后将其传递给剩余参数的
         *          apply 方法，实现参数的逐层累积和最终调用。
         */
        template <class F>
        auto apply(F f) const -> decltype(rest.apply(Helper<F>(f, value))) {
            return rest.apply(Helper<F>(f, value));
        }
    };

    /**
     * @brief 便捷函数，用于构造 Params 对象。
     * @tparam Ts 参数类型包，由编译器自动推导。
     * @param values 要打包的参数值。
     * @return Params<typename std::decay<Ts>::type...> 包含所有参数值的 Params 对象。
     * @details 利用模板参数推导，自动创建 Params 实例。
     *          使用 std::decay 退化数组、去除引用和 cv 限定符，确保值语义。
     *          使用示例：auto p = params(42, "hello", 3.14);
     *                    p.apply([](int a, const char* b, double c) { ... });
     */
    template <class... Ts>
    Params<typename std::decay<Ts>::type...> params(const Ts &...values) {
        return Params<typename std::decay<Ts>::type...>(values...);
    }
}
