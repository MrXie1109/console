/**
 * @file iter.cpp
 * @brief 测试迭代器扩展模块 (IteratorPair, TiedIterators, zip)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/iter.h"

#include <list>
#include <string>
#include <vector>

#include "../include/test.h"

/**
 * @brief 测试 IteratorPair 基本功能。
 * @note 期望：可以正确遍历迭代器范围。
 */
TEST(IteratorPairBasic) {
    std::vector<int> vec  = {1, 2, 3, 4, 5};
    auto             pair = console::iterpair(vec.begin(), vec.end());
    std::vector<int> result;
    for (int x : pair) {
        result.push_back(x);
    }
    std::vector<int> expected = {1, 2, 3, 4, 5};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 IteratorPair 空范围。
 * @note 期望：空范围遍历不执行任何操作。
 */
TEST(IteratorPairEmpty) {
    std::vector<int> vec;
    auto             pair  = console::iterpair(vec.begin(), vec.end());
    int              count = 0;
    for (int x : pair) {
        (void)x;
        ++count;
    }
    ASSERT_EQ(0, count);
}

/**
 * @brief 测试 IteratorPair 类型别名。
 * @note 期望：类型别名正确。
 */
TEST(IteratorPairTypeAliases) {
    std::vector<int> vec  = {1, 2, 3};
    auto             pair = console::iterpair(vec.begin(), vec.end());
    using PairType        = decltype(pair);
    typename PairType::iterator   it  = pair.begin();
    typename PairType::value_type v   = *it;
    typename PairType::reference  ref = *it;
    typename PairType::pointer    ptr = &ref;
    (void)it;
    (void)v;
    (void)ref;
    (void)ptr;
    ASSERT_EQ(1, v);
}

/**
 * @brief 测试 zip 两个 vector。
 * @note 期望：正确配对两个容器的元素。
 */
TEST(ZipTwoVectors) {
    std::vector<int>                 v1 = {1, 2, 3, 4};
    std::vector<int>                 v2 = {10, 20, 30, 40};
    std::vector<std::pair<int, int>> result;
    for (auto [a, b] : console::zip(v1, v2)) {
        result.push_back({a, b});
    }
    std::vector<std::pair<int, int>> expected
        = {{1, 10}, {2, 20}, {3, 30}, {4, 40}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 zip 不同长度的容器（较短者结束）。
 * @note 期望：在较短容器结束时停止。
 */
TEST(ZipDifferentLengths) {
    std::vector<int>                 v1 = {1, 2, 3, 4, 5};
    std::vector<int>                 v2 = {10, 20, 30};
    std::vector<std::pair<int, int>> result;
    for (auto [a, b] : console::zip(v1, v2)) {
        result.push_back({a, b});
    }
    std::vector<std::pair<int, int>> expected = {{1, 10}, {2, 20}, {3, 30}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 zip 第一个容器更长。
 * @note 期望：在第二个容器结束时停止。
 */
TEST(ZipFirstLonger) {
    std::vector<int>                 v1 = {1, 2, 3, 4, 5, 6};
    std::vector<int>                 v2 = {10, 20};
    std::vector<std::pair<int, int>> result;
    for (auto [a, b] : console::zip(v1, v2)) {
        result.push_back({a, b});
    }
    std::vector<std::pair<int, int>> expected = {{1, 10}, {2, 20}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 zip 第二个容器更长。
 * @note 期望：在第一个容器结束时停止。
 */
TEST(ZipSecondLonger) {
    std::vector<int>                 v1 = {1, 2};
    std::vector<int>                 v2 = {10, 20, 30, 40, 50};
    std::vector<std::pair<int, int>> result;
    for (auto [a, b] : console::zip(v1, v2)) {
        result.push_back({a, b});
    }
    std::vector<std::pair<int, int>> expected = {{1, 10}, {2, 20}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 zip 不同类型的容器。
 * @note 期望：不同类型容器正确配对。
 */
TEST(ZipDifferentContainers) {
    std::vector<int>                         v = {1, 2, 3};
    std::list<std::string>                   l = {"one", "two", "three"};
    std::vector<std::pair<int, std::string>> result;
    for (auto [a, b] : console::zip(v, l)) {
        result.push_back({a, b});
    }
    std::vector<std::pair<int, std::string>> expected
        = {{1, "one"}, {2, "two"}, {3, "three"}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 zip 常量容器。
 * @note 期望：常量容器正确配对。
 */
TEST(ZipConstContainers) {
    const std::vector<int>           v1 = {1, 2, 3};
    const std::vector<int>           v2 = {10, 20, 30};
    std::vector<std::pair<int, int>> result;
    for (auto [a, b] : console::zip(v1, v2)) {
        result.push_back({a, b});
    }
    std::vector<std::pair<int, int>> expected = {{1, 10}, {2, 20}, {3, 30}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 zip 混合常量非常量容器。
 * @note 期望：混合类型正确配对。
 */
TEST(ZipMixedConst) {
    std::vector<int>                 v1 = {1, 2, 3};
    const std::vector<int>           v2 = {10, 20, 30};
    std::vector<std::pair<int, int>> result1;
    for (auto [a, b] : console::zip(v1, v2)) {
        result1.push_back({a, b});
    }
    const std::vector<int>           v3 = {100, 200, 300};
    std::vector<int>                 v4 = {1, 2, 3};
    std::vector<std::pair<int, int>> result2;
    for (auto [a, b] : console::zip(v3, v4)) {
        result2.push_back({a, b});
    }
    std::vector<std::pair<int, int>> expected1 = {{1, 10}, {2, 20}, {3, 30}};
    std::vector<std::pair<int, int>> expected2 = {{100, 1}, {200, 2}, {300, 3}};
    ASSERT_EQ(expected1, result1);
    ASSERT_EQ(expected2, result2);
}

/**
 * @brief 测试 zip 空容器。
 * @note 期望：任意容器为空时遍历不执行。
 */
TEST(ZipEmptyContainer) {
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2;
    int              count = 0;
    for (auto [a, b] : console::zip(v1, v2)) {
        (void)a;
        (void)b;
        ++count;
    }
    ASSERT_EQ(0, count);
    std::vector<int> v3;
    std::vector<int> v4 = {1, 2, 3};
    count               = 0;
    for (auto [a, b] : console::zip(v3, v4)) {
        (void)a;
        (void)b;
        ++count;
    }
    ASSERT_EQ(0, count);
}

/**
 * @brief 测试 zip 两个空容器。
 * @note 期望：遍历不执行。
 */
TEST(ZipBothEmpty) {
    std::vector<int> v1;
    std::vector<int> v2;
    int              count = 0;
    for (auto [a, b] : console::zip(v1, v2)) {
        (void)a;
        (void)b;
        ++count;
    }
    ASSERT_EQ(0, count);
}

/**
 * @brief 测试 TiedIterators 的 operator++ 前缀。
 * @note 期望：前缀自增正确推进两个迭代器。
 */
TEST(TiedIteratorsPrefixIncrement) {
    std::vector<int> v1    = {1, 2, 3};
    std::vector<int> v2    = {10, 20, 30};
    auto             begin = console::TiedIterators(v1.begin(), v2.begin());
    auto             end   = console::TiedIterators(v1.end(), v2.end());
    auto             it    = begin;
    ASSERT_NE(it, end);
    ++it;
    ASSERT_NE(it, end);
    ++it;
    ASSERT_NE(it, end);
    ++it;
    ASSERT_EQ(it, end);
}

/**
 * @brief 测试 TiedIterators 的 operator*。
 * @note 期望：解引用返回正确的 pair。
 */
TEST(TiedIteratorsDereference) {
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {10, 20, 30};
    auto             it = console::TiedIterators(v1.begin(), v2.begin());
    auto [a, b]         = *it;
    ASSERT_EQ(1, a);
    ASSERT_EQ(10, b);
    ++it;
    auto [a2, b2] = *it;
    ASSERT_EQ(2, a2);
    ASSERT_EQ(20, b2);
}

/**
 * @brief 测试 zip 修改容器元素。
 * @note 期望：通过引用可以修改元素。
 */
TEST(ZipModifyElements) {
    std::vector<int> v1 = {1, 2, 3};
    std::vector<int> v2 = {10, 20, 30};
    for (auto [a, b] : console::zip(v1, v2)) {
        a *= 2;
        b *= 2;
    }
    std::vector<int> expected1 = {2, 4, 6};
    std::vector<int> expected2 = {20, 40, 60};
    ASSERT_EQ(expected1, v1);
    ASSERT_EQ(expected2, v2);
}

/**
 * @brief 测试 zip 不同容器类型的使用。
 * @note 期望：vector 和 list 正确配对。
 */
TEST(ZipVectorAndList) {
    std::vector<int>                 v = {1, 2, 3};
    std::list<int>                   l = {10, 20, 30, 40, 50};
    std::vector<std::pair<int, int>> result;
    for (auto [a, b] : console::zip(v, l)) {
        result.push_back({a, b});
    }
    std::vector<std::pair<int, int>> expected = {{1, 10}, {2, 20}, {3, 30}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 zip 字符串与 vector。
 * @note 期望：字符串和 vector 正确配对。
 */
TEST(ZipStringAndVector) {
    std::string                       s = "abc";
    std::vector<int>                  v = {1, 2, 3, 4};
    std::vector<std::pair<char, int>> result;
    for (auto [c, n] : console::zip(s, v)) {
        result.push_back({c, n});
    }
    std::vector<std::pair<char, int>> expected = {{'a', 1}, {'b', 2}, {'c', 3}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 iterpair 的直接使用。
 * @note 期望：iterpair 可用于算法。
 */
TEST(IterPairWithAlgorithm) {
    std::vector<int> vec  = {5, 2, 8, 1, 9};
    auto             pair = console::iterpair(vec.begin(), vec.end());
    int              sum  = 0;
    for (int x : pair) {
        sum += x;
    }
    ASSERT_EQ(25, sum);
}

/**
 * @brief 测试 TiedIterators 的类型别名。
 * @note 期望：类型别名正确。
 */
TEST(TiedIteratorsTypeAliases) {
    std::vector<int> v1 = {1, 2};
    std::vector<int> v2 = {10, 20};
    using It
        = console::TiedIterators<decltype(v1.begin()), decltype(v2.begin())>;
    typename It::value_type        vt;
    typename It::difference_type   diff;
    typename It::iterator_category cat;
    (void)vt;
    (void)diff;
    (void)cat;
    ASSERT_TRUE(true);
}

/**
 * @brief 测试 zip 返回的 IteratorPair 可多次遍历。
 * @note 期望：IteratorPair 可以多次遍历。
 */
TEST(ZipMultipleTraversal) {
    std::vector<int> v1     = {1, 2, 3};
    std::vector<int> v2     = {10, 20, 30};
    auto             zipped = console::zip(v1, v2);
    int              count1 = 0;
    for (auto [a, b] : zipped) {
        (void)a;
        (void)b;
        ++count1;
    }
    ASSERT_EQ(3, count1);
    int count2 = 0;
    for (auto [a, b] : zipped) {
        (void)a;
        (void)b;
        ++count2;
    }
    ASSERT_EQ(3, count2);
}

#ifndef NOMAIN
TEST_MAIN
#endif
