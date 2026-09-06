/**
 * @file coroutine.cpp
 * @brief 测试协程模块 (Coroutine, cr_begin, cr_yield, cr_return, cr_throw, cr_end)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../../include/util/coroutine.h"

#include <string>
#include <vector>

#include "../../include/util/test.h"

/**
 * @brief 测试协程基本 Fibonacci 生成。
 * @note 期望：生成正确的 Fibonacci 序列。
 */
TEST(CoroutineFibonacci) {
    auto fib = []() {
        int a = 0, b = 1;
        int i = 0;
        cr_begin(int);
        for (; i < 10; ++i) {
            cr_yield(a);
            int tmp = a;
            a       = b;
            b += tmp;
        }
        cr_end;
    }();
    console::Coroutine<int()> coro(fib);
    std::vector<int>          result;
    for (int x : coro) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 1, 2, 3, 5, 8, 13, 21, 34};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试协程带参 accumulate。
 * @note 期望：每次传入参数，累加并返回。
 */
TEST(CoroutineAccumulate) {
    auto acc = []() {
        int sum = 0;
        cr_begin(int, int i);
        while (true) {
            sum += i;
            cr_yield(sum);
        }
        cr_end;
    }();
    console::Coroutine<int(int)> coro(acc);
    ASSERT_EQ(1, coro(1));
    ASSERT_EQ(3, coro(2));
    ASSERT_EQ(6, coro(3));
    ASSERT_EQ(10, coro(4));
    ASSERT_EQ(15, coro(5));
}

/**
 * @brief 测试协程有限序列。
 * @note 期望：产生有限数量值后抛出 StopIteration。
 */
TEST(CoroutineFiniteSequence) {
    auto gen = []() {
        int i = 0;
        cr_begin(int);
        for (; i < 3; ++i) {
            cr_yield(i);
        }
        cr_end;
    }();
    console::Coroutine<int()> coro(gen);
    std::vector<int>          result;
    for (int x : coro) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2};
    ASSERT_EQ(expected, result);
    ASSERT_THROWS(coro(), console::StopIteration);
}

/**
 * @brief 测试协程 cr_return 提前终止。
 * @note 期望：cr_return 返回值并终止协程。
 */
TEST(CoroutineReturn) {
    auto gen = []() {
        cr_begin(int);
        cr_yield(1);
        cr_yield(2);
        cr_return(3);
        cr_yield(4);
        cr_end;
    }();
    console::Coroutine<int()> coro(gen);
    std::vector<int>          result;
    try {
        while (true) {
            result.push_back(coro());
        }
    } catch (const console::StopIteration &) {
        // 预期停止
    }
    std::vector<int> expected = {1, 2, 3};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试协程 cr_throw 抛出异常。
 * @note 期望：cr_throw 抛出指定异常并终止协程。
 */
TEST(CoroutineThrow) {
    auto gen = []() {
        cr_begin(int);
        cr_yield(1);
        cr_yield(2);
        cr_throw(std::runtime_error("test error"));
        cr_yield(3);
        cr_end;
    }();
    console::Coroutine<int()> coro(gen);
    std::vector<int>          result;
    try {
        result.push_back(coro());
        result.push_back(coro());
        result.push_back(coro());
    } catch (const std::runtime_error &e) {
        ASSERT_STREQ("test error", e.what());
    }
    std::vector<int> expected = {1, 2};
    ASSERT_EQ(expected, result);
    ASSERT_THROWS(coro(), console::StopIteration);
}

/**
 * @brief 测试协程空序列。
 * @note 期望：空协程立即抛出 StopIteration。
 */
TEST(CoroutineEmpty) {
    auto gen = []() {
        cr_begin(int);
        cr_end;
    }();
    console::Coroutine<int()> coro(gen);
    ASSERT_THROWS(coro(), console::StopIteration);
    std::vector<int> result;
    for (int x : coro) {
        result.push_back(x);
    }
    ASSERT_TRUE(result.empty());
}

/**
 * @brief 测试协程返回字符串。
 * @note 期望：正确生成字符串序列。
 */
TEST(CoroutineString) {
    auto gen = []() {
        std::string prefix = "item";
        int         i      = 0;
        cr_begin(std::string);
        for (; i < 5; ++i) {
            cr_yield(prefix + std::to_string(i));
        }
        cr_end;
    }();
    console::Coroutine<std::string()> coro(gen);
    std::vector<std::string>          result;
    for (std::string s : coro) {
        result.push_back(s);
    }
    std::vector<std::string> expected
        = {"item0", "item1", "item2", "item3", "item4"};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试协程迭代器语义。
 * @note 期望：begin/end 支持范围 for。
 */
TEST(CoroutineIteratorSemantics) {
    auto gen = []() {
        int i = 0;
        cr_begin(int);
        for (; i < 5; ++i) {
            cr_yield(i);
        }
        cr_end;
    }();
    console::Coroutine<int()> coro(gen);
    auto                      it  = coro.begin();
    auto                      end = coro.end();
    std::vector<int>          result;
    for (; it != end; ++it) {
        result.push_back(*it);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试协程 operator* 缓存值。
 * @note 期望：连续解引用返回相同值。
 */
TEST(CoroutineDereference) {
    auto gen = []() {
        int i = 0;
        cr_begin(int);
        for (; i < 3; ++i) {
            cr_yield(i);
        }
        cr_end;
    }();
    console::Coroutine<int()> coro(gen);
    auto                      it = coro.begin();
    ASSERT_EQ(0, *it);
    ASSERT_EQ(0, *it);
    ASSERT_EQ(0, *it);
    ++it;
    ASSERT_EQ(1, *it);
    ASSERT_EQ(1, *it);
}

/**
 * @brief 测试协程返回引用。
 * @note 期望：返回内部变量的引用。
 */
TEST(CoroutineReference) {
    auto gen = []() {
        int value = 0;
        int i     = 0;
        cr_begin(int &);
        for (; i < 5; ++i) {
            value = i * 2;
            cr_yield(value);
        }
        cr_end;
    }();
    console::Coroutine<int &()> coro(gen);
    std::vector<int>            result;
    for (int x : coro) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 2, 4, 6, 8};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试协程嵌套 yield。
 * @note 期望：嵌套协程正常工作。
 */
TEST(CoroutineNestedYield) {
    auto inner = []() -> console::Coroutine<int()> {
        int i = 0;
        cr_begin(int);
        for (; i < 3; ++i) {
            cr_yield(i * 10);
        }
        cr_end;
    }();
    auto outer = [&]() {
        int  i  = 0;
        auto it = inner.begin();
        cr_begin(int);
        for (; i < 3; ++i) {
            cr_yield(i);
        }
        for (; it != inner.end(); ++it) {
            cr_yield(*it);
        }
        cr_end;
    }();
    console::Coroutine<int()> coro(outer);
    std::vector<int>          result;
    for (int x : coro) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 0, 10, 20};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试协程带参数并使用 std::ref。
 * @note 期望：通过 std::ref 修改外部变量。
 */
TEST(CoroutineWithRef) {
    int  external = 0;
    auto gen      = [&]() {
        int i = 0;
        cr_begin(int, int step);
        for (; i < 5; ++i) {
            external += step;
            cr_yield(external);
        }
        cr_end;
    }();
    console::Coroutine<int(int)> coro(gen);
    ASSERT_EQ(2, coro(2));
    ASSERT_EQ(4, coro(2));
    ASSERT_EQ(6, coro(2));
    ASSERT_EQ(8, coro(2));
    ASSERT_EQ(10, coro(2));
}

/**
 * @brief 测试协程 cr_return 后迭代器结束。
 * @note 期望：cr_return 后迭代器到达末尾。
 */
TEST(CoroutineReturnEndsIteration) {
    auto gen = []() {
        cr_begin(int);
        cr_yield(1);
        cr_yield(2);
        cr_return(3);
        cr_end;
    }();
    console::Coroutine<int()> coro(gen);
    std::vector<int>          result;
    int                       count = 0;
    for (int x : coro) {
        result.push_back(x);
        if (++count >= 5) break;
    }
    std::vector<int> expected = {1, 2, 3};
    ASSERT_EQ(expected, result);
}

#ifndef NOMAIN
TEST_MAIN
#endif
