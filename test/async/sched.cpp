/**
 * @file sched.cpp
 * @brief 测试调度器模块 (Scheduler)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../../include/async/sched.h"

#include <atomic>
#include <chrono>
#include <thread>

#include "../../include/util/test.h"

/**
 * @brief 测试 Scheduler schedule 延迟执行。
 * @note 期望：函数在指定延迟后执行。
 */
TEST(SchedulerScheduleDelay) {
    console::Scheduler sched;
    std::atomic<bool>  executed{false};
    sched.schedule(std::chrono::milliseconds(50), [&]() { executed = true; });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Scheduler schedule 不提前执行。
 * @note 期望：延迟未到时不执行。
 */
TEST(SchedulerScheduleNotEarly) {
    console::Scheduler sched;
    std::atomic<bool>  executed{false};
    sched.schedule(std::chrono::milliseconds(100), [&]() { executed = true; });
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    ASSERT_FALSE(executed.load());
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Scheduler schedule 使用 Time 参数。
 * @note 期望：使用 console::Time 正确延迟。
 */
TEST(SchedulerScheduleTimeParameter) {
    console::Scheduler sched;
    std::atomic<bool>  executed{false};
    sched.schedule(console::milliseconds(50), [&]() { executed = true; });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Scheduler schedule 使用 double 秒数。
 * @note 期望：使用 double 秒数正确延迟。
 */
TEST(SchedulerScheduleDoubleParameter) {
    console::Scheduler sched;
    std::atomic<bool>  executed{false};
    sched.schedule(0.05, [&]() { executed = true; });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_TRUE(executed.load());
}

/**
 * @brief 测试 Scheduler schedule 多个任务。
 * @note 期望：多个任务按不同延迟执行。
 */
TEST(SchedulerScheduleMultiple) {
    console::Scheduler sched;
    std::atomic<int>   count{0};
    sched.schedule(
        std::chrono::milliseconds(30), [&]() { count.fetch_add(1); });
    sched.schedule(
        std::chrono::milliseconds(60), [&]() { count.fetch_add(2); });
    sched.schedule(
        std::chrono::milliseconds(90), [&]() { count.fetch_add(3); });
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
    ASSERT_EQ(6, count.load());
}

/**
 * @brief 测试 Scheduler interval 周期性执行。
 * @note 期望：函数按间隔周期执行。
 */
TEST(SchedulerIntervalPeriodic) {
    console::Scheduler sched;
    std::atomic<int>   count{0};
    sched.interval(
        std::chrono::milliseconds(20), [&]() { count.fetch_add(1); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sched.cancel_all();
    // 100ms / 20ms = 5 次，允许一定误差
    ASSERT_GE(count.load(), 3);
    ASSERT_LE(count.load(), 7);
}

/**
 * @brief 测试 Scheduler interval 使用 Time 参数。
 * @note 期望：使用 console::Time 正确间隔。
 */
TEST(SchedulerIntervalTimeParameter) {
    console::Scheduler sched;
    std::atomic<int>   count{0};
    sched.interval(console::milliseconds(15), [&]() { count.fetch_add(1); });
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    sched.cancel_all();
    ASSERT_GE(count.load(), 3);
    ASSERT_LE(count.load(), 7);
}

/**
 * @brief 测试 Scheduler interval 使用 double 秒数。
 * @note 期望：使用 double 秒数正确间隔。
 */
TEST(SchedulerIntervalDoubleParameter) {
    console::Scheduler sched;
    std::atomic<int>   count{0};
    sched.interval(0.02, [&]() { count.fetch_add(1); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sched.cancel_all();
    ASSERT_GE(count.load(), 3);
    ASSERT_LE(count.load(), 7);
}

/**
 * @brief 测试 Scheduler cancel_all 取消待执行任务。
 * @note 期望：cancel_all 阻止未执行的 schedule 任务。
 */
TEST(SchedulerCancelAllSchedule) {
    console::Scheduler sched;
    std::atomic<bool>  executed{false};
    sched.schedule(std::chrono::milliseconds(100), [&]() { executed = true; });
    std::this_thread::sleep_for(std::chrono::milliseconds(30));
    sched.cancel_all();
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    ASSERT_FALSE(executed.load());
}

/**
 * @brief 测试 Scheduler cancel_all 取消周期性任务。
 * @note 期望：cancel_all 停止 interval 任务。
 */
TEST(SchedulerCancelAllInterval) {
    console::Scheduler sched;
    std::atomic<int>   count{0};
    sched.interval(
        std::chrono::milliseconds(10), [&]() { count.fetch_add(1); });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    sched.cancel_all();
    int snapshot = count.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(snapshot, count.load());
}

/**
 * @brief 测试 Scheduler 析构自动取消和 join。
 * @note 期望：析构时 cancel_all 并 join 所有 interval 线程。
 */
TEST(SchedulerDestructor) {
    std::atomic<int> count{0};
    {
        console::Scheduler sched;
        sched.interval(
            std::chrono::milliseconds(10), [&]() { count.fetch_add(1); });
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
    int snapshot = count.load();
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_EQ(snapshot, count.load());
}

/**
 * @brief 测试 Scheduler 多个 interval 任务。
 * @note 期望：多个周期性任务并行执行。
 */
TEST(SchedulerMultipleInterval) {
    console::Scheduler sched;
    std::atomic<int>   count1{0};
    std::atomic<int>   count2{0};
    sched.interval(
        std::chrono::milliseconds(15), [&]() { count1.fetch_add(1); });
    sched.interval(
        std::chrono::milliseconds(25), [&]() { count2.fetch_add(1); });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sched.cancel_all();
    ASSERT_GT(count1.load(), 0);
    ASSERT_GT(count2.load(), 0);
}

/**
 * @brief 测试 Scheduler schedule 和 interval 混合。
 * @note 期望：schedule 和 interval 互不干扰。
 */
TEST(SchedulerMixedScheduleInterval) {
    console::Scheduler sched;
    std::atomic<bool>  scheduled{false};
    std::atomic<int>   interval_count{0};
    sched.interval(
        std::chrono::milliseconds(15), [&]() { interval_count.fetch_add(1); });
    sched.schedule(std::chrono::milliseconds(50), [&]() { scheduled = true; });
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    sched.cancel_all();
    ASSERT_TRUE(scheduled.load());
    ASSERT_GT(interval_count.load(), 0);
}

/**
 * @brief 测试 Scheduler 取消后不再执行新任务。
 * @note 期望：cancel_all 后新提交的 schedule 不执行。
 */
TEST(SchedulerCancelThenSchedule) {
    console::Scheduler sched;
    std::atomic<bool>  executed{false};
    sched.cancel_all();
    sched.schedule(std::chrono::milliseconds(10), [&]() { executed = true; });
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    ASSERT_FALSE(executed.load());
}

/**
 * @brief 测试 Scheduler interval 执行顺序。
 * @note 期望：interval 任务按时间间隔执行。
 */
TEST(SchedulerIntervalOrder) {
    console::Scheduler sched;
    std::atomic<int>   count{0};
    std::atomic<int>   last_time{0};
    sched.interval(std::chrono::milliseconds(10), [&]() {
        int t = static_cast<int>(
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now().time_since_epoch())
                .count());
        if (count.load() > 0) {
            int diff = t - last_time.load();
            ASSERT_GE(diff, 5);
        }
        last_time = t;
        count.fetch_add(1);
    });
    std::this_thread::sleep_for(std::chrono::milliseconds(80));
    sched.cancel_all();
    ASSERT_GE(count.load(), 4);
}

#ifndef NOMAIN
TEST_MAIN
#endif
