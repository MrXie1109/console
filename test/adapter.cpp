/**
 * @file adapter.cpp
 * @brief 测试容器适配器扩展模块 (Stack, Queue, PriorityQueue)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/adapter.h"

#include <list>
#include <string>
#include <vector>

#include "../include/test.h"

/**
 * @brief 测试 Stack 的基本 push 和 pop 操作。
 * @note 期望：push 的元素按 LIFO 顺序弹出，pop 返回正确的值。
 */
TEST(StackBasicPushPop) {
    console::Stack<int> s;
    s.push(10);
    s.push(20);
    s.push(30);
    ASSERT_EQ(30, s.pop());
    ASSERT_EQ(20, s.pop());
    ASSERT_EQ(10, s.pop());
    ASSERT_TRUE(s.empty());
}

/**
 * @brief 测试 Stack 的 pop 返回值为移动语义。
 * @note 期望：pop 返回的值可以安全移动，且原容器元素已被移除。
 */
TEST(StackPopMoveSemantics) {
    console::Stack<std::string> s;
    std::string                 str = "Hello, World!";
    s.push(std::move(str));
    ASSERT_EQ(std::string("Hello, World!"), s.top());
    std::string result = s.pop();
    ASSERT_EQ(std::string("Hello, World!"), result);
    ASSERT_TRUE(s.empty());
}

/**
 * @brief 测试 Stack 的 top 和 size 方法。
 * @note 期望：top 返回栈顶元素的引用，size 返回正确的大小。
 */
TEST(StackTopAndSize) {
    console::Stack<double> s;
    s.push(1.1);
    s.push(2.2);
    s.push(3.3);
    ASSERT_EQ(3, s.size());
    ASSERT_EQ(3.3, s.top());
    s.pop();
    ASSERT_EQ(2, s.size());
    ASSERT_EQ(2.2, s.top());
}

/**
 * @brief 测试 Stack 使用 vector 作为底层容器。
 * @note 期望：使用 std::vector 作为底层容器时功能正常。
 */
TEST(StackWithVectorContainer) {
    console::Stack<int, std::vector<int>> s;
    s.push(5);
    s.push(15);
    s.push(25);
    ASSERT_EQ(25, s.pop());
    ASSERT_EQ(15, s.pop());
    ASSERT_EQ(5, s.pop());
    ASSERT_TRUE(s.empty());
}

/**
 * @brief 测试 Stack 的 empty 方法。
 * @note 期望：空栈返回 true，非空栈返回 false。
 */
TEST(StackEmptyCheck) {
    console::Stack<int> s;
    ASSERT_TRUE(s.empty());
    s.push(42);
    ASSERT_FALSE(s.empty());
    s.pop();
    ASSERT_TRUE(s.empty());
}

/**
 * @brief 测试 Queue 的基本 push 和 pop 操作。
 * @note 期望：push 的元素按 FIFO 顺序弹出，pop 返回正确的值。
 */
TEST(QueueBasicPushPop) {
    console::Queue<int> q;
    q.push(10);
    q.push(20);
    q.push(30);
    ASSERT_EQ(10, q.pop());
    ASSERT_EQ(20, q.pop());
    ASSERT_EQ(30, q.pop());
    ASSERT_TRUE(q.empty());
}

/**
 * @brief 测试 Queue 的 pop 返回值为移动语义。
 * @note 期望：pop 返回的值可以安全移动，且原容器元素已被移除。
 */
TEST(QueuePopMoveSemantics) {
    console::Queue<std::string> q;
    std::string                 str = "Queue Test";
    q.push(std::move(str));
    ASSERT_EQ(std::string("Queue Test"), q.front());
    std::string result = q.pop();
    ASSERT_EQ(std::string("Queue Test"), result);
    ASSERT_TRUE(q.empty());
}

/**
 * @brief 测试 Queue 的 front、back 和 size 方法。
 * @note 期望：front 返回队首元素，back 返回队尾元素，size 返回正确的大小。
 */
TEST(QueueFrontBackAndSize) {
    console::Queue<int> q;
    q.push(1);
    q.push(2);
    q.push(3);
    ASSERT_EQ(3, q.size());
    ASSERT_EQ(1, q.front());
    ASSERT_EQ(3, q.back());
    q.pop();
    ASSERT_EQ(2, q.size());
    ASSERT_EQ(2, q.front());
    ASSERT_EQ(3, q.back());
}

/**
 * @brief 测试 Queue 使用 list 作为底层容器。
 * @note 期望：使用 std::list 作为底层容器时功能正常。
 */
TEST(QueueWithListContainer) {
    console::Queue<int, std::list<int>> q;
    q.push(100);
    q.push(200);
    q.push(300);
    ASSERT_EQ(100, q.pop());
    ASSERT_EQ(200, q.pop());
    ASSERT_EQ(300, q.pop());
    ASSERT_TRUE(q.empty());
}

/**
 * @brief 测试 Queue 的 empty 方法。
 * @note 期望：空队列返回 true，非空队列返回 false。
 */
TEST(QueueEmptyCheck) {
    console::Queue<double> q;
    ASSERT_TRUE(q.empty());
    q.push(3.14);
    ASSERT_FALSE(q.empty());
    q.pop();
    ASSERT_TRUE(q.empty());
}

/**
 * @brief 测试 PriorityQueue 的基本 push 和 pop 操作。
 * @note 期望：push 的元素按优先级顺序弹出(默认最大堆)，pop 返回最大值。
 */
TEST(PriorityQueueBasicPushPop) {
    console::PriorityQueue<int> pq;
    pq.push(30);
    pq.push(10);
    pq.push(50);
    pq.push(20);
    pq.push(40);
    ASSERT_EQ(50, pq.pop());
    ASSERT_EQ(40, pq.pop());
    ASSERT_EQ(30, pq.pop());
    ASSERT_EQ(20, pq.pop());
    ASSERT_EQ(10, pq.pop());
    ASSERT_TRUE(pq.empty());
}

/**
 * @brief 测试 PriorityQueue 的 pop 返回值为移动语义。
 * @note 期望：pop 返回的值可以安全移动，且原容器元素已被移除。
 */
TEST(PriorityQueuePopMoveSemantics) {
    console::PriorityQueue<std::string> pq;
    std::string                         str1 = "Apple";
    std::string                         str2 = "Banana";
    pq.push(std::move(str1));
    pq.push(std::move(str2));
    std::string result = pq.pop();
    ASSERT_EQ(std::string("Banana"), result);
    result = pq.pop();
    ASSERT_EQ(std::string("Apple"), result);
    ASSERT_TRUE(pq.empty());
}

/**
 * @brief 测试 PriorityQueue 使用最小堆。
 * @note 期望：使用 std::greater 比较器时，pop 返回最小值。
 */
TEST(PriorityQueueMinHeap) {
    console::PriorityQueue<int, std::vector<int>, std::greater<int>> pq;
    pq.push(30);
    pq.push(10);
    pq.push(50);
    pq.push(20);
    pq.push(40);
    ASSERT_EQ(10, pq.pop());
    ASSERT_EQ(20, pq.pop());
    ASSERT_EQ(30, pq.pop());
    ASSERT_EQ(40, pq.pop());
    ASSERT_EQ(50, pq.pop());
    ASSERT_TRUE(pq.empty());
}

/**
 * @brief 测试 PriorityQueue 的 top 和 size 方法。
 * @note 期望：top 返回优先级最高的元素，size 返回正确的大小。
 */
TEST(PriorityQueueTopAndSize) {
    console::PriorityQueue<int> pq;
    pq.push(5);
    pq.push(15);
    pq.push(10);
    ASSERT_EQ(3, pq.size());
    ASSERT_EQ(15, pq.top());
    pq.pop();
    ASSERT_EQ(2, pq.size());
    ASSERT_EQ(10, pq.top());
}

/**
 * @brief 测试 PriorityQueue 使用自定义比较器。
 * @note 期望：自定义比较器(按字符串长度)正确排序。
 */
TEST(PriorityQueueCustomComparator) {
    struct CompareLength {
        bool operator()(const std::string &a, const std::string &b) const {
            return a.length() < b.length();
        }
    };
    console::PriorityQueue<std::string, std::vector<std::string>, CompareLength>
        pq;
    pq.push("short");
    pq.push("longer");
    pq.push("medium");
    pq.push("tiny");
    ASSERT_EQ(std::string("longer"), pq.pop());
    ASSERT_EQ(std::string("medium"), pq.pop());
    ASSERT_EQ(std::string("short"), pq.pop());
    ASSERT_EQ(std::string("tiny"), pq.pop());
    ASSERT_TRUE(pq.empty());
}

/**
 * @brief 测试 PriorityQueue 的 empty 方法。
 * @note 期望：空优先队列返回 true，非空返回 false。
 */
TEST(PriorityQueueEmptyCheck) {
    console::PriorityQueue<int> pq;
    ASSERT_TRUE(pq.empty());
    pq.push(42);
    ASSERT_FALSE(pq.empty());
    pq.pop();
    ASSERT_TRUE(pq.empty());
}

/**
 * @brief 测试 Stack 的流输出运算符。
 * @note 期望：输出格式与底层容器一致，内容正确。
 */
TEST(StackOutputOperator) {
    console::Stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    std::ostringstream oss;
    oss << s;
    std::string str = oss.str();
    ASSERT_STRCONTAINS(str.c_str(), "1");
    ASSERT_STRCONTAINS(str.c_str(), "2");
    ASSERT_STRCONTAINS(str.c_str(), "3");
}

/**
 * @brief 测试 Queue 的流输出运算符。
 * @note 期望：输出格式与底层容器一致，内容正确。
 */
TEST(QueueOutputOperator) {
    console::Queue<int> q;
    q.push(10);
    q.push(20);
    q.push(30);
    std::ostringstream oss;
    oss << q;
    std::string str = oss.str();
    ASSERT_STRCONTAINS(str.c_str(), "10");
    ASSERT_STRCONTAINS(str.c_str(), "20");
    ASSERT_STRCONTAINS(str.c_str(), "30");
}

/**
 * @brief 测试 PriorityQueue 的流输出运算符。
 * @note 期望：输出格式与底层容器一致，内容正确。
 */
TEST(PriorityQueueOutputOperator) {
    console::PriorityQueue<int> pq;
    pq.push(5);
    pq.push(15);
    pq.push(10);
    std::ostringstream oss;
    oss << pq;
    std::string str = oss.str();
    ASSERT_STRCONTAINS(str.c_str(), "5");
    ASSERT_STRCONTAINS(str.c_str(), "15");
    ASSERT_STRCONTAINS(str.c_str(), "10");
}

/**
 * @brief 测试 Stack 在复杂类型上的操作。
 * @note 期望：自定义类型在 Stack 中正常工作。
 */
TEST(StackWithCustomType) {
    struct Point {
        int  x, y;
        bool operator==(const Point &other) const {
            return x == other.x && y == other.y;
        }
    };
    console::Stack<Point> s;
    s.push({1, 2});
    s.push({3, 4});
    Point p = s.pop();
    ASSERT_EQ(3, p.x);
    ASSERT_EQ(4, p.y);
    p = s.pop();
    ASSERT_EQ(1, p.x);
    ASSERT_EQ(2, p.y);
}

/**
 * @brief 测试 Queue 在复杂类型上的操作。
 * @note 期望：自定义类型在 Queue 中正常工作。
 */
TEST(QueueWithCustomType) {
    struct Person {
        std::string name;
        int         age;
        bool        operator==(const Person &other) const {
            return name == other.name && age == other.age;
        }
    };
    console::Queue<Person> q;
    q.push({"Alice", 25});
    q.push({"Bob", 30});
    Person p = q.pop();
    ASSERT_EQ(std::string("Alice"), p.name);
    ASSERT_EQ(25, p.age);
    p = q.pop();
    ASSERT_EQ(std::string("Bob"), p.name);
    ASSERT_EQ(30, p.age);
}

/**
 * @brief 测试 PriorityQueue 在复杂类型上的操作。
 * @note 期望：自定义类型在 PriorityQueue 中正常工作。
 */
TEST(PriorityQueueWithCustomType) {
    struct Person {
        std::string name;
        int         age;
        bool operator<(const Person &other) const { return age < other.age; }
        bool operator==(const Person &other) const {
            return name == other.name && age == other.age;
        }
    };
    console::PriorityQueue<Person> pq;
    pq.push({"Alice", 25});
    pq.push({"Bob", 30});
    pq.push({"Charlie", 20});
    Person p = pq.pop();
    ASSERT_EQ(std::string("Bob"), p.name);
    ASSERT_EQ(30, p.age);
    p = pq.pop();
    ASSERT_EQ(std::string("Alice"), p.name);
    ASSERT_EQ(25, p.age);
    p = pq.pop();
    ASSERT_EQ(std::string("Charlie"), p.name);
    ASSERT_EQ(20, p.age);
}

/**
 * @brief 测试 Stack 在混合 push 和 pop 操作下的状态一致性。
 * @note 期望：经过一系列操作后，栈的状态保持一致。
 */
TEST(StackMixedOperations) {
    console::Stack<int> s;
    s.push(1);
    s.push(2);
    ASSERT_EQ(2, s.pop());
    s.push(3);
    s.push(4);
    ASSERT_EQ(4, s.pop());
    ASSERT_EQ(3, s.pop());
    s.push(5);
    ASSERT_EQ(5, s.pop());
    ASSERT_EQ(1, s.pop());
    ASSERT_TRUE(s.empty());
}

/**
 * @brief 测试 Queue 在混合 push 和 pop 操作下的状态一致性。
 * @note 期望：经过一系列操作后，队列的状态保持一致。
 */
TEST(QueueMixedOperations) {
    console::Queue<int> q;
    q.push(1);
    q.push(2);
    ASSERT_EQ(1, q.pop());
    q.push(3);
    q.push(4);
    ASSERT_EQ(2, q.pop());
    ASSERT_EQ(3, q.pop());
    q.push(5);
    ASSERT_EQ(4, q.pop());
    ASSERT_EQ(5, q.pop());
    ASSERT_TRUE(q.empty());
}

/**
 * @brief 测试 PriorityQueue 在混合 push 和 pop 操作下的状态一致性。
 * @note 期望：经过一系列操作后，优先队列的状态保持一致。
 */
TEST(PriorityQueueMixedOperations) {
    console::PriorityQueue<int> pq;
    pq.push(5);
    pq.push(3);
    ASSERT_EQ(5, pq.pop());
    pq.push(8);
    pq.push(1);
    ASSERT_EQ(8, pq.pop());
    pq.push(6);
    ASSERT_EQ(6, pq.pop());
    ASSERT_EQ(3, pq.pop());
    ASSERT_EQ(1, pq.pop());
    ASSERT_TRUE(pq.empty());
}

#ifndef NOMAIN
TEST_MAIN
#endif
