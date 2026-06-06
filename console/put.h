/**
 * @file put.h
 * @brief 提供在 output.h 中用于输出单个值的 put 函数。
 * @details
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
#include "repr.h"

namespace console
{
    /**
     * @brief 如果是字符和串或字符，直接输出，不含引号。
     * @tparam StrOrCharType
     * @param os 输出流。
     * @param value 要输出的值。
     * @note 直接输出。
     */
    template <class StrOrCharType>
    typename std::enable_if<
        is_string<typename std::decay<StrOrCharType>::type>::value ||
        is_char<typename std::decay<StrOrCharType>::type>::value>::type
    put(std::ostream &os, StrOrCharType &&value)
    {
        os << value;
    }

    /**
     * @brief 否则，调用 repr 格式化。
     * @tparam OtherType
     * @param os 输出流。
     * @param value 要输出的值。
     * @note 调用 repr(value, os)。
     */
    template <class OtherType>
    typename std::enable_if<
        !is_string<typename std::decay<OtherType>::type>::value &&
        !is_char<typename std::decay<OtherType>::type>::value>::type
    put(std::ostream &os, OtherType &&value)
    {
        repr(value, os);
    }
}
