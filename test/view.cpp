/**
 * @file view.cpp
 * @brief 测试容器视图模块 (View, make_view)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/view.h"

#include <sstream>
#include <string>
#include <vector>

#include "../include/test.h"

/**
 * @brief 测试 View 整个容器。
 * @note 期望：视图包含容器所有元素。
 */
TEST(ViewWholeContainer) {
    std::vector<int> v    = {1, 2, 3, 4, 5};
    auto             view = console::View<std::vector<int>>(v);
    ASSERT_EQ(5, view.size());
    int sum = 0;
    for (int x : view) {
        sum += x;
    }
    ASSERT_EQ(15, sum);
}

/**
 * @brief 测试 View 容器子区间。
 * @note 期望：视图只包含指定区间元素。
 */
TEST(ViewSubrange) {
    std::vector<int> v    = {1, 2, 3, 4, 5, 6, 7, 8};
    auto             view = console::View<std::vector<int>>(v, 2, 6);
    ASSERT_EQ(4, view.size());
    std::vector<int> result;
    for (int x : view) {
        result.push_back(x);
    }
    std::vector<int> expected = {3, 4, 5, 6};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 View 从迭代器对构造。
 * @note 期望：视图包含迭代器对区间元素。
 */
TEST(ViewFromIterators) {
    std::vector<int> v     = {10, 20, 30, 40, 50};
    auto             begin = v.begin() + 1;
    auto             end   = v.begin() + 4;
    auto             view  = console::View<std::vector<int>>(begin, end);
    ASSERT_EQ(3, view.size());
    std::vector<int> result;
    for (int x : view) {
        result.push_back(x);
    }
    std::vector<int> expected = {20, 30, 40};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 View 常量容器。
 * @note 期望：常量视图只读，包含所有元素。
 */
TEST(ViewConstContainer) {
    const std::vector<int> v    = {1, 2, 3, 4};
    auto                   view = console::View<const std::vector<int>>(v);
    ASSERT_EQ(4, view.size());
    int sum = 0;
    for (int x : view) {
        sum += x;
    }
    ASSERT_EQ(10, sum);
}

/**
 * @brief 测试 View const 子区间。
 * @note 期望：常量视图包含指定区间元素。
 */
TEST(ViewConstSubrange) {
    const std::vector<int> v = {1, 2, 3, 4, 5, 6};
    auto view                = console::View<const std::vector<int>>(v, 1, 4);
    ASSERT_EQ(3, view.size());
    std::vector<int> result;
    for (int x : view) {
        result.push_back(x);
    }
    std::vector<int> expected = {2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 View 下标访问。
 * @note 期望：正确返回指定索引元素。
 */
TEST(ViewSubscript) {
    std::vector<int> v    = {10, 20, 30, 40, 50};
    auto             view = console::View<std::vector<int>>(v, 1, 4);
    ASSERT_EQ(20, view[0]);
    ASSERT_EQ(30, view[1]);
    ASSERT_EQ(40, view[2]);
}

/**
 * @brief 测试 View at 边界检查成功。
 * @note 期望：有效索引返回元素。
 */
TEST(ViewAtSuccess) {
    std::vector<int> v    = {10, 20, 30, 40};
    auto             view = console::View<std::vector<int>>(v, 1, 3);
    ASSERT_EQ(20, view.at(0));
    ASSERT_EQ(30, view.at(1));
}

/**
 * @brief 测试 View at 边界检查失败。
 * @note 期望：越界抛出 IndexError。
 */
TEST(ViewAtFailure) {
    std::vector<int> v    = {10, 20, 30};
    auto             view = console::View<std::vector<int>>(v);
    ASSERT_THROWS(view.at(3), console::IndexError);
    ASSERT_THROWS(view.at(100), console::IndexError);
}

/**
 * @brief 测试 View collect 复制数据。
 * @note 期望：collect 返回包含视图元素的新容器。
 */
TEST(ViewCollect) {
    std::vector<int> v         = {1, 2, 3, 4, 5};
    auto             view      = console::View<std::vector<int>>(v, 1, 4);
    auto             collected = view.collect();
    std::vector<int> expected  = {2, 3, 4};
    ASSERT_EQ(expected, collected);
    // 修改原容器不影响已收集的数据
    v[2] = 100;
    ASSERT_EQ(2, collected[0]);
    ASSERT_EQ(3, collected[1]);
    ASSERT_EQ(4, collected[2]);
}

/**
 * @brief 测试 View 原生数组 T[]。
 * @note 期望：正确包装原生数组。
 */
TEST(ViewNativeArray) {
    int  arr[] = {10, 20, 30, 40, 50};
    auto view  = console::View<int[]>(arr, arr + 5);
    ASSERT_EQ(5, view.size());
    int sum = 0;
    for (int x : view) {
        sum += x;
    }
    ASSERT_EQ(150, sum);
    ASSERT_EQ(10, view[0]);
    ASSERT_EQ(50, view[4]);
}

/**
 * @brief 测试 View 原生数组从指针和长度构造。
 * @note 期望：正确包装原生数组。
 */
TEST(ViewNativeArrayWithCount) {
    int  arr[] = {1, 2, 3, 4};
    auto view  = console::View<int[]>(arr, 4);
    ASSERT_EQ(4, view.size());
    std::vector<int> result;
    for (int x : view) {
        result.push_back(x);
    }
    std::vector<int> expected = {1, 2, 3, 4};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 View const T[]。
 * @note 期望：正确包装常量原生数组。
 */
TEST(ViewConstNativeArray) {
    const int arr[] = {10, 20, 30, 40};
    auto      view  = console::View<const int[]>(arr, arr + 4);
    ASSERT_EQ(4, view.size());
    ASSERT_EQ(10, view[0]);
    ASSERT_EQ(40, view[3]);
    int sum = 0;
    for (int x : view) {
        sum += x;
    }
    ASSERT_EQ(100, sum);
}

/**
 * @brief 测试 View const T[] 从指针和长度构造。
 * @note 期望：正确包装常量原生数组。
 */
TEST(ViewConstNativeArrayWithCount) {
    const int arr[] = {5, 6, 7, 8};
    auto      view  = console::View<const int[]>(arr, 3);
    ASSERT_EQ(3, view.size());
    ASSERT_EQ(5, view[0]);
    ASSERT_EQ(6, view[1]);
    ASSERT_EQ(7, view[2]);
}

/**
 * @brief 测试 View 原生数组 at 边界检查。
 * @note 期望：有效索引返回元素，越界抛出 IndexError。
 */
TEST(ViewNativeArrayAt) {
    int  arr[] = {1, 2, 3};
    auto view  = console::View<int[]>(arr, arr + 3);
    ASSERT_EQ(1, view.at(0));
    ASSERT_EQ(3, view.at(2));
    ASSERT_THROWS(view.at(3), console::IndexError);
}

/**
 * @brief 测试 make_view 工厂函数（容器可变）。
 * @note 期望：创建容器视图。
 */
TEST(MakeViewContainer) {
    std::vector<int> v    = {1, 2, 3, 4};
    auto             view = console::make_view(v);
    ASSERT_EQ(4, view.size());
    ASSERT_EQ(1, view[0]);
    ASSERT_EQ(4, view[3]);
}

/**
 * @brief 测试 make_view 工厂函数（容器常量）。
 * @note 期望：创建常量容器视图。
 */
TEST(MakeViewConstContainer) {
    const std::vector<int> v    = {10, 20, 30};
    auto                   view = console::make_view(v);
    ASSERT_EQ(3, view.size());
    ASSERT_EQ(10, view[0]);
    ASSERT_EQ(30, view[2]);
}

/**
 * @brief 测试 make_view 工厂函数（容器子区间可变）。
 * @note 期望：创建子区间视图。
 */
TEST(MakeViewSubrange) {
    std::vector<int> v    = {1, 2, 3, 4, 5, 6};
    auto             view = console::make_view(v, 2, 5);
    ASSERT_EQ(3, view.size());
    std::vector<int> result;
    for (int x : view) {
        result.push_back(x);
    }
    std::vector<int> expected = {3, 4, 5};
    ASSERT_EQ(expected, result);
}

/**
 * @brief 测试 make_view 工厂函数（容器子区间常量）。
 * @note 期望：创建常量子区间视图。
 */
TEST(MakeViewConstSubrange) {
    const std::vector<int> v    = {10, 20, 30, 40, 50};
    auto                   view = console::make_view(v, 1, 4);
    ASSERT_EQ(3, view.size());
    ASSERT_EQ(20, view[0]);
    ASSERT_EQ(40, view[2]);
}

/**
 * @brief 测试 make_view 工厂函数（迭代器对可变）。
 * @note 期望：从迭代器对创建视图。
 */
TEST(MakeViewIterators) {
    std::vector<int> v = {100, 200, 300, 400, 500};
    auto             view
        = console::make_view<std::vector<int>>(v.begin() + 1, v.begin() + 4);
    ASSERT_EQ(3, view.size());
    ASSERT_EQ(200, view[0]);
    ASSERT_EQ(300, view[1]);
    ASSERT_EQ(400, view[2]);
}

/**
 * @brief 测试 make_view 工厂函数（迭代器对常量）。
 * @note 期望：从常量迭代器对创建视图。
 */
TEST(MakeViewConstIterators) {
    const std::vector<int> v = {1, 2, 3, 4, 5};
    auto                   view
        = console::make_view<std::vector<int>>(v.cbegin() + 1, v.cbegin() + 4);
    ASSERT_EQ(3, view.size());
    ASSERT_EQ(2, view[0]);
    ASSERT_EQ(3, view[1]);
    ASSERT_EQ(4, view[2]);
}

/**
 * @brief 测试 make_view 工厂函数（指针对可变）。
 * @note 期望：从指针对创建视图。
 */
TEST(MakeViewPointers) {
    int  arr[] = {10, 20, 30, 40};
    auto view  = console::make_view(arr, arr + 4);
    ASSERT_EQ(4, view.size());
    ASSERT_EQ(10, view[0]);
    ASSERT_EQ(40, view[3]);
}

/**
 * @brief 测试 make_view 工厂函数（指针对常量）。
 * @note 期望：从常量指针对创建视图。
 */
TEST(MakeViewConstPointers) {
    const int arr[] = {5, 6, 7, 8};
    auto      view  = console::make_view(arr, arr + 4);
    ASSERT_EQ(4, view.size());
    ASSERT_EQ(5, view[0]);
    ASSERT_EQ(8, view[3]);
}

/**
 * @brief 测试 make_view 工厂函数（指针和长度可变）。
 * @note 期望：从指针和长度创建视图。
 */
TEST(MakeViewPointerCount) {
    int  arr[] = {1, 2, 3, 4, 5};
    auto view  = console::make_view(arr, 5);
    ASSERT_EQ(5, view.size());
    ASSERT_EQ(1, view[0]);
    ASSERT_EQ(5, view[4]);
}

/**
 * @brief 测试 make_view 工厂函数（指针和长度常量）。
 * @note 期望：从常量指针和长度创建视图。
 */
TEST(MakeViewConstPointerCount) {
    const int arr[] = {100, 200, 300};
    auto      view  = console::make_view(arr, 3);
    ASSERT_EQ(3, view.size());
    ASSERT_EQ(100, view[0]);
    ASSERT_EQ(300, view[2]);
}

/**
 * @brief 测试 View 流输出。
 * @note 期望：输出容器格式。
 */
TEST(ViewOutputStream) {
    std::vector<int>   v    = {1, 2, 3, 4, 5};
    auto               view = console::View<std::vector<int>>(v, 1, 4);
    std::ostringstream oss;
    oss << view;
    ASSERT_EQ(std::string("[2, 3, 4]"), oss.str());
}

/**
 * @brief 测试 View 空视图。
 * @note 期望：空视图 size 为 0，遍历不执行。
 */
TEST(ViewEmpty) {
    std::vector<int> v    = {1, 2, 3};
    auto             view = console::View<std::vector<int>>(v, 0, 0);
    ASSERT_EQ(0, view.size());
    int count = 0;
    for (int x : view) {
        (void)x;
        ++count;
    }
    ASSERT_EQ(0, count);
    ASSERT_THROWS(view.at(0), console::IndexError);
}

/**
 * @brief 测试 View 修改底层容器影响视图。
 * @note 期望：视图反映底层容器的变化。
 */
TEST(ViewReflectsContainerChanges) {
    std::vector<int> v    = {1, 2, 3, 4, 5};
    auto             view = console::View<std::vector<int>>(v, 1, 4);
    ASSERT_EQ(2, view[0]);
    v[1] = 100;
    ASSERT_EQ(100, view[0]);
    v[2] = 200;
    ASSERT_EQ(200, view[1]);
}

/**
 * @brief 测试 View 字符串视图。
 * @note 期望：字符串视图正确输出。
 */
TEST(ViewString) {
    std::string        s    = "hello world";
    auto               view = console::View<std::string>(s, 0, 5);
    std::ostringstream oss;
    oss << view;
    ASSERT_EQ(std::string("hello"), oss.str());
    auto collected = view.collect();
    ASSERT_EQ(std::string("hello"), collected);
}

#ifndef NOMAIN
TEST_MAIN
#endif
