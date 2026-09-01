/**
 * @file coroutine.h
 * @brief 轻量级 C++ 协程/生成器宏实现。
 * @details 一个极度轻量、基于宏的状态机协程实现，仅占用一个 int 状态变量。
 *          通过宏，将普通函数转换为可多次调用、每次产生一个值的生成器。
 *          可以使用持久化变量保存状态，调用方在每次调用后得到下一个产出值。
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
#include <functional>
#include <memory>
#include <utility>

#include "csexc.h"

// clang-format off

/**
 * @def CO_BEGIN(type, ...)
 * @brief 开始一个协程/生成器函数。
 * @details 该宏定义一个协程的起点。它初始化状态变量并返回一个可调用的 lambda 函数对象，
 *          该对象在每次调用时根据内部状态继续执行协程。
 * @param type 协程的返回类型（例如 `int` 或 `double`）。
 * @param ...  协程函数的参数列表（例如 `int a, int b`）。
 * @note 必须与 @ref CO_YIELD 和 @ref CO_END 配合使用。
 */
#define CO_BEGIN(type, ...)                                                    \
    int _state = 0;                                                            \
    return [=](__VA_ARGS__) mutable -> type {                                  \
        switch (_state) {                                                      \
        case 0:

/**
 * @def CO_YIELD(value)
 * @brief 从协程中产生一个值并挂起执行。
 * @details 该宏保存当前执行位置（通过 __LINE__），返回指定值给调用者。
 *          下次调用协程时，将从该位置继续执行。
 * @param value 要产生的值。
 * @note 必须与 @ref CO_BEGIN 和 @ref CO_END 配合使用。
 */
#define CO_YIELD(value)                                                        \
        do {                                                                   \
            _state = __LINE__;                                                 \
            return value;                                                      \
            case __LINE__:;                                                    \
        } while (false)

/**
 * @def CO_END
 * @brief 结束一个协程/生成器函数。
 * @details 该宏标记协程的结束。当协程执行完毕后再次被调用时，抛出
 *          StopIteration("StopIteration") 异常。
 *
 * @note 必须与 @ref CO_BEGIN 和 @ref CO_YIELD 配合使用。
 */
#define CO_END                                                                 \
        default:                                                               \
            throw console::StopIteration("StopIteration");                     \
        }                                                                      \
    }

// clang-format on

namespace console {
    /**
     * @brief 遍历协程产生的所有值，直到抛出的 StopIteration 异常。
     * @details 内部使用 while(true) 循环反复调用协程，每次将产生的值传给 f 处理，
     *          捕获到 StopIteration 时返回。
     * @tparam Coro 协程类型。
     * @tparam F 函数类型。
     * @param coro 协程对象 (可调用，返回生成值)。
     * @param f 接收每个生成值的函数。
     * @note 对于无尽协程，函数永不返回。
     */
    template <class Coro, class F>
    auto for_each(Coro &&coro, F &&f) -> typename std::enable_if<
        std::is_same<decltype(f(coro())), void>::value>::type {
        try {
            while (true) f(coro());
        } catch (const StopIteration &) {
            return;
        }
    }

    /**
     * @brief 遍历协程产生的所有值，直到抛出的 StopIteration 异常。
     * @details 内部使用 while (f(coro())) 循环反复调用协程，每次将产生的值传给 f 处理，
     *          捕获到 StopIteration 或 f 返回 false 时返回。
     * @tparam Coro 协程类型。
     * @tparam F 函数类型。
     * @param coro 协程对象 (可调用，返回生成值)。
     * @param f 接收每个生成值的函数。
     * @note f 返回 false 时，提前结束遍历。
     */
    template <class Coro, class F>
    auto for_each(Coro &&coro, F &&f) -> typename std::enable_if<
        !std::is_same<decltype(f(coro())), void>::value>::type {
        try {
            while (f(coro()));
        } catch (const StopIteration &) {
            return;
        }
    }
}

/**
 * 这里是作者。这个功能可能看起来不太直观，因为这是一个类似 DSL 的功能，我需要向你介绍。
 * 不过相信我，在明白使用方法后，你一定会喜欢上这个协程模型的。这是一个示例：
 *
 * @code
 * std::function<int()> fibonacci(int a, int b) { ///< (1)
 *     CO_BEGIN(int); ///< (2)
 *     while (true) {
 *         CO_YIELD(a); ///< (3)
 *         int tmp = a;
 *         a       = b;
 *         b += tmp;
 *     }
 *     CO_END; ///< (4)
 * }
 * @endcode
 *
 * 1) 这是一个函数，返回一个协程 (事实上就是一个 lambda)，
 *    类型不一定非是 std::function<int()>。
 * 2) 宣告协程区域开始。在此之前声明的变量 (包括函数参数) 将被持久化，
 *    注意，这之后的普通变量不会被持久化！
 * 3) 产出一个值，并挂起协程，下次接着断点执行。
 * 4) 宣告协程区域结束。在这之后，再试图获取值只会得到一个 StopIteration 异常。
 *
 * 另一个示例：
 *
 * @code
 * auto accumulate() { ///< (1)
 *     int sum = 0; ///< (2)
 *     CO_BEGIN(int, int i); ///< (3)
 *     while (true) {
 *         sum += i;
 *         CO_YIELD(sum);
 *     }
 *     CO_END;
 * }
 * @endcode
 *
 * 1) 与 fibonacci 不同，accumulate 不接受参数。
 *    同时，这里使用 auto 返回值而不是 std::function<int(int)>，也是可行的。
 * 2) sum 将被持久化。
 * 3) 调用时，还需要发送一个值入内 (即普通函数调用语法)。
 *
 * 这个协程模型是极其轻量的，只占用一个 int 的额外空间，你可以轻松获得一大批协程对象，
 * 而不占用许多的系统内存。然而，任何事情都有利弊，以下是一些可能出现的问题。
 *
 * 1) 持久化变量的声明 *必须* 位于 CO_BEGIN 之前。
 * 2) 持久化变量 *必须* 可复制 (因为 [=] 捕获)。
 * 3) CO_YIELD *不能* 跨越变量的初始化语句 (语言限制)
 * 4) 内部若存在 switch 块，则 *不能* 在其中使用 CO_YIELD。
 * 5) 如果出现了某些错误，编译器的报错 *可能* 晦涩难懂。
 *
 * 当然，这基本是可以接受的问题，有困难就克服它！
 * (P.S.: 注释多过代码真是少见，毕竟核心逻辑的那三个宏就那么十几行)
 */
