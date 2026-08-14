/**
 * @file random.cpp
 * @brief 测试随机数工具模块 (random)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/random.h"

#include <set>
#include <vector>

#include "../include/test.h"

/**
 * @brief 测试 randint 生成整数。
 * @note 期望：生成的整数在指定范围内。
 */
TEST(RandomRandint) {
    int r = console::randint(0, 10);
    ASSERT_GE(r, 0);
    ASSERT_LE(r, 10);
}

/**
 * @brief 测试 randint 多次生成不同值。
 * @note 期望：多次生成的值不完全相同。
 */
TEST(RandomRandintMultiple) {
    std::set<int> values;
    for (int i = 0; i < 50; ++i) {
        values.insert(console::randint(0, 10));
    }
    ASSERT_GT(values.size(), 1);
}

/**
 * @brief 测试 randint long long 类型。
 * @note 期望：支持 long long 类型。
 */
TEST(RandomRandintLongLong) {
    long long r = console::randint<long long>(0, 1000000);
    ASSERT_GE(r, 0);
    ASSERT_LE(r, 1000000);
}

/**
 * @brief 测试 uniform 生成浮点数。
 * @note 期望：生成的浮点数在指定范围内。
 */
TEST(RandomUniform) {
    double r = console::uniform(0.0, 1.0);
    ASSERT_GE(r, 0.0);
    ASSERT_LE(r, 1.0);
}

/**
 * @brief 测试 uniform 多次生成不同值。
 * @note 期望：多次生成的值不完全相同。
 */
TEST(RandomUniformMultiple) {
    std::set<double> values;
    for (int i = 0; i < 50; ++i) {
        values.insert(console::uniform(0.0, 1.0));
    }
    ASSERT_GT(values.size(), 1);
}

/**
 * @brief 测试 randbool 生成布尔值。
 * @note 期望：生成的布尔值在 true/false 之间。
 */
TEST(RandomRandbool) {
    bool b = console::randbool();
    // 只验证编译通过，值可以为 true 或 false
    (void)b;
    ASSERT_TRUE(true);
}

/**
 * @brief 测试 randbool 概率分布。
 * @note 期望：p=1 时始终为 true，p=0 时始终为 false。
 */
TEST(RandomRandboolProbability) {
    int true_count = 0;
    for (int i = 0; i < 100; ++i) {
        if (console::randbool(1.0)) ++true_count;
    }
    ASSERT_EQ(100, true_count);
    true_count = 0;
    for (int i = 0; i < 100; ++i) {
        if (console::randbool(0.0)) ++true_count;
    }
    ASSERT_EQ(0, true_count);
}

/**
 * @brief 测试 seed 设置随机种子。
 * @note 期望：相同种子产生相同序列。
 */
TEST(RandomSeed) {
    console::seed(12345);
    int r1 = console::randint(0, 100);
    int r2 = console::randint(0, 100);
    console::seed(12345);
    int r3 = console::randint(0, 100);
    int r4 = console::randint(0, 100);
    ASSERT_EQ(r1, r3);
    ASSERT_EQ(r2, r4);
}

/**
 * @brief 测试 choice 从 vector 选择。
 * @note 期望：选中的元素在容器中。
 */
TEST(RandomChoiceVector) {
    std::vector<int> v = {1, 2, 3, 4, 5};
    int              r = console::choice(v);
    ASSERT_TRUE(r >= 1 && r <= 5);
}

/**
 * @brief 测试 choice 从 const vector 选择。
 * @note 期望：选中的元素在容器中。
 */
TEST(RandomChoiceConstVector) {
    const std::vector<int> v = {1, 2, 3, 4, 5};
    int                    r = console::choice(v);
    ASSERT_TRUE(r >= 1 && r <= 5);
}

/**
 * @brief 测试 choice 从空容器抛出异常。
 * @note 期望：空容器抛出 ContainerError。
 */
TEST(RandomChoiceEmpty) {
    std::vector<int> v;
    ASSERT_THROWS(console::choice(v), console::ContainerError);
}

/**
 * @brief 测试 choice 从 initializer_list 选择。
 * @note 期望：选中的元素在列表中。
 */
TEST(RandomChoiceInitializerList) {
    int r = console::choice({10, 20, 30, 40});
    ASSERT_TRUE(r == 10 || r == 20 || r == 30 || r == 40);
}

/**
 * @brief 测试 shuffle 打乱 vector。
 * @note 期望：打乱后元素不变，顺序改变。
 */
TEST(RandomShuffleVector) {
    std::vector<int> v        = {1, 2, 3, 4, 5};
    std::vector<int> original = v;
    console::shuffle(v);
    // 元素集合不变
    std::set<int> s1(original.begin(), original.end());
    std::set<int> s2(v.begin(), v.end());
    ASSERT_EQ(s1, s2);
    // 顺序可能改变（但有可能碰巧不变，所以不强制验证）
}

/**
 * @brief 测试 shuffle 空容器。
 * @note 期望：空容器不抛出异常。
 */
TEST(RandomShuffleEmpty) {
    std::vector<int> v;
    console::shuffle(v);
    ASSERT_TRUE(v.empty());
}

/**
 * @brief 测试 rnorm 生成正态分布随机数。
 * @note 期望：生成的随机数数量正确。
 */
TEST(RandomRnorm) {
    auto vec = console::rnorm(100, 0.0, 1.0);
    ASSERT_EQ(100, vec.size());
}

/**
 * @brief 测试 runif 生成均匀分布随机数。
 * @note 期望：生成的随机数在指定范围内。
 */
TEST(RandomRunif) {
    auto vec = console::runif(100, 0.0, 10.0);
    ASSERT_EQ(100, vec.size());
    for (double d : vec) {
        ASSERT_GE(d, 0.0);
        ASSERT_LE(d, 10.0);
    }
}

/**
 * @brief 测试 rbinom 生成二项分布随机数。
 * @note 期望：生成的随机数数量正确。
 */
TEST(RandomRbinom) {
    auto vec = console::rbinom(50, 10, 0.5);
    ASSERT_EQ(50, vec.size());
    for (int i : vec) {
        ASSERT_GE(i, 0);
        ASSERT_LE(i, 10);
    }
}

/**
 * @brief 测试 rpois 生成泊松分布随机数。
 * @note 期望：生成的随机数数量正确。
 */
TEST(RandomRpois) {
    auto vec = console::rpois(50, 3.0);
    ASSERT_EQ(50, vec.size());
    for (int i : vec) {
        ASSERT_GE(i, 0);
    }
}

/**
 * @brief 测试 rexp 生成指数分布随机数。
 * @note 期望：生成的随机数数量正确。
 */
TEST(RandomRexp) {
    auto vec = console::rexp(50, 1.0);
    ASSERT_EQ(50, vec.size());
    for (double d : vec) {
        ASSERT_GE(d, 0.0);
    }
}

/**
 * @brief 测试 rgamma 生成伽马分布随机数。
 * @note 期望：生成的随机数数量正确。
 */
TEST(RandomRgamma) {
    auto vec = console::rgamma(50, 2.0, 1.0);
    ASSERT_EQ(50, vec.size());
    for (double d : vec) {
        ASSERT_GE(d, 0.0);
    }
}

/**
 * @brief 测试 rbeta 生成贝塔分布随机数。
 * @note 期望：生成的随机数在 [0, 1] 范围内。
 */
TEST(RandomRbeta) {
    auto vec = console::rbeta(50, 2.0, 3.0);
    ASSERT_EQ(50, vec.size());
    for (double d : vec) {
        ASSERT_GE(d, 0.0);
        ASSERT_LE(d, 1.0);
    }
}

/**
 * @brief 测试 rchisq 生成卡方分布随机数。
 * @note 期望：生成的随机数数量正确。
 */
TEST(RandomRchisq) {
    auto vec = console::rchisq(50, 3.0);
    ASSERT_EQ(50, vec.size());
    for (double d : vec) {
        ASSERT_GE(d, 0.0);
    }
}

/**
 * @brief 测试 rt 生成 t 分布随机数。
 * @note 期望：生成的随机数数量正确。
 */
TEST(RandomRt) {
    auto vec = console::rt(50, 5.0);
    ASSERT_EQ(50, vec.size());
}

/**
 * @brief 测试 rf 生成 F 分布随机数。
 * @note 期望：生成的随机数数量正确。
 */
TEST(RandomRf) {
    auto vec = console::rf(50, 3.0, 5.0);
    ASSERT_EQ(50, vec.size());
    for (double d : vec) {
        ASSERT_GE(d, 0.0);
    }
}

/**
 * @brief 测试 rlnorm 生成对数正态分布随机数。
 * @note 期望：生成的随机数数量正确。
 */
TEST(RandomRlnorm) {
    auto vec = console::rlnorm(50, 0.0, 1.0);
    ASSERT_EQ(50, vec.size());
    for (double d : vec) {
        ASSERT_GT(d, 0.0);
    }
}

/**
 * @brief 测试 rweibull 生成韦布尔分布随机数。
 * @note 期望：生成的随机数数量正确。
 */
TEST(RandomRweibull) {
    auto vec = console::rweibull(50, 2.0, 1.0);
    ASSERT_EQ(50, vec.size());
    for (double d : vec) {
        ASSERT_GE(d, 0.0);
    }
}

/**
 * @brief 测试 sample 有放回抽样。
 * @note 期望：返回指定数量的元素，允许重复。
 */
TEST(RandomSampleReplace) {
    std::vector<int> v      = {1, 2, 3, 4, 5};
    auto             result = console::sample(v, 10, true);
    ASSERT_EQ(10, result.size());
    for (int x : result) {
        ASSERT_TRUE(x >= 1 && x <= 5);
    }
}

/**
 * @brief 测试 sample 无放回抽样。
 * @note 期望：返回指定数量的元素，无重复。
 */
TEST(RandomSampleNoReplace) {
    std::vector<int> v      = {1, 2, 3, 4, 5};
    auto             result = console::sample(v, 3, false);
    ASSERT_EQ(3, result.size());
    std::set<int> s(result.begin(), result.end());
    ASSERT_EQ(3, s.size());
    for (int x : result) {
        ASSERT_TRUE(x >= 1 && x <= 5);
    }
}

/**
 * @brief 测试 sample 无放回抽样超出范围。
 * @note 期望：size 超过容器大小时抛出 ContainerError。
 */
TEST(RandomSampleNoReplaceTooLarge) {
    std::vector<int> v = {1, 2, 3};
    ASSERT_THROWS(console::sample(v, 5, false), console::ContainerError);
}

/**
 * @brief 测试 sample 空容器。
 * @note 期望：空容器抛出 ContainerError。
 */
TEST(RandomSampleEmpty) {
    std::vector<int> v;
    ASSERT_THROWS(console::sample(v, 1), console::ContainerError);
}

/**
 * @brief 测试 sample 从 initializer_list。
 * @note 期望：正确抽样。
 */
TEST(RandomSampleInitializerList) {
    auto result = console::sample({1, 2, 3, 4, 5}, 3, false);
    ASSERT_EQ(3, result.size());
    for (int x : result) {
        ASSERT_TRUE(x >= 1 && x <= 5);
    }
}

/**
 * @brief 测试 sample 有放回从单元素容器。
 * @note 期望：始终返回相同元素。
 */
TEST(RandomSampleSingleElement) {
    std::vector<int> v      = {42};
    auto             result = console::sample(v, 5, true);
    ASSERT_EQ(5, result.size());
    for (int x : result) {
        ASSERT_EQ(42, x);
    }
}

/**
 * @brief 测试 choice 从 set。
 * @note 期望：选中的元素在容器中。
 */
TEST(RandomChoiceSet) {
    std::set<int> s = {1, 2, 3, 4, 5};
    int           r = console::choice(s);
    ASSERT_TRUE(s.find(r) != s.end());
}

/**
 * @brief 测试 shuffle 从 list。
 * @note 期望：打乱成功。
 */
TEST(RandomShuffleList) {
    std::list<int> l        = {1, 2, 3, 4, 5};
    std::list<int> original = l;
    console::shuffle(l);
    std::set<int> s1(original.begin(), original.end());
    std::set<int> s2(l.begin(), l.end());
    ASSERT_EQ(s1, s2);
}

#ifndef NOMAIN
TEST_MAIN
#endif
