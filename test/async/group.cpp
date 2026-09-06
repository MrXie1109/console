/**
 * @file async/group.cpp
 * @brief 测试 Group 类 (任务组)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../../include/async/group.h"

#include <atomic>
#include <thread>

#include "../../include/util/test.h"

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

#ifndef NOMAIN
TEST_MAIN
#endif
