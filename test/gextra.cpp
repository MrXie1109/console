/**
 * @file gextra.cpp
 * @brief 测试生成器库扩展模块。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */
#include "../include/gextra.h"

#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

#include "../include/test.h"

static std::string create_temp_file(const std::string &content) {
    std::string filename
        = "gextra_test_temp_" + std::to_string(rand()) + ".tmp";
    FILE *f = std::fopen(filename.c_str(), "w");
    if (!f) {
        throw std::runtime_error("Failed to create temp file");
    }
    std::fwrite(content.c_str(), 1, content.size(), f);
    std::fclose(f);
    return filename;
}

static void remove_temp_file(const std::string &filename) {
    std::remove(filename.c_str());
}

/**
 * @brief 测试 Random 生成器基本功能。
 * @note 期望：生成指定范围内的随机数。
 */
TEST(RandomGenerator) {
    auto             r = console::gen::random(0, 10);
    std::vector<int> result;
    int              count = 0;
    for (int x : r) {
        result.push_back(x);
        if (++count >= 10) break;
    }
    ASSERT_EQ(10, result.size());
    for (int x : result) {
        ASSERT_GE(x, 0);
        ASSERT_LE(x, 10);
    }
}

/**
 * @brief 测试 Random 生成器不同种子。
 * @note 期望：不同种子生成不同的序列。
 */
TEST(RandomDifferentSeeds) {
    auto             r1 = console::gen::random(0, 100, 12345);
    auto             r2 = console::gen::random(0, 100, 54321);
    std::vector<int> v1, v2;
    int              count = 0;
    for (int x : r1) {
        v1.push_back(x);
        if (++count >= 10) break;
    }
    count = 0;
    for (int x : r2) {
        v2.push_back(x);
        if (++count >= 10) break;
    }
    ASSERT_NE(v1, v2);
}

/**
 * @brief 测试 Random 生成器浮点数。
 * @note 期望：生成指定范围内的浮点数。
 */
TEST(RandomFloat) {
    auto                r = console::gen::random(0.0, 1.0);
    std::vector<double> result;
    int                 count = 0;
    for (double x : r) {
        result.push_back(x);
        if (++count >= 10) break;
    }
    ASSERT_EQ(10, result.size());
    for (double x : result) {
        ASSERT_GE(x, 0.0);
        ASSERT_LE(x, 1.0);
    }
}

/**
 * @brief 测试 FileLines 生成器。
 * @note 期望：逐行读取文件内容。
 */
TEST(FileLinesGenerator) {
    std::string              content  = "Line 1\nLine 2\nLine 3\nLine 4";
    std::string              filename = create_temp_file(content);
    auto                     lines    = console::gen::file_lines(filename);
    std::vector<std::string> result;
    for (std::string line : lines) {
        result.push_back(line);
    }
    std::vector<std::string> expected
        = {"Line 1", "Line 2", "Line 3", "Line 4"};
    ASSERT_EQ(expected, result);
    remove_temp_file(filename);
}

/**
 * @brief 测试 FileLines 生成器空文件。
 * @note 期望：空文件返回空序列。
 */
TEST(FileLinesEmptyFile) {
    std::string              filename = create_temp_file("");
    auto                     lines    = console::gen::file_lines(filename);
    std::vector<std::string> result;
    for (std::string line : lines) {
        result.push_back(line);
    }
    ASSERT_TRUE(result.empty());
    remove_temp_file(filename);
}

/**
 * @brief 测试 FileLines 生成器不存在的文件。
 * @note 期望：抛出 FileError 异常。
 */
TEST(FileLinesNonexistent) {
    ASSERT_THROWS(console::gen::file_lines("nonexistent_file_12345.tmp"),
        console::FileError);
}

/**
 * @brief 测试 FileChunks 生成器。
 * @note 期望：按块读取文件内容。
 */
TEST(FileChunksGenerator) {
    std::string              content  = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    std::string              filename = create_temp_file(content);
    auto                     chunks   = console::gen::file_chunks(filename, 10);
    std::vector<std::string> result;
    for (std::string chunk : chunks) {
        result.push_back(chunk);
    }
    std::vector<std::string> expected = {"ABCDEFGHIJ", "KLMNOPQRST", "UVWXYZ"};
    ASSERT_EQ(expected, result);
    remove_temp_file(filename);
}

/**
 * @brief 测试 FileChunks 生成器空文件。
 * @note 期望：空文件返回空序列。
 */
TEST(FileChunksEmptyFile) {
    std::string              filename = create_temp_file("");
    auto                     chunks   = console::gen::file_chunks(filename, 10);
    std::vector<std::string> result;
    for (std::string chunk : chunks) {
        result.push_back(chunk);
    }
    ASSERT_TRUE(result.empty());
    remove_temp_file(filename);
}

/**
 * @brief 测试 Window 生成器。
 * @note 期望：生成固定大小的滑动窗口。
 */
TEST(WindowGenerator) {
    auto                          r = console::gen::range(0, 10);
    auto                          w = console::gen::Window<decltype(r)>(r, 3);
    std::vector<std::vector<int>> result;
    for (auto window : w) {
        result.push_back(window);
    }
    std::vector<std::vector<int>> expected = {{0, 1, 2},
        {1, 2, 3},
        {2, 3, 4},
        {3, 4, 5},
        {4, 5, 6},
        {5, 6, 7},
        {6, 7, 8},
        {7, 8, 9}};
    ASSERT_EQ(expected.size(), result.size());
    for (size_t i = 0; i < expected.size(); ++i) {
        ASSERT_EQ(expected[i], result[i]);
    }
}

/**
 * @brief 测试 Window 管道操作。
 * @note 期望：使用管道操作符创建窗口生成器。
 */
TEST(WindowPipeOperator) {
    auto                          r = console::gen::range(0, 8);
    auto                          w = r | console::gen::window(4);
    std::vector<std::vector<int>> result;
    for (auto window : w) {
        result.push_back(window);
    }
    std::vector<std::vector<int>> expected = {
        {0, 1, 2, 3}, {1, 2, 3, 4}, {2, 3, 4, 5}, {3, 4, 5, 6}, {4, 5, 6, 7}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Window 生成器窗口大小为0。
 * @note 期望：抛出 ValueError 异常。
 */
TEST(WindowZeroSize) {
    auto r = console::gen::range(0, 5);
    ASSERT_THROWS(console::gen::Window<decltype(r)>(r, 0), console::ValueError);
}

/**
 * @brief 测试 Chunk 生成器。
 * @note 期望：将元素按块分组。
 */
TEST(ChunkGenerator) {
    auto                          r = console::gen::range(0, 10);
    auto                          c = console::gen::Chunk<decltype(r)>(r, 3);
    std::vector<std::vector<int>> result;
    for (auto chunk : c) {
        result.push_back(chunk);
    }
    std::vector<std::vector<int>> expected
        = {{0, 1, 2}, {3, 4, 5}, {6, 7, 8}, {9}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Chunk 管道操作。
 * @note 期望：使用管道操作符创建分块生成器。
 */
TEST(ChunkPipeOperator) {
    auto                          r = console::gen::range(0, 10);
    auto                          c = r | console::gen::chunk(4);
    std::vector<std::vector<int>> result;
    for (auto chunk : c) {
        result.push_back(chunk);
    }
    std::vector<std::vector<int>> expected
        = {{0, 1, 2, 3}, {4, 5, 6, 7}, {8, 9}};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Chunk 生成器块大小为0。
 * @note 期望：抛出 ValueError 异常。
 */
TEST(ChunkZeroSize) {
    auto r = console::gen::range(0, 5);
    ASSERT_THROWS(console::gen::Chunk<decltype(r)>(r, 0), console::ValueError);
}

/**
 * @brief 测试 Chain 生成器两个生成器连接。
 * @note 期望：两个生成器首尾连接。
 */
TEST(ChainTwoGenerators) {
    auto             r1 = console::gen::range(0, 3);
    auto             r2 = console::gen::range(10, 13);
    auto             c  = console::gen::chain(r1, r2);
    std::vector<int> result;
    for (int x : c) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 10, 11, 12};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Chain 生成器多个生成器连接。
 * @note 期望：多个生成器首尾连接。
 */
TEST(ChainMultipleGenerators) {
    auto             r1 = console::gen::range(0, 2);
    auto             r2 = console::gen::range(5, 7);
    auto             r3 = console::gen::range(10, 12);
    auto             c  = console::gen::chain(r1, r2, r3);
    std::vector<int> result;
    for (int x : c) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 5, 6, 10, 11};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Chain + 操作符。
 * @note 期望：使用 + 操作符连接两个生成器。
 */
TEST(ChainPlusOperator) {
    auto             r1 = console::gen::range(0, 3);
    auto             r2 = console::gen::range(10, 13);
    auto             c  = r1 + r2;
    std::vector<int> result;
    for (int x : c) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 10, 11, 12};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 StepBy 生成器。
 * @note 期望：按指定步长跳过元素。
 */
TEST(StepByGenerator) {
    auto             r = console::gen::range(0, 10);
    auto             s = console::gen::StepBy<decltype(r)>(r, 2);
    std::vector<int> result;
    for (int x : s) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 2, 4, 6, 8};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 StepBy 管道操作。
 * @note 期望：使用管道操作符创建步进生成器。
 */
TEST(StepByPipeOperator) {
    auto             r = console::gen::range(0, 10);
    auto             s = r | console::gen::step_by(3);
    std::vector<int> result;
    for (int x : s) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 3, 6, 9};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 StepBy 生成器步长为0。
 * @note 期望：抛出 ValueError 异常。
 */
TEST(StepByZeroStep) {
    auto r = console::gen::range(0, 5);
    ASSERT_THROWS(console::gen::StepBy<decltype(r)>(r, 0), console::ValueError);
}

/**
 * @brief 测试 TakeUntil 生成器。
 * @note 期望：取元素直到谓词成立（包含第一个满足条件的元素）。
 */
TEST(TakeUntilGenerator) {
    auto r  = console::gen::range(0, 10);
    auto tu = console::gen::TakeUntil<decltype(r), console::ops::Gt<int>>(
        r, console::ops::gt(5));
    std::vector<int> result;
    for (int x : tu) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4, 5};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 TakeUntil 管道操作。
 * @note 期望：使用管道操作符取元素直到谓词成立。
 */
TEST(TakeUntilPipeOperator) {
    auto             r  = console::gen::range(0, 10);
    auto             tu = r | console::gen::take_until(console::ops::ge(5));
    std::vector<int> result;
    for (int x : tu) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 DropUntil 生成器。
 * @note 期望：跳过元素直到谓词成立。
 */
TEST(DropUntilGenerator) {
    auto r  = console::gen::range(0, 10);
    auto du = console::gen::DropUntil<decltype(r), console::ops::Gt<int>>(
        r, console::ops::gt(5));
    std::vector<int> result;
    for (int x : du) {
        result.push_back(x);
    }
    std::vector<int> expected = {6, 7, 8, 9};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 DropUntil 管道操作。
 * @note 期望：使用管道操作符跳过元素直到谓词成立。
 */
TEST(DropUntilPipeOperator) {
    auto             r  = console::gen::range(0, 10);
    auto             du = r | console::gen::drop_until(console::ops::ge(5));
    std::vector<int> result;
    for (int x : du) {
        result.push_back(x);
    }
    std::vector<int> expected = {5, 6, 7, 8, 9};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Pipeline 组合多个适配器。
 * @note 期望：多个适配器按顺序应用。
 */
TEST(PipelineComposition) {
    auto r  = console::gen::range(0, 20);
    auto pl = console::gen::pipeline(console::gen::filter(console::ops::even),
        console::gen::map(console::ops::square),
        console::gen::take(5));
    auto result = r | pl;
    std::vector<int> vec;
    for (int x : result) {
        vec.push_back(x);
    }
    std::vector<int> expected = {0, 4, 16, 36, 64};
    ASSERT_EQ(expected, vec);
}

/**
 * @brief 测试 Pipeline % 操作符。
 * @note 期望：使用 % 操作符组合适配器。
 */
TEST(PipelinePercentOperator) {
    auto r  = console::gen::range(1, 10);
    auto pl = console::gen::filter(console::ops::odd)
              % console::gen::map(console::ops::square) % console::gen::take(3);
    auto             result = r | pl;
    std::vector<int> vec;
    for (int x : result) {
        vec.push_back(x);
    }
    std::vector<int> expected = {1, 9, 25};
    ASSERT_EQ(expected, vec);
}

/**
 * @brief 测试 Scan 生成器。
 * @note 期望：生成累积状态序列。
 */
TEST(ScanGenerator) {
    auto             r = console::gen::range(1, 6);
    auto             s = r | console::gen::scan(0, console::ops::plus);
    std::vector<int> result;
    for (int x : s) {
        result.push_back(x);
    }
    std::vector<int> expected = {1, 3, 6, 10, 15};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Scan 生成器使用乘法。
 * @note 期望：生成累积乘积序列。
 */
TEST(ScanMultiply) {
    auto             r = console::gen::range(1, 6);
    auto             s = r | console::gen::scan(1, console::ops::multiplies);
    std::vector<int> result;
    for (int x : s) {
        result.push_back(x);
    }
    std::vector<int> expected = {1, 2, 6, 24, 120};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 reduce 归约。
 * @note 期望：将生成器所有元素归约为单个值。
 */
TEST(ReduceSum) {
    auto r   = console::gen::range(1, 6);
    int  sum = r | console::gen::reduce(0, console::ops::plus);
    ASSERT_EQ(15, sum);
}

/**
 * @brief 测试 reduce 乘法归约。
 * @note 期望：将生成器所有元素归约为乘积。
 */
TEST(ReduceProduct) {
    auto r       = console::gen::range(1, 6);
    int  product = r | console::gen::reduce(1, console::ops::multiplies);
    ASSERT_EQ(120, product);
}

/**
 * @brief 测试 ops 幂运算变换器。
 * @note 期望：正确计算幂。
 */
TEST(OpsPow) {
    auto             r = console::gen::range(1, 5);
    auto             p = r | console::gen::map(console::ops::pow(3));
    std::vector<int> result;
    for (int x : p) {
        result.push_back(x);
    }
    std::vector<int> expected = {1, 8, 27, 64};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 ops 三角函数变换器。
 * @note 期望：正确计算三角函数。
 */
TEST(OpsTrigonometric) {
    auto                r = console::gen::range(0, 5);
    auto                s = r | console::gen::map(console::ops::sin);
    std::vector<double> result;
    for (double x : s) {
        result.push_back(x);
    }
    ASSERT_NEAR(0.0, result[0], 0.001);
    ASSERT_NEAR(0.84147, result[1], 0.001);
    ASSERT_NEAR(0.90930, result[2], 0.001);
}

/**
 * @brief 测试 ops 字符串操作变换器。
 * @note 期望：正确操作字符串。
 */
TEST(OpsStringOperations) {
    std::vector<std::string> data = {"hello", "WORLD", "  test  "};
    auto                     r    = console::gen::list(data);
    auto transformed              = r | console::gen::map(console::ops::trim)
                       | console::gen::map(console::ops::to_upper)
                       | console::gen::map(console::ops::reverse_string);
    std::vector<std::string> result;
    for (std::string x : transformed) {
        result.push_back(x);
    }
    std::vector<std::string> expected = {"OLLEH", "DLROW", "TSET"};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 ops 字符串长度变换器。
 * @note 期望：正确计算字符串长度。
 */
TEST(OpsStrlen) {
    std::vector<std::string> data = {"a", "abc", "hello"};
    auto                     r    = console::gen::list(data);
    auto                lengths   = r | console::gen::map(console::ops::strlen);
    std::vector<size_t> result;
    for (size_t x : lengths) {
        result.push_back(x);
    }
    std::vector<size_t> expected = {1, 3, 5};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 ops 区间谓词 between。
 * @note 期望：正确判断元素是否在区间内。
 */
TEST(OpsBetween) {
    auto             r = console::gen::range(0, 10);
    auto             f = r | console::gen::filter(console::ops::between(3, 7));
    std::vector<int> result;
    for (int x : f) {
        result.push_back(x);
    }
    std::vector<int> expected = {3, 4, 5, 6, 7};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 ops 区间谓词 between_exclusive。
 * @note 期望：正确判断元素是否在开区间内。
 */
TEST(OpsBetweenExclusive) {
    auto r = console::gen::range(0, 10);
    auto f = r | console::gen::filter(console::ops::between_exclusive(3, 7));
    std::vector<int> result;
    for (int x : f) {
        result.push_back(x);
    }
    std::vector<int> expected = {4, 5, 6};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 ops 整除谓词 divisible_by。
 * @note 期望：正确判断元素是否可被整除。
 */
TEST(OpsDivisibleBy) {
    auto r = console::gen::range(0, 20);
    auto f = r | console::gen::filter(console::ops::divisible_by(5));
    std::vector<int> result;
    for (int x : f) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 5, 10, 15};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 ops power_of_two 谓词。
 * @note 期望：正确判断元素是否为2的幂次。
 */
TEST(OpsPowerOfTwo) {
    auto             r = console::gen::range(0, 20);
    auto             f = r | console::gen::filter(console::ops::power_of_two);
    std::vector<int> result;
    for (int x : f) {
        result.push_back(x);
    }
    std::vector<int> expected = {1, 2, 4, 8, 16};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 ops palindrome 谓词。
 * @note 期望：正确判断元素是否为回文数。
 */
TEST(OpsPalindrome) {
    auto             r = console::gen::range(0, 30);
    auto             f = r | console::gen::filter(console::ops::palindrome);
    std::vector<int> result;
    for (int x : f) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 22};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 ops clamp 限幅变换器。
 * @note 期望：将值限制在指定范围内。
 */
TEST(OpsClamp) {
    auto             r = console::gen::range(-5, 10);
    auto             c = r | console::gen::map(console::ops::clamp(0, 5));
    std::vector<int> result;
    for (int x : c) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 5, 5, 5, 5, 5};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 ops always_true/always_false 谓词。
 * @note 期望：始终返回 true/false。
 */
TEST(OpsAlwaysTrueFalse) {
    auto             r = console::gen::range(0, 5);
    auto             t = r | console::gen::filter(console::ops::always_true);
    std::vector<int> result_t;
    for (int x : t) {
        result_t.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, result_t);
    auto             f = r | console::gen::filter(console::ops::always_false);
    std::vector<int> result_f;
    for (int x : f) {
        result_f.push_back(x);
    }
    ASSERT_EMPTY(result_f);
}

/**
 * @brief 测试 ops nth 获取元组元素。
 * @note 期望：正确获取 pair/tuple 的第 N 个元素。
 */
TEST(OpsNth) {
    std::vector<std::pair<int, std::string>> data
        = {{1, "one"}, {2, "two"}, {3, "three"}};
    auto             r       = console::gen::list(data);
    auto             firsts  = r | console::gen::map(console::ops::nth<0>());
    auto             seconds = r | console::gen::map(console::ops::nth<1>());
    std::vector<int> f_result;
    for (int x : firsts) {
        f_result.push_back(x);
    }
    std::vector<int> f_expected = {1, 2, 3};
    ASSERT_EQ(f_expected, f_result);
    std::vector<std::string> s_result;
    for (std::string x : seconds) {
        s_result.push_back(x);
    }
    std::vector<std::string> s_expected = {"one", "two", "three"};
    ASSERT_EQ(s_expected, s_result);
}

/**
 * @brief 测试 ops is_null/not_null 谓词。
 * @note 期望：正确判断指针是否为空。
 */
TEST(OpsIsNullNotNull) {
    std::vector<int *> data
        = {nullptr, nullptr, (int *)0x1234, nullptr, (int *)0x5678};
    auto               r     = console::gen::list(data);
    auto               nulls = r | console::gen::filter(console::ops::is_null);
    std::vector<int *> null_result;
    for (int *p : nulls) {
        null_result.push_back(p);
    }
    ASSERT_EQ(3, null_result.size());
    auto not_nulls = r | console::gen::filter(console::ops::not_null);
    std::vector<int *> not_null_result;
    for (int *p : not_nulls) {
        not_null_result.push_back(p);
    }
    ASSERT_EQ(2, not_null_result.size());
}

/**
 * @brief 测试 Debug 生成器（非调试模式）。
 * @note 期望：Debug 生成器正常传递值。
 */
TEST(DebugGenerator) {
    auto               r = console::gen::range(0, 5);
    std::ostringstream oss;
    auto               d = console::gen::Debug<decltype(r)>(r, "DEBUG: ", oss);
    std::vector<int>   result;
    for (int x : d) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 Debug 管道操作。
 * @note 期望：使用管道操作符创建调试生成器。
 */
TEST(DebugPipeOperator) {
    auto               r = console::gen::range(0, 5);
    std::ostringstream oss;
    auto               d = r | console::gen::debug("TEST: ", oss);
    std::vector<int>   result;
    for (int x : d) {
        result.push_back(x);
    }
    std::vector<int> expected = {0, 1, 2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 ops max_element/min_element 变换器。
 * @note 期望：正确获取容器最大/最小值。
 */
TEST(OpsMaxMinElement) {
    std::vector<std::vector<int>> data = {{1, 5, 3}, {10, 20, 15}, {7, 2, 9}};
    auto                          r    = console::gen::list(data);
    auto             maxes = r | console::gen::map(console::ops::max_element);
    auto             mins  = r | console::gen::map(console::ops::min_element);
    std::vector<int> max_result;
    for (int x : maxes) {
        max_result.push_back(x);
    }
    std::vector<int> max_expected = {5, 20, 9};
    ASSERT_EQ(max_expected, max_result);
    std::vector<int> min_result;
    for (int x : mins) {
        min_result.push_back(x);
    }
    std::vector<int> min_expected = {1, 10, 2};
    ASSERT_EQ(min_expected, min_result);
}

/**
 * @brief 测试 ops sum/average 变换器。
 * @note 期望：正确计算范围和/平均值。
 */
TEST(OpsSumAverage) {
    auto r   = console::gen::range(1, 6);
    int  sum = r | console::gen::reduce(0, console::ops::plus);
    ASSERT_EQ(15, sum);
}

#ifndef NOMAIN
TEST_MAIN
#endif
