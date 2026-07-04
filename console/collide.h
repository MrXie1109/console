/**
 * @file collide.h
 * @brief 在运行时实现双重分派（double dispatch）。
 * @details 该模块提供了一个通用的双重分派表，允许根据两个对象的运行时类型，
 *          动态调用对应的处理函数。常用于碰撞检测、事件分发等场景。
 *          使用方式：创建 Table<BaseType> 实例，注册类型对对应的处理函数，
 *          然后通过 call() 方法自动根据两个对象的实际类型调用相应函数。
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
#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>
#include <utility>

#include "csexc.h"
#ifdef __GNUG__
#include <cxxabi.h>
#endif

namespace console {
    /**
     * @struct PairHash
     * @brief 用于 std::pair 类型的哈希函数对象。
     * @details 该结构体为 std::pair 提供哈希支持，使得 std::unordered_map 可以
     *          将 std::pair 作为键。哈希值通过组合两个元素的哈希值计算得出。
     */
    struct PairHash {
        /**
         * @brief 计算一对值的哈希值。
         * @tparam T1 第一个元素的类型。
         * @tparam T2 第二个元素的类型。
         * @param p 要计算哈希值的键值对。
         * @return std::size_t 组合后的哈希值。
         */
        template <typename T1, typename T2>
        std::size_t operator()(std::pair<T1, T2> p) const {
            auto h1 = std::hash<T1>{}(p.first);
            auto h2 = std::hash<T2>{}(p.second);
            return h1 ^ (h2 << 1);
        }
    };

    /**
     * @class Table
     * @brief 双重分派函数表，用于根据两个对象的运行时类型调用注册的函数。
     * @tparam T 所有操作的基类类型，函数表处理的对象都是 T 的派生类型。
     * @tparam F 存储的函数类型，默认为 void (*)(T&, T&)，即接受两个 T
     * 引用的函数指针。
     * @details 该类维护一个从类型对到处理函数的映射。当需要处理两个对象时，
     *          根据它们的实际类型（通过 typeid 获取）查找对应的处理函数并调用。
     *          支持非对称和对称注册（自动注册两个方向）。
     * @code
     * // 使用示例：
     * Table<Shape> collisionTable;
     *
     * class Shape { ... };     // 提供了 collide 接口，调用
     * collisionTable.call(*this, other); class Circle : public Shape { ... };
     * class Rect : public Shape { ... };
     * // 更多类型...
     *
     * int register = []() -> int
     * {
     *     collisionTable.add_symmetric(typeid(Circle), typeid(Rect),
     * onCircleRect);
     *     // 更多注册...
     *     return 0;
     * }();  // 注册的最佳实践。
     *
     * Circle c;
     * Rect r;
     *
     * c.collide(r); // 自动调用合适的函数。
     * @endcode
     */
    template <class T, class F = void (*)(T &, T &)> class Table {
    public:
        /// @brief 用作映射键的类型：type_index 对。
        typedef std::pair<std::type_index, std::type_index> KeyType;
        /// @brief 存储的函数类型。
        typedef F FunctionType;

        /**
         * @brief 构造 Table。
         * @param default_handler
         * 可选的默认处理函数，当没有找到特定类型对的函数时调用。
         */
        Table(FunctionType default_handler = F{}) :
            default_handler(default_handler) {}

        /**
         * @brief 注册一个类型对的处理函数（非对称）。
         * @param type1 第一个对象的类型信息。
         * @param type2 第二个对象的类型信息。
         * @param func  当遇到 (type1, type2) 组合时调用的函数。
         * @note 如果 type1 和 type2 顺序调换，需要另外注册。
         */
        void add(const std::type_index &type1,
            const std::type_index      &type2,
            FunctionType                func) {
            table[{type1, type2}] = func;
        }

        /**
         * @brief 对称地注册一个类型对的处理函数。
         * @param type1 第一个对象的类型信息。
         * @param type2 第二个对象的类型信息。
         * @param func  当遇到 (type1, type2) 或 (type2, type1)
         * 组合时调用的函数。
         * @details 该函数会同时注册 (type1, type2) 和 (type2, type1)
         * 两个方向的函数，
         *          且两个方向使用同一个处理函数。适用于碰撞检测等对称操作。
         */
        void add_symmetric(const std::type_info &type1,
            const std::type_info                &type2,
            FunctionType                         func) {
            add(type1, type2, func);
            add(type2, type1, func);
        }

        /**
         * @brief 根据两个类型获取对应的处理函数。
         * @param type1 第一个对象的类型信息。
         * @param type2 第二个对象的类型信息。
         * @return FunctionType 对应的处理函数。
         * @throw TypeError 如果没有为给定的类型对注册函数。
         */
        FunctionType operator()(
            const std::type_index &type1, const std::type_index &type2) const {
            auto it = table.find({type1, type2});
            if (it != table.end())
                return it->second;
            else if (default_handler)
                return default_handler;
            else {
                static auto get_name
                    = [](const std::type_index &ti) -> std::string {
#ifdef _MSC_VER
                    return ti.name();
#elif defined(__GNUG__)
                    int                                     status = 0;
                    std::unique_ptr<char, void (*)(void *)> result(
                        abi::__cxa_demangle(
                            ti.name(), nullptr, nullptr, &status),
                        std::free);
                    return (status == 0) ? result.get() : ti.name();
#else
                    return ti.name();
#endif
                };
                throw TypeError("Bad Collide Type: <" + get_name(type1)
                                + "> & <" + get_name(type2) + ">");
            }
        }

        /**
         * @brief 根据两个对象的运行时类型调用对应的处理函数。
         * @param a 第一个对象（基类引用）。
         * @param b 第二个对象（基类引用）。
         * @details 该函数自动获取 a 和 b 的实际类型，查找对应的处理函数并调用。
         * @throw TypeError 如果没有为这两个对象的实际类型注册函数。
         */
        void call(T &a, T &b) const { (*this)(typeid(a), typeid(b))(a, b); }

        /**
         * @brief 检查是否为特定的类型对注册了处理函数。
         * @param type1 第一个类型的类型信息。
         * @param type2 第二个类型的类型信息。
         * @return bool 如果已注册则返回 true，否则返回 false。
         */
        bool
        has(const std::type_index &type1, const std::type_index &type2) const {
            return table.find({type1, type2}) != table.end();
        }

        /**
         * @brief 检查是否设置了默认处理函数。
         * @return bool 如果设置了默认处理函数则返回 true，否则返回 false。
         */
        bool has_default() const { return default_handler; }

        /**
         * @brief
         * 检查是否为特定的类型对注册了处理函数，或者是否设置了默认处理函数。
         * @param type1 第一个类型的类型信息。
         * @param type2 第二个类型的类型信息。
         * @return bool 如果已注册或有默认处理函数则返回 true，否则返回 false
         */
        bool callable(
            const std::type_index &type1, const std::type_index &type2) const {
            return has(type1, type2) || default_handler;
        }

        /**
         * @brief 设置默认处理函数，当没有找到特定类型对的函数时调用。
         * @param func 默认处理函数指针。
         */
        void set_default(FunctionType func = F{}) { default_handler = func; }

        /**
         * @brief 移除一个类型对的处理函数。
         * @param type1 第一个类型的类型信息。
         * @param type2 第二个类型的类型信息。
         */
        void
        remove(const std::type_index &type1, const std::type_index &type2) {
            table.erase({type1, type2});
        }

        /**
         * @brief 清空所有注册的类型对和处理函数。
         * @details
         * 该函数会移除表中所有的类型对和对应的处理函数，但不会修改默认处理函数。
         */
        void clear() { table.clear(); }

    private:
        /**
         * @brief 存储类型对到函数的映射。
         * @details 键为 std::type_index 对，值为处理函数。
         */
        std::unordered_map<KeyType, FunctionType, PairHash> table;

        /**
         * @brief 默认处理函数。
         */
        FunctionType default_handler;
    };
}
