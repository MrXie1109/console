/**
 * @file async.cpp
 * @brief 测试异步模块 (Event, Thread)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/async.h"

#include <atomic>
#include <thread>

#include "../include/test.h"

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

    std::thread t([&]() {
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

    std::thread t([&]() {
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

    std::thread t([&]() {
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

    std::thread t([&]() {
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

    std::thread t([&]() {
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
    console::Event   event;
    std::atomic<int> count{0};
    const int        NUM_WAITERS = 5;

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

/**
 * @brief 测试 Thread 的默认构造。
 * @note 期望：默认构造的 Thread 对象未启动线程。
 */
TEST(ThreadDefaultConstruction) {
    console::Thread t;
    ASSERT_FALSE(t.is_joinable());
}

/**
 * @brief 测试 Thread 的构造和启动（不带 Event 参数）。
 * @note 期望：线程正常启动并执行任务。
 */
TEST(ThreadConstructionWithoutEvent) {
    std::atomic<bool> executed{false};

    console::Thread t([&]() { executed = true; });

    t.join();
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Thread 的构造和启动（带 Event 参数）。
 * @note 期望：线程正常启动，Event 参数正确传递，线程可响应 stop 信号。
 */
TEST(ThreadConstructionWithEvent) {
    std::atomic<bool> running{true};
    std::atomic<int>  counter{0};

    console::Thread t([&](const console::Event &ev) {
        while (!ev.is_set()) {
            ++counter;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        running = false;
    });

    // 让线程运行一段时间
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    t.stop();
    t.join();

    ASSERT_FALSE(running.load());
    ASSERT_GT(counter.load(), 0);
}

/**
 * @brief 测试 Thread 的 stop 方法。
 * @note 期望：stop 设置事件标志，线程检测到后退出。
 */
TEST(ThreadStop) {
    std::atomic<bool> stopped{false};

    console::Thread t([&](const console::Event &ev) {
        ev.wait();
        stopped = true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    t.stop();
    t.join();
    ASSERT_TRUE(stopped.load());
}

/**
 * @brief 测试 Thread 的 join 方法。
 * @note 期望：join 阻塞直到线程完成。
 */
TEST(ThreadJoin) {
    std::atomic<bool> executed{false};

    console::Thread t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed = true;
    });

    t.join();
    ASSERT_TRUE(executed.load());
    ASSERT_FALSE(t.is_joinable());
}

/**
 * @brief 测试 Thread 的 detach 方法。
 * @note 期望：detach 后线程分离，is_joinable 返回 false。
 */
TEST(ThreadDetach) {
    std::atomic<bool> executed{false};

    console::Thread t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed = true;
    });

    t.detach();
    ASSERT_FALSE(t.is_joinable());

    // 等待分离的线程完成
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Thread 的 get_id 方法。
 * @note 期望：返回有效的线程 ID。
 */
TEST(ThreadGetId) {
    console::Thread t(
        []() { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });

    std::thread::id id = t.get_id();
    ASSERT_NE(std::thread::id(), id);

    t.join();
}

/**
 * @brief 测试 Thread 的移动构造。
 * @note 期望：移动后新对象拥有线程，原对象为空。
 */
TEST(ThreadMoveConstruction) {
    std::atomic<bool> executed{false};

    console::Thread t1([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed = true;
    });

    ASSERT_TRUE(t1.is_joinable());

    console::Thread t2(std::move(t1));
    ASSERT_FALSE(t1.is_joinable());
    ASSERT_TRUE(t2.is_joinable());

    t2.join();
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Thread 的移动赋值。
 * @note 期望：移动后目标对象拥有线程，源对象为空。
 */
TEST(ThreadMoveAssignment) {
    std::atomic<bool> executed1{false};
    std::atomic<bool> executed2{false};

    console::Thread t1([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed1 = true;
    });

    console::Thread t2([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed2 = true;
    });

    t2 = std::move(t1);
    ASSERT_FALSE(t1.is_joinable());
    ASSERT_TRUE(t2.is_joinable());

    t2.join();
    ASSERT_TRUE(executed1.load());
    // t2 原来的线程应该被 join 了
    ASSERT_TRUE(executed2.load());
}

/**
 * @brief 测试 Thread 的 swap 方法。
 * @note 期望：两个线程对象交换其内部状态。
 */
TEST(ThreadSwap) {
    std::atomic<bool> executed1{false};
    std::atomic<bool> executed2{false};

    console::Thread t1([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed1 = true;
    });

    console::Thread t2([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed2 = true;
    });

    t1.swap(t2);
    t1.join();
    t2.join();

    ASSERT_TRUE(executed1.load());
    ASSERT_TRUE(executed2.load());
}

/**
 * @brief 测试 Thread 的 friend swap 函数。
 * @note 期望：两个线程对象通过 friend swap 交换内部状态。
 */
TEST(ThreadFriendSwap) {
    std::atomic<bool> executed1{false};
    std::atomic<bool> executed2{false};

    console::Thread t1([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed1 = true;
    });

    console::Thread t2([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed2 = true;
    });

    swap(t1, t2);
    t1.join();
    t2.join();

    ASSERT_TRUE(executed1.load());
    ASSERT_TRUE(executed2.load());
}

/**
 * @brief 测试 Thread 析构函数自动 join。
 * @note 期望：析构时自动 stop 并 join 线程。
 */
TEST(ThreadDestructorJoins) {
    std::atomic<bool> executed{false};

    {
        console::Thread t([&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            executed = true;
        });
    } // t 析构，应该 join

    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Thread 的 stop 在析构时自动调用。
 * @note 期望：析构时自动调用 stop，线程收到停止信号。
 */
TEST(ThreadDestructorStops) {
    std::atomic<bool> stopped{false};

    {
        console::Thread t([&](const console::Event &ev) {
            ev.wait();
            stopped = true;
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    } // t 析构，应该 stop 并 join

    ASSERT_TRUE(stopped.load());
}

/**
 * @brief 测试 Thread 的参数转发（多个参数）。
 * @note 期望：所有参数正确转发给线程函数。
 */
TEST(ThreadMultipleArguments) {
    int               a = 10;
    double            b = 3.14;
    std::string       c = "hello";
    std::atomic<bool> executed{false};

    console::Thread t(
        [&](int x, double y, std::string z) {
            ASSERT_EQ(10, x);
            ASSERT_NEAR(3.14, y, 0.001);
            ASSERT_EQ(std::string("hello"), z);
            executed = true;
        },
        a,
        b,
        c);

    t.join();
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Thread 带 Event 参数且额外参数转发。
 * @note 期望：Event 参数和额外参数都正确传递。
 */
TEST(ThreadWithEventAndExtraArguments) {
    std::atomic<bool> executed{false};
    int               value = 42;

    console::Thread t(
        [&](const console::Event &ev, int x) {
            ASSERT_EQ(42, x);
            ev.wait();
            executed = true;
        },
        value);

    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    t.stop();
    t.join();
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Thread 的移动赋值自赋值安全。
 * @note 期望：自赋值不会导致问题。
 */
TEST(ThreadSelfMoveAssignment) {
    std::atomic<bool> executed{false};

    console::Thread t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed = true;
    });

    t = std::move(t); // 自赋值
    t.join();
    ASSERT_TRUE(executed.load());
}

#ifndef NOMAIN
TEST_MAIN
#endif
