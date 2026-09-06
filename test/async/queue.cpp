/**
 * @file async/queue.cpp
 * @brief 测试高性能并发队列模块 (LFQueue, MultiLFQueue)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../../include/async/queue.h"

#include <atomic>
#include <thread>
#include <vector>

#include "../../include/util/test.h"

/**
 * @brief 测试 LFQueue 单线程基本 push/pop。
 * @note 期望：push 后 pop 返回相同值。
 */
TEST(LFQueueBasicPushPop) {
    console::LFQueue<int> q;
    q.push(42);
    int v;
    ASSERT_TRUE(q.pop(v));
    ASSERT_EQ(42, v);
}

/**
 * @brief 测试 LFQueue 单线程多个元素 FIFO 顺序。
 * @note 期望：FIFO 顺序正确。
 */
TEST(LFQueueFIFO) {
    console::LFQueue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    int v;
    ASSERT_TRUE(q.pop(v));
    ASSERT_EQ(1, v);
    ASSERT_TRUE(q.pop(v));
    ASSERT_EQ(2, v);
    ASSERT_TRUE(q.pop(v));
    ASSERT_EQ(3, v);
    ASSERT_FALSE(q.pop(v));
}

/**
 * @brief 测试 LFQueue 移动语义 push。
 * @note 期望：移动 push 正确。
 */
TEST(LFQueueMovePush) {
    console::LFQueue<std::string> q;
    std::string                   s = "hello";
    q.push(std::move(s));
    std::string v;
    ASSERT_TRUE(q.pop(v));
    ASSERT_EQ(std::string("hello"), v);
}

/**
 * @brief 测试 LFQueue 空队列 pop 返回 false。
 * @note 期望：空队列 pop 返回 false。
 */
TEST(LFQueueEmptyPop) {
    console::LFQueue<int> q;
    int                   v;
    ASSERT_FALSE(q.pop(v));
}

/**
 * @brief 测试 LFQueue pop 返回 unique_ptr。
 * @note 期望：成功返回非空指针，失败返回 nullptr。
 */
TEST(LFQueuePopUniquePtr) {
    console::LFQueue<int> q;
    q.push(42);
    auto ptr = q.pop();
    ASSERT_NE(nullptr, ptr);
    ASSERT_EQ(42, *ptr);
    ptr = q.pop();
    ASSERT_EQ(nullptr, ptr);
}

/**
 * @brief 测试 LFQueue 批量 push 迭代器。
 * @note 期望：批量 push 所有元素。
 */
TEST(LFQueueBatchPushIterator) {
    console::LFQueue<int> q;
    std::vector<int>      input = {1, 2, 3, 4, 5};
    q.push(input.begin(), input.end());
    std::vector<int> output;
    int              v;
    while (q.pop(v)) {
        output.push_back(v);
    }
    ASSERT_EQ(input, output);
}

/**
 * @brief 测试 LFQueue 批量 push 带数量。
 * @note 期望：批量 push 指定数量元素。
 */
TEST(LFQueueBatchPushCount) {
    console::LFQueue<int> q;
    std::vector<int>      input = {10, 20, 30, 40, 50};
    q.push(input.begin(), 3);
    std::vector<int> output;
    int              v;
    while (q.pop(v)) {
        output.push_back(v);
    }
    std::vector<int> expected = {10, 20, 30};
    ASSERT_EQ(expected, output);
}

/**
 * @brief 测试 LFQueue 批量 pop 到迭代器。
 * @note 期望：批量 pop 指定数量元素。
 */
TEST(LFQueueBatchPopIterator) {
    console::LFQueue<int> q;
    for (int i = 0; i < 10; ++i) q.push(i);
    std::vector<int> output(5);
    size_t           popped = q.pop(output.begin(), 5);
    ASSERT_EQ(5, popped);
    for (int i = 0; i < 5; ++i) ASSERT_EQ(i, output[i]);
}

/**
 * @brief 测试 LFQueue 批量 pop 返回 vector。
 * @note 期望：批量 pop 返回 vector。
 */
TEST(LFQueueBatchPopVector) {
    console::LFQueue<int> q;
    for (int i = 0; i < 10; ++i) q.push(i);
    auto result = q.pop(5);
    ASSERT_EQ(5, result.size());
    for (int i = 0; i < 5; ++i) ASSERT_EQ(i, result[i]);
}

/**
 * @brief 测试 LFQueue 批量 pop 不足数量。
 * @note 期望：返回实际存在的元素数量。
 */
TEST(LFQueueBatchPopInsufficient) {
    console::LFQueue<int> q;
    q.push(1);
    q.push(2);
    auto result = q.pop(5);
    ASSERT_EQ(2, result.size());
    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
}

/**
 * @brief 测试 LFQueue 批量 push 空范围。
 * @note 期望：空范围不执行任何操作。
 */
TEST(LFQueueBatchPushEmpty) {
    console::LFQueue<int> q;
    std::vector<int>      input;
    q.push(input.begin(), input.end());
    int v;
    ASSERT_FALSE(q.pop(v));
}

/**
 * @brief 测试 LFQueue 单生产者单消费者。
 * @note 期望：所有数据正确传递。
 */
TEST(LFQueueSPSC) {
    console::LFQueue<int> q;
    const int             N = 1000;
    std::thread           producer([&]() {
        for (int i = 0; i < N; ++i) q.push(i);
    });
    std::thread           consumer([&]() {
        int v;
        for (int i = 0; i < N; ++i) {
            while (!q.pop(v)) std::this_thread::yield();
            ASSERT_EQ(i, v);
        }
    });
    producer.join();
    consumer.join();
}

/**
 * @brief 测试 LFQueue 多生产者单消费者。
 * @note 期望：所有数据正确传递，顺序由竞争决定。
 */
TEST(LFQueueMPSC) {
    console::LFQueue<int>    q;
    const int                N = 500;
    const int                M = 4;
    std::atomic<int>         total{0};
    std::vector<std::thread> producers;
    for (int t = 0; t < M; ++t) {
        producers.emplace_back([&, t]() {
            for (int i = 0; i < N; ++i) {
                q.push(t * N + i);
                total.fetch_add(1);
            }
        });
    }
    std::thread consumer([&]() {
        int v;
        int count = 0;
        while (count < N * M) {
            if (q.pop(v))
                ++count;
            else
                std::this_thread::yield();
        }
        ASSERT_EQ(N * M, count);
    });
    for (auto &t : producers) t.join();
    consumer.join();
    ASSERT_EQ(N * M, total.load());
}

/**
 * @brief 测试 LFQueue 单生产者多消费者。
 * @note 期望：所有数据被消费一次。
 */
TEST(LFQueueSPMC) {
    console::LFQueue<int>    q;
    const int                N = 1000;
    const int                M = 4;
    std::atomic<int>         consumed{0};
    std::thread              producer([&]() {
        for (int i = 0; i < N; ++i) q.push(i);
    });
    std::vector<std::thread> consumers;
    for (int t = 0; t < M; ++t) {
        consumers.emplace_back([&]() {
            int v;
            while (consumed.load() < N) {
                if (q.pop(v))
                    consumed.fetch_add(1);
                else
                    std::this_thread::yield();
            }
        });
    }
    producer.join();
    for (auto &t : consumers) t.join();
    ASSERT_EQ(N, consumed.load());
}

/**
 * @brief 测试 MultiLFQueue 基本 push/pop。
 * @note 期望：push 后 pop 返回相同值。
 */
TEST(MultiLFQueueBasicPushPop) {
    console::MultiLFQueue<int> q(4);
    q.push(42);
    int v;
    ASSERT_TRUE(q.pop(v));
    ASSERT_EQ(42, v);
}

/**
 * @brief 测试 MultiLFQueue 多个元素。
 * @note 期望：所有元素都被消费。
 */
TEST(MultiLFQueueMultipleElements) {
    console::MultiLFQueue<int> q(4);
    const int                  N = 100;
    for (int i = 0; i < N; ++i) q.push(i);
    std::vector<int> result;
    int              v;
    for (int i = 0; i < N; ++i) {
        while (!q.pop(v)) std::this_thread::yield();
        result.push_back(v);
    }
    ASSERT_EQ(N, result.size());
    std::sort(result.begin(), result.end());
    for (int i = 0; i < N; ++i) ASSERT_EQ(i, result[i]);
}

/**
 * @brief 测试 MultiLFQueue 多生产者。
 * @note 期望：所有数据正确传递。
 */
TEST(MultiLFQueueMultiProducer) {
    console::MultiLFQueue<int> q(4);
    const int                  N = 500;
    const int                  M = 4;
    std::atomic<int>           total{0};
    std::vector<std::thread>   producers;
    for (int t = 0; t < M; ++t) {
        producers.emplace_back([&, t]() {
            for (int i = 0; i < N; ++i) {
                q.push(t * N + i);
                total.fetch_add(1);
            }
        });
    }
    std::vector<int> result;
    int              v;
    while ((int)result.size() < N * M) {
        if (q.pop(v))
            result.push_back(v);
        else
            std::this_thread::yield();
    }
    for (auto &t : producers) t.join();
    ASSERT_EQ(N * M, result.size());
    std::sort(result.begin(), result.end());
    for (int i = 0; i < N * M; ++i) ASSERT_EQ(i, result[i]);
}

/**
 * @brief 测试 MultiLFQueue 多消费者。
 * @note 期望：所有数据被消费一次。
 */
TEST(MultiLFQueueMultiConsumer) {
    console::MultiLFQueue<int> q(4);
    const int                  N = 2000;
    const int                  M = 4;
    std::atomic<int>           consumed{0};
    for (int i = 0; i < N; ++i) q.push(i);
    std::vector<std::thread> consumers;
    for (int t = 0; t < M; ++t) {
        consumers.emplace_back([&]() {
            int v;
            while (consumed.load() < N) {
                if (q.pop(v))
                    consumed.fetch_add(1);
                else
                    std::this_thread::yield();
            }
        });
    }
    for (auto &t : consumers) t.join();
    ASSERT_EQ(N, consumed.load());
}

/**
 * @brief 测试 MultiLFQueue 批量 pop。
 * @note 期望：批量 pop 正确返回元素。
 */
TEST(MultiLFQueueBatchPop) {
    console::MultiLFQueue<int> q(4);
    const int                  N = 100;
    for (int i = 0; i < N; ++i) q.push(i);
    auto result = q.pop(50);
    ASSERT_EQ(50, result.size());
    // 元素可能来自不同子队列，排序后验证
    std::sort(result.begin(), result.end());
    for (int i = 0; i < 50; ++i) ASSERT_EQ(i, result[i]);
}

/**
 * @brief 测试 MultiLFQueue 混合场景。
 * @note 期望：大量数据在多个线程间正确传递。
 */
TEST(MultiLFQueueMixed) {
    console::MultiLFQueue<int> q(8);
    const int                  N = 500;
    const int                  M = 4;
    std::atomic<int>           produced{0};
    std::atomic<int>           consumed{0};
    std::vector<std::thread>   producers;
    std::vector<std::thread>   consumers;
    for (int t = 0; t < M; ++t) {
        producers.emplace_back([&]() {
            for (int i = 0; i < N; ++i) {
                q.push(i);
                produced.fetch_add(1);
            }
        });
    }
    for (int t = 0; t < M; ++t) {
        consumers.emplace_back([&]() {
            int v;
            while (consumed.load() < N * M) {
                if (q.pop(v))
                    consumed.fetch_add(1);
                else
                    std::this_thread::yield();
            }
        });
    }
    for (auto &t : producers) t.join();
    for (auto &t : consumers) t.join();
    ASSERT_EQ(N * M, produced.load());
    ASSERT_EQ(N * M, consumed.load());
}

#ifndef NOMAIN
TEST_MAIN
#endif
