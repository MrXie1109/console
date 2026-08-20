# Console Library

**A Modern C++ Console Utility Library** | **v7.4.0** · _"Re-Go!"_

[![C++11](https://img.shields.io/badge/C%2B%2B-11-blue.svg)](https://en.cppreference.com/w/cpp/11)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Documentation](https://img.shields.io/badge/docs-online-brightgreen.svg)](https://mrxie1109.github.io/console)

**[README: American English](README.md)** |
**[README: 简体中文](README_zh.md)**

---

## Overview

Console is a comprehensive, header-only C++ library that provides a wide range of utilities for console/terminal application development. It aims to offer Python-like convenience while maintaining C++ performance and type safety.

P.S.: `Console` is just a symbol, don't care it.

**Key capabilities include:**

- **Container adapters** with value-returning `pop()` operations
- **Multi-dimensional arrays** with compile-time fixed dimensions
- **Functional programming** with generator pipelines and lazy evaluation
- **Type-safe formatting** with Python-style `format()` and `fmt()`
- **Logging** with color-coded severity levels
- **Regex** with Python `re`-like interface
- **Random number generation** with extensive distributions
- **Thread pool** for parallel task execution
- **Progress bars** for iteration visualization
- **AES-128 encryption**, **SHA256**, **MD5**, and **Base64**
- **INI configuration** file parsing
- **Cross-platform terminal control** (colors, cursor, screen)
- **MIDI playback** on Windows
- **Process management** on Linux
- **Result/Optional types** similar to Rust
- **Cooperative thread management** with Event-based stop mechanism
- **Asynchronous tasks** -- comes with a bunch of async capabilities
- **Scope exit guards** using the `defer` macro (RAII-style)
- **Unit testing framework** with macro-based assertions and automatic test registration (assertions, exception testing, performance benchmarking)
- **Copy-on-Write** `Cow` class for lazy copying of shared data
- And much more...

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
/*
// Or include it when needed
#include <console/output.h>
#include <console/logging.h>
#include <console/gen.h>
#include <console/fmt.h>
 */
using namespace console;

int main() {
    // Colorful output
    std::cout << color::Red << "Hello" << color::Reset << " World\n";

    // Python-style print
    print("The answer is", 42);

    // Logging
    logger.info("Application started");
    logger.warn("Low memory warning");

    // Range-based iteration with generators
    for (auto i : gen::range(10)) {
        print(i);
    }

    // Formatting
    auto msg = format("Value: {:.2f}", 3.14159);

    return 0;
}
```

### Working with Containers

```cpp
#include <console/all.h>
using namespace console;

void container_example() {
    // Stack with value-returning pop
    Stack<int> s;
    s.push(1);
    s.push(2);
    int value = s.pop();  // returns 2

    // Queue
    Queue<std::string> q;
    q.push("first");
    q.push("second");
    auto str = q.pop();   // returns "first"

    // Priority Queue
    PriorityQueue<int> pq;
    pq.push(5);
    pq.push(1);
    pq.push(3);
    int max = pq.pop();   // returns 5
}
```

### Generators and Pipelines

```cpp
#include <console/all.h>
using namespace console;

void generator_example() {
    // Create a pipeline using the pipe operator
    auto result = gen::range(1, 20)
        | gen::filter(ops::even)
        | gen::map(ops::square)
        | gen::collect();

    // result = [4, 16, 36, 64, 100, 144, 196, 256, 324]

    // Chain generators
    auto nums = gen::range(5) + gen::range(10, 15);

    // Enumerate with index
    for (auto [idx, val] : gen::range(5) | gen::enumerate) {
        print(idx, ":", val);
    }
}
```

### Thread Pool

```cpp
#include <console/all.h>
using namespace console;

void thread_pool_example() {
    ThreadPool pool(4);  // 4 worker threads

    // Submit tasks and get futures
    auto f1 = pool.submit([](int x) { return x * x; }, 5);
    auto f2 = pool.submit([](int x) { return x + x; }, 10);

    int result1 = f1.get();  // 25
    int result2 = f2.get();  // 20

    // Map a function over a container in parallel
    std::vector<int> data = {1, 2, 3, 4, 5};
    auto futures = pool.map([](int x) { return x * 2; }, data);

    for (auto& f : futures) {
        print(f.get());
    }
}
```

### Multi-dimensional Arrays

```cpp
#include <console/all.h>
using namespace console;

void multiarray_example() {
    // 3x3 matrix
    MultiArray<double, 3, 3> A = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9
    };

    // Element-wise operations
    auto B = A + 10;
    auto C = A * 2.5;

    // Statistics
    double mean_val = mean(A);
    double std_val = stddev(A);

    // Matrix multiplication
    auto D = matmul(A, B);
}
```

### Regex

```cpp
#include <console/all.h>
using namespace console;

void regex_example() {
    // Compile pattern
    auto r = re::compile(R"(\d+-\d+-\d+)");

    // Search
    auto m = r.search("Date: 2026-08-07");
    if (m) {
        print("Found:", m.group(0));
    }

    // Find all matches
    auto matches = r.findall("2026-08-07 2026-08-08 2026-08-09");

    // Split
    auto parts = re::split(R"(\s+)", "one two three");
}
```

### Configuration Files

```cpp
#include <console/all.h>
using namespace console;

void config_example() {
    INIConfig config("settings.ini");

    // Read values (with automatic type conversion)
    int port = config.get("server.port", 8080);
    bool debug = config.get("app.debug", false);
    std::string host = config.get("server.host", "localhost");

    // Write values
    config.set("app.version", "2.0.0");
    config.save("settings.ini");
}
```

### Asynchronous Programming

```cpp
#include <console/all.h>
using namespace console;

// --- Thread with cooperative stop ---
void thread_example() {
    Thread t([](const Event& stop_event) {
        while (!stop_event.is_set()) {
            // Do work...
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        print("Thread stopped gracefully");
    });

    std::this_thread::sleep_for(std::chrono::seconds(2));
    t.stop();
    t.join();
}

// --- Asynchronous Tasks ---
void task_example() {
    Task<int> task([]() { return 42; });
    int result = task.get();  // 42

    SharedTask<int> shared([]() { return 100; });
    auto copy = shared;
    int r1 = copy.get();   // 100
    int r2 = shared.get(); // 100 (can call get() multiple times)
}

// --- Channel (MPMC Ring Buffer) ---
void channel_example() {
    // Buffered channel with capacity 10
    Channel<int, 10> ch;

    std::thread producer([&ch]() {
        for (int i = 0; i < 100; ++i) {
            ch << i;
        }
        close(ch);
    });

    for (int value : ch) {
        print("Received:", value);
    }
    producer.join();

    // Unbuffered channel (synchronous handshake)
    Channel<std::string, 0> sync_ch;

    std::thread worker([&sync_ch]() {
        std::string msg;
        sync_ch >> msg;
        print("Got:", msg);
    });

    sync_ch << "Hello from main!";
    worker.join();
}

// --- Task Group ---
void group_example() {
    // Create a group to manage async tasks
    Group group(3, []() {
        print("All tasks completed!");
    });

    // Launch three tasks concurrently
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

    // Wait for all tasks to complete
    group.wait();

    t1.join();
    t2.join();
    t3.join();
    // Output:
    // Task 1 finished
    // Task 2 finished
    // Task 3 finished
    // All tasks completed!
}
```

### Defer (Scope Exit Guard)

```cpp
#include <console/all.h>
using namespace console;

void defer_example() {
    FILE* file = fopen("data.txt", "r");
    if (!file) return;

    // Ensure the file is closed when leaving the scope
    defer(fclose(file));

    // Use the file...
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        print(buffer);
    }
    // fclose(file) is automatically called here
}
```

### Unit Testing

```cpp
#include <console/all.h>
using namespace console;

TEST(AdditionWorks) {
    ASSERT_EQ(2 + 2, 4); // assert 2 + 2 equals 4
}

TEST(ContainerContains) {
    std::vector<int> v = {1, 2, 3, 4};
    ASSERT_CONTAINS(v, 3); // assert v contains 3
    ASSERT_SIZE_EQ(v, 4); // assert v's size equals 4
}

TEST(ExceptionThrows) {
    ASSERT_THROWS(
        throw std::runtime_error("error"),
        std::runtime_error
    ); // assert throws std::runtime_error
}

TEST_MAIN // equivalent to `int main() {}`
```

### Copy-on-Write

```cpp
#include <console/all.h>
using namespace console;

void cow_example() {
    // Create a Cow wrapping an initial value
    Cow<std::string> str("Hello");

    // Read the underlying data (no copy occurs)
    print(str.reader());              // Hello
    print(str.read([](const auto& s) { return s.size(); }));  // 5

    // Create a copy-on-write copy (shares underlying data)
    Cow<std::string> copy = str;

    // Both share the same underlying data (no copy yet)
    print(copy == str);               // true

    // Writing triggers detach (copies underlying data first)
    str.write([](std::string& s) { s += " World"; });

    // Now `str` and `copy` hold independent data
    print(str.reader());              // Hello World
    print(copy.reader());             // Hello
}
```

---

## Documentation

Full API documentation is available at:

## **[https://mrxie1109.github.io/console](https://mrxie1109.github.io/console)**

---

## License

This library is distributed under the [MIT License](LICENSE).

---

## Contributing

Contributions are welcome! Please open an issue or submit a pull request.

---

## Author

**MrXie1109**
