# Console Library

**一个现代 C++ 控制台工具库** | **v8.0.0** · _"One, Two, Three, Four."_

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

- **容器与数据结构** —— 返回值 pop 的容器适配器、写时复制 Cow、Maybe/Result、可判别联合体 Union、非拥有视图 View、编译期固定维度多维数组
- **函数式编程** —— 生成器管道与惰性求值、轻量级协程/生成器宏
- **文本处理** —— 字符串工具、格式化、正则表达式、repr 表示、INI 配置解析
- **异步与并发** —— 线程池、MPMC 通道、任务组、协作式线程、调度器、无锁队列
- **数值计算** —— 随机数生成、有理数、矩阵运算、π 常量
- **加密与编码** —— AES-128、SHA256、MD5、Base64
- **输入输出与终端控制** —— 彩色输出、进度条、跨平台键盘输入、终端控制、文件操作
- **工具与实用功能** —— 作用域退出守卫 defer、参数解析、系统命令、类型名反混淆、单元测试框架、时间测量
- **跨平台** —— Windows MIDI 播放、Linux 进程管理

---

## 环境要求

- **C++11** 或更高版本(完全兼容)
- 标准库
- CMake(可选，用于构建测试/示例)
- Windows 平台：MIDI 支持需要 `winmm.lib`
- Linux 平台：标准 POSIX 头文件

---

## 快速上手

### 基础用法

```cpp
#include <console/all.h>
using namespace console;

int main() {
    std::cout << color::Red << "你好" << color::Reset << " 世界\n";

    print("答案是", 42);

    logger.info("应用程序已启动");
    logger.warn("内存不足警告");

    for (auto i : gen::range(10)) {
        print(i);
    }

    auto msg = format("数值: {:.2f}", 3.14159);

    return 0;
}
```

---

### 容器

```cpp
#include <console/all.h>
using namespace console;

void stack_queue_example() {
    Stack<int> s;
    s.push(1);
    s.push(2);
    int v1 = s.pop();  // 2

    Queue<std::string> q;
    q.push("first");
    q.push("second");
    auto str = q.pop();  // "first"

    PriorityQueue<int> pq;
    pq.push(5);
    pq.push(1);
    pq.push(3);
    int max = pq.pop();  // 5
}

void box_maybe_result_example() {
    Box<int> b = make_box(42);
    *b = 100;

    Maybe<int> m = 10;
    if (m) print(*m);

    Result<int, std::string> res = Ok(42);
    if (res.is_ok()) print(res.unwrap());
}

void cow_union_example() {
    Cow<std::string> str("Hello");
    Cow<std::string> copy = str;
    str.write([](std::string& s) { s += " World"; });
    print(str.reader());   // Hello World
    print(copy.reader());  // Hello

    Union<int, double, std::string> u = 42;
    u.visit(
        [](int i) { print("int:", i); },
        [](double d) { print("double:", d); },
        [](const std::string& s) { print("string:", s); }
    );
}

void view_multiarray_example() {
    std::vector<int> data = {1, 2, 3, 4, 5};
    View<int> v(data);

    MultiArray<double, 3, 3> A = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    auto B = A + 10;
    auto C = matmul(A, B);
}
```

---

### 生成器与管道

```cpp
#include <console/all.h>
using namespace console;

void generator_pipeline_example() {
    auto result = gen::range(1, 20)
        | gen::filter(ops::even)
        | gen::map(ops::square)
        | gen::collect();
    // [4, 16, 36, 64, 100, 144, 196, 256, 324]

    auto nums = gen::range(5) + gen::range(10, 15);

    for (auto [idx, val] : gen::range(5) | gen::enumerate) {
        print(idx, ":", val);
    }
}

void coroutine_example() {
    auto fibonacci = []() {
        int a = 0, b = 1;
        cr_begin(int);
        while (true) {
            cr_yield(a);
            int tmp = a;
            a = b;
            b += tmp;
        }
        cr_end;
    };

    Coroutine<int()> fib(fibonacci());
    for (int i = 0; i < 10; ++i) print(fib());  // 0 1 1 2 3 5 8 13 21 34
}
```

---

### 线程池与并发

```cpp
#include <console/all.h>
using namespace console;

void thread_pool_example() {
    ThreadPool pool(4);

    auto f1 = pool.submit([](int x) { return x * x; }, 5);
    auto f2 = pool.submit([](int x) { return x + x; }, 10);
    print(f1.get());  // 25
    print(f2.get());  // 20

    std::vector<int> data = {1, 2, 3, 4, 5};
    auto futures = pool.map([](int x) { return x * 2; }, data);
    for (auto& f : futures) print(f.get());
}

void channel_example() {
    Channel<int, 10> ch;
    std::thread producer([&ch]() {
        for (int i = 0; i < 100; ++i) ch << i;
        close(ch);
    });
    for (int value : ch) print("收到:", value);
    producer.join();

    Channel<std::string, 0> sync_ch;
    std::thread worker([&sync_ch]() {
        std::string msg;
        sync_ch >> msg;
        print("获取:", msg);
    });
    sync_ch << "来自主线程的问候!";
    worker.join();
}

void task_group_example() {
    Group group(3, []() { print("所有任务已完成！"); });

    std::thread t1([&group]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        print("任务 1 完成");
        group.done();
    });
    std::thread t2([&group]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        print("任务 2 完成");
        group.done();
    });
    std::thread t3([&group]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        print("任务 3 完成");
        group.done();
    });

    group.wait();
    t1.join(); t2.join(); t3.join();
}

void thread_scheduler_example() {
    Thread t([](const Event& stop) {
        while (!stop.is_set()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        print("线程已优雅停止");
    });
    std::this_thread::sleep_for(std::chrono::seconds(2));
    t.stop();
    t.join();

    Scheduler sched;
    sched.schedule(1.0, []() { print("1 秒后的单次任务"); });
    int count = 0;
    sched.interval(0.5, [&count, &sched]() {
        print("滴答", ++count);
        if (count >= 5) sched.cancel_all();
    });
    std::this_thread::sleep_for(std::chrono::seconds(4));
}
```

---

### 数值计算

```cpp
#include <console/all.h>
using namespace console;

void random_example() {
    Random rng;
    int i = rng.randint(1, 100);
    double d = rng.uniform(0.0, 1.0);
    auto vec = rng.normal(0.0, 1.0, 100);
    std::vector<int> data = {1, 2, 3, 4, 5};
    rng.shuffle(data);
}

void rational_example() {
    Rational a(1, 2);
    Rational b(1, 3);
    Rational c = a + b;  // 5/6
    print(a);            // 1/2
    print(c);            // 5/6
}

void matools_example() {
    MultiArray<double, 3, 3> A = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    MultiArray<double, 3, 3> B = {9, 8, 7, 6, 5, 4, 3, 2, 1};

    auto C = matmul(A, B);
    double mean_val = mean(A);
    double std_val = stddev(A);
    auto D = matmul_parallel(A, B);
}
```

---

### 文本处理

```cpp
#include <console/all.h>
using namespace console;

void string_utilities_example() {
    std::string s = to_string("Hello", " ", "World", 42);
    print(s);  // Hello World42

    auto parts = split("one,two,three", ",");
    auto joined = join(parts, " | ");  // "one | two | three"

    std::string lower = lower("Hello World");
    std::string upper = upper("Hello World");
    std::string trimmed = trim("  hello  ");
}

void format_example() {
    auto msg = format("数值: {:.2f}, 名称: {}", 3.14159, "Alice");
    print(msg);  // 数值: 3.14, 名称: Alice

    auto s = format("{1} -> {0}", "world", "hello");  // hello -> world
}

void repr_example() {
    std::string s = "Hello\nWorld";
    print(repr(s));   // "Hello\nWorld"
    print(repr(42));  // 42
    print(repr(true));// true
    print(repr(nullptr)); // <nullptr>
}

void config_regex_example() {
    INIConfig config("settings.ini");
    int port = config.get("server.port", 8080);
    bool debug = config.get("app.debug", false);
    std::string host = config.get("server.host", "localhost");
    config.set("app.version", "2.0.0");
    config.save("settings.ini");

    auto r = re::compile(R"(\d+-\d+-\d+)");
    auto m = r.search("日期: 2026-08-07");
    if (m) print("找到:", m.group(0));

    auto matches = r.findall("2026-08-07 2026-08-08 2026-08-09");
    auto parts = re::split(R"(\s+)", "one two three");
}
```

---

### 工具与实用功能

```cpp
#include <console/all.h>
using namespace console;

void defer_time_example() {
    FILE* file = fopen("data.txt", "r");
    if (!file) return;
    defer(fclose(file));

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) print(buffer);

    using namespace console::literals;
    Time t = 5_s + 100_ms;
    print(t);  // 5100000000ns

    auto elapsed = timer([]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    });
    print("耗时:", elapsed);
}

void crypto_example() {
    std::string plaintext = "Hello World!";
    std::string key = "0123456789abcdef";
    auto cipher = crypto::aes_encrypt(plaintext, key);
    auto decrypted = crypto::aes_decrypt(cipher, key);
    print(decrypted);

    auto hash = crypto::sha256("Hello World!");
    print(hash);
    auto md5_hash = crypto::md5("Hello World!");
    auto encoded = crypto::base64_encode("Hello");
    auto decoded = crypto::base64_decode(encoded);
}

void syscmd_collide_example() {
    auto result = syscmd("ls -la");
    print(result.output);
    print("退出码:", result.exit_code);

    std::string demangled = tiname(typeid(std::vector<int>));
    print(demangled);

    auto range = iterpair(0, 10);
    for (int i : range) print(i);
}

void params_test_example() {
    auto p = params(1, "hello", 3.14);
    p.apply([](int a, const char* b, double c) {
        print(a, b, c);
    });

    logger.info("应用程序已启动");
    logger.warn("内存不足警告");
    logger.error("打开文件失败");
    logger.set(Logging::Level::Warning);

    TEST(加法测试) {
        ASSERT_EQ(2 + 2, 4);
    }

    TEST(容器包含测试) {
        std::vector<int> v = {1, 2, 3, 4};
        ASSERT_CONTAINS(v, 3);
        ASSERT_SIZE_EQ(v, 4);
    }

    TEST(异常测试) {
        ASSERT_THROWS(
            throw std::runtime_error("error"),
            std::runtime_error
        );
    }

    TEST_MAIN
}
```

---

### 输入输出与终端控制

```cpp
#include <console/all.h>
using namespace console;

void colorful_output_example() {
    std::cout << color::Red << "红色" << color::Reset << " "
              << color::Green << "绿色" << color::Reset << "\n";
    std::cout << color::BgBlue << "蓝色背景" << color::Reset << "\n";
    std::cout << color::Bold << "粗体" << color::Reset << "\n";
    std::cout << color::Underline << "下划线" << color::Reset << "\n";
}

void input_progress_example() {
    if (kbhit()) {
        int ch = getch();
        print("按键:", ch);
    }

    std::vector<int> data(100);
    ProgressConfig cfg(std::cout);
    for (auto _ : progress(data, cfg)) {
        // 自动更新进度
    }
}

void term_file_example() {
    clear();
    gotoxy(10, 5);
    hide_cursor();
    std::cout << "定位在此处";
    show_cursor();
    auto [rows, cols] = get_terminal_size();

    Path f("data.txt");
    auto content = f.read_text();
    print(content);
    f.write_text("Hello, File!");
}
```

---

## 文档

完整 API 文档请访问：

## [https://mrxie1109.github.io/console](https://mrxie1109.github.io/console)

---

## 许可证

本库基于 [MIT 许可证](LICENSE) 分发。

---

## 贡献

欢迎贡献！请提交 Issue 或 Pull Request。

---

## 作者

**MrXie1109**
