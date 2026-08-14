/**
 * @file multiarray.cpp
 * @brief 测试多维数组模块 (_V2::MultiArray)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/multiarray.h"

#include <sstream>
#include <string>

#include "../include/test.h"

/**
 * @brief 测试 MultiArray 一维默认构造。
 * @note 期望：默认构造成功，元素未初始化但可访问。
 */
TEST(MultiArrayV2_1DDefaultConstruction) {
    console::_V2::MultiArray<int, 5> arr;
    arr[0] = 10;
    arr[4] = 20;
    ASSERT_EQ(10, arr[0]);
    ASSERT_EQ(20, arr[4]);
}

/**
 * @brief 测试 MultiArray 一维填充构造。
 * @note 期望：所有元素填充为指定值。
 */
TEST(MultiArrayV2_1DFillConstruction) {
    console::_V2::MultiArray<int, 5> arr(42);
    for (size_t i = 0; i < 5; ++i) {
        ASSERT_EQ(42, arr[i]);
    }
}

/**
 * @brief 测试 MultiArray 一维初始化列表构造。
 * @note 期望：元素按初始化列表顺序赋值。
 */
TEST(MultiArrayV2_1DInitializerList) {
    console::_V2::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    ASSERT_EQ(1, arr[0]);
    ASSERT_EQ(3, arr[2]);
    ASSERT_EQ(5, arr[4]);
}

/**
 * @brief 测试 MultiArray 一维拷贝构造。
 * @note 期望：深拷贝，独立拥有者。
 */
TEST(MultiArrayV2_1DCopyConstruction) {
    console::_V2::MultiArray<int, 5> arr1{1, 2, 3, 4, 5};
    console::_V2::MultiArray<int, 5> arr2(arr1);
    arr1[0] = 100;
    ASSERT_EQ(1, arr2[0]);
    ASSERT_EQ(100, arr1[0]);
}

/**
 * @brief 测试 MultiArray 一维移动构造。
 * @note 期望：移动后目标拥有数据，源变为视图。
 */
TEST(MultiArrayV2_1DMoveConstruction) {
    console::_V2::MultiArray<int, 5> arr1{1, 2, 3, 4, 5};
    console::_V2::MultiArray<int, 5> arr2(std::move(arr1));
    ASSERT_EQ(1, arr2[0]);
    ASSERT_EQ(5, arr2[4]);
}

/**
 * @brief 测试 MultiArray 一维拷贝赋值。
 * @note 期望：深拷贝，独立拥有者。
 */
TEST(MultiArrayV2_1DCopyAssignment) {
    console::_V2::MultiArray<int, 5> arr1{1, 2, 3, 4, 5};
    console::_V2::MultiArray<int, 5> arr2{10, 20, 30, 40, 50};
    arr2    = arr1;
    arr1[0] = 100;
    ASSERT_EQ(1, arr2[0]);
    ASSERT_EQ(100, arr1[0]);
}

/**
 * @brief 测试 MultiArray 一维移动赋值。
 * @note 期望：移动后目标拥有数据，源变为视图。
 */
TEST(MultiArrayV2_1DMoveAssignment) {
    console::_V2::MultiArray<int, 5> arr1{1, 2, 3, 4, 5};
    console::_V2::MultiArray<int, 5> arr2{10, 20, 30, 40, 50};
    arr2 = std::move(arr1);
    ASSERT_EQ(1, arr2[0]);
    ASSERT_EQ(5, arr2[4]);
}

/**
 * @brief 测试 MultiArray 一维 at 边界检查。
 * @note 期望：有效索引返回引用，越界抛出 MultiArrayError。
 */
TEST(MultiArrayV2_1DAt) {
    console::_V2::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    ASSERT_EQ(3, arr.at(2));
    arr.at(2) = 30;
    ASSERT_EQ(30, arr.at(2));
    ASSERT_THROWS(arr.at(5), console::MultiArrayError);
    const auto &carr = arr;
    ASSERT_EQ(30, carr.at(2));
    ASSERT_THROWS(carr.at(5), console::MultiArrayError);
}

/**
 * @brief 测试 MultiArray 一维 operator() 边界检查。
 * @note 期望：有效索引返回引用，越界抛出 MultiArrayError。
 */
TEST(MultiArrayV2_1DOperatorParens) {
    console::_V2::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    ASSERT_EQ(4, arr(3));
    arr(3) = 40;
    ASSERT_EQ(40, arr(3));
    ASSERT_THROWS(arr(5), console::MultiArrayError);
    const auto &carr = arr;
    ASSERT_EQ(40, carr(3));
    ASSERT_THROWS(carr(5), console::MultiArrayError);
}

/**
 * @brief 测试 MultiArray 一维 front 和 back。
 * @note 期望：返回首尾元素引用。
 */
TEST(MultiArrayV2_1DFrontBack) {
    console::_V2::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    ASSERT_EQ(1, arr.front());
    ASSERT_EQ(5, arr.back());
    arr.front() = 10;
    arr.back()  = 50;
    ASSERT_EQ(10, arr.front());
    ASSERT_EQ(50, arr.back());
    const auto &carr = arr;
    ASSERT_EQ(10, carr.front());
    ASSERT_EQ(50, carr.back());
}

/**
 * @brief 测试 MultiArray 一维 fill。
 * @note 期望：所有元素填充为指定值。
 */
TEST(MultiArrayV2_1DFill) {
    console::_V2::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    arr.fill(99);
    for (size_t i = 0; i < 5; ++i) {
        ASSERT_EQ(99, arr[i]);
    }
}

/**
 * @brief 测试 MultiArray 一维 for_each。
 * @note 期望：遍历所有元素并应用函数。
 */
TEST(MultiArrayV2_1DForEach) {
    console::_V2::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    arr.for_each([](int &x) { x *= 2; });
    ASSERT_EQ(2, arr[0]);
    ASSERT_EQ(6, arr[2]);
    ASSERT_EQ(10, arr[4]);
    int sum = 0;
    arr.for_each([&sum](const int &x) { sum += x; });
    ASSERT_EQ(30, sum);
}

/**
 * @brief 测试 MultiArray 一维迭代器。
 * @note 期望：迭代器正确遍历元素。
 */
TEST(MultiArrayV2_1DIterator) {
    console::_V2::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    int                              sum = 0;
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        sum += *it;
    }
    ASSERT_EQ(15, sum);
    sum = 0;
    for (int x : arr) {
        sum += x;
    }
    ASSERT_EQ(15, sum);
}

/**
 * @brief 测试 MultiArray 一维反向迭代器。
 * @note 期望：反向迭代器正确遍历元素。
 */
TEST(MultiArrayV2_1DReverseIterator) {
    console::_V2::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    std::vector<int>                 result;
    for (auto it = arr.rbegin(); it != arr.rend(); ++it) {
        result.push_back(*it);
    }
    std::vector<int> expected = {5, 4, 3, 2, 1};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 MultiArray 一维流输出。
 * @note 期望：输出格式为 [1, 2, 3, 4, 5]。
 */
TEST(MultiArrayV2_1DOutput) {
    console::_V2::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    std::ostringstream               oss;
    oss << arr;
    ASSERT_EQ(std::string("[1, 2, 3, 4, 5]"), oss.str());
}

/**
 * @brief 测试 MultiArray 一维 swap。
 * @note 期望：两个数组内容交换。
 */
TEST(MultiArrayV2_1DSwap) {
    console::_V2::MultiArray<int, 5> arr1{1, 2, 3, 4, 5};
    console::_V2::MultiArray<int, 5> arr2{10, 20, 30, 40, 50};
    arr1.swap(arr2);
    ASSERT_EQ(10, arr1[0]);
    ASSERT_EQ(50, arr1[4]);
    ASSERT_EQ(1, arr2[0]);
    ASSERT_EQ(5, arr2[4]);
}

/**
 * @brief 测试 MultiArray 二维构造和访问。
 * @note 期望：二维数组正确构造，元素可访问。
 */
TEST(MultiArrayV2_2DConstruction) {
    console::_V2::MultiArray<int, 2, 3> arr;
    arr(0, 0) = 1;
    arr(0, 1) = 2;
    arr(0, 2) = 3;
    arr(1, 0) = 4;
    arr(1, 1) = 5;
    arr(1, 2) = 6;
    ASSERT_EQ(1, arr(0, 0));
    ASSERT_EQ(3, arr(0, 2));
    ASSERT_EQ(4, arr(1, 0));
    ASSERT_EQ(6, arr(1, 2));
}

/**
 * @brief 测试 MultiArray 二维填充构造。
 * @note 期望：所有元素填充为指定值。
 */
TEST(MultiArrayV2_2DFillConstruction) {
    console::_V2::MultiArray<int, 2, 3> arr(42);
    for (size_t i = 0; i < 2; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            ASSERT_EQ(42, arr(i, j));
        }
    }
}

/**
 * @brief 测试 MultiArray 二维初始化列表构造。
 * @note 期望：元素按嵌套初始化列表顺序赋值。
 */
TEST(MultiArrayV2_2DInitializerList) {
    console::_V2::MultiArray<int, 2, 3> arr{{1, 2, 3}, {4, 5, 6}};
    ASSERT_EQ(1, arr(0, 0));
    ASSERT_EQ(3, arr(0, 2));
    ASSERT_EQ(4, arr(1, 0));
    ASSERT_EQ(6, arr(1, 2));
}

/**
 * @brief 测试 MultiArray 二维子数组视图。
 * @note 期望：operator[] 返回子数组视图，修改视图影响原数组。
 */
TEST(MultiArrayV2_2DSubview) {
    console::_V2::MultiArray<int, 2, 3> arr{{1, 2, 3}, {4, 5, 6}};
    auto                                sub = arr[0];
    ASSERT_EQ(1, sub[0]);
    ASSERT_EQ(3, sub[2]);
    sub[1] = 20;
    ASSERT_EQ(20, arr(0, 1));
    sub.fill(99);
    ASSERT_EQ(99, arr(0, 0));
    ASSERT_EQ(99, arr(0, 2));
    ASSERT_EQ(4, arr(1, 0));
}

/**
 * @brief 测试 MultiArray 二维多维下标访问。
 * @note 期望：operator() 支持多维索引。
 */
TEST(MultiArrayV2_2DMultiIndex) {
    console::_V2::MultiArray<int, 2, 3> arr{{1, 2, 3}, {4, 5, 6}};
    ASSERT_EQ(2, arr(0, 1));
    ASSERT_EQ(5, arr(1, 1));
    arr(0, 2) = 30;
    ASSERT_EQ(30, arr(0, 2));
    ASSERT_THROWS(arr(2, 0), console::MultiArrayError);
    ASSERT_THROWS(arr(0, 3), console::MultiArrayError);
    const auto &carr = arr;
    ASSERT_EQ(30, carr(0, 2));
    ASSERT_THROWS(carr(2, 0), console::MultiArrayError);
}

/**
 * @brief 测试 MultiArray 二维 at 边界检查。
 * @note 期望：有效索引返回引用，越界抛出 MultiArrayError。
 */
TEST(MultiArrayV2_2DAt) {
    console::_V2::MultiArray<int, 2, 3> arr{{1, 2, 3}, {4, 5, 6}};
    auto                                sub = arr.at(0);
    ASSERT_EQ(1, sub[0]);
    ASSERT_EQ(3, sub[2]);
    ASSERT_THROWS(arr.at(2), console::MultiArrayError);
    const auto &carr = arr;
    auto        csub = carr.at(1);
    ASSERT_EQ(4, csub[0]);
    ASSERT_EQ(6, csub[2]);
    ASSERT_THROWS(carr.at(2), console::MultiArrayError);
}

/**
 * @brief 测试 MultiArray 二维迭代器。
 * @note 期望：迭代器遍历子数组视图。
 */
TEST(MultiArrayV2_2DIterator) {
    console::_V2::MultiArray<int, 2, 3> arr{{1, 2, 3}, {4, 5, 6}};
    int                                 sum = 0;
    for (auto it = arr.begin(); it != arr.end(); ++it) {
        for (int x : *it) {
            sum += x;
        }
    }
    ASSERT_EQ(21, sum);
}

/**
 * @brief 测试 MultiArray 二维流输出。
 * @note 期望：输出格式为 [[1, 2, 3], [4, 5, 6]]。
 */
TEST(MultiArrayV2_2DOutput) {
    console::_V2::MultiArray<int, 2, 3> arr{{1, 2, 3}, {4, 5, 6}};
    std::ostringstream                  oss;
    oss << arr;
    ASSERT_EQ(std::string("[[1, 2, 3], [4, 5, 6]]"), oss.str());
}

/**
 * @brief 测试 MultiArray 三维构造和访问。
 * @note 期望：三维数组正确构造，元素可访问。
 */
TEST(MultiArrayV2_3DConstruction) {
    console::_V2::MultiArray<int, 2, 2, 2> arr;
    arr(0, 0, 0) = 1;
    arr(0, 0, 1) = 2;
    arr(0, 1, 0) = 3;
    arr(0, 1, 1) = 4;
    arr(1, 0, 0) = 5;
    arr(1, 0, 1) = 6;
    arr(1, 1, 0) = 7;
    arr(1, 1, 1) = 8;
    ASSERT_EQ(1, arr(0, 0, 0));
    ASSERT_EQ(4, arr(0, 1, 1));
    ASSERT_EQ(5, arr(1, 0, 0));
    ASSERT_EQ(8, arr(1, 1, 1));
}

/**
 * @brief 测试 MultiArray 三维初始化列表构造。
 * @note 期望：元素按嵌套初始化列表顺序赋值。
 */
TEST(MultiArrayV2_3DInitializerList) {
    console::_V2::MultiArray<int, 2, 2, 2> arr{
        {{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}};
    ASSERT_EQ(1, arr(0, 0, 0));
    ASSERT_EQ(4, arr(0, 1, 1));
    ASSERT_EQ(5, arr(1, 0, 0));
    ASSERT_EQ(8, arr(1, 1, 1));
}

/**
 * @brief 测试 MultiArray 三维流输出。
 * @note 期望：输出格式为 [[[1, 2], [3, 4]], [[5, 6], [7, 8]]]。
 */
TEST(MultiArrayV2_3DOutput) {
    console::_V2::MultiArray<int, 2, 2, 2> arr{
        {{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}};
    std::ostringstream oss;
    oss << arr;
    ASSERT_EQ(std::string("[[[1, 2], [3, 4]], [[5, 6], [7, 8]]]"), oss.str());
}

/**
 * @brief 测试 MultiArray flatten 一维视图。
 * @note 期望：flatten 返回一维视图，修改视图影响原数组。
 */
TEST(MultiArrayV2_Flatten) {
    console::_V2::MultiArray<int, 2, 3> arr{{1, 2, 3}, {4, 5, 6}};
    auto                               &flat = arr.flatten();
    ASSERT_EQ(6, flat.fsize());
    ASSERT_EQ(1, flat[0]);
    ASSERT_EQ(6, flat[5]);
    flat[0] = 10;
    flat[5] = 60;
    ASSERT_EQ(10, arr(0, 0));
    ASSERT_EQ(60, arr(1, 2));
}

/**
 * @brief 测试 MultiArray fbegin/fend 扁平迭代器。
 * @note 期望：fbegin/fend 正确遍历所有元素。
 */
TEST(MultiArrayV2_FBeginFEnd) {
    console::_V2::MultiArray<int, 2, 3> arr{{1, 2, 3}, {4, 5, 6}};
    int                                 sum = 0;
    for (auto it = arr.fbegin(); it != arr.fend(); ++it) {
        sum += *it;
    }
    ASSERT_EQ(21, sum);
    const auto &carr = arr;
    sum              = 0;
    for (auto it = carr.fbegin(); it != carr.fend(); ++it) {
        sum += *it;
    }
    ASSERT_EQ(21, sum);
}

/**
 * @brief 测试 MultiArray 算术运算 (数组 + 数组)。
 * @note 期望：逐元素相加。
 */
TEST(MultiArrayV2_ArrayPlusArray) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    console::_V2::MultiArray<int, 3> b{10, 20, 30};
    auto                             c = a + b;
    ASSERT_EQ(11, c[0]);
    ASSERT_EQ(22, c[1]);
    ASSERT_EQ(33, c[2]);
}

/**
 * @brief 测试 MultiArray 算术运算 (数组 + 标量)。
 * @note 期望：逐元素加标量。
 */
TEST(MultiArrayV2_ArrayPlusScalar) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    auto                             c = a + 10;
    ASSERT_EQ(11, c[0]);
    ASSERT_EQ(12, c[1]);
    ASSERT_EQ(13, c[2]);
}

/**
 * @brief 测试 MultiArray 算术运算 (标量 + 数组)。
 * @note 期望：标量逐元素加数组。
 */
TEST(MultiArrayV2_ScalarPlusArray) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    auto                             c = 10 + a;
    ASSERT_EQ(11, c[0]);
    ASSERT_EQ(12, c[1]);
    ASSERT_EQ(13, c[2]);
}

/**
 * @brief 测试 MultiArray 算术运算 (数组 - 数组)。
 * @note 期望：逐元素相减。
 */
TEST(MultiArrayV2_ArrayMinusArray) {
    console::_V2::MultiArray<int, 3> a{10, 20, 30};
    console::_V2::MultiArray<int, 3> b{1, 2, 3};
    auto                             c = a - b;
    ASSERT_EQ(9, c[0]);
    ASSERT_EQ(18, c[1]);
    ASSERT_EQ(27, c[2]);
}

/**
 * @brief 测试 MultiArray 算术运算 (数组 * 数组)。
 * @note 期望：逐元素相乘。
 */
TEST(MultiArrayV2_ArrayTimesArray) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    console::_V2::MultiArray<int, 3> b{10, 20, 30};
    auto                             c = a * b;
    ASSERT_EQ(10, c[0]);
    ASSERT_EQ(40, c[1]);
    ASSERT_EQ(90, c[2]);
}

/**
 * @brief 测试 MultiArray 算术运算 (数组 / 数组)。
 * @note 期望：逐元素相除。
 */
TEST(MultiArrayV2_ArrayDivArray) {
    console::_V2::MultiArray<int, 3> a{10, 20, 30};
    console::_V2::MultiArray<int, 3> b{2, 4, 5};
    auto                             c = a / b;
    ASSERT_EQ(5, c[0]);
    ASSERT_EQ(5, c[1]);
    ASSERT_EQ(6, c[2]);
}

/**
 * @brief 测试 MultiArray 复合赋值 (+=)。
 * @note 期望：逐元素相加并赋值。
 */
TEST(MultiArrayV2_ArrayPlusEqual) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    console::_V2::MultiArray<int, 3> b{10, 20, 30};
    a += b;
    ASSERT_EQ(11, a[0]);
    ASSERT_EQ(22, a[1]);
    ASSERT_EQ(33, a[2]);
}

/**
 * @brief 测试 MultiArray 复合赋值 (标量 +=)。
 * @note 期望：逐元素加标量并赋值。
 */
TEST(MultiArrayV2_ArrayPlusEqualScalar) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    a += 10;
    ASSERT_EQ(11, a[0]);
    ASSERT_EQ(12, a[1]);
    ASSERT_EQ(13, a[2]);
}

/**
 * @brief 测试 MultiArray 比较运算 (==)。
 * @note 期望：逐元素比较，返回 bool 数组。
 */
TEST(MultiArrayV2_ArrayEqual) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    console::_V2::MultiArray<int, 3> b{1, 20, 3};
    auto                             c = (a == b);
    ASSERT_TRUE(c[0]);
    ASSERT_FALSE(c[1]);
    ASSERT_TRUE(c[2]);
}

/**
 * @brief 测试 MultiArray 比较运算 (数组 < 标量)。
 * @note 期望：逐元素比较，返回 bool 数组。
 */
TEST(MultiArrayV2_ArrayLessScalar) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    auto                             c = (a < 3);
    ASSERT_TRUE(c[0]);
    ASSERT_TRUE(c[1]);
    ASSERT_FALSE(c[2]);
}

/**
 * @brief 测试 MultiArray 一元负运算。
 * @note 期望：逐元素取负。
 */
TEST(MultiArrayV2_UnaryMinus) {
    console::_V2::MultiArray<int, 3> a{1, -2, 3};
    auto                             c = -a;
    ASSERT_EQ(-1, c[0]);
    ASSERT_EQ(2, c[1]);
    ASSERT_EQ(-3, c[2]);
}

/**
 * @brief 测试 MultiArray 逻辑非运算。
 * @note 期望：逐元素逻辑非，返回 bool 数组。
 */
TEST(MultiArrayV2_LogicalNot) {
    console::_V2::MultiArray<int, 3> a{1, 0, 3};
    auto                             c = !a;
    ASSERT_FALSE(c[0]);
    ASSERT_TRUE(c[1]);
    ASSERT_FALSE(c[2]);
}

/**
 * @brief 测试 MultiArray 位运算 (&)。
 * @note 期望：逐元素按位与。
 */
TEST(MultiArrayV2_BitwiseAnd) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    console::_V2::MultiArray<int, 3> b{1, 3, 5};
    auto                             c = a & b;
    ASSERT_EQ(1, c[0]);
    ASSERT_EQ(2, c[1]);
    ASSERT_EQ(1, c[2]);
}

/**
 * @brief 测试 MultiArray 位运算 (|)。
 * @note 期望：逐元素按位或。
 */
TEST(MultiArrayV2_BitwiseOr) {
    console::_V2::MultiArray<int, 3> a{1, 2, 4};
    console::_V2::MultiArray<int, 3> b{1, 3, 5};
    auto                             c = a | b;
    ASSERT_EQ(1, c[0]);
    ASSERT_EQ(3, c[1]);
    ASSERT_EQ(5, c[2]);
}

/**
 * @brief 测试 MultiArray sum 函数。
 * @note 期望：返回所有元素之和。
 */
TEST(MultiArrayV2_Sum) {
    console::_V2::MultiArray<int, 2, 3> arr{{1, 2, 3}, {4, 5, 6}};
    int                                 s = console::sum(arr);
    ASSERT_EQ(21, s);
}

/**
 * @brief 测试 MultiArray min 函数。
 * @note 期望：返回所有元素最小值。
 */
TEST(MultiArrayV2_Min) {
    console::_V2::MultiArray<int, 2, 3> arr{{5, 2, 8}, {1, 9, 3}};
    int                                 m = console::min(arr);
    ASSERT_EQ(1, m);
}

/**
 * @brief 测试 MultiArray max 函数。
 * @note 期望：返回所有元素最大值。
 */
TEST(MultiArrayV2_Max) {
    console::_V2::MultiArray<int, 2, 3> arr{{5, 2, 8}, {1, 9, 3}};
    int                                 m = console::max(arr);
    ASSERT_EQ(9, m);
}

/**
 * @brief 测试 MultiArray all 函数。
 * @note 期望：所有元素为真时返回 true。
 */
TEST(MultiArrayV2_All) {
    console::_V2::MultiArray<bool, 3> a{true, true, true};
    ASSERT_TRUE(console::all(a));
    console::_V2::MultiArray<bool, 3> b{true, false, true};
    ASSERT_FALSE(console::all(b));
}

/**
 * @brief 测试 MultiArray any 函数。
 * @note 期望：任一元素为真时返回 true。
 */
TEST(MultiArrayV2_Any) {
    console::_V2::MultiArray<bool, 3> a{false, false, false};
    ASSERT_FALSE(console::any(a));
    console::_V2::MultiArray<bool, 3> b{false, true, false};
    ASSERT_TRUE(console::any(b));
}

/**
 * @brief 测试 MultiArray equals 函数。
 * @note 期望：相同元素返回 true，不同返回 false。
 */
TEST(MultiArrayV2_Equals) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    console::_V2::MultiArray<int, 3> b{1, 2, 3};
    console::_V2::MultiArray<int, 3> c{1, 2, 4};
    ASSERT_TRUE(console::equals(a, b));
    ASSERT_FALSE(console::equals(a, c));
}

/**
 * @brief 测试 MultiArray equals 自定义比较器。
 * @note 期望：使用自定义比较器进行比较。
 */
TEST(MultiArrayV2_EqualsWithOp) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    console::_V2::MultiArray<int, 3> b{2, 4, 6};
    bool                             result
        = console::equals(a, b, [](int x, int y) { return x * 2 == y; });
    ASSERT_TRUE(result);
}

/**
 * @brief 测试 MultiArray equals 维度不匹配。
 * @note 期望：维度不匹配时返回 false。
 */
TEST(MultiArrayV2_EqualsDimensionMismatch) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    console::_V2::MultiArray<int, 4> b{1, 2, 3, 4};
    ASSERT_FALSE(console::equals(a, b));
}

/**
 * @brief 测试 MultiArray compare 函数。
 * @note 期望：字典序比较。
 */
TEST(MultiArrayV2_Compare) {
    console::_V2::MultiArray<int, 3> a{1, 2, 3};
    console::_V2::MultiArray<int, 3> b{1, 2, 4};
    console::_V2::MultiArray<int, 3> c{1, 2, 2};
    ASSERT_EQ(-1, console::compare(a, b));
    ASSERT_EQ(1, console::compare(a, c));
    ASSERT_EQ(0, console::compare(a, a));
}

/**
 * @brief 测试 MultiArray reshape。
 * @note 期望：重新调整形状，总元素数不变。
 */
TEST(MultiArrayV2_Reshape) {
    console::_V2::MultiArray<int, 2, 3> arr{{1, 2, 3}, {4, 5, 6}};
    auto                               &reshaped = arr.reshape<3, 2>();
    ASSERT_EQ(6, reshaped.fsize());
    ASSERT_EQ(1, reshaped(0, 0));
    ASSERT_EQ(2, reshaped(0, 1));
    ASSERT_EQ(3, reshaped(1, 0));
    ASSERT_EQ(4, reshaped(1, 1));
    ASSERT_EQ(5, reshaped(2, 0));
    ASSERT_EQ(6, reshaped(2, 1));
}

/**
 * @brief 测试 MultiArray clone 深拷贝。
 * @note 期望：clone 返回独立拥有者。
 */
TEST(MultiArrayV2_Clone) {
    console::_V2::MultiArray<int, 3> arr{1, 2, 3};
    auto                             cloned = arr.clone();
    arr[0]                                  = 100;
    ASSERT_EQ(1, cloned[0]);
    ASSERT_EQ(100, arr[0]);
}

#ifndef NOMAIN
TEST_MAIN
#endif
