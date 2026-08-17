# Console Library

**一个现代 C++ 控制台工具库** | **v7.2.0** · _"Copy-on-Write"_

[![C++11](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](https://en.cppreference.com/w/cpp/11)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Documentation](https://img.shields.io/badge/docs-online-brightgreen.svg)](https://mrxie1109.github.io/console)

**[README: American English](README.md)** |
**[README: 简体中文](README_zh.md)**

---

## 概述

Console 是一个功能全面、仅头文件的 C++ 库，为控制台/终端应用程序开发提供了丰富的工具集。它旨在提供类似 Python 的便捷性，同时保持 C++ 的高性能和类型安全。

附注：`Console` 只是一个符号名称，不必在意。

**主要能力包括：**

- **容器适配器** —— 支持返回值 `pop()` 操作
- **多维数组** —— 编译期固定维度
- **函数式编程** —— 生成器管道与惰性求值
- **类型安全的格式化** —— 类似 Python 风格的 `format()` 和 `fmt()`
- **日志系统** —— 带颜色编码的严重级别
- **正则表达式** —— 类似 Python `re` 模块的接口
- **随机数生成** —— 支持丰富的分布类型
- **线程池** —— 并行任务执行
- **进度条** —— 迭代可视化
- **AES-128 加密**、**SHA256**、**MD5** 和 **Base64**
- **INI 配置文件**解析
- **跨平台终端控制**(颜色、光标、屏幕)
- **Windows 平台 MIDI 播放**
- **Linux 平台进程管理**
- **Result/Optional 类型** —— 类似 Rust
- **协作式线程管理** —— 基于 Event 的停止机制
- **异步任务** —— 独占式 (Task) 或共享式 (SharedTask) 访问，支持取消操作
- **作用域退出守卫** —— 使用 `defer` 宏(RAII风格)
- **单元测试框架** —— 基于宏的断言和自动测试注册(断言、异常测试、性能基准测试)
- **写时复制** `Cow` 类 —— 延迟复制共享数据
- 以及更多……

---

## 环境要求

- **C++11** 或更高版本(完全兼容)
- 标准库
- CMake(可选，用于构建测试/示例)
- Windows 平台：MIDI 支持需要 `winmm.lib`
- Linux 平台：标准 POSIX 头文件

---

## 快速上手

### 基本用法

```cpp
#include <console/all.h>
/*
// 或者按需引入
#include <console/output.h>
#include <console/logging.h>
#include <console/gen.h>
#include <console/fmt.h>
 */
using namespace console;

int main() {
    // 彩色输出
    std::cout << color::Red << "你好" << color::Reset << " 世界\n";

    // Python 风格的 print
    print("答案是", 42);

    // 日志
    logger.info("应用程序已启动");
    logger.warn("内存不足警告");

    // 使用生成器进行区间迭代
    for (auto i : gen::range(10)) {
        print(i);
    }

    // 格式化
    auto msg = format("数值: {:.2f}", 3.14159);

    return 0;
}
```

### 使用容器

```cpp
#include <console/all.h>
using namespace console;

void container_example() {
    // 支持返回值 pop 的栈
    Stack<int> s;
    s.push(1);
    s.push(2);
    int value = s.pop();  // 返回 2

    // 队列
    Queue<std::string> q;
    q.push("first");
    q.push("second");
    auto str = q.pop();   // 返回 "first"

    // 优先队列
    PriorityQueue<int> pq;
    pq.push(5);
    pq.push(1);
    pq.push(3);
    int max = pq.pop();   // 返回 5
}
```

### 生成器与管道

```cpp
#include <console/all.h>
using namespace console;

void generator_example() {
    // 使用管道运算符创建数据处理流水线
    auto result = gen::range(1, 20)
        | gen::filter(ops::even)
        | gen::map(ops::square)
        | gen::collect();

    // result = [4, 16, 36, 64, 100, 144, 196, 256, 324]

    // 生成器链式拼接
    auto nums = gen::range(5) + gen::range(10, 15);

    // 带索引的枚举
    for (auto [idx, val] : gen::range(5) | gen::enumerate) {
        print(idx, ":", val);
    }
}
```

### 线程池

```cpp
#include <console/all.h>
using namespace console;

void thread_pool_example() {
    ThreadPool pool(4);  // 4 个工作线程

    // 提交任务并获取 future
    auto f1 = pool.submit([](int x) { return x * x; }, 5);
    auto f2 = pool.submit([](int x) { return x + x; }, 10);

    int result1 = f1.get();  // 25
    int result2 = f2.get();  // 20

    // 对容器中的元素并行执行映射函数
    std::vector<int> data = {1, 2, 3, 4, 5};
    auto futures = pool.map([](int x) { return x * 2; }, data);

    for (auto& f : futures) {
        print(f.get());
    }
}
```

### 多维数组

```cpp
#include <console/all.h>
using namespace console;

void multiarray_example() {
    // 3x3 矩阵
    MultiArray<double, 3, 3> A = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    // 逐元素运算
    auto B = A + 10;
    auto C = A * 2.5;

    // 统计
    double mean_val = mean(A);
    double std_val = stddev(A);

    // 矩阵乘法
    auto D = matmul(A, B);
}
```

### 正则表达式

```cpp
#include <console/all.h>
using namespace console;

void regex_example() {
    // 编译正则模式
    auto r = re::compile(R"(\d+-\d+-\d+)");

    // 搜索
    auto m = r.search("日期: 2026-08-07");
    if (m) {
        print("找到:", m.group(0));
    }

    // 查找所有匹配
    auto matches = r.findall("2026-08-07 2026-08-08 2026-08-09");

    // 分割
    auto parts = re::split(R"(\s+)", "one two three");
}
```

### 配置文件

```cpp
#include <console/all.h>
using namespace console;

void config_example() {
    INIConfig config("settings.ini");

    // 读取配置(支持自动类型转换)
    int port = config.get("server.port", 8080);
    bool debug = config.get("app.debug", false);
    std::string host = config.get("server.host", "localhost");

    // 写入配置
    config.set("app.version", "2.0.0");
    config.save("settings.ini");
}
```

### 协作式线程管理

```cpp
#include <console/all.h>
using namespace console;

void thread_example() {
    // 创建接受 Event& 参数的线程，支持协作式停止
    Thread t([](const Event& stop_event) {
        while (!stop_event.is_set()) {
            // 执行工作...
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        print("线程已优雅停止");
    });

    // 让其运行一段时间
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // 请求线程停止
    t.stop();

    // 等待线程结束(非必要)
    t.join();
}
```

### 异步任务

```cpp
#include <console/all.h>
using namespace console;

void task_example() {
    Task<int> task([]() { return 42; });
    int result = task.get();  // 42

    SharedTask<int> shared([]() { return 100; });
    auto copy = shared;
    int r1 = copy.get();   // 100
    int r2 = shared.get(); // 100 (可多次调用 get())
}
```

### Defer(作用域退出守卫)

```cpp
#include <console/all.h>
using namespace console;

void defer_example() {
    FILE* file = fopen("data.txt", "r");
    if (!file) return;

    // 确保离开作用域时自动关闭文件
    defer(fclose(file));

    // 使用文件...
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        print(buffer);
    }
    // 此处自动调用 fclose(file)
}
```

### 单元测试

```cpp
#include <console/all.h>
using namespace console;

TEST(AdditionWorks) {
    ASSERT_EQ(2 + 2, 4); // 断言 2 + 2 等于 4
}

TEST(ContainerContains) {
    std::vector<int> v = {1, 2, 3, 4};
    ASSERT_CONTAINS(v, 3); // 断言 v 包含 3
    ASSERT_SIZE_EQ(v, 4); // 断言 v 的大小等于 4
}

TEST(ExceptionThrows) {
    ASSERT_THROWS(
        throw std::runtime_error("error"),
        std::runtime_error
    ); // 断言抛出 std::runtime_error 异常
}

TEST_MAIN // 等价于 `int main() {}`
```

### 写时复制

```cpp
#include <console/all.h>
using namespace console;

void cow_example() {
    // 创建一个包装初始值的 Cow
    Cow<std::string> str("Hello");

    // 读取底层数据 (无复制)
    print(str.reader());              // Hello
    print(str.read([](const auto& s) { return s.size(); }));  // 5

    // 创建一个写时复制的副本 (共享底层数据)
    Cow<std::string> copy = str;

    // 共享底层数据 (无复制)
    print(&copy.reader() == &str.reader());               // true

    // 写入触发分离 (复制底层数据)
    str.write([](std::string& s) { s += " World"; });

    // 写入触发分离后，`str` 和 `copy` 持有独立的数据
    print(str.reader());              // Hello World
    print(copy.reader());             // Hello
}
```

---

## 文档

完整 API 文档请访问：

## **[https://mrxie1109.github.io/console](https://mrxie1109.github.io/console)**

---

## 许可证

本库基于 [MIT 许可证](LICENSE) 分发。

---

## 贡献

欢迎贡献！请提交 Issue 或 Pull Request。

---

## 作者

**MrXie1109**
