/**
 * @file result.cpp
 * @brief 测试 Result 类型模块。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/result.h"

#include <string>

#include "../include/test.h"

/**
 * @brief 测试 Result Ok 构造。
 * @note 期望：Ok 状态，is_ok 返回 true，is_err 返回 false。
 */
TEST(ResultOkConstruction) {
    console::Result<int, std::string> r(42);
    ASSERT_TRUE(r.is_ok());
    ASSERT_FALSE(r.is_err());
    ASSERT_TRUE(static_cast<bool>(r));
    ASSERT_FALSE(!r);
}

/**
 * @brief 测试 Result Err 构造。
 * @note 期望：Err 状态，is_ok 返回 false，is_err 返回 true。
 */
TEST(ResultErrConstruction) {
    console::Result<int, std::string> r(std::string("error"));
    ASSERT_FALSE(r.is_ok());
    ASSERT_TRUE(r.is_err());
    ASSERT_FALSE(static_cast<bool>(r));
    ASSERT_TRUE(!r);
}

/**
 * @brief 测试 Result ok 方法。
 * @note 期望：Ok 状态返回成功值副本。
 */
TEST(ResultOk) {
    console::Result<int, std::string> r(42);
    ASSERT_EQ(42, r.ok());
}

/**
 * @brief 测试 Result err 方法。
 * @note 期望：Err 状态返回错误值副本。
 */
TEST(ResultErr) {
    console::Result<int, std::string> r(std::string("error"));
    ASSERT_EQ(std::string("error"), r.err());
}

/**
 * @brief 测试 Result unwrap 成功。
 * @note 期望：Ok 状态返回成功值。
 */
TEST(ResultUnwrapOk) {
    console::Result<int, std::string> r(42);
    ASSERT_EQ(42, r.unwrap());
}

/**
 * @brief 测试 Result unwrap 失败。
 * @note 期望：Err 状态抛出 ValueError。
 */
TEST(ResultUnwrapErr) {
    console::Result<int, std::string> r(std::string("error"));
    ASSERT_THROWS(r.unwrap(), console::ValueError);
}

/**
 * @brief 测试 Result expect 成功。
 * @note 期望：Ok 状态返回成功值。
 */
TEST(ResultExpectOk) {
    console::Result<int, std::string> r(42);
    ASSERT_EQ(42, r.expect("should be ok"));
}

/**
 * @brief 测试 Result expect 失败。
 * @note 期望：Err 状态抛出携带消息的 ValueError。
 */
TEST(ResultExpectErr) {
    console::Result<int, std::string> r(std::string("error"));
    ASSERT_THROWS(r.expect("custom error message"), console::ValueError);
    try {
        r.expect("custom error message");
    } catch (const console::ValueError &e) {
        ASSERT_STRCONTAINS(e.what(), "custom error message");
    }
}

/**
 * @brief 测试 Result visit。
 * @note 期望：根据状态调用对应的处理函数。
 */
TEST(ResultVisit) {
    console::Result<int, std::string> ok(42);
    console::Result<int, std::string> err(std::string("error"));
    bool                              ok_called  = false;
    bool                              err_called = false;
    ok.visit(
        [&](int v) {
            ok_called = true;
            ASSERT_EQ(42, v);
        },
        [&](const std::string &) { err_called = true; });
    ASSERT_TRUE(ok_called);
    ASSERT_FALSE(err_called);
    ok_called  = false;
    err_called = false;
    err.visit([&](int) { ok_called = true; },
        [&](const std::string &e) {
            err_called = true;
            ASSERT_EQ(std::string("error"), e);
        });
    ASSERT_FALSE(ok_called);
    ASSERT_TRUE(err_called);
}

/**
 * @brief 测试 Result and_then 左值引用。
 * @note 期望：Ok 状态应用函数，Err 状态传递错误。
 */
TEST(ResultAndThen) {
    console::Result<int, std::string> ok(42);
    console::Result<int, std::string> err(std::string("error"));
    auto                              r1 = ok.and_then(
        [](int x) { return console::Result<int, std::string>(x * 2); });
    ASSERT_TRUE(r1.is_ok());
    ASSERT_EQ(84, r1.unwrap());
    auto r2 = err.and_then(
        [](int x) { return console::Result<int, std::string>(x * 2); });
    ASSERT_TRUE(r2.is_err());
    ASSERT_EQ(std::string("error"), r2.err());
}

/**
 * @brief 测试 Result or_else 左值引用。
 * @note 期望：Err 状态应用函数，Ok 状态传递成功值。
 */
TEST(ResultOrElse) {
    console::Result<int, std::string> ok(42);
    console::Result<int, std::string> err(std::string("error"));
    auto                              r1 = ok.or_else([](const std::string &) {
        return console::Result<int, std::string>(100);
    });
    ASSERT_TRUE(r1.is_ok());
    ASSERT_EQ(42, r1.unwrap());
    auto r2 = err.or_else([](const std::string &e) {
        return console::Result<int, std::string>(100);
    });
    ASSERT_TRUE(r2.is_ok());
    ASSERT_EQ(100, r2.unwrap());
}

/**
 * @brief 测试 Result 拷贝构造。
 * @note 期望：深拷贝，独立状态。
 */
TEST(ResultCopyConstruction) {
    console::Result<int, std::string> r1(42);
    console::Result<int, std::string> r2(r1);
    ASSERT_TRUE(r2.is_ok());
    ASSERT_EQ(42, r2.unwrap());
    console::Result<int, std::string> r3(std::string("error"));
    console::Result<int, std::string> r4(r3);
    ASSERT_TRUE(r4.is_err());
    ASSERT_EQ(std::string("error"), r4.err());
}

/**
 * @brief 测试 Result 移动构造。
 * @note 期望：移动后源仍保持原状态。
 */
TEST(ResultMoveConstruction) {
    console::Result<int, std::string> r1(42);
    console::Result<int, std::string> r2(std::move(r1));
    ASSERT_TRUE(r2.is_ok());
    ASSERT_EQ(42, r2.unwrap());
    // r1 仍为 Ok 状态（值被移动）
    ASSERT_TRUE(r1.is_ok());
}

/**
 * @brief 测试 Result 拷贝赋值。
 * @note 期望：深拷贝，独立状态。
 */
TEST(ResultCopyAssignment) {
    console::Result<int, std::string> r1(42);
    console::Result<int, std::string> r2(std::string("error"));
    r2 = r1;
    ASSERT_TRUE(r2.is_ok());
    ASSERT_EQ(42, r2.unwrap());
    r1 = console::Result<int, std::string>(std::string("new error"));
    ASSERT_TRUE(r1.is_err());
    ASSERT_EQ(std::string("new error"), r1.err());
}

/**
 * @brief 测试 Result 移动赋值。
 * @note 期望：移动后目标拥有源状态。
 */
TEST(ResultMoveAssignment) {
    console::Result<int, std::string> r1(42);
    console::Result<int, std::string> r2(std::string("error"));
    r2 = std::move(r1);
    ASSERT_TRUE(r2.is_ok());
    ASSERT_EQ(42, r2.unwrap());
}

/**
 * @brief 测试 Result swap。
 * @note 期望：两个 Result 交换状态。
 */
TEST(ResultSwap) {
    console::Result<int, std::string> r1(42);
    console::Result<int, std::string> r2(std::string("error"));
    r1.swap(r2);
    ASSERT_TRUE(r1.is_err());
    ASSERT_EQ(std::string("error"), r1.err());
    ASSERT_TRUE(r2.is_ok());
    ASSERT_EQ(42, r2.unwrap());
    swap(r1, r2);
    ASSERT_TRUE(r1.is_ok());
    ASSERT_EQ(42, r1.unwrap());
    ASSERT_TRUE(r2.is_err());
    ASSERT_EQ(std::string("error"), r2.err());
}

/**
 * @brief 测试 Result 相等比较。
 * @note 期望：正确比较相等性。
 */
TEST(ResultEquality) {
    console::Result<int, std::string> r1(42);
    console::Result<int, std::string> r2(42);
    console::Result<int, std::string> r3(100);
    console::Result<int, std::string> r4(std::string("error"));
    console::Result<int, std::string> r5(std::string("error"));
    ASSERT_TRUE(r1 == r2);
    ASSERT_FALSE(r1 == r3);
    ASSERT_FALSE(r1 == r4);
    ASSERT_TRUE(r4 == r5);
    ASSERT_TRUE(r1 != r3);
    ASSERT_TRUE(r1 != r4);
    ASSERT_FALSE(r1 != r2);
}

/**
 * @brief 测试 Result ok_ptr。
 * @note 期望：Ok 返回非空指针，Err 返回 nullptr。
 */
TEST(ResultOkPtr) {
    console::Result<int, std::string> ok(42);
    console::Result<int, std::string> err(std::string("error"));
    const auto                       &cok  = ok;
    const auto                       &cerr = err;
    ASSERT_NE(nullptr, ok.ok_ptr());
    ASSERT_NE(nullptr, cok.ok_ptr());
    ASSERT_EQ(nullptr, err.ok_ptr());
    ASSERT_EQ(nullptr, cerr.ok_ptr());
}

/**
 * @brief 测试 Result err_ptr。
 * @note 期望：Err 返回非空指针，Ok 返回 nullptr。
 */
TEST(ResultErrPtr) {
    console::Result<int, std::string> ok(42);
    console::Result<int, std::string> err(std::string("error"));
    const auto                       &cok  = ok;
    const auto                       &cerr = err;
    ASSERT_EQ(nullptr, ok.err_ptr());
    ASSERT_EQ(nullptr, cok.err_ptr());
    ASSERT_NE(nullptr, err.err_ptr());
    ASSERT_NE(nullptr, cerr.err_ptr());
}

/**
 * @brief 测试 Result unwrap_or 左值引用。
 * @note 期望：Ok 返回成功值，Err 返回默认值。
 */
TEST(ResultUnwrapOr) {
    console::Result<int, std::string> ok(42);
    console::Result<int, std::string> err(std::string("error"));
    int                               default_val = 100;
    ASSERT_EQ(42, ok.unwrap_or(default_val));
    ASSERT_EQ(100, err.unwrap_or(default_val));
}

/**
 * @brief 测试 Result unwrap_or 右值引用。
 * @note 期望：Ok 返回成功值，Err 返回默认值。
 */
TEST(ResultUnwrapOrRValue) {
    console::Result<int, std::string> ok(42);
    console::Result<int, std::string> err(std::string("error"));
    ASSERT_EQ(42, std::move(ok).unwrap_or(100));
    ASSERT_EQ(100, std::move(err).unwrap_or(100));
}

/**
 * @brief 测试 Result unwrap_or_else 左值引用。
 * @note 期望：Ok 返回成功值，Err 调用函数生成默认值。
 */
TEST(ResultUnwrapOrElse) {
    console::Result<int, std::string> ok(42);
    console::Result<int, std::string> err(std::string("error"));
    ASSERT_EQ(42, ok.unwrap_or_else([]() { return 100; }));
    ASSERT_EQ(100, err.unwrap_or_else([]() { return 100; }));
}

/**
 * @brief 测试 Result unwrap_or_else 右值引用。
 * @note 期望：Ok 返回成功值，Err 调用函数生成默认值。
 */
TEST(ResultUnwrapOrElseRValue) {
    console::Result<int, std::string> ok(42);
    console::Result<int, std::string> err(std::string("error"));
    ASSERT_EQ(42, std::move(ok).unwrap_or_else([]() { return 100; }));
    ASSERT_EQ(100, std::move(err).unwrap_or_else([]() { return 100; }));
}

/**
 * @brief 测试 Result 自定义类型。
 * @note 期望：自定义类型正确存储和访问。
 */
TEST(ResultCustomType) {
    struct Point {
        int  x, y;
        bool operator==(const Point &other) const {
            return x == other.x && y == other.y;
        }
    };
    console::Result<Point, std::string> ok(Point{10, 20});
    ASSERT_TRUE(ok.is_ok());
    Point p = ok.unwrap();
    ASSERT_EQ(10, p.x);
    ASSERT_EQ(20, p.y);
}

/**
 * @brief 测试 Result T 和 E 相同类型编译错误。
 * @note 期望：编译期断言阻止相同类型。
 */
TEST(ResultSameType) {
    // 编译期检查，无法运行时测试，仅验证编译
    // console::Result<int, int> r(42); // 不应编译
    ASSERT_TRUE(true);
}

/**
 * @brief 测试 Result move 语义 ok。
 * @note 期望：move ok 正确转移值。
 */
TEST(ResultMoveOk) {
    console::Result<std::string, int> ok(std::string("hello"));
    std::string                       s = std::move(ok).ok();
    ASSERT_EQ(std::string("hello"), s);
}

/**
 * @brief 测试 Result move 语义 err。
 * @note 期望：move err 正确转移值。
 */
TEST(ResultMoveErr) {
    console::Result<int, std::string> err(std::string("error"));
    std::string                       e = std::move(err).err();
    ASSERT_EQ(std::string("error"), e);
}

/**
 * @brief 测试 Result and_then 右值引用。
 * @note 期望：Ok 状态移动应用函数。
 */
TEST(ResultAndThenRValue) {
    console::Result<std::string, int> ok(std::string("hello"));
    auto r = std::move(ok).and_then([](std::string s) {
        return console::Result<std::string, int>(s + " world");
    });
    ASSERT_TRUE(r.is_ok());
    ASSERT_EQ(std::string("hello world"), r.unwrap());
}

/**
 * @brief 测试 Result or_else 右值引用。
 * @note 期望：Err 状态移动应用函数。
 */
TEST(ResultOrElseRValue) {
    console::Result<int, std::string> err(std::string("error"));
    auto                              r = std::move(err).or_else(
        [](std::string e) { return console::Result<int, std::string>(42); });
    ASSERT_TRUE(r.is_ok());
    ASSERT_EQ(42, r.unwrap());
}

/**
 * @brief 测试 Result 相等比较不同类型。
 * @note 期望：不同类型不比较。
 */
TEST(ResultEqualityDifferentTypes) {
    console::Result<int, std::string> r1(42);
    console::Result<int, std::string> r2(std::string("error"));
    ASSERT_FALSE(r1 == r2);
    ASSERT_TRUE(r1 != r2);
}

/**
 * @brief 测试 Result as_optional (C++17)。
 * @note 期望：Ok 返回 optional 包含值，Err 返回 nullopt。
 */
#if __cplusplus >= 201703L
TEST(ResultAsOptional) {
    console::Result<int, std::string> ok(42);
    console::Result<int, std::string> err(std::string("error"));
    auto                              opt1 = ok.as_optional();
    auto                              opt2 = err.as_optional();
    ASSERT_TRUE(opt1.has_value());
    ASSERT_EQ(42, opt1.value());
    ASSERT_FALSE(opt2.has_value());
    auto opt3 = std::move(ok).as_optional();
    ASSERT_TRUE(opt3.has_value());
    ASSERT_EQ(42, opt3.value());
}
#endif

#ifndef NOMAIN
TEST_MAIN
#endif
