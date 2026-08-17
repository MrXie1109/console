/**
 * @file cow.cpp
 * @brief 测试写时复制模块 (Cow)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/cow.h"

#include <string>
#include <vector>

#include "../include/test.h"

/**
 * @brief 测试 Cow 默认构造。
 * @note 期望：默认构造成功，内部对象已初始化。
 */
TEST(CowDefaultConstruction) {
    console::Cow<int> cow;
    ASSERT_EQ(0, cow.reader());
}

/**
 * @brief 测试 Cow 从值构造。
 * @note 期望：内部存储指定值。
 */
TEST(CowValueConstruction) {
    console::Cow<int> cow(42);
    ASSERT_EQ(42, cow.reader());
}

/**
 * @brief 测试 Cow 从参数包构造。
 * @note 期望：使用参数构造内部对象。
 */
TEST(CowArgsConstruction) {
    console::Cow<std::string> cow(5, 'A');
    ASSERT_EQ(std::string("AAAAA"), cow.reader());
}

/**
 * @brief 测试 Cow 拷贝构造。
 * @note 期望：两个 Cow 共享同一份数据，写时分离。
 */
TEST(CowCopyConstruction) {
    console::Cow<int> cow1(42);
    console::Cow<int> cow2(cow1);
    ASSERT_EQ(42, cow2.reader());
    cow2.writer() = 100;
    ASSERT_EQ(42, cow1.reader());
    ASSERT_EQ(100, cow2.reader());
}

/**
 * @brief 测试 Cow 移动构造。
 * @note 期望：数据所有权转移。
 */
TEST(CowMoveConstruction) {
    console::Cow<int> cow1(42);
    console::Cow<int> cow2(std::move(cow1));
    ASSERT_EQ(42, cow2.reader());
}

/**
 * @brief 测试 Cow 拷贝赋值。
 * @note 期望：两个 Cow 共享同一份数据，写时分离。
 */
TEST(CowCopyAssignment) {
    console::Cow<int> cow1(42);
    console::Cow<int> cow2(100);
    cow2 = cow1;
    ASSERT_EQ(42, cow2.reader());
    cow2.writer() = 200;
    ASSERT_EQ(42, cow1.reader());
    ASSERT_EQ(200, cow2.reader());
}

/**
 * @brief 测试 Cow 移动赋值。
 * @note 期望：数据所有权转移。
 */
TEST(CowMoveAssignment) {
    console::Cow<int> cow1(42);
    console::Cow<int> cow2(100);
    cow2 = std::move(cow1);
    ASSERT_EQ(42, cow2.reader());
}

/**
 * @brief 测试 Cow 从 T 拷贝赋值。
 * @note 期望：内部数据替换为新值。
 */
TEST(CowAssignFromValue) {
    console::Cow<int> cow(42);
    cow = 100;
    ASSERT_EQ(100, cow.reader());
}

/**
 * @brief 测试 Cow 从 T 移动赋值。
 * @note 期望：内部数据替换为新值。
 */
TEST(CowAssignFromRValue) {
    console::Cow<std::string> cow("hello");
    std::string               s = "world";
    cow                         = std::move(s);
    ASSERT_EQ(std::string("world"), cow.reader());
}

/**
 * @brief 测试 Cow reader 返回常量引用。
 * @note 期望：读取操作不触发写时复制。
 */
TEST(CowReader) {
    console::Cow<std::vector<int>> cow(std::vector<int>{1, 2, 3});
    const auto                    &v = cow.reader();
    ASSERT_EQ(3, v.size());
    ASSERT_EQ(1, v[0]);
    ASSERT_EQ(1, cow.data().use_count());
}

/**
 * @brief 测试 Cow writer 触发写时复制。
 * @note 期望：写入操作在共享时复制数据。
 */
TEST(CowWriter) {
    console::Cow<std::vector<int>> cow(std::vector<int>{1, 2, 3});
    auto                           cow2 = cow;
    cow.writer().push_back(4);
    ASSERT_EQ(4, cow.reader().size());
    ASSERT_EQ(3, cow2.reader().size());
}

/**
 * @brief 测试 Cow detach 手动分离。
 * @note 期望：detach 后独立拥有数据。
 */
TEST(CowDetach) {
    console::Cow<std::vector<int>> cow(std::vector<int>{1, 2, 3});
    auto                           cow2 = cow;
    cow.detach();
    cow.writer().push_back(4);
    ASSERT_EQ(4, cow.reader().size());
    ASSERT_EQ(3, cow2.reader().size());
}

/**
 * @brief 测试 Cow detach 已独立时不复制。
 * @note 期望：已独立时 detach 不做任何操作。
 */
TEST(CowDetachAlreadyUnique) {
    console::Cow<std::vector<int>> cow(std::vector<int>{1, 2, 3});
    cow.detach();
    ASSERT_EQ(1, cow.data().use_count());
    cow.writer().push_back(4);
    ASSERT_EQ(4, cow.reader().size());
}

/**
 * @brief 测试 Cow read 函数。
 * @note 期望：read 函数正确读取数据。
 */
TEST(CowRead) {
    console::Cow<int> cow(42);
    int               result = cow.read([](int x) { return x * 2; });
    ASSERT_EQ(84, result);
    ASSERT_EQ(42, cow.reader());
}

/**
 * @brief 测试 Cow write 函数。
 * @note 期望：write 函数正确修改数据。
 */
TEST(CowWrite) {
    console::Cow<int> cow(42);
    cow.write([](int &x) { x *= 2; });
    ASSERT_EQ(84, cow.reader());
}

/**
 * @brief 测试 Cow r 别名。
 * @note 期望：r 等价于 reader。
 */
TEST(CowRAlias) {
    console::Cow<int> cow(42);
    ASSERT_EQ(42, cow.r());
}

/**
 * @brief 测试 Cow w 别名。
 * @note 期望：w 等价于 writer。
 */
TEST(CowWAlias) {
    console::Cow<int> cow(42);
    cow.w() = 100;
    ASSERT_EQ(100, cow.reader());
}

/**
 * @brief 测试 Cow r 函数重载。
 * @note 期望：r(F) 等价于 read(F)。
 */
TEST(CowRFunction) {
    console::Cow<int> cow(42);
    int               result = cow.r([](int x) { return x + 1; });
    ASSERT_EQ(43, result);
}

/**
 * @brief 测试 Cow w 函数重载。
 * @note 期望：w(F) 等价于 write(F)。
 */
TEST(CowWFunction) {
    console::Cow<int> cow(42);
    cow.w([](int &x) { x += 10; });
    ASSERT_EQ(52, cow.reader());
}

/**
 * @brief 测试 Cow 共享语义。
 * @note 期望：多个 Cow 共享数据，写入时复制。
 */
TEST(CowSharingSemantics) {
    console::Cow<std::vector<int>> cow1(std::vector<int>{1, 2, 3});
    auto                           cow2 = cow1;
    auto                           cow3 = cow1;
    ASSERT_EQ(3, cow1.data().use_count());
    cow2.writer().push_back(4);
    ASSERT_EQ(3, cow1.reader().size());
    ASSERT_EQ(3, cow3.reader().size());
    ASSERT_EQ(4, cow2.reader().size());
    ASSERT_EQ(2, cow1.data().use_count());
    ASSERT_EQ(1, cow2.data().use_count());
}

/**
 * @brief 测试 Cow 自定义类型。
 * @note 期望：自定义类型正确处理。
 */
TEST(CowCustomType) {
    struct Point {
        int x, y;
        Point(int a = 0, int b = 0) : x(a), y(b) {}
    };

    console::Cow<Point> cow(10, 20);
    ASSERT_EQ(10, cow.reader().x);
    ASSERT_EQ(20, cow.reader().y);
    cow.writer().x = 30;
    ASSERT_EQ(30, cow.reader().x);
}

/**
 * @brief 测试 Cow 空值构造。
 * @note 期望：默认构造的内部对象使用默认构造函数。
 */
TEST(CowEmptyConstruction) {
    console::Cow<std::vector<int>> cow;
    ASSERT_TRUE(cow.reader().empty());
}

/**
 * @brief 测试 Cow 字符串。
 * @note 期望：字符串正确存储和修改。
 */
TEST(CowString) {
    console::Cow<std::string> cow("hello");
    ASSERT_EQ(std::string("hello"), cow.reader());
    cow.writer() += " world";
    ASSERT_EQ(std::string("hello world"), cow.reader());
}

/**
 * @brief 测试危险用途 (请勿模仿)。
 * @note 期望：错误的用法产生错误的行为。
 */
TEST(CowWriterDangerousUsage) {
    console::Cow<int> cow(42);
    auto             &ref  = cow.writer();
    auto              copy = cow;
    ref                    = 43;
    ASSERT_EQ(43, copy.reader()); // Not 42!
}

#ifndef NOMAIN
TEST_MAIN
#endif
