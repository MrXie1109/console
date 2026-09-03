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
 * @def cr_begin(type, ...)
 * @brief 开始一个协程/生成器函数。
 * @details 该宏定义一个协程的起点。它初始化状态变量并返回一个可调用的 lambda 函数对象，
 *          该对象在每次调用时根据内部状态继续执行协程。
 * @param type 协程的返回类型 (例如 `int` 或 `double`)。
 * @param ...  协程函数的参数列表 (例如 `int a, int b`)。
 * @note 必须与 @ref cr_end 配合使用。
 */
#define cr_begin(type, ...)                                                    \
    int _state = 0;                                                            \
    return [=](__VA_ARGS__) mutable -> type {                                  \
        switch (_state) {                                                      \
        case 0:

/**
 * @def cr_yield(value)
 * @brief 从协程中产生一个值并挂起执行。
 * @details 该宏保存当前执行位置 (通过 __LINE__)，返回指定值给调用者。
 *          下次调用协程时，将从该位置继续执行。
 * @param value 要产生的值。
 * @note 必须与 @ref cr_begin 和 @ref cr_end 配合使用。
 */
#define cr_yield(value)                                                        \
        do {                                                                   \
            _state = __LINE__;                                                 \
            return value;                                                      \
        case __LINE__:;                                                        \
        } while (false)

/**
 * @def cr_return(value)
 * @brief 从协程中返回值，并立刻终止协程。
 * @details 该宏保存当前执行位置 (通过 __LINE__)，返回指定值给调用者。
 *          协程会立刻被终止，下次调用协程时，将会得到 StopIteration。
 * @param value 要产生的值。
 * @note 必须与 @ref cr_begin 和 @ref cr_end 配合使用。
 */
#define cr_return(value)                                                       \
        do {                                                                   \
            _state = __LINE__;                                                 \
            return value;                                                      \
        case __LINE__:                                                         \
            break;                                                             \
        } while (false)

/**
 * @def cr_throw(exception)
 * @brief 从协程中抛出一个异常，并立刻终止协程。
 * @details 该宏保存当前执行位置 (通过 __LINE__)，抛出指定异常给调用者。
 *          协程会立刻被终止，下次调用协程时，将会得到 StopIteration。
 * @param exception 要抛出的异常。
 * @note 必须与 @ref cr_begin 和 @ref cr_end 配合使用。
 */
#define cr_throw(exception)                                                    \
        do {                                                                   \
            _state = __LINE__;                                                 \
            throw exception;                                                   \
        case __LINE__:                                                         \
            break;                                                             \
        } while (false)

/**
 * @def cr_end
 * @brief 结束一个协程/生成器函数。
 * @details 该宏标记协程的结束。当协程执行完毕后再次被调用时，
 *          抛出 StopIteration 异常。
 * @note 必须与 @ref cr_begin 配合使用。
 */
#define cr_end                                                                 \
            _state = __LINE__;                                                 \
        case __LINE__:;                                                        \
        }                                                                      \
        throw console::StopIteration("StopIteration");                         \
    }

// clang-format on

namespace console {
    /**
     * @class Coroutine
     * @brief 协程/生成器包装器。
     * @details 将基于宏的协程函数包装为可迭代的对象 (无参版本，适配范围 for 循环)
     *          或普通可调用对象 (带参版本)。
     * @tparam 函数签名类型，例如 `int()` 或 `int(int, double)`。
     */
    template <class>
    class Coroutine;

    /**
     * @class Coroutine<Ret()>
     * @brief 无参数协程的特化，可用作输入迭代器。
     * @details 每次调用 `operator()` 获取下一个产出值。该特化支持范围 for 循环
     *          (通过 `begin()` 和 `end()`)，也提供迭代器语义。
     *          当协程结束 (抛出 StopIteration)后，
     *          迭代被视为到达末尾。
     * @tparam Ret 产出值的类型。
     */
    template <class Ret>
    class Coroutine<Ret()> {
        /** @brief 底层协程函数 (由宏生成的 lambda)。 */
        std::function<Ret()> coro;
        /** @brief 缓存当前产出值；协程结束后为 `nullptr`。 */
        std::shared_ptr<Ret> ptr;

    public:
        /**
         * @brief 从可调用对象构造协程迭代器。
         * @param f 协程函数 (如宏组合生成的 lambda)。
         */
        template <class F>
        Coroutine(F f) : coro(std::move(f)) {}

        /**
         * @brief 直接调用协程，取下一个产出值。
         * @return 下一个产出值。
         * @throw console::StopIteration 当协程已结束 (无更多产出)。
         */
        Ret operator()() { return coro(); }

        /** @brief 值类型。 */
        using value_type = Ret;
        /** @brief 差值类型。 */
        using difference_type = ptrdiff_t;
        /** @brief 指针类型。 */
        using pointer = value_type *;
        /** @brief 引用类型。 */
        using reference = value_type &;
        /** @brief 迭代器类别 (输入迭代器)。 */
        using iterator_category = std::input_iterator_tag;

        /**
         * @brief 解引用操作符，返回当前缓存值。
         * @details 若尚未取过值，则先推进协程一次。
         * @return 当前产出的值。
         * @throw console::StopIteration 若协程刚刚结束且无产出。
         */
        value_type operator*() {
            if (!ptr) ++(*this);
            if (!ptr) throw StopIteration("StopIteration");
            return *ptr;
        }

        /**
         * @brief 前置自增，推进协程。
         * @return 自身引用。
         */
        Coroutine &operator++() {
            try {
                if (ptr)
                    *ptr = coro();
                else
                    ptr.reset(new Ret(coro()));
            } catch (const StopIteration &) {
                ptr.reset();
            }
            return *this;
        }

        /**
         * @brief 后置自增，推进协程。
         * @return 旧的 (推进前的) 副本。
         */
        Coroutine operator++(int) {
            Coroutine old = *this;
            ++(*this);
            return old;
        }

        /**
         * @brief 相等比较。
         * @details 若尚未到达末尾，先推进协程再判断。
         * @return `true` 当协程已结束 (到达末尾)。
         */
        bool operator==(const Coroutine &) {
            if (!ptr) ++(*this);
            return !ptr;
        }

        /**
         * @brief 不相等比较。
         * @return `true` 当协程尚未结束。
         */
        bool operator!=(const Coroutine &) {
            if (!ptr) ++(*this);
            return !!ptr;
        }

        /** @brief 返回自身作为范围 for 的起始迭代器。 */
        Coroutine begin() { return *this; }
        /** @brief 返回自身作为范围 for 的结束哨兵迭代器。 */
        Coroutine end() { return *this; }
    };

    /**
     * @class Coroutine<Ret(Args...)>
     * @brief 带参数协程的特化，仅作为可调用对象使用。
     * @details 每次调用传入参数，执行协程体并返回一个产出值。
     * @tparam Ret  产出值的类型。
     * @tparam Args 协程调用时所需的参数类型列表。
     */
    template <class Ret, class... Args>
    class Coroutine<Ret(Args...)> {
        /** @brief 底层协程函数 (由宏生成的 lambda)。 */
        std::function<Ret(Args...)> coro;

    public:
        /**
         * @brief 从可调用对象构造带参协程。
         * @param f 协程函数。
         */
        template <class F>
        Coroutine(F &&f) : coro(std::forward<F>(f)) {}

        /**
         * @brief 调用协程并获取产出值。
         * @param args 传入协程的参数，依次展开并转发。
         * @return 协程本次产出的值。
         * @throw console::StopIteration 当协程结束且无产出。
         */
        Ret operator()(Args &&...args) {
            return coro(std::forward<Args>(args)...);
        }
    };
}

/**
 * 这里是作者。这个功能可能看起来不太直观，因为这是一个类似 DSL 的功能，我需要向你介绍。
 * 不过相信我，在明白使用方法后，你一定会喜欢上这个协程模型的。这是一个示例：
 *
 * @code
 * Coroutine<int()> fibonacci(int a, int b) { ///< (1)
 *     cr_begin(int); ///< (2)
 *     while (true) {
 *         cr_yield(a); ///< (3)
 *         int tmp = a;
 *         a       = b;
 *         b += tmp;
 *     }
 *     cr_end; ///< (4)
 * }
 * @endcode
 *
 * 1) 这是一个函数，返回一个协程 (事实上就是一个 lambda)，
 *    类型不一定非是 Coroutine<int()>，但 Coroutine 提供了迭代器接口。
 * 2) 宣告协程区域开始。在此之前声明的变量 (包括函数参数) 将被持久化，
 *    注意，这之后的普通变量不会被持久化 (比如: tmp)！
 * 3) 产出一个值，并挂起协程，下次接着断点执行。
 * 4) 宣告协程区域结束。在这之后，再试图获取值只会得到一个 StopIteration 异常。
 *
 * 另一个示例：
 *
 * @code
 * auto accumulate() { ///< (1)
 *     int sum = 0; ///< (2)
 *     cr_begin(int, int i); ///< (3)
 *     while (true) {
 *         sum += i;
 *         cr_yield(sum);
 *     }
 *     cr_end;
 * }
 * @endcode
 *
 * 1) 与 fibonacci 不同，accumulate 不接受参数。
 *    同时，这里使用 auto 返回值而不是 Coroutine<int(int)>，也是可行的。
 *    但不会得到 Coroutine 类型。在这里无所谓，因为需要参数驱动的协程本就不可迭代。
 * 2) sum 将被持久化。
 * 3) 调用时，还需要发送一个值入内 (即普通函数调用语法)。
 *
 * 这个协程模型是极其轻量的，只占用一个 int 的额外空间，你可以轻松获得一大批协程对象，
 * 而不占用许多的系统内存。然而，任何事情都有利弊，以下是一些可能出现的问题。
 *
 * 1) 持久化变量的声明 *必须* 位于 cr_begin 之前 (就像 K&R C)。
 * 2) 持久化变量 *必须* 可复制 (因为 [=] 捕获)。
 * 3) cr_yield *不能* 跨越变量的初始化语句 (语言限制)
 * 4) 内部若存在 switch 块，则 *不能* 在其中使用 cr_yield。
 * 5) 因为条款 2 的初始化变量位置限制，所以协程内 range-based for 不可用，
 *    同样的，哪怕是普通的 for 循环，也需要提前声明变量。
 *
 * 当然，这基本是可以接受的问题，有困难就克服它！
 */
