/**
 * @file async/task.cpp
 * @brief 测试 Task 和 SharedTask 类 (异步任务)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../../include/async/task.h"

#include <atomic>
#include <thread>
#include <vector>

#include "../../include/util/test.h"

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

#ifndef NOMAIN
TEST_MAIN
#endif
