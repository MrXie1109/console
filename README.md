# console —— 让 C++ 更简单

[![Language](https://img.shields.io/badge/language-C++11-blue.svg)](https://isocpp.org/)
[![Header-Only](https://img.shields.io/badge/header--only-yes-brightgreen.svg)](https://en.wikipedia.org/wiki/Header-only)
[![License](https://img.shields.io/badge/license-MIT-green.svg)](LICENSE)
[![Platform](https://img.shields.io/badge/platform-Windows%20%7C%20Linux%20%7C%20macOS-lightgrey.svg)]()

> **console** 是一个 **C++11 纯头文件库**，提供类似 Python 的简洁、现代化编程体验。无需复杂配置，`#include` 即用。

---

## 🎯 一句话概括

把 C++ 用出 Python 的舒适感，同时保留 C++ 的性能与掌控力。

---

## ✨ 核心特性

| 类别                        | 功能                                                                                         |
| --------------------------- | -------------------------------------------------------------------------------------------- |
| 📝 **字符串处理**           | `trim` / `split` / `join` / `upper` / `lower` / `title` / `f_string` 格式化 / `"_f"` 字面量  |
| 🎨 **控制台样式**           | ANSI 16色前景/背景 / 粗体/下划线/闪烁等样式 / `ColorGuard` RAII 守卫                         |
| ⌨️ **输入增强**             | 类型安全 `input<T>()` / 范围校验 / 行读取 / y/n 确认 / 非阻塞键盘 `Keyboard`                 |
| 📊 **容器输出**             | 直接 `cout << vector` / `map` / `set` / `tuple` / `pair` / `valarray`                        |
| 📦 **异构容器**             | `Box`（任意类型混合存储）+ `Item`（单值类型擦除）                                            |
| 🎯 **可选值**               | `Maybe<T>`（类似 `std::optional`，但更早支持 C++11）                                         |
| 🔄 **容器适配器**           | `Stack` / `Queue` / `PriorityQueue`，`pop()` 直接返回值                                      |
| 🧬 **生成器与管道**         | `range()` / `zip()` / `enumerate` / 惰性 `map`/`filter`/`take`/`drop` / 管道操作符 `\|`      |
| 📐 **多维数组**             | `MultiArray<T, Dims...>`（编译期维度）+ 元素级运算 + 统计函数 + 矩阵乘法                     |
| 🧮 **有理数**               | `Rational`（基于 `intmax_t` / `int64_t` 等），支持四则运算与比较                             |
| 🔐 **加密工具**             | SHA256 / MD5 / Base64 / AES-128-CTR                                                          |
| ⏱️ **时间工具**             | `Time` 类（纳秒精度）/ `now()` / `timer()` / `sleep()` / `datetime()` / `fps()` 帧率控制     |
| 🎲 **随机数**               | `randint` / `uniform` / `choice` / `shuffle` / 正态/泊松/伽马/贝塔/t/F/对数正态/韦布尔等分布 |
| 📁 **文件操作**             | `Path` 跨平台路径拼接 + 文本/二进制/POD 读写                                                 |
| 📜 **正则表达式**           | Python 风格 `re` 模块：`search` / `match` / `findall` / `finditer` / `sub` / `split`         |
| ⚙️ **配置解析**             | `INIConfig` INI 文件读写（支持空节/空键）                                                    |
| 📈 **进度条**               | 容器遍历自动显示进度条（多种预置样式）                                                       |
| 🖥️ **屏幕绘图**             | Unicode 盲文点阵屏：点 / 线 / 圆 / 矩形 / 实心填充                                           |
| 🎵 **MIDI 播放**（Windows） | 128 种 GM 乐器 / 音符播放 / BPM 控制 / 异步播放                                              |
| 🖱️ **系统命令**             | `syscmd()` 跨平台执行命令并捕获 stdout+stderr                                                |
| 🧵 **线程池**               | `ThreadPool` 类 + 全局线程池 / `submit` / `map` / `wait`                                     |
| 🔍 **双重分派**             | `Table<T>` 运行时双重分派表（碰撞检测/事件分发）                                             |
| 📎 **其他实用工具**         | `View` 非拥有视图 / `cursor_ptr` 双指针游标 / `Generator` 生成器框架 / 类型特征 `sfinae.h`   |

---

## 🚀 快速开始

### 环境要求

- **C++11** 或更高版本（部分高级特性需要 C++14/17，但核心模块兼容 C++11）
- 编译器：GCC 4.8+ / Clang 3.4+ / MSVC 2015+

### 使用方式

**纯头文件库**，复制 `console` 目录到项目，然后包含所需头文件即可：

```cpp
#include "console/all.h"          // 一键全包含
// 或按需包含：
#include "console/output.h"
#include "console/strpp.h"

int main() {
    std::vector<int> v = {1, 2, 3, 4, 5};
    console::print(v);                    // [1, 2, 3, 4, 5]

    std::string s = console::trim("  hello  ");  // "hello"

    for (int i : console::range(0, 10, 2)) {
        console::print(i);                // 0, 2, 4, 6, 8
    }

    return 0;
}
```

### 更多示例

```cpp
#include "console/all.h"
using namespace console;

int main() {
    // 1. 容器输出
    std::map<std::string, int> scores = {{"Alice", 95}, {"Bob", 87}};
    print(scores);   // {"Alice": 95, "Bob": 87}

    // 2. 字符串处理
    auto parts = split("a,b,c,d", ",");   // ["a", "b", "c", "d"]
    print(join(parts, " | "));            // "a | b | c | d"

    // 3. 格式化
    print("Hello, {}"_f % "world");       // "Hello, world"
    print("Pi ≈ {:.2f}"_f % 3.14159);     // "Pi ≈ 3.14"（需 C++11 的 to_string 支持）

    // 4. Maybe 安全处理
    Maybe<int> m = 42;
    if (m) {
        print(*m);                        // 42
    }
    m = nothing;                          // 置空

    // 5. 随机选择
    std::vector<std::string> fruits = {"apple", "banana", "cherry"};
    print(choice(fruits));                // 随机一个

    // 6. 进度条
    for (auto& x : progress(fruits, ProgressStyle::beautiful())) {
        // 自动显示进度条
    }

    // 7. 计时
    auto elapsed = timer([]{
        sleep(1_s);                       // _s 字面量，需要 using namespace literals;
    });
    print("Elapsed:", elapsed);           // 约 1s

    // 8. 正则表达式
    auto m = re::search(r"\d+", "age: 18");
    if (m) print(m.group());              // "18"

    // 9. 线程池
    auto pool = ThreadPool(4);
    auto f = pool.submit([]{ return 42; });
    print(f.get());                       // 42

    // 10. 生成器管道
    auto result = gen::range(1, 10)
                | gen::filter(ops::even)
                | gen::map(ops::square)
                | gen::collect<std::vector<int>>();
    print(result);                        // [4, 16, 36, 64]

    return 0;
}
```

---

## 📖 文档

完整 API 文档与进阶指南：  
**[https://mrxie1109.github.io/console/](https://mrxie1109.github.io/console/)**

---

## 📦 模块速览

| 头文件            | 描述                                                              |
| ----------------- | ----------------------------------------------------------------- |
| `all.h`           | 一键包含所有模块                                                  |
| `output.h`        | 容器输出 + `print` 函数                                           |
| `put.h`           | `put()` 单值输出（`repr` 适配）                                   |
| `repr.h`          | Python 风格 `repr()` 表示                                         |
| `strpp.h`         | 字符串处理 + `f_string` 格式化                                    |
| `literals.h`      | 时间字面量 `_s` / `_ms` / `_f` 等                                 |
| `colorful.h`      | ANSI 颜色/样式常量                                                |
| `input.h`         | 类型安全输入函数                                                  |
| `kb.h`            | 非阻塞键盘输入                                                    |
| `maybe.h`         | `Maybe<T>` 可选值                                                 |
| `box.h`           | 异构容器 `Box` + `Item`                                           |
| `adapter.h`       | `Stack` / `Queue` / `PriorityQueue`                               |
| `iter.h`          | `zip()` / `IteratorPair`                                          |
| `gen.h`           | 生成器框架 + `range` / `map` / `filter` / `take` / `drop` / 管道  |
| `gextra.h`        | 生成器扩展：随机/文件行/窗口/分块/链式/归约/扫描                  |
| `multiarray.h`    | 编译期多维数组 + 元素级运算                                       |
| `matools.h`       | `MultiArray` 数学工具：统计/线性代数/卷积/随机初始化              |
| `rational.h`      | 有理数 `Rational`                                                 |
| `time.h`          | `Time` / `now()` / `timer()` / `sleep()` / `datetime()` / `fps()` |
| `random.h`        | 随机数生成 + 多种分布                                             |
| `file.h`          | `Path` 跨平台文件操作                                             |
| `re.h`            | Python 风格正则表达式                                             |
| `config.h`        | `INIConfig` INI 解析                                              |
| `progress.h`      | 进度条                                                            |
| `screen.h`        | Unicode 盲文点阵屏                                                |
| `crypto.h`        | SHA256 / MD5 / Base64 / AES-128-CTR                               |
| `syscmd.h`        | 跨平台系统命令执行                                                |
| `pool.h`          | `ThreadPool` 线程池                                               |
| `collide.h`       | `Table<T>` 双重分派表                                             |
| `view.h`          | 非拥有视图 `View`                                                 |
| `cursor_ptr.h`    | 双指针游标智能指针                                                |
| `info.h`          | 版本/平台/编译器信息                                              |
| `csexc.h`         | 自定义异常层次                                                    |
| `sfinae.h`        | 类型特征检测工具                                                  |
| `outfwd.h`        | 容器输出前向声明                                                  |
| `std.h`           | 聚合 C++ 标准库头文件                                             |
| `term.h`          | 终端控制（光标/清屏/尺寸）                                        |
| `win/melody.h`    | Windows MIDI 音乐（需链接 `winmm`）                               |
| `linux/process.h` | Linux 进程管理                                                    |

---

## 🛠️ 构建与依赖

### 基本使用

**无需任何额外链接**，纯头文件，直接包含即可。

### Windows MIDI 支持

如需使用 `win/melody.h`，需链接 `winmm` 库：

```bash
# MinGW / GCC
g++ -std=c++11 main.cpp -lwinmm

# MSVC
cl /EHsc main.cpp winmm.lib
```

---

## 📝 命名空间

所有功能位于 `console` 命名空间：

```cpp
using namespace console;           // 推荐在 .cpp 中使用
// 或显式：
console::print("Hello");
console::range(0, 10);
```

子命名空间：

- `console::color` —— 颜色/样式常量
- `console::literals` —— 字面量运算符（需要 `using namespace console::literals;`）
- `console::re` —— 正则函数式接口
- `console::gen` —— 生成器工厂与适配器
- `console::ops` —— 预置变换器/谓词
- `console::crypto` —— 加密算法

---

## 🤝 贡献

欢迎提交 Issue 与 Pull Request！

---

## 📄 许可证

MIT License © 2026 MrXie1109

```
Copyright (c) 2026 MrXie1109

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```

---

## 💬 作者

**MrXie1109**

- GitHub: [@MrXie1109](https://github.com/MrXie1109)
- 文档: https://mrxie1109.github.io/console/

---

## ⭐ 支持

如果这个库对你有帮助，欢迎点亮 **Star** 支持一下！

---

> **console —— Make C++ Easy.**
