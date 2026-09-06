/**
 * @file union.cpp
 * @brief 测试类型安全可判别联合体模块 (Union)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../../include/container/union.h"

#include <string>
#include <vector>

#include "../../include/util/test.h"

std::string to_string(const std::string &str) {
    return str;
}
std::string to_string(int i) {
    return std::to_string(i);
}
std::string to_string(double d) {
    return std::to_string(d);
}

/**
 * @brief 测试 Union 默认构造为空。
 * @note 期望：默认构造的 Union 为空。
 */
TEST(UnionDefaultEmpty) {
    console::Union<int, double, std::string> u;
    ASSERT_TRUE(u.empty());
    ASSERT_EQ(static_cast<size_t>(-1), u.index());
}

/**
 * @brief 测试 Union 从 int 构造。
 * @note 期望：正确存储 int，index 匹配。
 */
TEST(UnionConstructInt) {
    console::Union<int, double, std::string> u(42);
    ASSERT_FALSE(u.empty());
    ASSERT_EQ(0, u.index());
    auto *ptr = u.get<int>();
    ASSERT_NE(nullptr, ptr);
    ASSERT_EQ(42, *ptr);
}

/**
 * @brief 测试 Union 从 double 构造。
 * @note 期望：正确存储 double，index 匹配。
 */
TEST(UnionConstructDouble) {
    console::Union<int, double, std::string> u(3.14);
    ASSERT_FALSE(u.empty());
    ASSERT_EQ(1, u.index());
    auto *ptr = u.get<double>();
    ASSERT_NE(nullptr, ptr);
    ASSERT_NEAR(3.14, *ptr, 0.001);
}

/**
 * @brief 测试 Union 从 string 构造。
 * @note 期望：正确存储 string，index 匹配。
 */
TEST(UnionConstructString) {
    console::Union<int, double, std::string> u(std::string("hello"));
    ASSERT_FALSE(u.empty());
    ASSERT_EQ(2, u.index());
    auto *ptr = u.get<std::string>();
    ASSERT_NE(nullptr, ptr);
    ASSERT_EQ(std::string("hello"), *ptr);
}

/**
 * @brief 测试 Union 拷贝构造。
 * @note 期望：拷贝构造独立副本。
 */
TEST(UnionCopyConstruction) {
    console::Union<int, double, std::string> u1(42);
    console::Union<int, double, std::string> u2(u1);
    ASSERT_FALSE(u2.empty());
    ASSERT_EQ(0, u2.index());
    auto *ptr = u2.get<int>();
    ASSERT_EQ(42, *ptr);
    u1 = 100;
    ASSERT_EQ(42, *u2.get<int>());
}

/**
 * @brief 测试 Union 移动构造。
 * @note 期望：移动后源变为空。
 */
TEST(UnionMoveConstruction) {
    console::Union<int, double, std::string> u1(42);
    console::Union<int, double, std::string> u2(std::move(u1));
    ASSERT_FALSE(u2.empty());
    ASSERT_EQ(0, u2.index());
    ASSERT_EQ(42, *u2.get<int>());
    ASSERT_TRUE(u1.empty());
}

/**
 * @brief 测试 Union 拷贝赋值。
 * @note 期望：拷贝赋值正确。
 */
TEST(UnionCopyAssignment) {
    console::Union<int, double, std::string> u1(42);
    console::Union<int, double, std::string> u2;
    u2 = u1;
    ASSERT_FALSE(u2.empty());
    ASSERT_EQ(0, u2.index());
    ASSERT_EQ(42, *u2.get<int>());
}

/**
 * @brief 测试 Union 移动赋值。
 * @note 期望：移动后源变为空。
 */
TEST(UnionMoveAssignment) {
    console::Union<int, double, std::string> u1(42);
    console::Union<int, double, std::string> u2;
    u2 = std::move(u1);
    ASSERT_FALSE(u2.empty());
    ASSERT_EQ(0, u2.index());
    ASSERT_EQ(42, *u2.get<int>());
    ASSERT_TRUE(u1.empty());
}

/**
 * @brief 测试 Union 赋值变更类型。
 * @note 期望：赋值不同类型后正确切换。
 */
TEST(UnionAssignDifferentType) {
    console::Union<int, double, std::string> u;
    u = 42;
    ASSERT_EQ(0, u.index());
    ASSERT_EQ(42, *u.get<int>());
    u = 3.14;
    ASSERT_EQ(1, u.index());
    ASSERT_NEAR(3.14, *u.get<double>(), 0.001);
    u = std::string("hello");
    ASSERT_EQ(2, u.index());
    ASSERT_EQ(std::string("hello"), *u.get<std::string>());
}

/**
 * @brief 测试 Union get 类型不匹配。
 * @note 期望：类型不匹配返回 nullptr。
 */
TEST(UnionGetTypeMismatch) {
    console::Union<int, double, std::string> u(42);
    ASSERT_EQ(nullptr, u.get<double>());
    ASSERT_EQ(nullptr, u.get<std::string>());
    const auto &cu = u;
    ASSERT_EQ(nullptr, cu.get<double>());
    ASSERT_EQ(nullptr, cu.get<std::string>());
}

/**
 * @brief 测试 Union get 空对象。
 * @note 期望：空对象 get 返回 nullptr。
 */
TEST(UnionGetEmpty) {
    console::Union<int, double, std::string> u;
    ASSERT_EQ(nullptr, u.get<int>());
    const auto &cu = u;
    ASSERT_EQ(nullptr, cu.get<int>());
}

/**
 * @brief 测试 Union visit 单函数有返回值。
 * @note 期望：正确访问存储的值并返回。
 */
TEST(UnionVisitSingleFunction) {
    console::Union<int, double, std::string> u(42);
    auto result = u.visit([](auto &&v) -> std::string { return to_string(v); });
    ASSERT_EQ(std::string("42"), result);
    u      = 3.14;
    result = u.visit([](auto &&v) -> std::string { return to_string(v); });
    ASSERT_STRCONTAINS(result.c_str(), "3.140000");
    u      = std::string("hello");
    result = u.visit([](auto &&v) -> std::string { return to_string(v); });
    ASSERT_EQ(std::string("hello"), result);
}

/**
 * @brief 测试 Union visit 单函数无返回值。
 * @note 期望：正确访问存储的值并执行操作。
 */
TEST(UnionVisitSingleFunctionVoid) {
    console::Union<int, double, long long> u(42);
    int                                    result = 0;
    u.visit([&](auto &&v) { result = static_cast<int>(v); });
    ASSERT_EQ(42, result);
    u = 3.14;
    u.visit([&](auto &&v) { result = static_cast<int>(v); });
    ASSERT_EQ(3, result);
}

/**
 * @brief 测试 Union visit 多函数有返回值。
 * @note 期望：按类型匹配正确的函数。
 */
TEST(UnionVisitMultipleFunctions) {
    console::Union<int, double, std::string> u(42);
    auto                                     result = u.visit(
        [](int x) -> std::string { return "int: " + std::to_string(x); },
        [](double x) -> std::string { return "double: " + std::to_string(x); },
        [](const std::string &x) -> std::string { return "string: " + x; });
    ASSERT_EQ(std::string("int: 42"), result);
    u      = 3.14;
    result = u.visit(
        [](int x) -> std::string { return "int: " + std::to_string(x); },
        [](double x) -> std::string { return "double: " + std::to_string(x); },
        [](const std::string &x) -> std::string { return "string: " + x; });
    ASSERT_STRCONTAINS(result.c_str(), "double: 3.140000");
    u      = std::string("hello");
    result = u.visit(
        [](int x) -> std::string { return "int: " + std::to_string(x); },
        [](double x) -> std::string { return "double: " + std::to_string(x); },
        [](const std::string &x) -> std::string { return "string: " + x; });
    ASSERT_EQ(std::string("string: hello"), result);
}

/**
 * @brief 测试 Union visit 多函数无返回值。
 * @note 期望：按类型匹配执行正确的函数。
 */
TEST(UnionVisitMultipleFunctionsVoid) {
    console::Union<int, double, std::string> u(42);
    std::string                              result;
    u.visit([&](int x) { result = "int: " + std::to_string(x); },
        [&](double x) { result = "double: " + std::to_string(x); },
        [&](const std::string &x) { result = "string: " + x; });
    ASSERT_EQ(std::string("int: 42"), result);
    u = 3.14;
    u.visit([&](int x) { result = "int: " + std::to_string(x); },
        [&](double x) { result = "double: " + std::to_string(x); },
        [&](const std::string &x) { result = "string: " + x; });
    ASSERT_STRCONTAINS(result.c_str(), "double: 3.140000");
}

/**
 * @brief 测试 Union visit const 版本。
 * @note 期望：const Union 正确访问。
 */
TEST(UnionVisitConst) {
    const console::Union<int, double, std::string> u(42);
    auto result = u.visit([](auto &&v) -> std::string { return to_string(v); });
    ASSERT_EQ(std::string("42"), result);
}

/**
 * @brief 测试 Union 多个类型。
 * @note 期望：支持多个不同类型。
 */
TEST(UnionMultipleTypes) {
    console::Union<int, double, std::string, char, bool> u;
    u = 'x';
    ASSERT_EQ(3, u.index());
    ASSERT_EQ('x', *u.get<char>());
    u = true;
    ASSERT_EQ(4, u.index());
    ASSERT_TRUE(*u.get<bool>());
}

/**
 * @brief 测试 Union 自定义类型。
 * @note 期望：支持自定义类型。
 */
TEST(UnionCustomType) {
    struct Point {
        int x, y;
        Point(int a = 0, int b = 0) : x(a), y(b) {}
        bool operator==(const Point &other) const {
            return x == other.x && y == other.y;
        }
    };
    console::Union<int, Point, std::string> u;
    u = Point(10, 20);
    ASSERT_EQ(1, u.index());
    auto *ptr = u.get<Point>();
    ASSERT_NE(nullptr, ptr);
    ASSERT_EQ(10, ptr->x);
    ASSERT_EQ(20, ptr->y);
    console::Union<int, Point, std::string> u2(u);
    auto                                   *ptr2 = u2.get<Point>();
    ASSERT_NE(nullptr, ptr2);
    ASSERT_EQ(10, ptr2->x);
    ASSERT_EQ(20, ptr2->y);
}

/**
 * @brief 测试 Union 移动大对象。
 * @note 期望：大对象移动正确。
 */
TEST(UnionMoveLargeObject) {
    console::Union<std::vector<int>, std::string> u;
    std::vector<int>                              vec(1000, 42);
    u = std::move(vec);
    ASSERT_EQ(0, u.index());
    auto *ptr = u.get<std::vector<int>>();
    ASSERT_NE(nullptr, ptr);
    ASSERT_EQ(1000, ptr->size());
    ASSERT_EQ(42, (*ptr)[0]);
}

/**
 * @brief 测试 Union 返回类型不一致。
 * @note 期望：不同返回类型使用 common_type 统一。
 */
TEST(UnionVisitDifferentReturnTypes) {
    console::Union<int, double, std::string> u(42);
    auto result = u.visit([](int) -> double { return 1.5; },
        [](double) -> double { return 2.5; },
        [](const std::string &) -> double { return 3.5; });
    ASSERT_NEAR(1.5, result, 0.001);
    u      = 3.14;
    result = u.visit([](int) -> double { return 1.5; },
        [](double) -> double { return 2.5; },
        [](const std::string &) -> double { return 3.5; });
    ASSERT_NEAR(2.5, result, 0.001);
}

#ifndef NOMAIN
TEST_MAIN
#endif
