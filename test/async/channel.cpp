/**
 * @file async/channel.cpp
 * @brief 测试 Channel 类 (通道)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../../include/async/channel.h"

#include <atomic>
#include <thread>
#include <vector>

#include "../../include/util/test.h"

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
