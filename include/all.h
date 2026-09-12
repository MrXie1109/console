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
#include "./async.h"
#include "./container.h"
#include "./core.h"
#include "./io.h"
#include "./numeric.h"
#include "./text.h"
#include "./util.h"
#include "./wav.h"

/**
 * @namespace console
 * @brief 本库所有组件所在的顶层命名空间。
 */
namespace console {
    /// @brief 本库的元数据，用于标识库的版本和作者 (顺便充当二进制水印)。
    static constexpr char meta[] = "CONSOLE-LIB-2026-09-12-MRXIE1109";
}
