/**
 * @file pool.h
 * @brief 简单的线程池实现，模仿了 Python 的 concurrent.futures.ThreadPoolExecutor 接口。
 * @details 该模块提供了一个高效的线程池实现，支持任务提交、批量映射执行、
 *          等待所有任务完成、优雅关闭等功能。适用于需要并发执行多个独立任务的场景。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @warning 该入口以被弃用，短期内保留以兼容，未来版本(v8.0.0)将移除。
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
#include "async/pool.h"

namespace console {}
