/**
 * @file collide.cpp
 * @brief 测试双重分派模块 (Table)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */
#include "../include/collide.h"

#include <string>

#include "../include/test.h"

class Shape {
public:
    virtual ~Shape() = default;
};

class Circle : public Shape {};
class Rectangle : public Shape {};
class Triangle : public Shape {};
class Ellipse : public Shape {};

/**
 * @brief 测试 Table 的默认构造。
 * @note 期望：默认构造的 Table 为空，无默认处理函数。
 */
TEST(TableDefaultConstruction) {
    console::Table<Shape> table;
    ASSERT_FALSE(table.has_default());
    ASSERT_FALSE(table.callable(typeid(Circle), typeid(Rectangle)));
}

/**
 * @brief 测试 Table 的 add 方法注册函数。
 * @note 期望：注册后 has 返回 true，调用返回正确结果。
 */
TEST(TableAddAndHas) {
    console::Table<Shape> table;
    bool                  called = false;
    table.add(typeid(Circle), typeid(Rectangle), [&](Shape &, Shape &) {
        called = true;
    });
    ASSERT_TRUE(table.has(typeid(Circle), typeid(Rectangle)));
    ASSERT_FALSE(table.has(typeid(Rectangle), typeid(Circle)));
    Circle    c;
    Rectangle r;
    table.call(c, r);
    ASSERT_TRUE(called);
}

/**
 * @brief 测试 Table 的 add_symmetric 方法。
 * @note 期望：两个方向都注册，调用任一方向都触发。
 */
TEST(TableAddSymmetric) {
    console::Table<Shape> table;
    int                   callCount = 0;
    table.add_symmetric(typeid(Circle),
        typeid(Rectangle),
        [&](Shape &, Shape &) { ++callCount; });
    ASSERT_TRUE(table.has(typeid(Circle), typeid(Rectangle)));
    ASSERT_TRUE(table.has(typeid(Rectangle), typeid(Circle)));
    Circle    c;
    Rectangle r;
    table.call(c, r);
    ASSERT_EQ(1, callCount);
    table.call(r, c);
    ASSERT_EQ(2, callCount);
}

/**
 * @brief 测试 Table 的 call 方法。
 * @note 期望：根据对象的实际类型调用对应的处理函数。
 */
TEST(TableCall) {
    console::Table<Shape> table;
    std::string           result;
    table.add(typeid(Circle), typeid(Rectangle), [&](Shape &, Shape &) {
        result = "Circle-Rectangle";
    });
    table.add(typeid(Rectangle), typeid(Circle), [&](Shape &, Shape &) {
        result = "Rectangle-Circle";
    });
    table.add(typeid(Circle), typeid(Circle), [&](Shape &, Shape &) {
        result = "Circle-Circle";
    });
    Circle    c;
    Rectangle r;
    table.call(c, r);
    ASSERT_EQ(std::string("Circle-Rectangle"), result);
    table.call(r, c);
    ASSERT_EQ(std::string("Rectangle-Circle"), result);
    table.call(c, c);
    ASSERT_EQ(std::string("Circle-Circle"), result);
}

/**
 * @brief 测试 Table 的 operator() 获取函数。
 * @note 期望：operator() 返回注册的函数。
 */
TEST(TableOperatorParens) {
    console::Table<Shape> table;
    bool                  called = false;
    table.add(typeid(Circle), typeid(Triangle), [&](Shape &, Shape &) {
        called = true;
    });
    auto     func = table(typeid(Circle), typeid(Triangle));
    Circle   c;
    Triangle t;
    func(c, t);
    ASSERT_TRUE(called);
}

/**
 * @brief 测试 Table 未注册类型对时抛出 TypeError。
 * @note 期望：调用未注册的类型对抛出 TypeError。
 */
TEST(TableUnregisteredThrows) {
    console::Table<Shape> table;
    table.add(typeid(Circle), typeid(Rectangle), [](Shape &, Shape &) {});
    Circle   c;
    Triangle t;
    ASSERT_THROWS(table.call(c, t), console::TypeError);
}

/**
 * @brief 测试 Table 的默认处理函数。
 * @note 期望：未注册时调用默认处理函数。
 */
TEST(TableDefaultHandler) {
    console::Table<Shape> table;
    bool                  defaultCalled = false;
    table.set_default([&](Shape &, Shape &) { defaultCalled = true; });
    ASSERT_TRUE(table.has_default());
    Circle   c;
    Triangle t;
    table.call(c, t);
    ASSERT_TRUE(defaultCalled);
    bool registeredCalled = false;
    table.add(typeid(Circle), typeid(Triangle), [&](Shape &, Shape &) {
        registeredCalled = true;
    });
    defaultCalled = false;
    table.call(c, t);
    ASSERT_TRUE(registeredCalled);
    ASSERT_FALSE(defaultCalled);
}

/**
 * @brief 测试 Table 的 callable 方法。
 * @note 期望：已注册或设有默认处理函数时返回 true。
 */
TEST(TableCallable) {
    console::Table<Shape> table;
    ASSERT_FALSE(table.callable(typeid(Circle), typeid(Rectangle)));
    table.add(typeid(Circle), typeid(Rectangle), [](Shape &, Shape &) {});
    ASSERT_TRUE(table.callable(typeid(Circle), typeid(Rectangle)));
    ASSERT_FALSE(table.callable(typeid(Rectangle), typeid(Circle)));
    table.set_default([](Shape &, Shape &) {});
    ASSERT_TRUE(table.callable(typeid(Circle), typeid(Triangle)));
}

/**
 * @brief 测试 Table 的 remove 方法。
 * @note 期望：移除后 has 返回 false。
 */
TEST(TableRemove) {
    console::Table<Shape> table;
    table.add(typeid(Circle), typeid(Rectangle), [](Shape &, Shape &) {});
    ASSERT_TRUE(table.has(typeid(Circle), typeid(Rectangle)));
    table.remove(typeid(Circle), typeid(Rectangle));
    ASSERT_FALSE(table.has(typeid(Circle), typeid(Rectangle)));
    table.remove(typeid(Triangle), typeid(Ellipse));
}

/**
 * @brief 测试 Table 的 clear 方法。
 * @note 期望：clear 后所有注册被移除，但默认处理函数保留。
 */
TEST(TableClear) {
    console::Table<Shape> table;
    table.add(typeid(Circle), typeid(Rectangle), [](Shape &, Shape &) {});
    table.add(typeid(Triangle), typeid(Ellipse), [](Shape &, Shape &) {});
    table.set_default([](Shape &, Shape &) {});
    ASSERT_TRUE(table.has(typeid(Circle), typeid(Rectangle)));
    ASSERT_TRUE(table.has(typeid(Triangle), typeid(Ellipse)));
    table.clear();
    ASSERT_FALSE(table.has(typeid(Circle), typeid(Rectangle)));
    ASSERT_FALSE(table.has(typeid(Triangle), typeid(Ellipse)));
    ASSERT_TRUE(table.has_default()); // 默认处理函数保留
}

/**
 * @brief 测试 Table 的 set_default 清除默认处理函数。
 * @note 期望：传入空函数后 has_default 返回 false。
 */
TEST(TableSetDefaultClear) {
    console::Table<Shape> table;
    table.set_default([](Shape &, Shape &) {});
    ASSERT_TRUE(table.has_default());
    table.set_default();
    ASSERT_FALSE(table.has_default());
}

/**
 * @brief 测试 Table 处理多个派生类型。
 * @note 期望：所有类型组合正确分发。
 */
TEST(TableMultipleTypes) {
    console::Table<Shape> table;
    std::string           result;
    auto                  handler = [&](const std::string &name) {
        return [&, name](Shape &, Shape &) { result = name; };
    };
    table.add(typeid(Circle), typeid(Circle), handler("CC"));
    table.add(typeid(Circle), typeid(Rectangle), handler("CR"));
    table.add(typeid(Rectangle), typeid(Circle), handler("RC"));
    table.add(typeid(Rectangle), typeid(Rectangle), handler("RR"));
    table.add(typeid(Circle), typeid(Triangle), handler("CT"));
    table.add(typeid(Triangle), typeid(Circle), handler("TC"));
    Circle    c;
    Rectangle r;
    Triangle  t;
    table.call(c, c);
    ASSERT_EQ(std::string("CC"), result);
    table.call(c, r);
    ASSERT_EQ(std::string("CR"), result);
    table.call(r, c);
    ASSERT_EQ(std::string("RC"), result);
    table.call(r, r);
    ASSERT_EQ(std::string("RR"), result);
    table.call(c, t);
    ASSERT_EQ(std::string("CT"), result);
    table.call(t, c);
    ASSERT_EQ(std::string("TC"), result);
}

/**
 * @brief 测试 Table 对称注册的多个类型。
 * @note 期望：对称注册正确处理多个类型。
 */
TEST(TableSymmetricMultipleTypes) {
    console::Table<Shape> table;
    int                   count   = 0;
    auto                  handler = [&](Shape &, Shape &) { ++count; };
    table.add_symmetric(typeid(Circle), typeid(Rectangle), handler);
    table.add_symmetric(typeid(Circle), typeid(Triangle), handler);
    Circle    c;
    Rectangle r;
    Triangle  t;
    count = 0;
    table.call(c, r);
    ASSERT_EQ(1, count);
    table.call(r, c);
    ASSERT_EQ(2, count);
    table.call(c, t);
    ASSERT_EQ(3, count);
    table.call(t, c);
    ASSERT_EQ(4, count);
    ASSERT_THROWS(table.call(c, c), console::TypeError);
}

/**
 * @brief 测试 Table 的 const 正确性。
 * @note 期望：const Table 可以调用 call 和 has。
 */
TEST(TableConstCorrectness) {
    console::Table<Shape> table;
    table.add(typeid(Circle), typeid(Rectangle), [](Shape &, Shape &) {});
    const console::Table<Shape> &constTable = table;
    ASSERT_TRUE(constTable.has(typeid(Circle), typeid(Rectangle)));
    ASSERT_FALSE(constTable.has(typeid(Rectangle), typeid(Circle)));
    auto func = constTable(typeid(Circle), typeid(Rectangle));
    (void)func;
    Circle    c;
    Rectangle r;
    constTable.call(c, r);
}

/**
 * @brief 测试 Table 的默认处理函数与显式注册的优先级。
 * @note 期望：显式注册优先于默认处理函数。
 */
TEST(TableDefaultVsRegistered) {
    console::Table<Shape> table;
    bool                  defaultCalled    = false;
    bool                  registeredCalled = false;
    table.set_default([&](Shape &, Shape &) { defaultCalled = true; });
    table.add(typeid(Circle), typeid(Rectangle), [&](Shape &, Shape &) {
        registeredCalled = true;
    });
    Circle    c;
    Rectangle r;
    table.call(c, r);
    ASSERT_TRUE(registeredCalled);
    ASSERT_FALSE(defaultCalled);
    registeredCalled = false;
    defaultCalled    = false;
    Triangle t;
    table.call(c, t);
    ASSERT_FALSE(registeredCalled);
    ASSERT_TRUE(defaultCalled);
}

/**
 * @brief 测试 Table 的 call 方法参数顺序重要性。
 * @note 期望：非对称注册时参数顺序影响调用结果。
 */
TEST(TableParameterOrder) {
    console::Table<Shape> table;
    std::string           result;
    table.add(typeid(Circle), typeid(Rectangle), [&](Shape &, Shape &) {
        result = "Circle-Rectangle";
    });
    Circle    c;
    Rectangle r;
    table.call(c, r);
    ASSERT_EQ(std::string("Circle-Rectangle"), result);
    ASSERT_THROWS(table.call(r, c), console::TypeError);
}

/**
 * @brief 测试 Table 的 PairHash 功能。
 * @note 期望：PairHash 正确计算哈希值，unordered_map 正常工作。
 */
TEST(TablePairHash) {
    console::Table<Shape> table;
    table.add(typeid(Circle), typeid(Rectangle), [](Shape &, Shape &) {});
    table.add(typeid(Rectangle), typeid(Circle), [](Shape &, Shape &) {});
    table.add(typeid(Circle), typeid(Triangle), [](Shape &, Shape &) {});
    table.add(typeid(Triangle), typeid(Circle), [](Shape &, Shape &) {});
    table.add(typeid(Rectangle), typeid(Triangle), [](Shape &, Shape &) {});
    ASSERT_TRUE(table.has(typeid(Circle), typeid(Rectangle)));
    ASSERT_TRUE(table.has(typeid(Rectangle), typeid(Circle)));
    ASSERT_TRUE(table.has(typeid(Circle), typeid(Triangle)));
    ASSERT_TRUE(table.has(typeid(Triangle), typeid(Circle)));
    ASSERT_TRUE(table.has(typeid(Rectangle), typeid(Triangle)));
    ASSERT_FALSE(table.has(typeid(Triangle), typeid(Rectangle)));
}

#ifndef NOMAIN
TEST_MAIN
#endif
