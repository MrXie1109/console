/**
 * @file async/event.cpp
 * @brief 测试 Event 类 (事件同步原语)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../../include/async/event.h"

#include <atomic>
#include <thread>
#include <vector>

#include "../../include/util/test.h"

/**
 * @brief 测试 Event 的默认状态。
 * @note 期望：新创建的 Event 标志为 false。
 */
TEST(EventDefaultState) {
    console::Event event;
    ASSERT_FALSE(event.is_set());
}

/**
 * @brief 测试 Event 的 set 和 is_set 方法。
 * @note 期望：set 后标志变为 true，多次 set 保持 true。
 */
TEST(EventSetAndIsSet) {
    console::Event event;
    event.set();
    ASSERT_TRUE(event.is_set());
    event.set();
    ASSERT_TRUE(event.is_set());
}

/**
 * @brief 测试 Event 的 clear 方法。
 * @note 期望：clear 后标志变为 false。
 */
TEST(EventClear) {
    console::Event event;
    event.set();
    ASSERT_TRUE(event.is_set());
    event.clear();
    ASSERT_FALSE(event.is_set());
    event.clear();
    ASSERT_FALSE(event.is_set());
}

/**
 * @brief 测试 Event 的 wait 方法（无超时）。
 * @note 期望：在另一个线程 set 后，wait 返回。
 */
TEST(EventWaitNoTimeout) {
    console::Event    event;
    std::atomic<bool> started{false};
    std::atomic<bool> done{false};
    std::thread       t([&]() {
        started = true;
        event.wait();
        done = true;
    });
    while (!started) {
        std::this_thread::yield();
    }
    ASSERT_FALSE(done.load());
    event.set();
    t.join();
    ASSERT_TRUE(done.load());
}

/**
 * @brief 测试 Event 的 wait 方法（带超时，成功）。
 * @note 期望：在超时前 set，wait 返回 true。
 */
TEST(EventWaitWithTimeoutSuccess) {
    console::Event    event;
    std::atomic<bool> started{false};
    std::atomic<bool> result{false};
    std::thread       t([&]() {
        started = true;
        result  = event.wait(std::chrono::milliseconds(500));
    });
    while (!started) {
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    event.set();
    t.join();
    ASSERT_TRUE(result.load());
}

/**
 * @brief 测试 Event 的 wait 方法（带超时，失败）。
 * @note 期望：超时后 wait 返回 false。
 */
TEST(EventWaitWithTimeoutFailure) {
    console::Event    event;
    std::atomic<bool> started{false};
    std::atomic<bool> result{true};
    std::thread       t([&]() {
        started = true;
        result  = event.wait(std::chrono::milliseconds(100));
    });
    while (!started) {
        std::this_thread::yield();
    }
    t.join();
    ASSERT_FALSE(result.load());
}

/**
 * @brief 测试 Event 的 wait 方法（double 参数超时，成功）。
 * @note 期望：使用 double 秒数参数，在超时前 set，wait 返回 true。
 */
TEST(EventWaitWithDoubleTimeoutSuccess) {
    console::Event    event;
    std::atomic<bool> started{false};
    std::atomic<bool> result{false};
    std::thread       t([&]() {
        started = true;
        result  = event.wait(0.5);
    });
    while (!started) {
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    event.set();
    t.join();
    ASSERT_TRUE(result.load());
}

/**
 * @brief 测试 Event 的 wait 方法（double 参数超时，失败）。
 * @note 期望：使用 double 秒数参数，超时后 wait 返回 false。
 */
TEST(EventWaitWithDoubleTimeoutFailure) {
    console::Event    event;
    std::atomic<bool> started{false};
    std::atomic<bool> result{true};
    std::thread       t([&]() {
        started = true;
        result  = event.wait(0.1);
    });
    while (!started) {
        std::this_thread::yield();
    }
    t.join();
    ASSERT_FALSE(result.load());
}

/**
 * @brief 测试 Event 的多个等待者。
 * @note 期望：所有等待者都能被 set 唤醒。
 */
TEST(EventMultipleWaiters) {
    console::Event           event;
    std::atomic<int>         count{0};
    const int                NUM_WAITERS = 5;
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_WAITERS; ++i) {
        threads.emplace_back([&]() {
            event.wait();
            ++count;
        });
    }
    // 确保所有线程都已开始等待
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_EQ(0, count.load());
    event.set();
    for (auto &t : threads) {
        t.join();
    }
    ASSERT_EQ(NUM_WAITERS, count.load());
}

/**
 * @brief 测试 Event 的 set 后再 wait。
 * @note 期望：如果事件已 set，wait 立即返回。
 */
TEST(EventSetThenWait) {
    console::Event    event;
    std::atomic<bool> done{false};
    event.set();
    std::thread t([&]() {
        event.wait();
        done = true;
    });
    t.join();
    ASSERT_TRUE(done.load());
}

#ifndef NOMAIN
TEST_MAIN
#endif
