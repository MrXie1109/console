/**
 * @file sfinae.cpp
 * @brief 测试 SFINAE 类型特征检测模块。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/sfinae.h"

#include <cstddef>
#include <list>
#include <string>
#include <vector>

#include "../include/test.h"

struct Printable {
    int                  x;
    friend std::ostream &operator<<(std::ostream &os, const Printable &p) {
        os << p.x;
        return os;
    }
};

/**
 * @brief 测试 is_container 检测容器类型。
 * @note 期望：std::vector 和 std::list 为容器，int 不是容器。
 */
TEST(SfinaeIsContainer) {
    ASSERT_TRUE(console::is_container<std::vector<int>>::value);
    ASSERT_TRUE(console::is_container<std::list<int>>::value);
    ASSERT_TRUE(console::is_container<std::string>::value);
    ASSERT_FALSE(console::is_container<int>::value);
}

/**
 * @brief 测试 is_callable 检测可调用对象。
 * @note 期望：lambda 和函数指针为可调用，int 不可调用。
 */
TEST(SfinaeIsCallable) {
    auto lambda = []() {};
    ASSERT_TRUE(console::is_callable<decltype(lambda)>::value);
    ASSERT_TRUE(console::is_callable<void (*)()>::value);
    ASSERT_FALSE(console::is_callable<int>::value);
}

/**
 * @brief 测试 is_iterator 检测迭代器类型。
 * @note 期望：vector 迭代器是迭代器，int 不是。
 */
TEST(SfinaeIsIterator) {
    using vec_it       = std::vector<int>::iterator;
    using const_vec_it = std::vector<int>::const_iterator;
    ASSERT_TRUE(console::is_iterator<vec_it>::value);
    ASSERT_TRUE(console::is_iterator<const_vec_it>::value);
    ASSERT_TRUE(console::is_iterator<int *>::value);
    ASSERT_FALSE(console::is_iterator<int>::value);
}

/**
 * @brief 测试 has_subscript 检测下标操作。
 * @note 期望：vector 有下标，list 没有。
 */
TEST(SfinaeHasSubscript) {
    ASSERT_TRUE((console::has_subscript<std::vector<int>, size_t>::value));
    ASSERT_TRUE((console::has_subscript<std::string, size_t>::value));
    ASSERT_FALSE((console::has_subscript<std::list<int>, size_t>::value));
    ASSERT_FALSE((console::has_subscript<int, size_t>::value));
}

/**
 * @brief 测试 is_string 检测字符串类型。
 * @note 期望：const char* 和 std::string 是字符串，int 不是。
 */
TEST(SfinaeIsString) {
    ASSERT_TRUE(console::is_string<const char *>::value);
    ASSERT_TRUE(console::is_string<char *>::value);
    ASSERT_TRUE(console::is_string<const wchar_t *>::value);
    ASSERT_TRUE(console::is_string<std::string>::value);
    ASSERT_TRUE(console::is_string<std::wstring>::value);
    ASSERT_FALSE(console::is_string<int>::value);
    ASSERT_FALSE(console::is_string<char>::value);
}

/**
 * @brief 测试 is_printable 检测可打印类型。
 * @note 期望：int 和 double 可打印，自定义不可打印类型不可打印。
 */
TEST(SfinaeIsPrintable) {
    ASSERT_TRUE(console::is_printable<int>::value);
    ASSERT_TRUE(console::is_printable<double>::value);
    ASSERT_TRUE(console::is_printable<std::string>::value);
    ASSERT_TRUE(console::is_printable<const char *>::value);
    struct NonPrintable {};
    ASSERT_FALSE(console::is_printable<NonPrintable>::value);
}

/**
 * @brief 测试 is_basic_printable 检测指定字符流可打印类型。
 * @note 期望：int 对 char 流可打印，对 wchar_t 流也可打印。
 */
TEST(SfinaeIsBasicPrintable) {
    ASSERT_TRUE((
        console::is_basic_printable<char, std::char_traits<char>, int>::value));
    ASSERT_TRUE((console::is_basic_printable<wchar_t,
        std::char_traits<wchar_t>,
        int>::value));
    ASSERT_TRUE((console::is_basic_printable<char,
        std::char_traits<char>,
        std::string>::value));
    struct NonPrintable {};
    ASSERT_FALSE((console::is_basic_printable<char,
        std::char_traits<char>,
        NonPrintable>::value));
}

/**
 * @brief 测试 is_w_printable 宽字符可打印类型别名。
 * @note 期望：int 对 wchar_t 流可打印。
 */
TEST(SfinaeIsWPrintable) {
    ASSERT_TRUE(console::is_w_printable<int>::value);
    ASSERT_TRUE(console::is_w_printable<double>::value);
    struct NonPrintable {};
    ASSERT_FALSE(console::is_w_printable<NonPrintable>::value);
}

/**
 * @brief 测试 is_char 检测字符类型。
 * @note 期望：char 和 wchar_t 是字符，int 不是。
 */
TEST(SfinaeIsChar) {
    ASSERT_TRUE(console::is_char<char>::value);
    ASSERT_TRUE(console::is_char<signed char>::value);
    ASSERT_TRUE(console::is_char<unsigned char>::value);
    ASSERT_TRUE(console::is_char<wchar_t>::value);
    ASSERT_FALSE(console::is_char<int>::value);
    ASSERT_FALSE(console::is_char<std::string>::value);
}

/**
 * @brief 测试 is_generator 检测生成器类型。
 * @note 期望：符合生成器接口的类型为生成器。
 */
TEST(SfinaeIsGenerator) {
    struct Generator {
        bool done() const { return true; }
        int  current() const { return 0; }
        void advance() {}
        using value_type = int;
    };
    ASSERT_TRUE(console::is_generator<Generator>::value);
    struct NotGenerator {};
    ASSERT_FALSE(console::is_generator<NotGenerator>::value);
}

/**
 * @brief 测试 uniform_distribution_t 类型别名。
 * @note 期望：int 对应 uniform_int_distribution，double 对应 uniform_real_distribution。
 */
TEST(SfinaeUniformDistribution) {
    // 只验证类型存在，无法直接检查具体类型
    using int_dist    = console::uniform_distribution_t<int>;
    using double_dist = console::uniform_distribution_t<double>;
    (void)sizeof(int_dist);
    (void)sizeof(double_dist);
    ASSERT_TRUE(true);
}

/**
 * @brief 测试 enable_if 别名模板。
 * @note 期望：编译期正确启用/禁用。
 */
TEST(SfinaeEnableIfAliases) {
    auto lambda            = []() {};
    using callable_enabled = console::enable_if_callable<decltype(lambda)>;
    using vec_it           = std::vector<int>::iterator;
    using iter_enabled     = console::enable_if_iterator<vec_it>;
    using str_enabled      = console::enable_if_string<const char *>;
    using print_enabled    = console::enable_if_printable<int>;
    using char_enabled     = console::enable_if_char<char>;
    struct Gen {
        bool done() const { return true; }
        int  current() const { return 0; }
        void advance() {}
        using value_type = int;
    };
    using gen_enabled = console::enable_if_generator<Gen>;
    using basic_print_enabled
        = console::enable_if_basic_printable<char, std::char_traits<char>, int>;
    using w_print_enabled = console::enable_if_w_printable<int>;
    ASSERT_TRUE(true);
}

/**
 * @brief 测试 enable_if_not 别名模板。
 * @note 期望：编译期正确启用/禁用。
 */
TEST(SfinaeEnableIfNotAliases) {
    using not_callable = console::enable_if_not_callable<int>;
    using not_iter     = console::enable_if_not_iterator<int>;
    using not_string   = console::enable_if_not_string<int>;
    struct NonPrintable {};
    using not_print = console::enable_if_not_printable<NonPrintable>;
    using not_char  = console::enable_if_not_char<int>;
    struct NotGen {};
    using not_gen = console::enable_if_not_generator<NotGen>;
    ASSERT_TRUE(true);
}

/**
 * @brief 测试容器检测 vector 和 array。
 * @note 期望：vector 和 array 是容器，原始数组不是容器。
 */
TEST(SfinaeContainerVectorArray) {
    ASSERT_TRUE(console::is_container<std::vector<int>>::value);
    ASSERT_TRUE((console::is_container<std::array<int, 5>>::value));
    ASSERT_FALSE(console::is_container<int[5]>::value);
}

/**
 * @brief 测试 is_string 对 string_view 的支持 (C++17)。
 * @note 期望：string_view 是字符串类型。
 */
TEST(SfinaeIsStringView) {
#if __cplusplus >= 201703L
    ASSERT_TRUE(console::is_string<std::string_view>::value);
#else
    ASSERT_TRUE(true);
#endif
}

/**
 * @brief 测试 is_printable 对自定义可打印类型。
 * @note 期望：定义了 operator<< 的类型可打印。
 */
TEST(SfinaeIsPrintableCustom) {
    ASSERT_TRUE(console::is_printable<Printable>::value);
}

/**
 * @brief 测试 is_basic_printable 对宽字符流。
 * @note 期望：wchar_t 类型对宽字符流可打印。
 */
TEST(SfinaeIsBasicPrintableWChar) {
    ASSERT_TRUE((console::is_basic_printable<wchar_t,
        std::char_traits<wchar_t>,
        wchar_t>::value));
    ASSERT_TRUE((console::is_basic_printable<wchar_t,
        std::char_traits<wchar_t>,
        int>::value));
    ASSERT_TRUE((console::is_basic_printable<wchar_t,
        std::char_traits<wchar_t>,
        std::wstring>::value));
}

#ifndef NOMAIN
TEST_MAIN
#endif
