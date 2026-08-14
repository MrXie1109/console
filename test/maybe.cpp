/**
 * @file maybe.cpp
 * @brief 测试可选值容器模块 (Maybe)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/maybe.h"

#include <sstream>
#include <string>

#include "../include/test.h"

/**
 * @brief 测试 Maybe 默认构造为空。
 * @note 期望：默认构造的 Maybe 为空，has_value 返回 false。
 */
TEST(MaybeDefaultConstruction) {
    console::Maybe<int> m;
    ASSERT_FALSE(m.has_value());
    ASSERT_FALSE(static_cast<bool>(m));
}

/**
 * @brief 测试 Maybe 从值构造。
 * @note 期望：包含值，has_value 返回 true，value 返回正确值。
 */
TEST(MaybeValueConstruction) {
    console::Maybe<int> m(42);
    ASSERT_TRUE(m.has_value());
    ASSERT_EQ(42, m.value());
    ASSERT_EQ(42, *m);
}

/**
 * @brief 测试 Maybe 从 nothing 构造。
 * @note 期望：构造为空，has_value 返回 false。
 */
TEST(MaybeNothingConstruction) {
    console::Maybe<int> m(console::nothing);
    ASSERT_FALSE(m.has_value());
}

/**
 * @brief 测试 Maybe 拷贝构造（有值）。
 * @note 期望：拷贝后两个 Maybe 独立，值相同。
 */
TEST(MaybeCopyConstructionWithValue) {
    console::Maybe<int> m1(42);
    console::Maybe<int> m2(m1);
    ASSERT_TRUE(m1.has_value());
    ASSERT_TRUE(m2.has_value());
    ASSERT_EQ(42, m1.value());
    ASSERT_EQ(42, m2.value());
    m1.value() = 100;
    ASSERT_EQ(100, m1.value());
    ASSERT_EQ(42, m2.value());
}

/**
 * @brief 测试 Maybe 拷贝构造（空）。
 * @note 期望：拷贝后两个 Maybe 均为空。
 */
TEST(MaybeCopyConstructionEmpty) {
    console::Maybe<int> m1;
    console::Maybe<int> m2(m1);
    ASSERT_FALSE(m1.has_value());
    ASSERT_FALSE(m2.has_value());
}

/**
 * @brief 测试 Maybe 移动构造。
 * @note 期望：移动后目标包含值，源为空。
 */
TEST(MaybeMoveConstruction) {
    console::Maybe<int> m1(42);
    console::Maybe<int> m2(std::move(m1));
    ASSERT_TRUE(m2.has_value());
    ASSERT_EQ(42, m2.value());
    ASSERT_FALSE(m1.has_value());
}

/**
 * @brief 测试 Maybe 从值拷贝赋值。
 * @note 期望：赋值后包含值。
 */
TEST(MaybeCopyAssignmentFromValue) {
    console::Maybe<int> m;
    m = 42;
    ASSERT_TRUE(m.has_value());
    ASSERT_EQ(42, m.value());
}

/**
 * @brief 测试 Maybe 从值移动赋值。
 * @note 期望：赋值后包含值。
 */
TEST(MaybeMoveAssignmentFromValue) {
    console::Maybe<int> m;
    int                 v = 42;
    m                     = std::move(v);
    ASSERT_TRUE(m.has_value());
    ASSERT_EQ(42, m.value());
}

/**
 * @brief 测试 Maybe 拷贝赋值（有值）。
 * @note 期望：拷贝后两个 Maybe 独立，值相同。
 */
TEST(MaybeCopyAssignmentWithValue) {
    console::Maybe<int> m1(42);
    console::Maybe<int> m2(100);
    m2 = m1;
    ASSERT_EQ(42, m1.value());
    ASSERT_EQ(42, m2.value());
    m1.value() = 200;
    ASSERT_EQ(200, m1.value());
    ASSERT_EQ(42, m2.value());
}

/**
 * @brief 测试 Maybe 拷贝赋值（空到有值）。
 * @note 期望：拷贝后目标为空。
 */
TEST(MaybeCopyAssignmentEmptyToValue) {
    console::Maybe<int> m1;
    console::Maybe<int> m2(42);
    m2 = m1;
    ASSERT_FALSE(m1.has_value());
    ASSERT_FALSE(m2.has_value());
}

/**
 * @brief 测试 Maybe 移动赋值。
 * @note 期望：移动后目标包含值，源为空。
 */
TEST(MaybeMoveAssignment) {
    console::Maybe<int> m1(42);
    console::Maybe<int> m2(100);
    m2 = std::move(m1);
    ASSERT_TRUE(m2.has_value());
    ASSERT_EQ(42, m2.value());
    ASSERT_FALSE(m1.has_value());
}

/**
 * @brief 测试 Maybe 从 nothing 赋值。
 * @note 期望：赋值为空。
 */
TEST(MaybeNothingAssignment) {
    console::Maybe<int> m(42);
    ASSERT_TRUE(m.has_value());
    m = console::nothing;
    ASSERT_FALSE(m.has_value());
}

/**
 * @brief 测试 Maybe value 方法有值。
 * @note 期望：value 返回正确值。
 */
TEST(MaybeValueWithValue) {
    console::Maybe<int> m(42);
    ASSERT_EQ(42, m.value());
    const auto &cm = m;
    ASSERT_EQ(42, cm.value());
}

/**
 * @brief 测试 Maybe value 方法空值。
 * @note 期望：空 Maybe 调用 value 抛出 TypeError。
 */
TEST(MaybeValueEmpty) {
    console::Maybe<int> m;
    ASSERT_THROWS(m.value(), console::TypeError);
    const auto &cm = m;
    ASSERT_THROWS(cm.value(), console::TypeError);
}

/**
 * @brief 测试 Maybe operator* 解引用。
 * @note 期望：operator* 返回值的引用。
 */
TEST(MaybeDereference) {
    console::Maybe<int> m(42);
    ASSERT_EQ(42, *m);
    *m = 100;
    ASSERT_EQ(100, *m);
    const auto &cm = m;
    ASSERT_EQ(100, *cm);
}

/**
 * @brief 测试 Maybe operator-> 成员访问。
 * @note 期望：operator-> 返回指向值的指针。
 */
TEST(MaybeArrowOperator) {
    struct Data {
        int x;
        int y;
    };
    console::Maybe<Data> m(Data{10, 20});
    ASSERT_EQ(10, m->x);
    ASSERT_EQ(20, m->y);
    m->x = 30;
    ASSERT_EQ(30, m->x);
    const auto &cm = m;
    ASSERT_EQ(30, cm->x);
    ASSERT_EQ(20, cm->y);
}

/**
 * @brief 测试 Maybe has_value。
 * @note 期望：有值时返回 true，空时返回 false。
 */
TEST(MaybeHasValue) {
    console::Maybe<int> m1(42);
    ASSERT_TRUE(m1.has_value());
    console::Maybe<int> m2;
    ASSERT_FALSE(m2.has_value());
}

/**
 * @brief 测试 Maybe operator bool。
 * @note 期望：有值时返回 true，空时返回 false。
 */
TEST(MaybeBoolOperator) {
    console::Maybe<int> m1(42);
    ASSERT_TRUE(static_cast<bool>(m1));
    console::Maybe<int> m2;
    ASSERT_FALSE(static_cast<bool>(m2));
}

/**
 * @brief 测试 Maybe reset 清空。
 * @note 期望：reset 后 Maybe 为空。
 */
TEST(MaybeReset) {
    console::Maybe<int> m(42);
    ASSERT_TRUE(m.has_value());
    m.reset();
    ASSERT_FALSE(m.has_value());
}

/**
 * @brief 测试 Maybe reset 从参数构造新值。
 * @note 期望：reset 后包含新构造的值。
 */
TEST(MaybeResetWithArgs) {
    struct Point {
        int x, y;
        Point(int a, int b) : x(a), y(b) {}
    };
    console::Maybe<Point> m;
    ASSERT_FALSE(m.has_value());
    m.reset(10, 20);
    ASSERT_TRUE(m.has_value());
    ASSERT_EQ(10, m->x);
    ASSERT_EQ(20, m->y);
}

/**
 * @brief 测试 Maybe value_or 有值。
 * @note 期望：返回当前值。
 */
TEST(MaybeValueOrWithValue) {
    console::Maybe<int> m(42);
    ASSERT_EQ(42, m.value_or(100));
    ASSERT_EQ(42, m.value_or(0));
}

/**
 * @brief 测试 Maybe value_or 空值。
 * @note 期望：返回默认值。
 */
TEST(MaybeValueOrEmpty) {
    console::Maybe<int> m;
    ASSERT_EQ(100, m.value_or(100));
    ASSERT_EQ(0, m.value_or(0));
    console::Maybe<std::string> ms;
    ASSERT_EQ(std::string("default"), ms.value_or("default"));
}

/**
 * @brief 测试 Maybe swap。
 * @note 期望：两个 Maybe 交换内容。
 */
TEST(MaybeSwap) {
    console::Maybe<int> m1(42);
    console::Maybe<int> m2(100);
    m1.swap(m2);
    ASSERT_EQ(100, m1.value());
    ASSERT_EQ(42, m2.value());
    console::Maybe<int> m3;
    m1.swap(m3);
    ASSERT_FALSE(m1.has_value());
    ASSERT_EQ(100, m3.value());
}

/**
 * @brief 测试 Maybe 流输出运算符（有值）。
 * @note 期望：输出值为字符串。
 */
TEST(MaybeOutputStreamWithValue) {
    console::Maybe<int> m(42);
    std::ostringstream  oss;
    oss << m;
    ASSERT_EQ(std::string("42"), oss.str());
}

/**
 * @brief 测试 Maybe 流输出运算符（空）。
 * @note 期望：输出 "(nothing)"。
 */
TEST(MaybeOutputStreamEmpty) {
    console::Maybe<int> m;
    std::ostringstream  oss;
    oss << m;
    ASSERT_EQ(std::string("(nothing)"), oss.str());
}

/**
 * @brief 测试 Maybe 流输入运算符（成功）。
 * @note 期望：成功读取值到 Maybe。
 */
TEST(MaybeInputStreamSuccess) {
    console::Maybe<int> m;
    std::istringstream  iss("42");
    iss >> m;
    ASSERT_TRUE(m.has_value());
    ASSERT_EQ(42, m.value());
}

/**
 * @brief 测试 Maybe 流输入运算符（失败）。
 * @note 期望：读取失败时 Maybe 为空，流错误被清除。
 */
TEST(MaybeInputStreamFailure) {
    console::Maybe<int> m(100);
    std::istringstream  iss("abc");
    iss >> m;
    ASSERT_FALSE(m.has_value());
    ASSERT_TRUE(iss.good());
}

/**
 * @brief 测试 Maybe 存储自定义类型。
 * @note 期望：自定义类型正确存储和访问。
 */
TEST(MaybeCustomType) {
    struct Person {
        std::string name;
        int         age;
        bool        operator==(const Person &other) const {
            return name == other.name && age == other.age;
        }
    };
    console::Maybe<Person> m(Person{"Alice", 25});
    ASSERT_TRUE(m.has_value());
    ASSERT_EQ(std::string("Alice"), m->name);
    ASSERT_EQ(25, m->age);
    m->name = "Bob";
    ASSERT_EQ(std::string("Bob"), m->name);
}

/**
 * @brief 测试 Maybe 存储字符串类型。
 * @note 期望：字符串正确存储和访问。
 */
TEST(MaybeString) {
    console::Maybe<std::string> m("hello");
    ASSERT_TRUE(m.has_value());
    ASSERT_EQ(std::string("hello"), m.value());
    m.value() = "world";
    ASSERT_EQ(std::string("world"), m.value());
}

/**
 * @brief 测试 Maybe 从字符串字面量构造。
 * @note 期望：正确构造 Maybe<std::string>。
 */
TEST(MaybeStringLiteral) {
    console::Maybe<std::string> m("hello");
    ASSERT_TRUE(m.has_value());
    ASSERT_EQ(std::string("hello"), m.value());
}

/**
 * @brief 测试 Maybe value_or 返回引用。
 * @note 期望：value_or 返回值类型正确。
 */
TEST(MaybeValueOrReference) {
    console::Maybe<std::string> m("hello");
    std::string                 result = m.value_or("default");
    ASSERT_EQ(std::string("hello"), result);
    console::Maybe<std::string> empty;
    std::string                 default_result = empty.value_or("default");
    ASSERT_EQ(std::string("default"), default_result);
}

/**
 * @brief 测试 Maybe 移动语义。
 * @note 期望：移动操作不抛出异常。
 */
TEST(MaybeMoveNoExcept) {
    console::Maybe<int> m1(42);
    console::Maybe<int> m2(std::move(m1));
    ASSERT_TRUE(m2.has_value());
    ASSERT_EQ(42, m2.value());
    ASSERT_FALSE(m1.has_value());
}

#ifndef NOMAIN
TEST_MAIN
#endif
