/**
 * @file rational.cpp
 * @brief 测试分数运算模块 (BasicRational)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/rational.h"

#include <sstream>
#include <string>

#include "../include/test.h"

/**
 * @brief 测试 Rational 默认构造。
 * @note 期望：默认构造为 0/1。
 */
TEST(RationalDefaultConstruction) {
    console::Rational r;
    ASSERT_EQ(0, r.numerator());
    ASSERT_EQ(1, r.denominator());
}

/**
 * @brief 测试 Rational 单参构造。
 * @note 期望：构造为 n/1。
 */
TEST(RationalSingleArgConstruction) {
    console::Rational r(42);
    ASSERT_EQ(42, r.numerator());
    ASSERT_EQ(1, r.denominator());
}

/**
 * @brief 测试 Rational 双参构造。
 * @note 期望：构造为 n/d 并约分。
 */
TEST(RationalTwoArgConstruction) {
    console::Rational r(2, 4);
    ASSERT_EQ(1, r.numerator());
    ASSERT_EQ(2, r.denominator());
}

/**
 * @brief 测试 Rational 双参构造负分母。
 * @note 期望：负分母转为负分子。
 */
TEST(RationalNegativeDenominator) {
    console::Rational r(1, -2);
    ASSERT_EQ(-1, r.numerator());
    ASSERT_EQ(2, r.denominator());
}

/**
 * @brief 测试 Rational 双参构造零分母。
 * @note 期望：抛出 DomainError。
 */
TEST(RationalZeroDenominator) {
    ASSERT_THROWS(console::Rational(1, 0), console::DomainError);
}

/**
 * @brief 测试 Rational 双参构造负数。
 * @note 期望：正确处理负数。
 */
TEST(RationalNegative) {
    console::Rational r(-3, 4);
    ASSERT_EQ(-3, r.numerator());
    ASSERT_EQ(4, r.denominator());
}

/**
 * @brief 测试 Rational 加法。
 * @note 期望：正确计算分数和。
 */
TEST(RationalAddition) {
    console::Rational a(1, 2);
    console::Rational b(1, 3);
    console::Rational c = a + b;
    ASSERT_EQ(5, c.numerator());
    ASSERT_EQ(6, c.denominator());
}

/**
 * @brief 测试 Rational 减法。
 * @note 期望：正确计算分数差。
 */
TEST(RationalSubtraction) {
    console::Rational a(1, 2);
    console::Rational b(1, 3);
    console::Rational c = a - b;
    ASSERT_EQ(1, c.numerator());
    ASSERT_EQ(6, c.denominator());
}

/**
 * @brief 测试 Rational 乘法。
 * @note 期望：正确计算分数积。
 */
TEST(RationalMultiplication) {
    console::Rational a(2, 3);
    console::Rational b(3, 4);
    console::Rational c = a * b;
    ASSERT_EQ(1, c.numerator());
    ASSERT_EQ(2, c.denominator());
}

/**
 * @brief 测试 Rational 除法。
 * @note 期望：正确计算分数商。
 */
TEST(RationalDivision) {
    console::Rational a(2, 3);
    console::Rational b(3, 4);
    console::Rational c = a / b;
    ASSERT_EQ(8, c.numerator());
    ASSERT_EQ(9, c.denominator());
}

/**
 * @brief 测试 Rational 除零。
 * @note 期望：除以零抛出 DomainError。
 */
TEST(RationalDivisionByZero) {
    console::Rational a(1, 2);
    console::Rational b(0, 1);
    ASSERT_THROWS(a / b, console::DomainError);
}

/**
 * @brief 测试 Rational 复合加法。
 * @note 期望：正确计算并赋值。
 */
TEST(RationalAddAssign) {
    console::Rational a(1, 2);
    console::Rational b(1, 3);
    a += b;
    ASSERT_EQ(5, a.numerator());
    ASSERT_EQ(6, a.denominator());
}

/**
 * @brief 测试 Rational 复合减法。
 * @note 期望：正确计算并赋值。
 */
TEST(RationalSubAssign) {
    console::Rational a(1, 2);
    console::Rational b(1, 3);
    a -= b;
    ASSERT_EQ(1, a.numerator());
    ASSERT_EQ(6, a.denominator());
}

/**
 * @brief 测试 Rational 复合乘法。
 * @note 期望：正确计算并赋值。
 */
TEST(RationalMulAssign) {
    console::Rational a(2, 3);
    console::Rational b(3, 4);
    a *= b;
    ASSERT_EQ(1, a.numerator());
    ASSERT_EQ(2, a.denominator());
}

/**
 * @brief 测试 Rational 复合除法。
 * @note 期望：正确计算并赋值。
 */
TEST(RationalDivAssign) {
    console::Rational a(2, 3);
    console::Rational b(3, 4);
    a /= b;
    ASSERT_EQ(8, a.numerator());
    ASSERT_EQ(9, a.denominator());
}

/**
 * @brief 测试 Rational 一元负。
 * @note 期望：返回相反数。
 */
TEST(RationalUnaryMinus) {
    console::Rational r(1, 2);
    console::Rational neg = -r;
    ASSERT_EQ(-1, neg.numerator());
    ASSERT_EQ(2, neg.denominator());
}

/**
 * @brief 测试 Rational 一元正。
 * @note 期望：返回自身。
 */
TEST(RationalUnaryPlus) {
    console::Rational r(1, 2);
    console::Rational pos = +r;
    ASSERT_EQ(1, pos.numerator());
    ASSERT_EQ(2, pos.denominator());
}

/**
 * @brief 测试 Rational 相等比较。
 * @note 期望：正确判断相等。
 */
TEST(RationalEqual) {
    console::Rational a(1, 2);
    console::Rational b(2, 4);
    console::Rational c(1, 3);
    ASSERT_TRUE(a == b);
    ASSERT_FALSE(a == c);
}

/**
 * @brief 测试 Rational 不等比较。
 * @note 期望：正确判断不等。
 */
TEST(RationalNotEqual) {
    console::Rational a(1, 2);
    console::Rational b(2, 4);
    console::Rational c(1, 3);
    ASSERT_FALSE(a != b);
    ASSERT_TRUE(a != c);
}

/**
 * @brief 测试 Rational 小于比较。
 * @note 期望：正确判断小于。
 */
TEST(RationalLessThan) {
    console::Rational a(1, 2);
    console::Rational b(1, 3);
    console::Rational c(2, 3);
    ASSERT_TRUE(b < a);
    ASSERT_TRUE(a < c);
    ASSERT_FALSE(a < b);
}

/**
 * @brief 测试 Rational 大于比较。
 * @note 期望：正确判断大于。
 */
TEST(RationalGreaterThan) {
    console::Rational a(1, 2);
    console::Rational b(1, 3);
    console::Rational c(2, 3);
    ASSERT_TRUE(a > b);
    ASSERT_TRUE(c > a);
    ASSERT_FALSE(b > a);
}

/**
 * @brief 测试 Rational 小于等于比较。
 * @note 期望：正确判断小于等于。
 */
TEST(RationalLessEqual) {
    console::Rational a(1, 2);
    console::Rational b(2, 4);
    console::Rational c(1, 3);
    ASSERT_TRUE(a <= b);
    ASSERT_TRUE(c <= a);
    ASSERT_FALSE(a <= c);
}

/**
 * @brief 测试 Rational 大于等于比较。
 * @note 期望：正确判断大于等于。
 */
TEST(RationalGreaterEqual) {
    console::Rational a(1, 2);
    console::Rational b(2, 4);
    console::Rational c(1, 3);
    ASSERT_TRUE(a >= b);
    ASSERT_TRUE(a >= c);
    ASSERT_FALSE(c >= a);
}

/**
 * @brief 测试 Rational 显式转换。
 * @note 期望：正确转换为目标类型。
 */
TEST(RationalExplicitCast) {
    console::Rational r(1, 2);
    double            d = static_cast<double>(r);
    ASSERT_NEAR(0.5, d, 0.001);
    int i = static_cast<int>(r);
    ASSERT_EQ(0, i);
}

/**
 * @brief 测试 Rational 流输出。
 * @note 期望：输出格式为 "n/d" 或 "n"。
 */
TEST(RationalOutputStream) {
    console::Rational  r1(1, 2);
    console::Rational  r2(3, 1);
    std::ostringstream oss1, oss2;
    oss1 << r1;
    oss2 << r2;
    ASSERT_EQ(std::string("1/2"), oss1.str());
    ASSERT_EQ(std::string("3"), oss2.str());
}

/**
 * @brief 测试 Rational 流输入。
 * @note 期望：正确读取分数格式。
 */
TEST(RationalInputStream) {
    console::Rational  r;
    std::istringstream iss("3/4");
    iss >> r;
    ASSERT_EQ(3, r.numerator());
    ASSERT_EQ(4, r.denominator());
}

/**
 * @brief 测试 Rational 流输入整数格式。
 * @note 期望：正确读取整数格式。
 */
TEST(RationalInputStreamInteger) {
    console::Rational  r;
    std::istringstream iss("5");
    iss >> r;
    ASSERT_EQ(5, r.numerator());
    ASSERT_EQ(1, r.denominator());
}

/**
 * @brief 测试 Rational 流输入负数。
 * @note 期望：正确读取负数。
 */
TEST(RationalInputStreamNegative) {
    console::Rational  r;
    std::istringstream iss("-2/3");
    iss >> r;
    ASSERT_EQ(-2, r.numerator());
    ASSERT_EQ(3, r.denominator());
}

/**
 * @brief 测试 Rational_8 类型。
 * @note 期望：使用 int8_t 存储。
 */
TEST(Rational8) {
    console::Rational_8 r(1, 2);
    ASSERT_EQ(1, r.numerator());
    ASSERT_EQ(2, r.denominator());
}

/**
 * @brief 测试 Rational_16 类型。
 * @note 期望：使用 int16_t 存储。
 */
TEST(Rational16) {
    console::Rational_16 r(3, 4);
    ASSERT_EQ(3, r.numerator());
    ASSERT_EQ(4, r.denominator());
}

/**
 * @brief 测试 Rational_32 类型。
 * @note 期望：使用 int32_t 存储。
 */
TEST(Rational32) {
    console::Rational_32 r(5, 6);
    ASSERT_EQ(5, r.numerator());
    ASSERT_EQ(6, r.denominator());
}

/**
 * @brief 测试 Rational_64 类型。
 * @note 期望：使用 int64_t 存储。
 */
TEST(Rational64) {
    console::Rational_64 r(7, 8);
    ASSERT_EQ(7, r.numerator());
    ASSERT_EQ(8, r.denominator());
}

/**
 * @brief 测试 Rational 大数运算。
 * @note 期望：大数正确运算（注意溢出风险）。
 */
TEST(RationalLargeNumbers) {
    console::Rational a(1000000, 1);
    console::Rational b(1, 1000000);
    console::Rational c = a * b;
    ASSERT_EQ(1, c.numerator());
    ASSERT_EQ(1, c.denominator());
}

/**
 * @brief 测试 Rational 与整数混合运算。
 * @note 期望：通过隐式转换与整数运算。
 */
TEST(RationalMixedOperations) {
    console::Rational a(1, 2);
    console::Rational b = a + 1;
    ASSERT_EQ(3, b.numerator());
    ASSERT_EQ(2, b.denominator());
    console::Rational c = 1 - a;
    ASSERT_EQ(1, c.numerator());
    ASSERT_EQ(2, c.denominator());
}

/**
 * @brief 测试 Rational 约分。
 * @note 期望：构造时自动约分。
 */
TEST(RationalReduction) {
    console::Rational r(4, 8);
    ASSERT_EQ(1, r.numerator());
    ASSERT_EQ(2, r.denominator());
}

/**
 * @brief 测试 Rational 零。
 * @note 期望：零分数正确表示。
 */
TEST(RationalZero) {
    console::Rational r(0, 5);
    ASSERT_EQ(0, r.numerator());
    ASSERT_EQ(1, r.denominator());
}

#ifndef NOMAIN
TEST_MAIN
#endif
