/**
 * @file pool.cpp
 * @brief 测试线程池模块 (ThreadPool, pool, async)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/pool.h"

#include <atomic>
#include <chrono>
#include <new>
#include <thread>

#include "../include/test.h"

/**
 * @brief 测试 ThreadPool 默认构造。
 * @note 期望：默认构造成功，工作线程数等于硬件并发数。
 */
TEST(ThreadPoolDefaultConstruction) {
    console::ThreadPool pool;
    size_t              expected = std::thread::hardware_concurrency();
    if (expected == 0) expected = 2;
    ASSERT_GE(pool.active_worker_count(), expected);
    pool.close();
}

/**
 * @brief 测试 ThreadPool 指定线程数构造。
 * @note 期望：创建指定数量的工作线程。
 */
TEST(ThreadPoolSpecifiedThreads) {
    console::ThreadPool pool(4);
    ASSERT_EQ(4, pool.active_worker_count());
    pool.close();
}

/**
 * @brief 测试 ThreadPool 提交简单任务。
 * @note 期望：任务正确执行并返回结果。
 */
TEST(ThreadPoolSubmitSimple) {
    console::ThreadPool pool(2);
    auto                future = pool.submit([]() { return 42; });
    ASSERT_EQ(42, future.get());
    pool.close();
}

/**
 * @brief 测试 ThreadPool 提交带参数任务。
 * @note 期望：参数正确传递，任务返回正确结果。
 */
TEST(ThreadPoolSubmitWithArgs) {
    console::ThreadPool pool(2);
    auto future = pool.submit([](int a, int b) { return a + b; }, 10, 20);
    ASSERT_EQ(30, future.get());
    pool.close();
}

/**
 * @brief 测试 ThreadPool 提交多个任务。
 * @note 期望：多个任务并行执行，结果正确。
 */
TEST(ThreadPoolSubmitMultiple) {
    console::ThreadPool           pool(4);
    std::vector<std::future<int>> futures;
    for (int i = 0; i < 10; ++i) {
        futures.push_back(pool.submit([i]() { return i * i; }));
    }
    for (int i = 0; i < 10; ++i) {
        ASSERT_EQ(i * i, futures[i].get());
    }
    pool.close();
}

/**
 * @brief 测试 ThreadPool map 批量任务。
 * @note 期望：map 对容器每个元素应用函数，返回正确结果。
 */
TEST(ThreadPoolMap) {
    console::ThreadPool pool(4);
    std::vector<int>    input   = {1, 2, 3, 4, 5};
    auto                futures = pool.map([](int x) { return x * x; }, input);
    for (size_t i = 0; i < input.size(); ++i) {
        ASSERT_EQ(input[i] * input[i], futures[i].get());
    }
    pool.close();
}

/**
 * @brief 测试 ThreadPool map 顺序正确性。
 * @note 期望：返回的 future 顺序与输入顺序一致。
 */
TEST(ThreadPoolMapOrder) {
    console::ThreadPool pool(4);
    std::vector<int>    input   = {5, 3, 1, 4, 2};
    auto                futures = pool.map([](int x) { return x; }, input);
    for (size_t i = 0; i < input.size(); ++i) {
        ASSERT_EQ(input[i], futures[i].get());
    }
    pool.close();
}

/**
 * @brief 测试 ThreadPool wait 等待所有任务完成。
 * @note 期望：wait 阻塞直到所有任务完成。
 */
TEST(ThreadPoolWait) {
    console::ThreadPool pool(2);
    std::atomic<bool>   completed1{false};
    std::atomic<bool>   completed2{false};
    pool.submit([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        completed1 = true;
    });
    pool.submit([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        completed2 = true;
    });
    pool.wait();
    ASSERT_TRUE(completed1.load());
    ASSERT_TRUE(completed2.load());
    pool.close();
}

/**
 * @brief 测试 ThreadPool waiting_task_count。
 * @note 期望：正确返回等待任务数量。
 */
TEST(ThreadPoolWaitingTaskCount) {
    console::ThreadPool pool(1);
    ASSERT_EQ(0, pool.waiting_task_count());
    auto future1 = pool.submit([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        return 1;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    size_t count_before = pool.waiting_task_count();
    auto   future2      = pool.submit([]() { return 2; });
    size_t count_after  = pool.waiting_task_count();
    ASSERT_EQ(0, count_before);
    ASSERT_EQ(1, count_after);
    pool.close();
}

/**
 * @brief 测试 ThreadPool active_task_count。
 * @note 期望：正确返回正在执行的任务数量。
 */
TEST(ThreadPoolActiveTaskCount) {
    console::ThreadPool pool(2);
    std::atomic<bool>   started{false};
    auto                future = pool.submit([&]() {
        started = true;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    ASSERT_TRUE(started.load());
    pool.close();
}

/**
 * @brief 测试 ThreadPool close 优雅关闭。
 * @note 期望：close 等待所有任务完成后关闭。
 */
TEST(ThreadPoolClose) {
    console::ThreadPool pool(2);
    std::atomic<bool>   executed1{false};
    std::atomic<bool>   executed2{false};
    pool.submit([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed1 = true;
    });
    pool.submit([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed2 = true;
    });
    pool.close();
    ASSERT_TRUE(executed1.load());
    ASSERT_TRUE(executed2.load());
    ASSERT_THROWS(pool.submit([]() { return 0; }), console::AsyncError);
}

/**
 * @brief 测试 ThreadPool abort 粗暴关闭。
 * @note 期望：abort 立即关闭，不等待任务完成。
 */
TEST(ThreadPoolAbort) {
    console::ThreadPool pool(2);
    std::atomic<bool>   executed{false};
    pool.submit([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        executed = true;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    pool.abort();
    ASSERT_THROWS(pool.submit([]() { return 0; }), console::AsyncError);
}

/**
 * @brief 测试 ThreadPool 析构自动 close。
 * @note 期望：析构时自动关闭线程池。
 */
TEST(ThreadPoolDestructorClose) {
    std::atomic<bool> executed{false};
    {
        console::ThreadPool pool(2);
        pool.submit([&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            executed = true;
        });
    }
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 pool 命名空间 submit。
 * @note 期望：使用 pool::submit 提交任务到全局线程池。
 */
TEST(PoolNamespaceSubmit) {
    auto future = console::pool::submit([]() { return 42; });
    ASSERT_EQ(42, future.get());
    console::pool::wait();
}

/**
 * @brief 测试 pool 命名空间 map。
 * @note 期望：使用 pool::map 批量提交任务。
 */
TEST(PoolNamespaceMap) {
    std::vector<int> input = {1, 2, 3};
    auto futures = console::pool::map([](int x) { return x * 2; }, input);
    for (size_t i = 0; i < input.size(); ++i) {
        ASSERT_EQ(input[i] * 2, futures[i].get());
    }
    console::pool::wait();
}

/**
 * @brief 测试 pool 命名空间 wait。
 * @note 期望：pool::wait 等待所有任务完成。
 */
TEST(PoolNamespaceWait) {
    std::atomic<bool> done{false};
    console::pool::submit([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        done = true;
    });
    console::pool::wait();
    ASSERT_TRUE(done.load());
    console::pool::wait();
}

/**
 * @brief 测试 pool 命名空间 close。
 * @note 期望：pool::close 关闭全局线程池。
 */
TEST(PoolNamespaceClose) {
    console::pool::close();
    ASSERT_THROWS(
        console::pool::submit([]() { return 0; }), console::AsyncError);
    console::pool::instance().~ThreadPool();
    new (&console::pool::instance()) console::ThreadPool();
}

/**
 * @brief 测试 async 全局函数。
 * @note 期望：async 提交任务到全局线程池。
 */
TEST(AsyncFunction) {
    auto future = console::async([](int a, int b) { return a + b; }, 5, 7);
    ASSERT_EQ(12, future.get());
    console::pool::wait();
}

/**
 * @brief 测试 async 返回 void。
 * @note 期望：void 返回类型任务正确执行。
 */
TEST(AsyncVoid) {
    std::atomic<bool> executed{false};
    auto              future = console::async([&]() { executed = true; });
    future.get();
    ASSERT_TRUE(executed.load());
    console::pool::wait();
}

/**
 * @brief 测试 ThreadPool 异常处理。
 * @note 期望：任务中抛出的异常在 future.get 时重新抛出。
 */
TEST(ThreadPoolException) {
    console::ThreadPool pool(2);
    auto                future = pool.submit([]() {
        throw std::runtime_error("test exception");
        return 0;
    });
    ASSERT_THROWS(future.get(), std::runtime_error);
    pool.close();
}

/**
 * @brief 测试 ThreadPool 提交任务到关闭池。
 * @note 期望：关闭后提交任务抛出 AsyncError。
 */
TEST(ThreadPoolSubmitAfterClose) {
    console::ThreadPool pool(2);
    pool.close();
    ASSERT_THROWS(pool.submit([]() { return 0; }), console::AsyncError);
}

/**
 * @brief 测试 ThreadPool 大量短任务。
 * @note 期望：大量短任务正确执行完成。
 */
TEST(ThreadPoolManyShortTasks) {
    console::ThreadPool            pool(4);
    std::atomic<int>               counter{0};
    std::vector<std::future<void>> futures;
    for (int i = 0; i < 100; ++i) {
        futures.push_back(pool.submit([&counter]() { counter.fetch_add(1); }));
    }
    for (auto &f : futures) {
        f.get();
    }
    ASSERT_EQ(100, counter.load());
    pool.close();
}

#ifndef NOMAIN
TEST_MAIN
#endif
