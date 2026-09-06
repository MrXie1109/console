# Console Library

**A Modern C++ Console Utility Library** | **v8.0.0** · _"One, Two, Three, Four."_

[![C++11](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](https://en.cppreference.com/w/cpp/11)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Documentation](https://img.shields.io/badge/docs-online-brightgreen.svg)](https://mrxie1109.github.io/console)

**[README: American English](README.md)** |
**[README: 简体中文](README_zh.md)**

---

## Overview

Console is a comprehensive, header-only C++ library that provides a wide range of utilities for console/terminal application development. It aims to offer Python-like convenience while maintaining C++ performance and type safety.

Note: `Console` is just a symbol name, don't worry about it.

**Key capabilities include:**

- **Containers & Data Structures** —— Container adapters with value-returning pop, copy-on-write Cow, Maybe/Result, tagged Union, non-owning View, compile-time fixed-dimension MultiArray
- **Functional Programming** —— Generator pipelines with lazy evaluation, lightweight coroutine/generator macros
- **Text Processing** —— String utilities, formatting, regex, repr representation, INI config parsing
- **Async & Concurrency** —— Thread pool, MPMC channels, task groups, cooperative threads, scheduler, lock-free queues
- **Numeric Computing** —— Random number generation, rational numbers, matrix operations, pi constant
- **Cryptography & Encoding** —— AES-128, SHA256, MD5, Base64
- **I/O & Terminal Control** —— Colorful output, progress bars, cross-platform keyboard input, terminal control, file operations
- **Utilities** —— Scope exit guard defer, argument parsing, system commands, type name demangling, unit testing framework, time measurement
- **Cross-platform** —— Windows MIDI playback, Linux process management

---

## Requirements

- **C++11** or later (full compatibility)
- Standard Library
- CMake (optional, for building tests/examples)
- On Windows: `winmm.lib` for MIDI support
- On Linux: standard POSIX headers

---

## Quick Start

### Basic Usage

```cpp
#include <console/all.h>
using namespace console;

int main() {
    std::cout << color::Red << "Hello" << color::Reset << " World\n";

    print("The answer is", 42);

    logger.info("Application started");
    logger.warn("Low memory warning");

    for (auto i : gen::range(10)) {
        print(i);
    }

    auto msg = format("Value: {:.2f}", 3.14159);

    return 0;
}
```

---

### Containers

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

### Generators & Pipelines

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

### Thread Pool & Concurrency

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
    for (int value : ch) print("Received:", value);
    producer.join();

    Channel<std::string, 0> sync_ch;
    std::thread worker([&sync_ch]() {
        std::string msg;
        sync_ch >> msg;
        print("Got:", msg);
    });
    sync_ch << "Hello from main!";
    worker.join();
}

void task_group_example() {
    Group group(3, []() { print("All tasks completed!"); });

    std::thread t1([&group]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
        print("Task 1 finished");
        group.done();
    });
    std::thread t2([&group]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(400));
        print("Task 2 finished");
        group.done();
    });
    std::thread t3([&group]() {
        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        print("Task 3 finished");
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
        print("Thread stopped gracefully");
    });
    std::this_thread::sleep_for(std::chrono::seconds(2));
    t.stop();
    t.join();

    Scheduler sched;
    sched.schedule(1.0, []() { print("One-shot after 1s"); });
    int count = 0;
    sched.interval(0.5, [&count, &sched]() {
        print("Tick", ++count);
        if (count >= 5) sched.cancel_all();
    });
    std::this_thread::sleep_for(std::chrono::seconds(4));
}
```

---

### Numeric Computing

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

### Text Processing

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
    auto msg = format("Value: {:.2f}, Name: {}", 3.14159, "Alice");
    print(msg);  // Value: 3.14, Name: Alice

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
    auto m = r.search("Date: 2026-08-07");
    if (m) print("Found:", m.group(0));

    auto matches = r.findall("2026-08-07 2026-08-08 2026-08-09");
    auto parts = re::split(R"(\s+)", "one two three");
}
```

---

### Utilities

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
    print("Elapsed:", elapsed);
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
    print("Exit code:", result.exit_code);

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

    logger.info("Application started");
    logger.warn("Low memory warning");
    logger.error("Failed to open file");
    logger.set(Logging::Level::Warning);

    TEST(AdditionWorks) {
        ASSERT_EQ(2 + 2, 4);
    }

    TEST(ContainerContains) {
        std::vector<int> v = {1, 2, 3, 4};
        ASSERT_CONTAINS(v, 3);
        ASSERT_SIZE_EQ(v, 4);
    }

    TEST(ExceptionThrows) {
        ASSERT_THROWS(
            throw std::runtime_error("error"),
            std::runtime_error
        );
    }

    TEST_MAIN
}
```

---

### I/O & Terminal Control

```cpp
#include <console/all.h>
using namespace console;

void colorful_output_example() {
    std::cout << color::Red << "Red" << color::Reset << " "
              << color::Green << "Green" << color::Reset << "\n";
    std::cout << color::BgBlue << "Blue Background" << color::Reset << "\n";
    std::cout << color::Bold << "Bold" << color::Reset << "\n";
    std::cout << color::Underline << "Underline" << color::Reset << "\n";
}

void input_progress_example() {
    if (kbhit()) {
        int ch = getch();
        print("Key pressed:", ch);
    }

    std::vector<int> data(100);
    ProgressConfig cfg(std::cout);
    for (auto _ : progress(data, cfg)) {
        // Progress automatically updated
    }
}

void term_file_example() {
    clear();
    gotoxy(10, 5);
    hide_cursor();
    std::cout << "Hello at position";
    show_cursor();
    auto [rows, cols] = get_terminal_size();

    Path f("data.txt");
    auto content = f.read_text();
    print(content);
    f.write_text("Hello, File!");
}
```

---

## Documentation

Full API documentation is available at:

## [https://mrxie1109.github.io/console](https://mrxie1109.github.io/console)

---

## License

This library is distributed under the [MIT License](LICENSE).

---

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

---

## Author

**MrXie1109**
