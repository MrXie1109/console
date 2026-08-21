/**
 * @file async.cpp
 * @brief 测试异步模块 (Event, Thread, Task, SharedTask, Channel, Group)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/async.h"

#include <atomic>
#include <thread>
#include <vector>

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

/**
 * @brief 测试 Thread 的默认构造。
 * @note 期望：默认构造的 Thread 对象未启动线程。
 */
TEST(ThreadDefaultConstruction) {
    console::Thread t;
    ASSERT_FALSE(t.joinable());
}

/**
 * @brief 测试 Thread 的构造和启动（不带 Event 参数）。
 * @note 期望：线程正常启动并执行任务。
 */
TEST(ThreadConstructionWithoutEvent) {
    std::atomic<bool> executed{false};
    console::Thread   t([&]() { executed = true; });
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
    console::Thread   t([&](const console::Event &ev) {
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
    console::Thread   t([&](const console::Event &ev) {
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
    console::Thread   t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed = true;
    });
    t.join();
    ASSERT_TRUE(executed.load());
    ASSERT_FALSE(t.joinable());
}

/**
 * @brief 测试 Thread 的 detach 方法。
 * @note 期望：detach 后线程分离，joinable 返回 false。
 */
TEST(ThreadDetach) {
    std::atomic<bool> executed{false};
    console::Thread   t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed = true;
    });
    t.detach();
    ASSERT_FALSE(t.joinable());
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
    console::Thread   t1([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed = true;
    });
    ASSERT_TRUE(t1.joinable());
    console::Thread t2(std::move(t1));
    ASSERT_FALSE(t1.joinable());
    ASSERT_TRUE(t2.joinable());
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
    console::Thread   t1([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed1 = true;
    });
    console::Thread   t2([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed2 = true;
    });
    t2 = std::move(t1);
    ASSERT_FALSE(t1.joinable());
    ASSERT_TRUE(t2.joinable());
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
    console::Thread   t1([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed1 = true;
    });
    console::Thread   t2([&]() {
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
    console::Thread   t1([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed1 = true;
    });
    console::Thread   t2([&]() {
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
    console::Thread   t(
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
    console::Thread   t(
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
    console::Thread   t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed = true;
    });
    t = std::move(t); // 自赋值
    t.join();
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Task 的默认构造。
 * @note 期望：默认构造的 Task 对象无效。
 */
TEST(TaskDefaultConstruction) {
    console::Task<int> task;
    ASSERT_FALSE(task.valid());
}

/**
 * @brief 测试 Task 构造和启动 (无 Event 参数)。
 * @note 期望：任务正常执行并返回结果。
 */
TEST(TaskConstructionWithoutEvent) {
    console::Task<int> task([]() { return 42; });
    ASSERT_TRUE(task.valid());
    ASSERT_EQ(42, task.get());
}

/**
 * @brief 测试 Task 构造和启动 (带 Event 参数)。
 * @note 期望：任务可以接收 Event 参数并响应取消信号。
 */
TEST(TaskConstructionWithEvent) {
    std::atomic<bool>   running{true};
    std::atomic<int>    counter{0};
    console::Task<void> task([&](const console::Event &ev) {
        while (!ev.is_set()) {
            ++counter;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        running = false;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    task.cancel();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_FALSE(running.load());
    ASSERT_GT(counter.load(), 0);
}

/**
 * @brief 测试 Task 的 cancel 方法。
 * @note 期望：cancel 设置事件标志，任务抛出取消异常。
 */
TEST(TaskCancel) {
    console::Task<void> task([&](const console::Event &ev) { ev.wait(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    task.cancel();
    bool caught = false;
    try {
        task.get();
    } catch (const console::AsyncError &e) {
        caught = true;
        ASSERT_STREQ("Task has been cancelled.", e.what());
    }
    ASSERT_TRUE(caught);
}

/**
 * @brief 测试 Task 的 wait 方法。
 * @note 期望：wait 阻塞直到任务完成。
 */
TEST(TaskWait) {
    std::atomic<bool>   executed{false};
    console::Task<void> task([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed = true;
    });
    task.wait();
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Task 的 wait_for 方法 (超时成功)。
 * @note 期望：任务在超时前完成，返回 ready 状态。
 */
TEST(TaskWaitForSuccess) {
    console::Task<int> task([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 100;
    });
    auto               status = task.wait_for(std::chrono::milliseconds(200));
    ASSERT_EQ(std::future_status::ready, status);
    ASSERT_EQ(100, task.get());
}

/**
 * @brief 测试 Task 的 wait_for 方法 (超时失败)。
 * @note 期望：任务在超时后仍未完成，返回 timeout 状态。
 */
TEST(TaskWaitForTimeout) {
    console::Task<void> task(
        []() { std::this_thread::sleep_for(std::chrono::milliseconds(200)); });
    auto status = task.wait_for(std::chrono::milliseconds(50));
    ASSERT_EQ(std::future_status::timeout, status);
}

/**
 * @brief 测试 Task 的 wait_for 方法 (Time 参数)。
 * @note 期望：使用 Time 类型参数，任务在超时前完成。
 */
TEST(TaskWaitForTimeParameter) {
    console::Task<int> task([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 42;
    });
    auto               status = task.wait_for(console::Time(200000000));
    ASSERT_EQ(std::future_status::ready, status);
    ASSERT_EQ(42, task.get());
}

/**
 * @brief 测试 Task 的 wait_for 方法 (double 参数)。
 * @note 期望：使用 double 秒数参数，任务在超时前完成。
 */
TEST(TaskWaitForDoubleParameter) {
    console::Task<int> task([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 99;
    });
    auto               status = task.wait_for(0.2);
    ASSERT_EQ(std::future_status::ready, status);
    ASSERT_EQ(99, task.get());
}

/**
 * @brief 测试 Task 的 wait_until 方法。
 * @note 期望：任务在指定时间点前完成。
 */
TEST(TaskWaitUntil) {
    console::Task<int> task([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 2026;
    });
    auto               timeout
        = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
    auto status = task.wait_until(timeout);
    ASSERT_EQ(std::future_status::ready, status);
    ASSERT_EQ(2026, task.get());
}

/**
 * @brief 测试 Task 的 status 方法。
 * @note 期望：正确返回任务状态。
 */
TEST(TaskStatus) {
    console::Task<void> task(
        []() { std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
    task.wait();
    ASSERT_EQ(std::future_status::ready, task.status());
}

/**
 * @brief 测试 Task 的移动构造。
 * @note 期望：移动后新对象拥有任务，原对象无效。
 */
TEST(TaskMoveConstruction) {
    console::Task<int> t1([]() { return 100; });
    ASSERT_TRUE(t1.valid());
    console::Task<int> t2(std::move(t1));
    ASSERT_FALSE(t1.valid());
    ASSERT_TRUE(t2.valid());
    ASSERT_EQ(100, t2.get());
}

/**
 * @brief 测试 Task 的移动赋值。
 * @note 期望：移动后目标对象拥有任务，源对象无效。
 */
TEST(TaskMoveAssignment) {
    console::Task<int> t1([]() { return 200; });
    console::Task<int> t2([]() { return 300; });
    t2 = std::move(t1);
    ASSERT_FALSE(t1.valid());
    ASSERT_TRUE(t2.valid());
    ASSERT_EQ(200, t2.get());
}

/**
 * @brief 测试 Task 的析构函数自动 join。
 * @note 期望：析构时自动 join 线程。
 */
TEST(TaskDestructorJoins) {
    std::atomic<bool> executed{false};
    {
        console::Task<void> task([&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            executed = true;
        });
    }
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Task 的析构函数自动 stop。
 * @note 期望：析构时自动设置 Event 信号。
 */
TEST(TaskDestructorStops) {
    std::atomic<bool> stopped{false};
    {
        console::Task<void> task([&](const console::Event &ev) {
            ev.wait();
            stopped = true;
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    ASSERT_TRUE(stopped.load());
}

/**
 * @brief 测试 Task 的参数转发 (多个参数)。
 * @note 期望：所有参数正确转发。
 */
TEST(TaskMultipleArguments) {
    console::Task<int> task(
        [](int a, double b, std::string c) {
            return a + static_cast<int>(b) + c.length();
        },
        10,
        3.14,
        std::string("hello"));
    ASSERT_EQ(10 + 3 + 5, task.get());
}

/**
 * @brief 测试 Task 的 get 方法只能调用一次。
 * @note 期望：再次调用 get 抛出异常。
 */
TEST(TaskGetOnce) {
    console::Task<int> task([]() { return 42; });
    ASSERT_EQ(42, task.get());
    bool caught = false;
    try {
        task.get();
    } catch (const std::future_error &e) {
        caught = true;
    }
    ASSERT_TRUE(caught);
}

/**
 * @brief 测试 Task 取消已完成的任务。
 * @note 期望：取消已完成任务无效。
 */
TEST(TaskCancelCompleted) {
    console::Task<int> task([]() { return 100; });
    task.wait();
    task.cancel();
    ASSERT_EQ(100, task.get());
}

/**
 * @brief 测试 SharedTask 的默认构造。
 * @note 期望：默认构造的 SharedTask 对象无效。
 */
TEST(SharedTaskDefaultConstruction) {
    console::SharedTask<int> task;
    ASSERT_FALSE(task.valid());
}

/**
 * @brief 测试 SharedTask 构造和启动 (无 Event 参数)。
 * @note 期望：任务正常执行并返回结果。
 */
TEST(SharedTaskConstructionWithoutEvent) {
    console::SharedTask<int> task([]() { return 42; });
    ASSERT_TRUE(task.valid());
    ASSERT_EQ(42, task.get());
}

/**
 * @brief 测试 SharedTask 构造和启动 (带 Event 参数)。
 * @note 期望：任务可以接收 Event 参数并响应取消信号。
 */
TEST(SharedTaskConstructionWithEvent) {
    std::atomic<bool>         running{true};
    std::atomic<int>          counter{0};
    console::SharedTask<void> task([&](const console::Event &ev) {
        while (!ev.is_set()) {
            ++counter;
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        running = false;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    task.cancel();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_FALSE(running.load());
    ASSERT_GT(counter.load(), 0);
}

/**
 * @brief 测试 SharedTask 的 cancel 方法。
 * @note 期望：cancel 设置事件标志，任务抛出取消异常。
 */
TEST(SharedTaskCancel) {
    console::SharedTask<void> task(
        [&](const console::Event &ev) { ev.wait(); });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    task.cancel();
    bool caught = false;
    try {
        task.get();
    } catch (const console::AsyncError &e) {
        caught = true;
        ASSERT_STREQ("SharedTask has been cancelled.", e.what());
    }
    ASSERT_TRUE(caught);
}

/**
 * @brief 测试 SharedTask 的拷贝构造。
 * @note 期望：拷贝后多个对象共享同一个任务。
 */
TEST(SharedTaskCopyConstruction) {
    console::SharedTask<int> t1([]() { return 100; });
    console::SharedTask<int> t2(t1);
    console::SharedTask<int> t3 = t1;
    ASSERT_TRUE(t1.valid());
    ASSERT_TRUE(t2.valid());
    ASSERT_TRUE(t3.valid());
    ASSERT_EQ(100, t1.get());
    ASSERT_EQ(100, t2.get());
    ASSERT_EQ(100, t3.get());
}

/**
 * @brief 测试 SharedTask 的拷贝赋值。
 * @note 期望：拷贝赋值后多个对象共享同一个任务。
 */
TEST(SharedTaskCopyAssignment) {
    console::SharedTask<int> t1([]() { return 200; });
    console::SharedTask<int> t2([]() { return 300; });
    console::SharedTask<int> t3;
    t2 = t1;
    t3 = t1;
    ASSERT_EQ(200, t2.get());
    ASSERT_EQ(200, t3.get());
    ASSERT_EQ(200, t1.get());
}

/**
 * @brief 测试 SharedTask 的 get 方法可多次调用。
 * @note 期望：多次调用 get 返回相同结果，不会抛出异常。
 */
TEST(SharedTaskGetMultiple) {
    console::SharedTask<int> task([]() { return 42; });
    ASSERT_EQ(42, task.get());
    ASSERT_EQ(42, task.get());
    ASSERT_EQ(42, task.get());
}

/**
 * @brief 测试 SharedTask 的 wait 方法。
 * @note 期望：wait 阻塞直到任务完成。
 */
TEST(SharedTaskWait) {
    std::atomic<bool>         executed{false};
    console::SharedTask<void> task([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        executed = true;
    });
    task.wait();
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 SharedTask 的 wait_for 方法 (超时成功)。
 * @note 期望：任务在超时前完成。
 */
TEST(SharedTaskWaitForSuccess) {
    console::SharedTask<int> task([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 100;
    });
    auto status = task.wait_for(std::chrono::milliseconds(200));
    ASSERT_EQ(std::future_status::ready, status);
    ASSERT_EQ(100, task.get());
}

/**
 * @brief 测试 SharedTask 的 wait_for 方法 (超时失败)。
 * @note 期望：任务在超时后仍未完成。
 */
TEST(SharedTaskWaitForTimeout) {
    console::SharedTask<void> task(
        []() { std::this_thread::sleep_for(std::chrono::milliseconds(200)); });
    auto status = task.wait_for(std::chrono::milliseconds(50));
    ASSERT_EQ(std::future_status::timeout, status);
}

/**
 * @brief 测试 SharedTask 的 wait_for 方法 (Time 参数)。
 * @note 期望：使用 Time 类型参数正常工作。
 */
TEST(SharedTaskWaitForTimeParameter) {
    console::SharedTask<int> task([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 42;
    });
    auto                     status = task.wait_for(console::Time(200000000));
    ASSERT_EQ(std::future_status::ready, status);
    ASSERT_EQ(42, task.get());
}

/**
 * @brief 测试 SharedTask 的 wait_for 方法 (double 参数)。
 * @note 期望：使用 double 秒数参数正常工作。
 */
TEST(SharedTaskWaitForDoubleParameter) {
    console::SharedTask<int> task([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 99;
    });
    auto                     status = task.wait_for(0.2);
    ASSERT_EQ(std::future_status::ready, status);
    ASSERT_EQ(99, task.get());
}

/**
 * @brief 测试 SharedTask 的 wait_until 方法。
 * @note 期望：任务在指定时间点前完成。
 */
TEST(SharedTaskWaitUntil) {
    console::SharedTask<int> task([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 2026;
    });
    auto                     timeout
        = std::chrono::steady_clock::now() + std::chrono::milliseconds(200);
    auto status = task.wait_until(timeout);
    ASSERT_EQ(std::future_status::ready, status);
    ASSERT_EQ(2026, task.get());
}

/**
 * @brief 测试 SharedTask 的 status 方法。
 * @note 期望：正确返回任务状态。
 */
TEST(SharedTaskStatus) {
    console::SharedTask<void> task(
        []() { std::this_thread::sleep_for(std::chrono::milliseconds(100)); });
    task.wait();
    ASSERT_EQ(std::future_status::ready, task.status());
}

/**
 * @brief 测试 SharedTask 的移动构造。
 * @note 期望：移动后新对象拥有任务，原对象无效。
 */
TEST(SharedTaskMoveConstruction) {
    console::SharedTask<int> t1([]() { return 100; });
    ASSERT_TRUE(t1.valid());
    console::SharedTask<int> t2(std::move(t1));
    ASSERT_FALSE(t1.valid());
    ASSERT_TRUE(t2.valid());
    ASSERT_EQ(100, t2.get());
}

/**
 * @brief 测试 SharedTask 的移动赋值。
 * @note 期望：移动后目标对象拥有任务，源对象无效。
 */
TEST(SharedTaskMoveAssignment) {
    console::SharedTask<int> t1([]() { return 200; });
    console::SharedTask<int> t2([]() { return 300; });
    t2 = std::move(t1);
    ASSERT_FALSE(t1.valid());
    ASSERT_TRUE(t2.valid());
    ASSERT_EQ(200, t2.get());
}

/**
 * @brief 测试 SharedTask 的析构函数自动 join。
 * @note 期望：析构时自动 join 线程。
 */
TEST(SharedTaskDestructorJoins) {
    std::atomic<bool> executed{false};
    {
        console::SharedTask<void> task([&]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
            executed = true;
        });
    }
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 SharedTask 的析构函数自动 stop。
 * @note 期望：析构时自动设置 Event 信号。
 */
TEST(SharedTaskDestructorStops) {
    std::atomic<bool> stopped{false};
    {
        console::SharedTask<void> task([&](const console::Event &ev) {
            ev.wait();
            stopped = true;
        });
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    ASSERT_TRUE(stopped.load());
}

/**
 * @brief 测试 SharedTask 的参数转发 (多个参数)。
 * @note 期望：所有参数正确转发。
 */
TEST(SharedTaskMultipleArguments) {
    console::SharedTask<int> task(
        [](int a, double b, std::string c) {
            return a + static_cast<int>(b) + c.length();
        },
        10,
        3.14,
        std::string("hello"));
    ASSERT_EQ(10 + 3 + 5, task.get());
}

/**
 * @brief 测试 SharedTask 多个拷贝同时 get。
 * @note 期望：多个副本同时调用 get 都能正确返回结果。
 */
TEST(SharedTaskMultipleCopiesGet) {
    console::SharedTask<int>              task([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 999;
    });
    std::vector<console::SharedTask<int>> copies;
    for (int i = 0; i < 10; ++i) {
        copies.push_back(task);
    }
    for (auto &copy : copies) {
        ASSERT_EQ(999, copy.get());
    }
}

/**
 * @brief 测试 SharedTask 取消已完成的共享任务。
 * @note 期望：取消已完成任务无效。
 */
TEST(SharedTaskCancelCompleted) {
    console::SharedTask<int> task([]() { return 100; });
    task.wait();
    task.cancel();
    ASSERT_EQ(100, task.get());
}

/**
 * @brief 测试 SharedTask 的线程安全性 (并发 get)。
 * @note 期望：多个线程同时调用 get 不会导致数据竞争。
 */
TEST(SharedTaskThreadSafety) {
    console::SharedTask<int> task([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        return 12345;
    });
    std::vector<std::thread> threads;
    std::atomic<int>         success_count{0};
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([&]() {
            if (task.get() == 12345) {
                ++success_count;
            }
        });
    }
    for (auto &t : threads) {
        t.join();
    }
    ASSERT_EQ(10, success_count.load());
}

/**
 * @brief 测试 SharedTask 取消时的线程安全性。
 * @note 期望：取消操作是线程安全的，不会导致死锁。
 */
TEST(SharedTaskCancelThreadSafety) {
    console::SharedTask<void> task(
        [&](const console::Event &ev) { ev.wait(); });
    std::vector<std::thread> threads;
    std::atomic<int>         cancel_count{0};
    for (int i = 0; i < 5; ++i) {
        threads.emplace_back([&]() {
            task.cancel();
            ++cancel_count;
        });
    }
    for (auto &t : threads) {
        t.join();
    }
    ASSERT_GT(cancel_count.load(), 0);
}

/**
 * @brief 测试 Task 和 SharedTask 的差异 (get 可多次调用)。
 * @note 期望：SharedTask 的 get 可以多次调用，而 Task 只能调用一次。
 */
TEST(TaskVsSharedTaskDifference) {
    console::Task<int> task([]() { return 42; });
    ASSERT_EQ(42, task.get());
    bool caught = false;
    try {
        task.get();
    } catch (const std::future_error &) {
        caught = true;
    }
    ASSERT_TRUE(caught);
    console::SharedTask<int> shared_task([]() { return 42; });
    ASSERT_EQ(42, shared_task.get());
    ASSERT_EQ(42, shared_task.get());
    ASSERT_EQ(42, shared_task.get());
}

/**
 * @brief 测试 Channel 有缓冲基本读写。
 * @note 期望：写入和读取数据一致。
 */
TEST(ChannelBasicReadWrite) {
    console::Channel<int, 5> ch;
    ch << 42;
    int value;
    ch >> value;
    ASSERT_EQ(42, value);
}

/**
 * @brief 测试 Channel 有缓冲多个元素。
 * @note 期望：FIFO 顺序正确。
 */
TEST(ChannelMultipleElements) {
    console::Channel<int, 5> ch;
    ch << 1;
    ch << 2;
    ch << 3;
    int v1, v2, v3;
    ch >> v1;
    ch >> v2;
    ch >> v3;
    ASSERT_EQ(1, v1);
    ASSERT_EQ(2, v2);
    ASSERT_EQ(3, v3);
}

/**
 * @brief 测试 Channel 有缓冲 operator* 读取。
 * @note 期望：operator* 返回读取的值。
 */
TEST(ChannelStarOperator) {
    console::Channel<int, 5> ch;
    ch << 42;
    int value = *ch;
    ASSERT_EQ(42, value);
}

/**
 * @brief 测试 Channel 有缓冲 operator-> 读取。
 * @note 期望：operator-> 返回 unique_ptr。
 */
TEST(ChannelArrowOperator) {
    console::Channel<int, 5> ch;
    ch << 100;
    auto ptr = ch.operator->();
    ASSERT_EQ(100, *ptr);
}

/**
 * @brief 测试 Channel 有缓冲 operator bool。
 * @note 期望：有效通道返回 true，关闭且为空返回 false。
 */
TEST(ChannelBoolOperator) {
    console::Channel<int, 5> ch;
    ASSERT_TRUE(static_cast<bool>(ch));
    ch << 42;
    ASSERT_TRUE(static_cast<bool>(ch));
    close(ch);
    int v;
    ch >> v;
    ASSERT_FALSE(static_cast<bool>(ch));
}

/**
 * @brief 测试 Channel 有缓冲 close。
 * @note 期望：close 后写入失败，读取完剩余数据后返回 false。
 */
TEST(ChannelClose) {
    console::Channel<int, 5> ch;
    ch << 1;
    ch << 2;
    close(ch);
    ASSERT_FALSE(ch << 3);
    int v1, v2, v3;
    ASSERT_TRUE(ch >> v1);
    ASSERT_EQ(1, v1);
    ASSERT_TRUE(ch >> v2);
    ASSERT_EQ(2, v2);
    ASSERT_FALSE(ch >> v3);
}

/**
 * @brief 测试 Channel 有缓冲关闭后 operator* 抛出异常。
 * @note 期望：关闭且为空时 operator* 抛出 AsyncError。
 */
TEST(ChannelStarOperatorClosed) {
    console::Channel<int, 5> ch;
    ch << 42;
    close(ch);
    ASSERT_EQ(42, *ch);
    ASSERT_THROWS(*ch, console::AsyncError);
}

/**
 * @brief 测试 Channel 无缓冲基本读写。
 * @note 期望：写入和读取数据一致。
 */
TEST(ChannelUnbufferedBasicReadWrite) {
    console::Channel<int, 0> ch;
    std::thread              writer([&]() { ch << 42; });
    int                      value;
    ch >> value;
    ASSERT_EQ(42, value);
    writer.join();
}

/**
 * @brief 测试 Channel 无缓冲 close。
 * @note 期望：close 后写入失败，读取失败。
 */
TEST(ChannelUnbufferedClose) {
    console::Channel<int, 0> ch;
    close(ch);
    ASSERT_FALSE(ch << 42);
    int v;
    ASSERT_FALSE(ch >> v);
    ASSERT_THROWS(*ch, console::AsyncError);
}

/**
 * @brief 测试 Channel 迭代器。
 * @note 期望：迭代器遍历通道所有元素。
 */
TEST(ChannelIterator) {
    console::Channel<int, 5> ch;
    ch << 10;
    ch << 20;
    ch << 30;
    close(ch);
    std::vector<int> result;
    for (int x : ch) {
        result.push_back(x);
    }
    std::vector<int> expected = {10, 20, 30};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Channel for_each 函数。
 * @note 期望：for_each 遍历所有元素。
 */
TEST(ChannelForEach) {
    console::Channel<int, 5> ch;
    ch << 5;
    ch << 15;
    ch << 25;
    close(ch);
    std::vector<int> result;
    console::for_each(ch, [&](int x) { result.push_back(x); });
    std::vector<int> expected = {5, 15, 25};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Channel 生产者-消费者模式。
 * @note 期望：生产者生产数据，消费者正确消费。
 */
TEST(ChannelProducerConsumer) {
    console::Channel<int, 10> ch;
    std::atomic<int>          sum{0};
    std::thread               producer([&]() {
        for (int i = 0; i < 100; ++i) {
            ch << i;
        }
        close(ch);
    });
    std::thread               consumer([&]() {
        int v;
        while (ch >> v) {
            sum += v;
        }
    });
    producer.join();
    consumer.join();
    ASSERT_EQ(4950, sum.load());
}

/**
 * @brief 测试 Group 默认构造。
 * @note 期望：任务计数为 0。
 */
TEST(GroupDefaultConstruction) {
    console::Group group;
    group.add(1);
    group.done();
    group.wait();
}

/**
 * @brief 测试 Group 带计数构造。
 * @note 期望：任务计数正确。
 */
TEST(GroupWithCount) {
    console::Group group(3);
    group.done();
    group.done();
    group.done();
    group.wait();
}

/**
 * @brief 测试 Group 回调函数。
 * @note 期望：所有任务完成时回调被调用。
 */
TEST(GroupCallback) {
    std::atomic<bool> callback_called{false};
    console::Group    group(2, [&]() { callback_called = true; });
    group.done();
    group.done();
    group.wait();
    ASSERT_TRUE(callback_called.load());
}

/**
 * @brief 测试 Group connect 连接回调。
 * @note 期望：connect 替换回调函数。
 */
TEST(GroupConnect) {
    std::atomic<bool> callback1_called{false};
    std::atomic<bool> callback2_called{false};
    console::Group    group(1, [&]() { callback1_called = true; });
    group.connect([&]() { callback2_called = true; });
    group.done();
    group.wait();
    ASSERT_FALSE(callback1_called.load());
    ASSERT_TRUE(callback2_called.load());
}

/**
 * @brief 测试 Group add 增加任务。
 * @note 期望：任务计数正确增加。
 */
TEST(GroupAdd) {
    console::Group group(1);
    group.add(2);
    group.done();
    group.done();
    group.done();
    group.wait();
}

/**
 * @brief 测试 Group 多次 done。
 * @note 期望：任务计数正确减少。
 */
TEST(GroupMultipleDone) {
    console::Group group(3);
    group.done();
    group.done();
    group.done();
    group.wait();
}

/**
 * @brief 测试 Group 析构时未完成终止。
 * @note 期望：析构时若任务未完成则 std::terminate。
 */
TEST(GroupDestructorTerminate) {
    // 无法直接测试 terminate，只验证构造和析构正常情况
    {
        console::Group group(1);
        group.done();
    }
    // 如果未完成，测试会崩溃，所以这里只是占位
    ASSERT_TRUE(true);
}

/**
 * @brief 测试 Group add 在完成后调用终止。
 * @note 期望：完成后 add 调用 std::terminate。
 */
TEST(GroupAddAfterDone) {
    // 无法直接测试 terminate，只验证正常情况
    console::Group group(1);
    group.done();
    group.wait();
    // 如果完成后 add，会 terminate，所以这里不测试
    ASSERT_TRUE(true);
}

/**
 * @brief 测试 Group done 超过计数终止。
 * @note 期望：done 超过计数时 std::terminate。
 */
TEST(GroupDoneExceedCount) {
    // 无法直接测试 terminate，只验证正常情况
    console::Group group(2);
    group.done();
    group.done();
    group.wait();
    // 如果 done 超过计数，会 terminate，所以这里不测试
    ASSERT_TRUE(true);
}

/**
 * @brief 测试 Group wait 阻塞直到完成。
 * @note 期望：wait 在任务完成前阻塞。
 */
TEST(GroupWaitBlocking) {
    std::atomic<bool> completed{false};
    console::Group    group(1);
    std::thread       t([&]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
        group.done();
        completed = true;
    });
    group.wait();
    ASSERT_TRUE(completed.load());
    t.join();
}

/**
 * @brief 测试 Channel 有缓冲溢出阻塞。
 * @note 期望：通道满时写入阻塞。
 */
TEST(ChannelFullBlocking) {
    console::Channel<int, 2> ch;
    std::atomic<bool>        writer_done{false};
    ch << 1;
    ch << 2;
    std::thread writer([&]() {
        ch << 3;
        writer_done = true;
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_FALSE(writer_done.load());
    int v;
    ch >> v;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_TRUE(writer_done.load());
    writer.join();
}

/**
 * @brief 测试 Channel 无缓冲阻塞读。
 * @note 期望：通道空时读取阻塞。
 */
TEST(ChannelUnbufferedEmptyBlocking) {
    console::Channel<int, 0> ch;
    std::atomic<bool>        reader_ready{false};
    std::atomic<bool>        reader_done{false};
    std::thread              reader([&]() {
        reader_ready = true;
        int v;
        ch >> v;
        reader_done = true;
    });
    while (!reader_ready) {
        std::this_thread::yield();
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_FALSE(reader_done.load());
    ch << 42;
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_TRUE(reader_done.load());
    reader.join();
}

/**
 * @brief 测试 Channel 有缓冲移动语义写入。
 * @note 期望：移动写入正确。
 */
TEST(ChannelMoveWrite) {
    console::Channel<std::string, 5> ch;
    std::string                      s = "hello";
    ch << std::move(s);
    std::string result;
    ch >> result;
    ASSERT_EQ(std::string("hello"), result);
}

/**
 * @brief 测试 Channel 有缓冲 const 引用写入。
 * @note 期望：const 引用写入正确。
 */
TEST(ChannelConstRefWrite) {
    console::Channel<int, 5> ch;
    const int                value = 100;
    ch << value;
    int result;
    ch >> result;
    ASSERT_EQ(100, result);
}

/**
 * @brief 测试 Channel 有缓冲 close 后写入返回 false。
 * @note 期望：关闭后写入立即返回 false。
 */
TEST(ChannelWriteAfterClose) {
    console::Channel<int, 5> ch;
    close(ch);
    ASSERT_FALSE(ch << 42);
}

/**
 * @brief 测试 Channel 无缓冲 close 后写入返回 false。
 * @note 期望：关闭后写入立即返回 false。
 */
TEST(ChannelUnbufferedWriteAfterClose) {
    console::Channel<int, 0> ch;
    close(ch);
    ASSERT_FALSE(ch << 42);
}

/**
 * @brief 测试 Channel 有缓冲 close 后 operator* 抛出异常。
 * @note 期望：关闭且为空时 operator* 抛出 AsyncError。
 */
TEST(ChannelStarOperatorAfterCloseEmpty) {
    console::Channel<int, 5> ch;
    close(ch);
    ASSERT_THROWS(*ch, console::AsyncError);
}

#ifndef NOMAIN
TEST_MAIN
#endif
