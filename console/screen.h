/**
 * @file screen.h
 * @brief
 * 使用8点位Unicode盲文实现的控制台艺术，需要终端支持，建议设置合理的字体和大小。
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
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <utility>

namespace console {
    /**
     * @class Screen
     * @brief 用于表示虚拟屏幕的类。
     * @details 使用八点位位盲文，每像素占用一比特位存储空间。
     * @tparam COLS 列数，必须是 2 的倍数。
     * @tparam ROWS 行数，必须是 4 的倍数。
     */
    template <unsigned COLS, unsigned ROWS>
    class Screen {
        /// 列数必须是 2 的倍数。
        static_assert(COLS % 2 == 0, "COLS must be a multiple of 2!");
        /// 行数必须是 4 的倍数。
        static_assert(ROWS % 4 == 0, "ROWS must be a multiple of 4!");

        std::array<std::array<unsigned char, COLS / 2>, ROWS / 4>
            masks; ///< 存储数据的二维数组

    public:
        /// @brief 默认构造函数，构造一个空白的 Screen
        Screen() : masks() {}

        /**
         * @brief 在屏幕上绘制一个点。
         * @param x 点的 X 坐标，向右为正。
         * @param y 点的 y 坐标，向下为正。
         * @param on 绘制 or 擦除？
         */
        void point(unsigned x, unsigned y, bool on = true) {
            if (x >= COLS || y >= ROWS) return; ///< 静默忽略，就当画在外边了
            unsigned                   braille_col = x / 2;
            unsigned                   braille_row = y / 4;
            unsigned                   sub_x       = x % 2;
            unsigned                   sub_y       = y % 4;
            static const unsigned char mapping[2][4]
                = {{0, 1, 2, 6}, {3, 4, 5, 7}};
            unsigned char bit = 1 << mapping[sub_x][sub_y];
            if (on)
                masks[braille_row][braille_col] |= bit;
            else
                masks[braille_row][braille_col] &= ~bit;
        }

        /**
         * @brief 在屏幕上绘制一条直线。
         * @details Bresenham 画线算法，效率通常足够。
         * @param x0 起始点的 x 坐标。
         * @param y0 起始点的 y 坐标。
         * @param x1 终止点的 x 坐标。
         * @param y1 终止点的 y 坐标。
         * @param on 绘制 or 擦除？
         */
        void line(int x0, int y0, int x1, int y1, bool on = true) {
            int dx  = std::abs(x1 - x0);
            int dy  = -std::abs(y1 - y0);
            int sx  = x0 < x1 ? 1 : -1;
            int sy  = y0 < y1 ? 1 : -1;
            int err = dx + dy;
            while (true) {
                point(x0, y0, on);
                if (x0 == x1 && y0 == y1) break;
                int e2 = 2 * err;
                if (e2 >= dy) err += dy, x0 += sx;
                if (e2 <= dx) err += dx, y0 += sy;
            }
        }

        /**
         * @brief 在屏幕上绘制空心矩形。
         * @details 仅调用 4 次 line，效率通常足够。
         * @param x0 点 A 的 x 坐标。
         * @param y0 点 A 的 y 坐标。
         * @param x1 点 B 的 x 坐标。
         * @param y1 点 B 的 y 坐标。
         * @param on 绘制 or 擦除？
         */
        void rect(int x0, int y0, int x1, int y1, bool on = true) {
            line(x0, y0, x1, y0, on);
            line(x1, y0, x1, y1, on);
            line(x1, y1, x0, y1, on);
            line(x0, y1, x0, y0, on);
        }

        /**
         * @brief 在屏幕上绘制实心矩形。
         * @details 仅调用若干次 point，效率通常足够。
         * @param x0 点 A 的 x 坐标。
         * @param y0 点 A 的 y 坐标。
         * @param x1 点 B 的 x 坐标。
         * @param y1 点 B 的 y 坐标。
         * @param on 绘制 or 擦除？
         */
        void fillRect(int x0, int y0, int x1, int y1, bool on = true) {
            if (x0 > x1) std::swap(x0, x1);
            if (y0 > y1) std::swap(y0, y1);
            for (int y = y0; y <= y1; ++y)
                for (int x = x0; x <= x1; ++x) point(x, y, on);
        }

        /**
         * @brief 在屏幕上绘制空心圆。
         * @details Bresenham 中点圆算法，效率通常足够。
         * @param cx 圆中点的 x 坐标。
         * @param cy 中点的 y 坐标。
         * @param r 圆的半径。
         * @param on 绘制 or 擦除？
         */
        void circle(int cx, int cy, int r, bool on = true) {
            int x = 0, y = r;
            int d = 3 - 2 * r;
            while (x <= y) {
                point(cx + x, cy + y, on);
                point(cx - x, cy + y, on);
                point(cx + x, cy - y, on);
                point(cx - x, cy - y, on);
                point(cx + y, cy + x, on);
                point(cx - y, cy + x, on);
                point(cx + y, cy - x, on);
                point(cx - y, cy - x, on);
                if (d < 0)
                    d = d + 4 * x + 6;
                else
                    d = d + 4 * (x - y) + 10, y--;
                x++;
            }
        }

        /**
         * @brief 在屏幕上绘制实心圆。
         * @details 基于中点圆的扫描线填充算法，效率通常足够。
         * @param cx 圆中点的 x 坐标。
         * @param cy 中点的 y 坐标。
         * @param r 圆的半径。
         * @param on 绘制 or 擦除？
         */
        void fillCircle(int cx, int cy, int r, bool on = true) {
            int x = 0, y = r;
            int d = 3 - 2 * r;
            while (x <= y) {
                for (int i = cx - x; i <= cx + x; ++i) {
                    point(i, cy + y, on);
                    point(i, cy - y, on);
                }
                for (int i = cx - y; i <= cx + y; ++i) {
                    point(i, cy + x, on);
                    point(i, cy - x, on);
                }

                if (d < 0) {
                    d += 4 * x + 6;
                } else {
                    d += 4 * (x - y) + 10;
                    y--;
                }
                x++;
            }
        }

        /// @brief 清空屏幕。
        void clear() {
            for (auto &row : masks)
                for (auto &cell : row) cell = 0;
        }

        /**
         * @brief 将 Screen 存储的虚拟屏幕输出到指定流。
         * @details 内部使用临时字符串减少输出次数，减少 IO 导致的效率问题。
         * @tparam CharT 字符类型。
         * @tparam Traits 字符特征类型。
         * @param os 发送到的输出流。
         * @param clear 是否清空屏幕。
         * @note 控制台 IO 通常是效率瓶颈，建议手动控制帧率。
         */
        template <class CharT, class Traits = std::char_traits<CharT>>
        void update(std::basic_ostream<CharT, Traits> &os    = std::cout,
            bool                                       clear = false) const {
            if (clear) {
                os << "\033[2J";
            }
            std::string out;
            out.reserve((ROWS / 4) * (COLS / 2 * 3 + 1));
            for (unsigned row = 0; row < ROWS / 4; ++row) {
                for (unsigned col = 0; col < COLS / 2; ++col) {
                    unsigned int code = 0x2800 + masks[row][col];
                    out += static_cast<char>(0xE0 | (code >> 12));
                    out += static_cast<char>(0x80 | ((code >> 6) & 0x3F));
                    out += static_cast<char>(0x80 | (code & 0x3F));
                }
                out += '\n';
            }
            os << "\033[H" << out.c_str() << std::flush;
        }
    };
}
