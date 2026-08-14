/**
 * @file gen.cpp
 * @brief 测试生成器模式模块 (Generator, gen, ops)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */
#include "../include/gen.h"

#include <string>
#include <vector>

#include "../include/test.h"

/**
 * @brief 测试 Range 生成器基本功能。
 * @note 期望：生成从 0 到 9 的整数序列。
 */
TEST(RangeBasic) {
    auto             r = console::gen::range(0, 10);
    std::vector<int> result;
    for (int x : r) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ASSERT_EQ(expected.size(), result.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        ASSERT_EQ(expected[i], result[i]);
    }
}

/**
 * @brief 测试 Range 生成器单参数。
 * @note 期望：生成从 0 到 4 的整数序列。
 */
TEST(RangeSingleArg) {
    auto             r = console::gen::range(5);
    std::vector<int> result;
    for (int x : r) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Range 生成器步长。
 * @note 期望：生成从 0 到 10 步长为 2 的序列。
 */
TEST(RangeWithStep) {
    auto             r = console::gen::range(0, 10, 2);
    std::vector<int> result;
    for (int x : r) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 2, 4, 6, 8};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Range 生成器负步长。
 * @note 期望：生成从 10 到 0 步长为 -2 的序列。
 */
TEST(RangeNegativeStep) {
    auto             r = console::gen::range(10, 0, -2);
    std::vector<int> result;
    for (int x : r) {
        result.push_back(x);
    }
    std::vector<int> expected = {10, 8, 6, 4, 2};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Range 生成器浮点数。
 * @note 期望：生成浮点数序列。
 */
TEST(RangeFloat) {
    auto                r = console::gen::range(0.0, 1.0, 0.2);
    std::vector<double> result;
    for (double x : r) {
        result.push_back(x);
    }
    ASSERT_NEAR(0.0, result[0], 0.001);
    ASSERT_NEAR(0.2, result[1], 0.001);
    ASSERT_NEAR(0.4, result[2], 0.001);
    ASSERT_NEAR(0.6, result[3], 0.001);
    ASSERT_NEAR(0.8, result[4], 0.001);
    ASSERT_EQ(5, result.size());
}

/**
 * @brief 测试 List 生成器从初始化列表。
 * @note 期望：生成初始化列表中的元素。
 */
TEST(ListFromInitializer) {
    auto             l = console::gen::list({1, 2, 3, 4, 5});
    std::vector<int> result;
    for (int x : l) {
        result.push_back(x);
    }
    std::vector<int> expected = {1, 2, 3, 4, 5};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 List 生成器从容器。
 * @note 期望：生成容器中的所有元素。
 */
TEST(ListFromContainer) {
    std::vector<int> vec = {10, 20, 30, 40};
    auto             l   = console::gen::list(vec);
    std::vector<int> result;
    for (int x : l) {
        result.push_back(x);
    }
    ASSERT_EQ(vec, result);
}

/**
 * @brief 测试 Views 生成器从迭代器范围。
 * @note 期望：生成迭代器范围内的元素。
 */
TEST(ViewsFromIterators) {
    std::vector<int> vec = {5, 6, 7, 8, 9};
    auto             v   = console::gen::views(vec.begin(), vec.end());
    std::vector<int> result;
    for (int x : v) {
        result.push_back(x);
    }
    ASSERT_EQ(vec, result);
}

/**
 * @brief 测试 Views 生成器从容器。
 * @note 期望：生成容器中的所有元素。
 */
TEST(ViewsFromContainer) {
    std::vector<int> vec = {100, 200, 300};
    auto             v   = console::gen::views(vec);
    std::vector<int> result;
    for (int x : v) {
        result.push_back(x);
    }
    ASSERT_EQ(vec, result);
}

/**
 * @brief 测试 Repeat 生成器有限次数。
 * @note 期望：重复生成相同值指定次数。
 */
TEST(RepeatFinite) {
    auto             r = console::gen::repeat(42, 5);
    std::vector<int> result;
    for (int x : r) {
        result.push_back(x);
    }
    std::vector<int> expected = {42, 42, 42, 42, 42};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Repeat 生成器无限次数（取前 n 个）。
 * @note 期望：无限重复生成相同值。
 */
TEST(RepeatInfinite) {
    auto             r = console::gen::repeat(7);
    std::vector<int> result;
    int              count = 0;
    for (int x : r) {
        result.push_back(x);
        if (++count >= 10) break;
    }
    std::vector<int> expected(10, 7);
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Cycle 生成器从初始化列表。
 * @note 期望：无限循环列表中的元素。
 */
TEST(CycleFromInitializer) {
    auto             c = console::gen::cycle({1, 2, 3});
    std::vector<int> result;
    int              count = 0;
    for (int x : c) {
        result.push_back(x);
        if (++count >= 10) break;
    }
    std::vector<int> expected = {1, 2, 3, 1, 2, 3, 1, 2, 3, 1};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Cycle 生成器从容器。
 * @note 期望：无限循环容器中的元素。
 */
TEST(CycleFromContainer) {
    std::vector<int> vec = {5, 6};
    auto             c   = console::gen::cycle(vec);
    std::vector<int> result;
    int              count = 0;
    for (int x : c) {
        result.push_back(x);
        if (++count >= 8) break;
    }
    std::vector<int> expected = {5, 6, 5, 6, 5, 6, 5, 6};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Counter 生成器默认参数。
 * @note 期望：从 0 开始步长 1 无限递增。
 */
TEST(CounterDefault) {
    auto             c = console::gen::counter<int>();
    std::vector<int> result;
    int              count = 0;
    for (int x : c) {
        result.push_back(x);
        if (++count >= 10) break;
    }
    std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Counter 生成器自定义起始和步长。
 * @note 期望：从 5 开始步长 3 递增。
 */
TEST(CounterCustomStartStep) {
    auto             c = console::gen::counter<int>(5, 3);
    std::vector<int> result;
    int              count = 0;
    for (int x : c) {
        result.push_back(x);
        if (++count >= 8) break;
    }
    std::vector<int> expected = {5, 8, 11, 14, 17, 20, 23, 26};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Counter 生成器有限次数。
 * @note 期望：生成指定次数的序列。
 */
TEST(CounterLimited) {
    auto             c = console::gen::counter<int>(0, 1, 5);
    std::vector<int> result;
    for (int x : c) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Map 生成器。
 * @note 期望：对每个元素应用映射函数。
 */
TEST(MapGenerator) {
    auto r = console::gen::range(0, 5);
    auto m = console::gen::Map<decltype(r), console::ops::square_t>(
        r, console::ops::square);
    std::vector<int> result;
    for (int x : m) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 4, 9, 16};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Map 管道操作。
 * @note 期望：使用管道操作符应用映射。
 */
TEST(MapPipeOperator) {
    auto             r = console::gen::range(1, 6);
    auto             m = r | console::gen::map(console::ops::square);
    std::vector<int> result;
    for (int x : m) {
        result.push_back(x);
    }
    std::vector<int> expected = {1, 4, 9, 16, 25};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Filter 生成器。
 * @note 期望：只保留满足谓词的元素。
 */
TEST(FilterGenerator) {
    auto r = console::gen::range(0, 10);
    auto f = console::gen::Filter<decltype(r), console::ops::even_t>(
        r, console::ops::even);
    std::vector<int> result;
    for (int x : f) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 2, 4, 6, 8};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Filter 管道操作。
 * @note 期望：使用管道操作符应用过滤。
 */
TEST(FilterPipeOperator) {
    auto             r = console::gen::range(0, 10);
    auto             f = r | console::gen::filter(console::ops::odd);
    std::vector<int> result;
    for (int x : f) {
        result.push_back(x);
    }
    std::vector<int> expected = {1, 3, 5, 7, 9};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Take 生成器。
 * @note 期望：只取前 n 个元素。
 */
TEST(TakeGenerator) {
    auto             r = console::gen::range(0, 100);
    auto             t = console::gen::Take<decltype(r)>(r, 5);
    std::vector<int> result;
    for (int x : t) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Take 管道操作。
 * @note 期望：使用管道操作符取前 n 个元素。
 */
TEST(TakePipeOperator) {
    auto             r = console::gen::range(0, 100);
    auto             t = r | console::gen::take(5);
    std::vector<int> result;
    for (int x : t) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Drop 生成器。
 * @note 期望：跳过前 n 个元素。
 */
TEST(DropGenerator) {
    auto             r = console::gen::range(0, 10);
    auto             d = console::gen::Drop<decltype(r)>(r, 5);
    std::vector<int> result;
    for (int x : d) {
        result.push_back(x);
    }
    std::vector<int> expected = {5, 6, 7, 8, 9};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Drop 管道操作。
 * @note 期望：使用管道操作符跳过前 n 个元素。
 */
TEST(DropPipeOperator) {
    auto             r = console::gen::range(0, 10);
    auto             d = r | console::gen::drop(3);
    std::vector<int> result;
    for (int x : d) {
        result.push_back(x);
    }
    std::vector<int> expected = {3, 4, 5, 6, 7, 8, 9};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Enumerate 生成器。
 * @note 期望：为每个元素附加索引。
 */
TEST(EnumerateGenerator) {
    auto r = console::gen::range(10, 15);
    auto e = console::gen::Enumerate<decltype(r)>(r);
    std::vector<std::pair<size_t, int>> result;
    for (auto p : e) {
        result.push_back(p);
    }
    std::vector<std::pair<size_t, int>> expected
        = {{0, 10}, {1, 11}, {2, 12}, {3, 13}, {4, 14}};
    ASSERT_EQ(expected.size(), result.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        ASSERT_EQ(expected[i].first, result[i].first);
        ASSERT_EQ(expected[i].second, result[i].second);
    }
}

/**
 * @brief 测试 Enumerate 管道操作。
 * @note 期望：使用管道操作符枚举元素。
 */
TEST(EnumeratePipeOperator) {
    auto                                r = console::gen::range(5, 8);
    auto                                e = r | console::gen::enumerate;
    std::vector<std::pair<size_t, int>> result;
    for (auto p : e) {
        result.push_back(p);
    }
    std::vector<std::pair<size_t, int>> expected = {{0, 5}, {1, 6}, {2, 7}};
    ASSERT_EQ(expected.size(), result.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        ASSERT_EQ(expected[i].first, result[i].first);
        ASSERT_EQ(expected[i].second, result[i].second);
    }
}

/**
 * @brief 测试 Zip 生成器。
 * @note 期望：将两个生成器的元素配对。
 */
TEST(ZipGenerator) {
    auto                             r1 = console::gen::range(0, 5);
    auto                             r2 = console::gen::range(10, 15);
    auto                             z  = console::gen::zip(r1, r2);
    std::vector<std::pair<int, int>> result;
    for (auto p : z) {
        result.push_back(p);
    }
    std::vector<std::pair<int, int>> expected
        = {{0, 10}, {1, 11}, {2, 12}, {3, 13}, {4, 14}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Zip & 操作符。
 * @note 期望：使用 & 操作符压缩两个生成器。
 */
TEST(ZipOperator) {
    auto                             r1 = console::gen::range(5, 8);
    auto                             r2 = console::gen::range(100, 103);
    auto                             z  = r1 & r2;
    std::vector<std::pair<int, int>> result;
    for (auto p : z) {
        result.push_back(p);
    }
    std::vector<std::pair<int, int>> expected = {{5, 100}, {6, 101}, {7, 102}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Generate 生成器。
 * @note 期望：反复调用函数生成无限序列。
 */
TEST(GenerateGenerator) {
    int              n = 0;
    auto             g = console::gen::generate([&n]() { return n++; });
    std::vector<int> result;
    int              count = 0;
    for (int x : g) {
        result.push_back(x);
        if (++count >= 10) break;
    }
    std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 TakeWhile 生成器。
 * @note 期望：取元素直到谓词不成立。
 */
TEST(TakeWhileGenerator) {
    auto r  = console::gen::range(0, 10);
    auto tw = console::gen::TakeWhile<decltype(r), console::ops::Lt<int>>(
        r, console::ops::lt(5));
    std::vector<int> result;
    for (int x : tw) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 TakeWhile 管道操作。
 * @note 期望：使用管道操作符取元素直到谓词不成立。
 */
TEST(TakeWhilePipeOperator) {
    auto             r  = console::gen::range(0, 10);
    auto             tw = r | console::gen::take_while(console::ops::lt(5));
    std::vector<int> result;
    for (int x : tw) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 DropWhile 生成器。
 * @note 期望：跳过元素直到谓词不成立。
 */
TEST(DropWhileGenerator) {
    auto r  = console::gen::range(0, 10);
    auto dw = console::gen::DropWhile<decltype(r), console::ops::Lt<int>>(
        r, console::ops::lt(5));
    std::vector<int> result;
    for (int x : dw) {
        result.push_back(x);
    }
    std::vector<int> expected = {5, 6, 7, 8, 9};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 DropWhile 管道操作。
 * @note 期望：使用管道操作符跳过元素直到谓词不成立。
 */
TEST(DropWhilePipeOperator) {
    auto             r  = console::gen::range(0, 10);
    auto             dw = r | console::gen::drop_while(console::ops::lt(5));
    std::vector<int> result;
    for (int x : dw) {
        result.push_back(x);
    }
    std::vector<int> expected = {5, 6, 7, 8, 9};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试链式管道操作。
 * @note 期望：多个操作符链式工作。
 */
TEST(ChainPipeOperators) {
    auto r      = console::gen::range(0, 20);
    auto result = r | console::gen::filter(console::ops::even)
                  | console::gen::map(console::ops::square)
                  | console::gen::take(5) | console::gen::drop(2);
    std::vector<int> vec;
    for (int x : result) {
        vec.push_back(x);
    }
    std::vector<int> expected = {16, 36, 64};
    ASSERT_EQ(expected, vec);
}

/**
 * @brief 测试 collect 收集到 vector。
 * @note 期望：将生成器元素收集到 vector。
 */
TEST(CollectToVector) {
    auto             r        = console::gen::range(0, 5);
    auto             vec      = r | console::gen::collect<>();
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, vec);
}

/**
 * @brief 测试 collect 收集到指定容器。
 * @note 期望：将生成器元素收集到指定容器。
 */
TEST(CollectToSpecificContainer) {
    auto             r        = console::gen::range(0, 5);
    auto             vec      = r | console::gen::collect<std::vector<int>>();
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, vec);
}

/**
 * @brief 测试 ops 函数组合。
 * @note 期望：函数正确组合。
 */
TEST(OpsCompose) {
    auto f = console::ops::compose(
        console::ops::add(1), console::ops::mul(2), console::ops::sub(3));
    ASSERT_EQ(9, f(5)); // ((5+1)*2)-3 = 9
    auto g
        = console::ops::add(1) >> console::ops::mul(2) >> console::ops::sub(3);
    ASSERT_EQ(9, g(5));
}

/**
 * @brief 测试 ops 谓词组合。
 * @note 期望：谓词正确组合。
 */
TEST(OpsPredicateComposition) {
    auto p = console::ops::gt(5) && console::ops::lt(10);
    ASSERT_TRUE(p(7));
    ASSERT_FALSE(p(3));
    ASSERT_FALSE(p(12));
    auto q = console::ops::lt(3) || console::ops::gt(8);
    ASSERT_TRUE(q(2));
    ASSERT_TRUE(q(10));
    ASSERT_FALSE(q(5));
    auto r = !console::ops::even;
    ASSERT_TRUE(r(3));
    ASSERT_FALSE(r(4));
}

/**
 * @brief 测试 ops 相邻去重。
 * @note 期望：相邻重复元素只保留一个。
 */
TEST(OpsUnique) {
    std::vector<int> input = {1, 1, 2, 2, 2, 3, 4, 4, 5};
    auto             r     = console::gen::list(input);
    auto             u = r | console::gen::filter(console::ops::unique<int>());
    std::vector<int> result;
    for (int x : u) {
        result.push_back(x);
    }
    std::vector<int> expected = {1, 2, 3, 4, 5};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 ops first 和 second。
 * @note 期望：正确提取 pair 的第一个和第二个元素。
 */
TEST(OpsFirstSecond) {
    std::vector<std::pair<int, std::string>> data
        = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto             r      = console::gen::list(data);
    auto             keys   = r | console::gen::map(console::ops::first);
    auto             values = r | console::gen::map(console::ops::second);
    std::vector<int> keys_result;
    for (int x : keys) {
        keys_result.push_back(x);
    }
    std::vector<int> keys_expected = {1, 2, 3};
    ASSERT_EQ(keys_expected, keys_result);
    std::vector<std::string> values_result;
    for (std::string x : values) {
        values_result.push_back(x);
    }
    std::vector<std::string> values_expected = {"one", "two", "three"};
    ASSERT_EQ(values_expected, values_result);
}

/**
 * @brief 测试 ops identity。
 * @note 期望：identity 返回输入值本身。
 */
TEST(OpsIdentity) {
    auto             r = console::gen::range(0, 5);
    auto             i = r | console::gen::map(console::ops::identity);
    std::vector<int> result;
    for (int x : i) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 ops as 转型。
 * @note 期望：将元素转型为指定类型。
 */
TEST(OpsAs) {
    auto                r = console::gen::range(0, 5);
    auto                a = r | console::gen::map(console::ops::as<double>());
    std::vector<double> result;
    for (double x : a) {
        result.push_back(x);
    }
    std::vector<double> expected = {0.0, 1.0, 2.0, 3.0, 4.0};
    ASSERT_EQ(expected.size(), result.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        ASSERT_NEAR(expected[i], result[i], 0.001);
    }
}

/**
 * @brief 测试 Generator 输出流。
 * @note 期望：输出格式为 [elem1, elem2, ...]。
 */
TEST(GeneratorOutputStream) {
    auto               r = console::gen::range(0, 5);
    std::ostringstream oss;
    oss << r;
    ASSERT_EQ(std::string("[0, 1, 2, 3, 4]"), oss.str());
}

/**
 * @brief 测试空生成器输出流。
 * @note 期望：空生成器输出 []。
 */
TEST(GeneratorEmptyOutputStream) {
    auto               r = console::gen::range(0, 0);
    std::ostringstream oss;
    oss << r;
    ASSERT_EQ(std::string("[]"), oss.str());
}

#ifndef NOMAIN
TEST_MAIN
#endif
