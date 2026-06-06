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
#include "std.h"
#include "output.h"
#include "input.h"
#include "time.h"
#include "random.h"
#include "strpp.h"
#include "colorful.h"
#include "logging.h"
#include "progress.h"
#include "csexc.h"
#include "file.h"
#include "box.h"
#include "multiarray.h"
#include "cursor_ptr.h"
#include "literals.h"
#include "view.h"
#include "compre.h"
#include "sfinae.h"
#include "re.h"
#include "info.h"
#include "repr.h"
#include "outfwd.h"
#include "maybe.h"
#include "matools.h"
#include "iter.h"
#include "rational.h"
#include "screen.h"
#include "kb.h" // windows.h
#include "adapter.h"
#include "syscmd.h"
#include "config.h"
#include "crypto.h"
#include "gen.h"
#include "put.h"
#include "pool.h"

// #include "win/melody.h"

/**
 * @namespace console
 * @brief 本库所有组件所在的顶层命名空间。
 */
namespace console
{
    static constexpr char meta[] = "CONSOLE-LIB-2026-06-06-MRXIE1109";
}
