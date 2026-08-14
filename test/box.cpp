/**
 * @file box.cpp
 * @brief 测试异构容器模块 (Item, Box)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */
#include "../include/box.h"

#include <sstream>
#include <string>
#include <vector>

#include "../include/test.h"

/**
 * @brief 测试 Item 的默认构造。
 * @note 期望：默认构造的 Item 为空。
 */
TEST(ItemDefaultConstruction) {
    console::Item item;
    ASSERT_THROWS(item.get<int>(), console::TypeError);
}

/**
 * @brief 测试 Item 存储 int 类型。
 * @note 期望：存储和获取 int 值正确。
 */
TEST(ItemStoreInt) {
    console::Item item(42);
    ASSERT_EQ(42, item.get<int>());
}

/**
 * @brief 测试 Item 存储 double 类型。
 * @note 期望：存储和获取 double 值正确。
 */
TEST(ItemStoreDouble) {
    console::Item item(3.14);
    ASSERT_NEAR(3.14, item.get<double>(), 0.001);
}

/**
 * @brief 测试 Item 存储 std::string 类型。
 * @note 期望：存储和获取 std::string 值正确。
 */
TEST(ItemStoreString) {
    console::Item item(std::string("hello"));
    ASSERT_EQ(std::string("hello"), item.get<std::string>());
}

/**
 * @brief 测试 Item 存储 C 风格字符串字面量。
 * @note 期望：存储 const char* 并正确获取。
 */
TEST(ItemStoreCString) {
    console::Item item("hello");
    ASSERT_STRCASEEQ("hello", item.get<const char *>());
}

/**
 * @brief 测试 Item 的拷贝构造。
 * @note 期望：拷贝后两个 Item 独立，值相同。
 */
TEST(ItemCopyConstruction) {
    console::Item item1(42);
    console::Item item2(item1);
    ASSERT_EQ(42, item1.get<int>());
    ASSERT_EQ(42, item2.get<int>());
    item2.get<int>() = 100;
    ASSERT_EQ(42, item1.get<int>());
    ASSERT_EQ(100, item2.get<int>());
}

/**
 * @brief 测试 Item 的移动构造。
 * @note 期望：移动后新 Item 拥有值，原 Item 为空。
 */
TEST(ItemMoveConstruction) {
    console::Item item1(42);
    console::Item item2(std::move(item1));
    ASSERT_EQ(42, item2.get<int>());
    ASSERT_THROWS(item1.get<int>(), console::TypeError);
}

/**
 * @brief 测试 Item 的拷贝赋值。
 * @note 期望：拷贝赋值后两个 Item 独立，值相同。
 */
TEST(ItemCopyAssignment) {
    console::Item item1(42);
    console::Item item2(100);
    item2 = item1;
    ASSERT_EQ(42, item1.get<int>());
    ASSERT_EQ(42, item2.get<int>());
    item2.get<int>() = 200;
    ASSERT_EQ(42, item1.get<int>());
    ASSERT_EQ(200, item2.get<int>());
}

/**
 * @brief 测试 Item 的移动赋值。
 * @note 期望：移动赋值后目标 Item 拥有值，源 Item 为空。
 */
TEST(ItemMoveAssignment) {
    console::Item item1(42);
    console::Item item2(100);
    item2 = std::move(item1);
    ASSERT_EQ(42, item2.get<int>());
    ASSERT_THROWS(item1.get<int>(), console::TypeError);
}

/**
 * @brief 测试 Item 的类型安全 get 方法（类型匹配）。
 * @note 期望：类型匹配时正确返回值。
 */
TEST(ItemGetTypeSafeMatch) {
    console::Item item(3.14);
    ASSERT_NEAR(3.14, item.get<double>(), 0.001);
}

/**
 * @brief 测试 Item 的类型安全 get 方法（类型不匹配）。
 * @note 期望：类型不匹配时抛出 TypeError。
 */
TEST(ItemGetTypeSafeMismatch) {
    console::Item item(42);
    ASSERT_THROWS(item.get<double>(), console::TypeError);
}

/**
 * @brief 测试 Item 的类型安全 get 方法（空 Item）。
 * @note 期望：空 Item 调用 get 抛出 TypeError。
 */
TEST(ItemGetTypeSafeEmpty) {
    console::Item item;
    ASSERT_THROWS(item.get<int>(), console::TypeError);
}

/**
 * @brief 测试 Item 的 unsafe_get 方法。
 * @note 期望：unsafe_get 不进行类型检查，正确返回引用。
 */
TEST(ItemUnsafeGet) {
    console::Item item(42);
    int          &ref = item.unsafe_get<int>();
    ASSERT_EQ(42, ref);
    ref = 100;
    ASSERT_EQ(100, item.unsafe_get<int>());
}

/**
 * @brief 测试 Item 的 str 方法。
 * @note 期望：str 返回值的字符串表示。
 */
TEST(ItemStr) {
    console::Item item(42);
    ASSERT_EQ(std::string("42"), item.str());
    console::Item item2(3.14);
    std::string   str = item2.str();
    ASSERT_STRCONTAINS(str.c_str(), "3.14");
    console::Item item3(std::string("hello"));
    ASSERT_EQ(std::string("\"hello\""), item3.str());
}

/**
 * @brief 测试 Item 的 wstr 方法。
 * @note 期望：wstr 返回值的宽字符串表示。
 */
TEST(ItemWstr) {
    console::Item item(42);
    std::wstring  expected = L"42";
    ASSERT_EQ(expected, item.wstr());
}

/**
 * @brief 测试 Item 的流输出运算符。
 * @note 期望：输出值的字符串表示。
 */
TEST(ItemOutputStream) {
    console::Item      item(42);
    std::ostringstream oss;
    oss << item;
    ASSERT_EQ(std::string("42"), oss.str());
}

/**
 * @brief 测试 Item 的宽字符流输出运算符。
 * @note 期望：输出值的宽字符串表示。
 */
TEST(ItemWideOutputStream) {
    console::Item       item(42);
    std::wostringstream woss;
    woss << item;
    ASSERT_EQ(std::wstring(L"42"), woss.str());
}

/**
 * @brief 测试 Box 的默认构造。
 * @note 期望：默认构造的 Box 为空。
 */
TEST(BoxDefaultConstruction) {
    console::Box box;
    ASSERT_TRUE(box.empty());
    ASSERT_EQ(0, box.size());
}

/**
 * @brief 测试 Box 从单个值构造。
 * @note 期望：Box 包含一个元素，值正确。
 */
TEST(BoxSingleValueConstruction) {
    console::Box box(42);
    ASSERT_EQ(1, box.size());
    ASSERT_EQ(42, box.get<int>(0));
}

/**
 * @brief 测试 Box 从多个值构造。
 * @note 期望：Box 按顺序包含所有元素，值正确。
 */
TEST(BoxMultipleValuesConstruction) {
    console::Box box(42, 3.14, std::string("hello"));
    ASSERT_EQ(3, box.size());
    ASSERT_EQ(42, box.get<int>(0));
    ASSERT_NEAR(3.14, box.get<double>(1), 0.001);
    ASSERT_EQ(std::string("hello"), box.get<std::string>(2));
}

/**
 * @brief 测试 Box 从异构类型构造。
 * @note 期望：Box 正确存储不同类型。
 */
TEST(BoxHeterogeneousConstruction) {
    console::Box box(100, 2.718, "world", true);
    ASSERT_EQ(4, box.size());
    ASSERT_EQ(100, box.get<int>(0));
    ASSERT_NEAR(2.718, box.get<double>(1), 0.001);
    ASSERT_STRCASEEQ("world", box.get<const char *>(2));
    ASSERT_TRUE(box.get<bool>(3));
}

/**
 * @brief 测试 Box 的类型安全 get 方法。
 * @note 期望：类型匹配时正确返回引用，类型不匹配时抛出 TypeError。
 */
TEST(BoxGetTypeSafe) {
    console::Box box(42, 3.14);
    int         &i = box.get<int>(0);
    ASSERT_EQ(42, i);
    double &d = box.get<double>(1);
    ASSERT_NEAR(3.14, d, 0.001);
    ASSERT_THROWS(box.get<double>(0), console::TypeError);
    ASSERT_THROWS(box.get<int>(1), console::TypeError);
}

/**
 * @brief 测试 Box 的 get 方法索引越界。
 * @note 期望：索引越界时抛出 IndexError。
 */
TEST(BoxGetIndexOutOfRange) {
    console::Box box(42);
    ASSERT_THROWS(box.get<int>(1), console::IndexError);
    ASSERT_THROWS(box.get<int>(100), console::IndexError);
}

/**
 * @brief 测试 Box 的 unsafe_get 方法。
 * @note 期望：unsafe_get 不进行边界和类型检查。
 */
TEST(BoxUnsafeGet) {
    console::Box box(42, 3.14);
    int         &i = box.unsafe_get<int>(0);
    ASSERT_EQ(42, i);
    double &d = box.unsafe_get<double>(1);
    ASSERT_NEAR(3.14, d, 0.001);
}

/**
 * @brief 测试 Box 的 unpack 方法（完整匹配）。
 * @note 期望：正确解包所有元素到变量。
 */
TEST(BoxUnpackCompleteMatch) {
    console::Box box(42, 3.14, std::string("hello"));
    int          i;
    double       d;
    std::string  s;
    box.unpack(i, d, s);
    ASSERT_EQ(42, i);
    ASSERT_NEAR(3.14, d, 0.001);
    ASSERT_EQ(std::string("hello"), s);
}

/**
 * @brief 测试 Box 的 unpack 方法（数量不匹配）。
 * @note 期望：数量不匹配时抛出 TypeError。
 */
TEST(BoxUnpackCountMismatch) {
    console::Box box(42, 3.14);
    int          i;
    double       d;
    std::string  s;
    ASSERT_THROWS(box.unpack(i, d, s), console::TypeError);
}

/**
 * @brief 测试 Box 的 unpack 方法（类型不匹配）。
 * @note 期望：类型不匹配时抛出 TypeError。
 */
TEST(BoxUnpackTypeMismatch) {
    console::Box box(42, 3.14);
    int          i;
    std::string  s;
    ASSERT_THROWS(box.unpack(i, s), console::TypeError);
}

/**
 * @brief 测试 Box 的 unsafe_unpack 方法。
 * @note 期望：不进行类型检查，直接解包。
 */
TEST(BoxUnsafeUnpack) {
    console::Box box(42, 3.14);
    int          i;
    double       d;
    box.unsafe_unpack(i, d);
    ASSERT_EQ(42, i);
    ASSERT_NEAR(3.14, d, 0.001);
}

/**
 * @brief 测试 Box 的流输出运算符（空 Box）。
 * @note 期望：空 Box 输出为 "()"。
 */
TEST(BoxOutputStreamEmpty) {
    console::Box       box;
    std::ostringstream oss;
    oss << box;
    ASSERT_EQ(std::string("()"), oss.str());
}

/**
 * @brief 测试 Box 的流输出运算符（非空 Box）。
 * @note 期望：输出格式为 "(elem1, elem2, ...)"。
 */
TEST(BoxOutputStreamNonEmpty) {
    console::Box       box(42, 3.14, std::string("hello"));
    std::ostringstream oss;
    oss << box;
    std::string str = oss.str();
    ASSERT_STRCONTAINS(str.c_str(), "42");
    ASSERT_STRCONTAINS(str.c_str(), "3.14");
    ASSERT_STRCONTAINS(str.c_str(), "hello");
    ASSERT_STRCONTAINS(str.c_str(), "(");
    ASSERT_STRCONTAINS(str.c_str(), ")");
}

/**
 * @brief 测试 Box 的宽字符流输出运算符。
 * @note 期望：宽字符流正确输出。
 */
TEST(BoxWideOutputStream) {
    console::Box        box(42, 3.14);
    std::wostringstream woss;
    woss << box;
    std::wstring result = woss.str();
    std::string  str    = std::string(result.begin(), result.end());
    ASSERT_STRCONTAINS(str.c_str(), "42");
    ASSERT_STRCONTAINS(str.c_str(), "3.14");
}

/**
 * @brief 测试 Box 与 std::vector 的兼容性。
 * @note 期望：Box 可以作为 std::vector 使用。
 */
TEST(BoxVectorCompatibility) {
    console::Box box;
    box.push_back(console::Item(42));
    box.push_back(console::Item(3.14));
    ASSERT_EQ(2, box.size());
    ASSERT_EQ(42, box[0].get<int>());
    ASSERT_NEAR(3.14, box[1].get<double>(), 0.001);
    box.emplace_back(std::string("hello"));
    ASSERT_EQ(3, box.size());
    ASSERT_EQ(std::string("hello"), box[2].get<std::string>());
}

/**
 * @brief 测试 Item 存储自定义类型。
 * @note 期望：自定义类型能正确存储和获取。
 */
TEST(ItemCustomType) {
    struct Point {
        int  x, y;
        bool operator==(const Point &other) const {
            return x == other.x && y == other.y;
        }
    };
    Point         p{10, 20};
    console::Item item(p);
    Point        &result = item.get<Point>();
    ASSERT_EQ(10, result.x);
    ASSERT_EQ(20, result.y);
}

/**
 * @brief 测试 Item 存储 std::vector。
 * @note 期望：容器类型能正确存储和获取。
 */
TEST(ItemStoreVector) {
    std::vector<int>  vec = {1, 2, 3, 4, 5};
    console::Item     item(vec);
    std::vector<int> &result = item.get<std::vector<int>>();
    ASSERT_EQ(5, result.size());
    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(3, result[2]);
    ASSERT_EQ(5, result[4]);
}

/**
 * @brief 测试 Box 嵌套存储。
 * @note 期望：Box 可以存储其他 Box。
 */
TEST(BoxNestedBox) {
    console::Box inner(42, 3.14);
    console::Box outer(100, inner, std::string("test"));
    ASSERT_EQ(3, outer.size());
    ASSERT_EQ(100, outer.get<int>(0));
    console::Box &innerRef = outer.get<console::Box>(1);
    ASSERT_EQ(2, innerRef.size());
    ASSERT_EQ(42, innerRef.get<int>(0));
    ASSERT_NEAR(3.14, innerRef.get<double>(1), 0.001);
    ASSERT_EQ(std::string("test"), outer.get<std::string>(2));
}

#ifndef NOMAIN
TEST_MAIN
#endif
