/**
 * @file all.h
 * @brief 一键式包含头文件。
 * @details 本文件包含了整个库的所有核心模块，方便用户快速引入。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 */

/*
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
*/

#pragma once
#include "adapter.h"
#include "box.h"
#include "colorful.h"
#include "config.h"
#include "crypto.h"
#include "csexc.h"
#include "cursor_ptr.h"
#include "file.h"
#include "fmt.h"
#include "gen.h"
#include "gextra.h"
#include "info.h"
#include "input.h"
#include "iter.h"
#include "kb.h" // windows.h on Windows
#include "literals.h"
#include "logging.h"
#include "matools.h"
#include "maybe.h"
#include "multiarray.h"
#include "outfwd.h"
#include "output.h"
#include "pool.h"
#include "progress.h"
#include "put.h"
#include "random.h"
#include "rational.h"
#include "re.h"
#include "repr.h"
#include "screen.h"
#include "sfinae.h"
#include "std.h"
#include "strpp.h"
#include "syscmd.h"
#include "term.h" // windows.h on Windows
#include "time.h"
#include "view.h"

// #include "win/melody.h" // 需要链接，所以不自动包含

#ifdef __linux__
#include "linux/process.h"
#endif

/**
 * @namespace console
 * @brief 本库所有组件所在的顶层命名空间。
 */
namespace console {
    static constexpr char meta[] = "CONSOLE-LIB-2026-07-21-MRXIE1109";
}
