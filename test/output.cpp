/**
 * @file output.cpp
 * @brief 测试输出模块 (output, BasicOutput, print, 容器输出)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/output.h"

#include <array>
#include <deque>
#include <forward_list>
#include <list>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <valarray>
#include <vector>

#include "../include/test.h"

using console::operator<<;

/**
 * @brief 测试 vector 输出。
 * @note 期望：输出格式为 [1, 2, 3]。
 */
TEST(OutputVector) {
    std::vector<int>   v = {1, 2, 3};
    std::ostringstream oss;
    oss << v;
    ASSERT_EQ(std::string("[1, 2, 3]"), oss.str());
}

/**
 * @brief 测试空 vector 输出。
 * @note 期望：空容器输出 []。
 */
TEST(OutputEmptyVector) {
    std::vector<int>   v;
    std::ostringstream oss;
    oss << v;
    ASSERT_EQ(std::string("[]"), oss.str());
}

/**
 * @brief 测试 list 输出。
 * @note 期望：输出格式为 [1, 2, 3]。
 */
TEST(OutputList) {
    std::list<int>     l = {1, 2, 3};
    std::ostringstream oss;
    oss << l;
    ASSERT_EQ(std::string("[1, 2, 3]"), oss.str());
}

/**
 * @brief 测试 deque 输出。
 * @note 期望：输出格式为 [1, 2, 3]。
 */
TEST(OutputDeque) {
    std::deque<int>    d = {1, 2, 3};
    std::ostringstream oss;
    oss << d;
    ASSERT_EQ(std::string("[1, 2, 3]"), oss.str());
}

/**
 * @brief 测试 forward_list 输出。
 * @note 期望：输出格式为 [1, 2, 3]。
 */
TEST(OutputForwardList) {
    std::forward_list<int> fl = {1, 2, 3};
    std::ostringstream     oss;
    oss << fl;
    ASSERT_EQ(std::string("[1, 2, 3]"), oss.str());
}

/**
 * @brief 测试 array 输出。
 * @note 期望：输出格式为 [1, 2, 3]。
 */
TEST(OutputArray) {
    std::array<int, 3> a = {1, 2, 3};
    std::ostringstream oss;
    oss << a;
    ASSERT_EQ(std::string("[1, 2, 3]"), oss.str());
}

/**
 * @brief 测试 set 输出。
 * @note 期望：输出格式为 {1, 2, 3}。
 */
TEST(OutputSet) {
    std::set<int>      s = {1, 2, 3};
    std::ostringstream oss;
    oss << s;
    ASSERT_EQ(std::string("{1, 2, 3}"), oss.str());
}

/**
 * @brief 测试空 set 输出。
 * @note 期望：空容器输出 {}。
 */
TEST(OutputEmptySet) {
    std::set<int>      s;
    std::ostringstream oss;
    oss << s;
    ASSERT_EQ(std::string("{}"), oss.str());
}

/**
 * @brief 测试 map 输出。
 * @note 期望：输出格式为 {1: a, 2: b}。
 */
TEST(OutputMap) {
    std::map<int, std::string> m = {{1, "a"}, {2, "b"}};
    std::ostringstream         oss;
    oss << m;
    ASSERT_EQ(std::string("{1: \"a\", 2: \"b\"}"), oss.str());
}

/**
 * @brief 测试空 map 输出。
 * @note 期望：空容器输出 {}。
 */
TEST(OutputEmptyMap) {
    std::map<int, std::string> m;
    std::ostringstream         oss;
    oss << m;
    ASSERT_EQ(std::string("{}"), oss.str());
}

/**
 * @brief 测试 multiset 输出。
 * @note 期望：输出格式为 {1, 2, 3}。
 */
TEST(OutputMultiset) {
    std::multiset<int> ms = {1, 2, 2, 3};
    std::ostringstream oss;
    oss << ms;
    ASSERT_EQ(std::string("{1, 2, 2, 3}"), oss.str());
}

/**
 * @brief 测试 multimap 输出。
 * @note 期望：输出格式为 {1: a, 2: b}。
 */
TEST(OutputMultimap) {
    std::multimap<int, std::string> mm = {{1, "a"}, {2, "b"}, {2, "c"}};
    std::ostringstream              oss;
    oss << mm;
    ASSERT_EQ(std::string("{1: \"a\", 2: \"b\", 2: \"c\"}"), oss.str());
}

/**
 * @brief 测试 unordered_set 输出。
 * @note 期望：输出格式为 {1, 2, 3}（顺序可能不同）。
 */
TEST(OutputUnorderedSet) {
    std::unordered_set<int> us = {1, 2, 3};
    std::ostringstream      oss;
    oss << us;
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "1");
    ASSERT_STRCONTAINS(output.c_str(), "2");
    ASSERT_STRCONTAINS(output.c_str(), "3");
    ASSERT_EQ('{', output[0]);
    ASSERT_EQ('}', output[output.size() - 1]);
}

/**
 * @brief 测试 unordered_map 输出。
 * @note 期望：输出格式为 {key: value, ...}。
 */
TEST(OutputUnorderedMap) {
    std::unordered_map<int, std::string> um = {{1, "a"}, {2, "b"}};
    std::ostringstream                   oss;
    oss << um;
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "1: \"a\"");
    ASSERT_STRCONTAINS(output.c_str(), "2: \"b\"");
    ASSERT_EQ('{', output[0]);
    ASSERT_EQ('}', output[output.size() - 1]);
}

/**
 * @brief 测试 valarray 输出。
 * @note 期望：输出格式为 [1, 2, 3]。
 */
TEST(OutputValarray) {
    std::valarray<int> va = {1, 2, 3};
    std::ostringstream oss;
    oss << va;
    ASSERT_EQ(std::string("[1, 2, 3]"), oss.str());
}

/**
 * @brief 测试 pair 输出。
 * @note 期望：输出格式为 (first, second)。
 */
TEST(OutputPair) {
    std::pair<int, std::string> p = {42, "hello"};
    std::ostringstream          oss;
    oss << p;
    ASSERT_EQ(std::string("(42, \"hello\")"), oss.str());
}

/**
 * @brief 测试 pair 嵌套输出。
 * @note 期望：嵌套 pair 正确输出。
 */
TEST(OutputNestedPair) {
    std::pair<int, std::pair<int, int>> p = {1, {2, 3}};
    std::ostringstream                  oss;
    oss << p;
    ASSERT_EQ(std::string("(1, (2, 3))"), oss.str());
}

/**
 * @brief 测试 tuple 输出。
 * @note 期望：输出格式为 (elem1, elem2, elem3)。
 */
TEST(OutputTuple) {
    std::tuple<int, std::string, double> t = {42, "hello", 3.14};
    std::ostringstream                   oss;
    oss << t;
    ASSERT_EQ(std::string("(42, \"hello\", 3.14)"), oss.str());
}

/**
 * @brief 测试空 tuple 输出。
 * @note 期望：空 tuple 输出 ()。
 */
TEST(OutputEmptyTuple) {
    std::tuple<>       t;
    std::ostringstream oss;
    oss << t;
    ASSERT_EQ(std::string("()"), oss.str());
}

/**
 * @brief 测试 tuple 嵌套输出。
 * @note 期望：嵌套 tuple 正确输出。
 */
TEST(OutputNestedTuple) {
    std::tuple<int, std::tuple<int, int>> t = {1, {2, 3}};
    std::ostringstream                    oss;
    oss << t;
    ASSERT_EQ(std::string("(1, (2, 3))"), oss.str());
}

/**
 * @brief 测试 vector 嵌套输出。
 * @note 期望：嵌套容器正确输出。
 */
TEST(OutputNestedVector) {
    std::vector<std::vector<int>> v = {{1, 2}, {3, 4, 5}};
    std::ostringstream            oss;
    oss << v;
    ASSERT_EQ(std::string("[[1, 2], [3, 4, 5]]"), oss.str());
}

/**
 * @brief 测试 to_array 转换。
 * @note 期望：C 数组转换为 std::array。
 */
TEST(ToArray) {
    int  arr[] = {1, 2, 3, 4, 5};
    auto a     = console::to_array(arr);
    ASSERT_EQ(5, a.size());
    ASSERT_EQ(1, a[0]);
    ASSERT_EQ(3, a[2]);
    ASSERT_EQ(5, a[4]);
}

/**
 * @brief 测试 to_vector 转换。
 * @note 期望：C 数组转换为 std::vector。
 */
TEST(ToVector) {
    int  arr[] = {1, 2, 3, 4, 5};
    auto v     = console::to_vector(arr);
    ASSERT_EQ(5, v.size());
    ASSERT_EQ(1, v[0]);
    ASSERT_EQ(3, v[2]);
    ASSERT_EQ(5, v[4]);
}

/**
 * @brief 测试 BasicOutput 默认构造。
 * @note 期望：默认构造输出到 cout，分隔符为空格。
 */
TEST(BasicOutputDefault) {
    std::ostringstream         oss;
    console::BasicOutput<char> out(oss, " ", "\n", false);
    out("Hello", "World", 42);
    ASSERT_EQ(std::string("Hello World 42\n"), oss.str());
}

/**
 * @brief 测试 BasicOutput 自定义分隔符。
 * @note 期望：使用自定义分隔符。
 */
TEST(BasicOutputCustomSeparator) {
    std::ostringstream         oss;
    console::BasicOutput<char> out(oss, ", ", "\n", false);
    out(1, 2, 3);
    ASSERT_EQ(std::string("1, 2, 3\n"), oss.str());
}

/**
 * @brief 测试 BasicOutput 自定义结尾符。
 * @note 期望：使用自定义结尾符。
 */
TEST(BasicOutputCustomEnd) {
    std::ostringstream         oss;
    console::BasicOutput<char> out(oss, " ", "", false);
    out("Hello", "World");
    ASSERT_EQ(std::string("Hello World"), oss.str());
}

/**
 * @brief 测试 BasicOutput 无参数调用。
 * @note 期望：仅输出结尾符。
 */
TEST(BasicOutputNoArgs) {
    std::ostringstream         oss;
    console::BasicOutput<char> out(oss, " ", "\n", false);
    out();
    ASSERT_EQ(std::string("\n"), oss.str());
}

/**
 * @brief 测试 BasicOutput 单参数调用。
 * @note 期望：输出单个参数加结尾符。
 */
TEST(BasicOutputSingleArg) {
    std::ostringstream         oss;
    console::BasicOutput<char> out(oss, " ", "\n", false);
    out(42);
    ASSERT_EQ(std::string("42\n"), oss.str());
}

/**
 * @brief 测试 BasicOutput 刷新标志。
 * @note 期望：刷新时流被刷新。
 */
TEST(BasicOutputFlush) {
    std::ostringstream         oss;
    console::BasicOutput<char> out(oss, " ", "\n", true);
    out("test");
    // 验证输出正确
    ASSERT_EQ(std::string("test\n"), oss.str());
}

/**
 * @brief 测试全局 print 对象。
 * @note 期望：全局 print 可正常输出。
 */
TEST(GlobalPrint) {
    std::ostringstream         oss;
    console::BasicOutput<char> custom(oss, " ", "\n", false);
    // 使用自定义对象模拟全局 print
    custom("Hello", "World");
    ASSERT_EQ(std::string("Hello World\n"), oss.str());
}

/**
 * @brief 测试输出 string 到容器。
 * @note 期望：string 在容器中正确输出。
 */
TEST(OutputStringContainer) {
    std::vector<std::string> v = {"hello", "world", "test"};
    std::ostringstream       oss;
    oss << v;
    ASSERT_EQ(std::string("[\"hello\", \"world\", \"test\"]"), oss.str());
}

/**
 * @brief 测试混合类型容器输出。
 * @note 期望：混合类型正确输出。
 */
TEST(OutputMixedTypes) {
    std::vector<std::pair<int, std::string>> v = {{1, "one"}, {2, "two"}};
    std::ostringstream                       oss;
    oss << v;
    ASSERT_EQ(std::string("[(1, \"one\"), (2, \"two\")]"), oss.str());
}

/**
 * @brief 测试 map 包含 string 键值。
 * @note 期望：string 键值正确输出。
 */
TEST(OutputMapStringKeys) {
    std::map<std::string, int> m = {{"a", 1}, {"b", 2}};
    std::ostringstream         oss;
    oss << m;
    ASSERT_EQ(std::string("{\"a\": 1, \"b\": 2}"), oss.str());
}

/**
 * @brief 测试 tuple 包含容器元素。
 * @note 期望：tuple 中的容器正确输出。
 */
TEST(OutputTupleWithContainer) {
    std::tuple<int, std::vector<int>> t = {1, {2, 3}};
    std::ostringstream                oss;
    oss << t;
    ASSERT_EQ(std::string("(1, [2, 3])"), oss.str());
}

/**
 * @brief 测试 map 嵌套 vector。
 * @note 期望：嵌套结构正确输出。
 */
TEST(OutputMapNestedVector) {
    std::map<int, std::vector<int>> m = {{1, {2, 3}}, {4, {5, 6}}};
    std::ostringstream              oss;
    oss << m;
    ASSERT_EQ(std::string("{1: [2, 3], 4: [5, 6]}"), oss.str());
}

/**
 * @brief 测试宽字符输出。
 * @note 期望：宽字符容器正确输出。
 */
TEST(OutputWideChar) {
    std::vector<std::wstring> v = {L"hello", L"world"};
    std::wostringstream       woss;
    woss << v;
    ASSERT_EQ(std::wstring(L"[\"hello\", \"world\"]"), woss.str());
}

/**
 * @brief 测试宽字符 pair 输出。
 * @note 期望：宽字符 pair 正确输出。
 */
TEST(OutputWidePair) {
    std::pair<int, std::wstring> p = {42, L"hello"};
    std::wostringstream          woss;
    woss << p;
    ASSERT_EQ(std::wstring(L"(42, \"hello\")"), woss.str());
}

#ifndef NOMAIN
TEST_MAIN
#endif
