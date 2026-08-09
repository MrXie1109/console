# Console Library

**A Modern C++ Console Utility Library**

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
- **Scope exit guards** using the `defer` macro (RAII-style)
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

### Thread with Cooperative Stop

```cpp
#include <console/all.h>
using namespace console;

void thread_example() {
    // Create a thread with a function that accepts an Event& for cooperative stop
    Thread t([](const Event& stop_event) {
        while (!stop_event.is_set()) {
            // Do work...
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
        print("Thread stopped gracefully");
    });

    // Let it run for a while
    std::this_thread::sleep_for(std::chrono::seconds(2));

    // Request the thread to stop
    t.stop();

    // Wait for the thread to finish (don't have to)
    t.join();
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
