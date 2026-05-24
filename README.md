# console —— 让 C++ 更简单

[![Language](https://img.shields.io/badge/language-C++11-blue.svg)](https://isocpp.org/)
[![Header-Only](https://img.shields.io/badge/header--only-yes-brightgreen.svg)](https://en.wikipedia.org/wiki/Header-only)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows&Linux&macOS-lightgrey.svg)]()

> **console** 是一个 **C++11 纯头文件库**，旨在为 C++ 开发者提供类似 Python 的简洁、现代化编程体验。无需复杂的构建配置，只需包含头文件即可开始使用。

---

## ✨ 特性

| 类别 | 功能 |
|------|------|
| 📝 **字符串处理** | `trim`, `split`, `join`, `upper/lower`, `f_string` 格式化, `"_f"` 字面量 |
| 🎨 **控制台样式** | ANSI 颜色/背景/样式 (`color::Red`, `color::Bold` 等) |
| ⌨️ **输入增强** | 类型安全输入、范围检查、y/n 确认、非阻塞键盘输入 |
| 📊 **容器输出** | 直接 `cout << vector`, `map`, `set`, `tuple`, `pair` |
| 📦 **异构容器** | `Box` (类似 Python `list` 可存任意类型) + `Item` 类型擦除 |
| 🎯 **可选值** | `Maybe<T>` 安全处理空值 (类似 `std::optional`) |
| 🔄 **容器适配器** | `Stack`, `Queue`, `PriorityQueue` 带返回值 `pop()` |
| 🧬 **生成器 & 迭代器** | `range()`, `zip()`, `enumerate`, `map/filter` 惰性求值 |
| 📐 **多维数组** | `MultiArray<T, Dims...>` 编译期维度 + 数学运算 |
| 🧮 **分数运算** | `Rational` 有理数类型 (支持 `intmax_t`/`int64_t` 等) |
| 🔐 **加密工具** | SHA256, MD5, Base64, AES-128-CTR |
| ⏱️ **时间工具** | `Time` 类, `now()`, `timer()`, `sleep()`, `datetime()` |
| 🎲 **随机数** | `randint`, `uniform`, `choice`, `shuffle`, 多种分布 |
| 📁 **文件操作** | `Path` 跨平台路径 + 文本/二进制/POD 读写 |
| 📜 **正则表达式** | Python 风格 `re` 模块接口 (`search`, `match`, `findall`, `sub`) |
| ⚙️ **配置解析** | `INIConfig` INI 文件读写 |
| 📈 **进度条** | 容器遍历自动显示进度 |
| 🖥️ **屏幕绘图** | Unicode 盲文位图屏幕 (点、线、圆、矩形) |
| 🎵 **MIDI 播放** (Windows) | 128 种 GM 乐器, 音符播放, BPM 控制 |
| 🖱️ **系统命令** | 跨平台 `syscmd()` 执行并捕获输出 |

---

## 🚀 快速开始

### 环境要求

- **C++11** 或更高版本
- 支持的编译器：GCC, Clang, MSVC

### 使用方式

本库是 **纯头文件库**，只需将 `console` 文件夹复制到你的项目中，然后包含所需头文件即可：

```cpp
#include "console/all.h"  // 一键包含所有功能
// 或选择性包含
#include "console/output.h"
#include "console/strpp.h"

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    console::print(v);                    // 输出: [1, 2, 3, 4, 5]

    std::string s = console::trim("  hello  ");  // "hello"

    for (int i : console::range(0, 10, 2)) {
        console::print(i);                // 0, 2, 4, 6, 8
    }

    return 0;
}
```

### 基本示例

```cpp
#include "console/all.h"
using namespace console;

int main() {
    // 1. 打印容器
    std::map<std::string, int> scores = {{"Alice", 95}, {"Bob", 87}};
    print(scores);  // {"Alice": 95, "Bob": 87}

    // 2. 字符串处理
    auto parts = split("a,b,c,d", ",");  // ["a", "b", "c", "d"]

    // 3. 格式化输出
    print("Hello, {}"_f % "world");     // "Hello, world"

    // 4. Maybe 安全处理
    Maybe<int> m = 42;
    if (m) {
        print(*m);                       // 42
    }

    // 5. 随机选择
    std::vector<std::string> fruits = {"apple", "banana", "cherry"};
    auto selected = choice(fruits);      // 随机一个水果

    // 6. 进度条
    for (auto& x : progress(fruits)) {
        // 自动显示进度条
    }

    // 7. 计时
    auto elapsed = timer([]{
        sleep(1_s);
    });
    print("Elapsed:", elapsed);          // 约 1s

    return 0;
}
```

---

## 📖 文档

完整文档请访问：**[https://mrxie1109.github.io/console---make-cpp-easy/](https://mrxie1109.github.io/console---make-cpp-easy/)**

文档包含：
- 详细 API 参考
- 各模块使用指南
- 更多代码示例

---

## 📦 模块概览

| 头文件 | 描述 |
|--------|------|
| `all.h` | 一键包含所有模块 |
| `output.h` | 容器输出 + `print` 函数 |
| `strpp.h` | 字符串处理 + 格式化 |
| `colorful.h` | ANSI 颜色/样式常量 |
| `input.h` | 类型安全输入 |
| `kb.h` | 非阻塞键盘输入 |
| `maybe.h` | `Maybe<T>` 可选值 |
| `box.h` | 异构容器 `Box` + `Item` |
| `adapter.h` | `Stack`/`Queue`/`PriorityQueue` |
| `iter.h` | `zip()` 迭代器适配 |
| `gen.h` | 生成器 (range, map, filter, take, skip) |
| `multiarray.h` | 编译期多维数组 |
| `matools.h` | 多维数组数学工具 |
| `rational.h` | 分数运算 |
| `time.h` | 时间/计时/休眠 |
| `random.h` | 随机数生成 |
| `file.h` | 跨平台文件操作 |
| `re.h` | Python 风格正则 |
| `config.h` | INI 配置解析 |
| `progress.h` | 进度条显示 |
| `screen.h` | 盲文位图屏幕 |
| `crypto.h` | SHA256/MD5/Base64/AES |
| `syscmd.h` | 系统命令执行 |
| `info.h` | 版本/平台/编译器信息 |
| `csexc.h` | 自定义异常类 |
| `sfinae.h` | 类型特征检测 |
| `repr.h` | Python 风格 `repr()` |
| `literals.h` | 时间/格式化字面量 |
| `view.h` | 容器非拥有视图 |
| `cursor_ptr.h` | 双指针游标智能指针 |
| `win/melody.h` | Windows MIDI 音乐 (需链接 winmm) |

---

## 🛠️ 构建与依赖

### Windows (MIDI 支持)

如需使用 MIDI 功能 (`win/melody.h`)，需链接 `winmm` 库：

```bash
# g++
g++ -std=c++11 main.cpp -lwinmm

# MSVC
cl /EHsc main.cpp winmm.lib
```

### 其他情况

无需额外链接，直接包含头文件即可。

---

## 📝 命名空间

所有功能位于 `console` 命名空间中：

```cpp
using namespace console;  // 推荐在源文件中使用
// 或显式使用 console::print, console::range 等
```

---

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！

---

## 📄 许可证

本项目采用 **MIT 许可证**。详见 [LICENSE](LICENSE) 文件。

```
Copyright (c) 2026 MrXie1109

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions...
```

---

## 💬 作者

**MrXie1109**

- GitHub: [@MrXie1109](https://github.com/MrXie1109)
- 文档网站: https://mrxie1109.github.io/console---make-cpp-easy/

---

## 🌟 致谢

感谢所有使用和反馈的朋友们！如果觉得这个库对你有帮助，欢迎 **Star** ⭐ 支持一下！

---

# console —— Make C++ Easy

[![Language](https://img.shields.io/badge/language-C++11-blue.svg)](https://isocpp.org/)
[![Header-Only](https://img.shields.io/badge/header--only-yes-brightgreen.svg)](https://en.wikipedia.org/wiki/Header-only)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows&Linux&macOS-lightgrey.svg)]()

> **console** is a **C++11 header-only library** designed to provide C++ developers with a clean, modern programming experience similar to Python. No complex build configuration — just include the headers and start coding.

---

## ✨ Features

| Category | Features |
|----------|----------|
| 📝 **String Processing** | `trim`, `split`, `join`, `upper/lower`, `f_string` formatting, `"_f"` literal |
| 🎨 **Console Styling** | ANSI colors/backgrounds/styles (`color::Red`, `color::Bold`, etc.) |
| ⌨️ **Enhanced Input** | Type-safe input, range validation, y/n confirmation, non-blocking keyboard |
| 📊 **Container Output** | Direct `cout << vector`, `map`, `set`, `tuple`, `pair` |
| 📦 **Heterogeneous Container** | `Box` (like Python `list` storing any type) + `Item` type erasure |
| 🎯 **Optional Value** | `Maybe<T>` safe null handling (like `std::optional`) |
| 🔄 **Container Adapters** | `Stack`, `Queue`, `PriorityQueue` with value-returning `pop()` |
| 🧬 **Generators & Iterators** | `range()`, `zip()`, `enumerate`, lazy `map`/`filter` |
| 📐 **Multi-dimensional Array** | `MultiArray<T, Dims...>` compile-time dimensions + math operations |
| 🧮 **Rational Arithmetic** | `Rational` fraction type (`intmax_t`/`int64_t` based) |
| 🔐 **Cryptography** | SHA256, MD5, Base64, AES-128-CTR |
| ⏱️ **Time Utilities** | `Time` class, `now()`, `timer()`, `sleep()`, `datetime()` |
| 🎲 **Random Numbers** | `randint`, `uniform`, `choice`, `shuffle`, multiple distributions |
| 📁 **File Operations** | `Path` cross-platform path + text/binary/POD read/write |
| 📜 **Regular Expressions** | Python-style `re` module interface (`search`, `match`, `findall`, `sub`) |
| ⚙️ **Config Parsing** | `INIConfig` INI file reader/writer |
| 📈 **Progress Bar** | Automatic progress display for container iteration |
| 🖥️ **Screen Drawing** | Unicode Braille bitmap screen (points, lines, circles, rectangles) |
| 🎵 **MIDI Playback** (Windows) | 128 GM instruments, note playback, BPM control |
| 🖱️ **System Commands** | Cross-platform `syscmd()` execution with output capture |

---

## 🚀 Quick Start

### Requirements

- **C++11** or later
- Supported compilers: GCC, Clang, MSVC

### Usage

This is a **header-only library**. Simply copy the `console` folder into your project and include the desired header:

```cpp
#include "console/all.h"  // Include everything
// Or selectively include:
#include "console/output.h"
#include "console/strpp.h"

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    console::print(v);                    // Output: [1, 2, 3, 4, 5]

    std::string s = console::trim("  hello  ");  // "hello"

    for (int i : console::range(0, 10, 2)) {
        console::print(i);                // 0, 2, 4, 6, 8
    }

    return 0;
}
```

### Basic Examples

```cpp
#include "console/all.h"
using namespace console;

int main() {
    // 1. Print containers
    std::map<std::string, int> scores = {{"Alice", 95}, {"Bob", 87}};
    print(scores);  // {"Alice": 95, "Bob": 87}

    // 2. String processing
    auto parts = split("a,b,c,d", ",");  // ["a", "b", "c", "d"]

    // 3. Formatted output
    print("Hello, {}"_f % "world");     // "Hello, world"

    // 4. Maybe for safe handling
    Maybe<int> m = 42;
    if (m) {
        print(*m);                       // 42
    }

    // 5. Random selection
    std::vector<std::string> fruits = {"apple", "banana", "cherry"};
    auto selected = choice(fruits);      // random fruit

    // 6. Progress bar
    for (auto& x : progress(fruits)) {
        // Progress bar automatically displayed
    }

    // 7. Timing
    auto elapsed = timer([]{
        sleep(1_s);
    });
    print("Elapsed:", elapsed);          // ~1s

    return 0;
}
```

---

## 📖 Documentation

Full documentation is available at: **[https://mrxie1109.github.io/console---make-cpp-easy/](https://mrxie1109.github.io/console---make-cpp-easy/)**

The documentation includes:
- Detailed API reference
- Module usage guides
- More code examples

---

## 📦 Module Overview

| Header | Description |
|--------|-------------|
| `all.h` | One‑include for all modules |
| `output.h` | Container output + `print` function |
| `strpp.h` | String processing + formatting |
| `colorful.h` | ANSI color/style constants |
| `input.h` | Type‑safe input |
| `kb.h` | Non‑blocking keyboard input |
| `maybe.h` | `Maybe<T>` optional value |
| `box.h` | Heterogeneous `Box` + `Item` |
| `adapter.h` | `Stack`/`Queue`/`PriorityQueue` |
| `iter.h` | `zip()` iterator adapter |
| `gen.h` | Generators (range, map, filter, take, skip) |
| `multiarray.h` | Compile‑time multi‑dimensional array |
| `matools.h` | Math tools for MultiArray |
| `rational.h` | Fraction arithmetic |
| `time.h` | Time/timer/sleep utilities |
| `random.h` | Random number generation |
| `file.h` | Cross‑platform file operations |
| `re.h` | Python‑style regex |
| `config.h` | INI configuration parsing |
| `progress.h` | Progress bar display |
| `screen.h` | Braille bitmap screen |
| `crypto.h` | SHA256/MD5/Base64/AES |
| `syscmd.h` | System command execution |
| `info.h` | Version/platform/compiler info |
| `csexc.h` | Custom exceptions |
| `sfinae.h` | Type trait detection |
| `repr.h` | Python‑style `repr()` |
| `literals.h` | Time/formatting literals |
| `view.h` | Non‑owning container view |
| `cursor_ptr.h` | Dual‑pointer cursor smart pointer |
| `win/melody.h` | Windows MIDI music (requires winmm linkage) |

---

## 🛠️ Build & Dependencies

### Windows (MIDI Support)

To use MIDI functionality (`win/melody.h`), link against `winmm`:

```bash
# g++
g++ -std=c++11 main.cpp -lwinmm

# MSVC
cl /EHsc main.cpp winmm.lib
```

### Otherwise

No additional linking required — just include the headers.

---

## 📝 Namespace

All functionality resides in the `console` namespace:

```cpp
using namespace console;  // Recommended in source files
// Or explicitly: console::print, console::range, etc.
```

---

## 🤝 Contributing

Issues and Pull Requests are welcome!

---

## 📄 License

This project is licensed under the **MIT License**. See [LICENSE](LICENSE) for details.

```
Copyright (c) 2026 MrXie1109

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions...
```

---

## 💬 Author

**MrXie1109**

- GitHub: [@MrXie1109](https://github.com/MrXie1109)
- Documentation: https://mrxie1109.github.io/console---make-cpp-easy/

---

## 🌟 Acknowledgments

Thanks to all who use and provide feedback! If this library helps you, please give it a **Star** ⭐ on GitHub!
