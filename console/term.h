/**
 * @file term.h
 * @brief 提供控制台控制功能。
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
#include <cstdio>
#include <utility>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN // 排除不常用的 Windows API
#define VC_EXTRALEAN        // 排除某些 MFC 扩展
#define NOCOMM              // 不包含 comm 相关
#define NOCRYPT             // 不包含加密 API
#define NOMCX               // 不包含 Modem 扩展
#define NOSERVICE           // 不包含服务相关
#define NOATOM              // 不包含原子操作
#define NOGDICAPMASKS       // 不包含 GDI 掩码
#define NOMETAFILE          // 不包含 MetaFile
#define NOMINMAX 1          // 不定义 min/max 宏
#define NOMSG               // 不包含消息结构
#define NOOPENFILE          // 不包含 OpenFile
#define NORPC               // 不包含 RPC
#define NOSOUND             // 不包含声音
#define NOSYSCOMMANDS       // 不包含系统命令
#define NOTEXTMETRIC        // 不包含 TEXTMETRIC
#define NOWH                // 不包含 WH_* 钩子
#define NOKANJI             // 不包含日文支持
#define NOHELP              // 不包含帮助
#define NODRAWTEXT          // 不包含 DrawText
#define NOGDI               // 不包含 GDI
#define NOUSER              // 不包含 USER
#define NOKERNEL            // 不包含 KERNEL
#define NOMB                // 不包含 MB_* 宏
#define NOMEMMGR            // 不包含内存管理
#define NOMETAFILE          // 不包含 MetaFile
#define NOMSG               // 不包含消息
#define NOOPENFILE          // 不包含 OpenFile
#define NOSCROLL            // 不包含滚动条
#define NOSHOW              // 不包含 ShowWindow
#define NOSIZE              // 不包含 SIZE
#define NOSOUND             // 不包含声音
#define NOSTRING            // 不包含字符串
#define NOSYSMETRICS        // 不包含系统指标
#define NOTEXTMETRIC        // 不包含 TEXTMETRIC
#define NOWINMESSAGES       // 不包含窗口消息
#define NOWINSTYLES         // 不包含窗口样式
#define NOWINOFFSETS        // 不包含窗口偏移
#define NOSYSCOMMANDS       // 不包含系统命令
#define NOCLIPBOARD         // 不包含剪贴板
#define NOCOLOR             // 不包含颜色
#define NODEFERWINDOWPOS    // 不包含 DeferWindowPos
#define NODRAWTEXT          // 不包含 DrawText
#define NOGDI               // 不包含 GDI
#define NOGDICAPMASKS       // 不包含 GDI 掩码
#define NOHELP              // 不包含帮助
#define NOICONS             // 不包含图标
#define NOMENUS             // 不包含菜单
#define NOMETAFILE          // 不包含 MetaFile
#define NOMSG               // 不包含消息
#define NOOPENFILE          // 不包含 OpenFile
#define NOPROFILER          // 不包含性能分析
#define NORASTEROPS         // 不包含光栅操作
#define NOSCROLL            // 不包含滚动条
#define NOSOUND             // 不包含声音
#define NOSYSCOMMANDS       // 不包含系统命令
#define NOTEXTMETRIC        // 不包含文本度量
#define NOWH                // 不包含 WH_* 钩子
#define NOWINOFFSETS        // 不包含窗口偏移
#define NOMB                // 不包含 MB_* 宏
#include <windows.h>
#endif

#ifdef __unix__
#include <sys/ioctl.h> // ioctl, TIOCGWINSZ
#include <unistd.h>    // STDOUT_FILENO
#endif

namespace console {
    /**
     * @brief 将光标移动到指定位置。
     * @param x 列坐标（从 1 开始）
     * @param y 行坐标（从 1 开始）
     */
    inline void gotoxy(int x, int y) noexcept {
        printf("\033[%d;%dH", y, x);
    }

    /**
     * @brief 清空整个屏幕并将光标复位到 (1,1)。
     */
    inline void clear() noexcept {
        printf("\033[2J\033[H");
    }

    /**
     * @brief 保存当前光标位置。
     */
    inline void save_cursor() noexcept {
        printf("\033[s");
    }

    /**
     * @brief 恢复之前保存的光标位置。
     */
    inline void restore_cursor() noexcept {
        printf("\033[u");
    }

    /**
     * @brief 隐藏光标（提高视觉效果）。
     */
    inline void hide_cursor() noexcept {
        printf("\033[?25l");
    }

    /**
     * @brief 显示光标（恢复默认）。
     */
    inline void show_cursor() noexcept {
        printf("\033[?25h");
    }

    /**
     * @brief 获取终端尺寸（行数和列数）。
     * @return std::pair<int, int>，first为rows，second为cols。
     * @note 失败时返回 {0, 0}。
     */
    inline std::pair<int, int> get_terminal_size() noexcept {
#ifdef _WIN32
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        HANDLE                     hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
        if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
            int cols = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            int rows = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
            return {rows, cols};
        }
        return {0, 0};
#elif defined(__unix__) || defined(__APPLE__)
        struct winsize w;
        if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0)
            return {w.ws_row, w.ws_col};
        return {0, 0};
#else
        return {0, 0};
#endif
    }

    /**
     * @brief 重置终端所有属性（颜色、样式、光标等）到默认状态。
     */
    inline void reset() noexcept {
        printf("\033[0m\033[?25h");
    }

    /**
     * @brief 清空当前光标所在行。
     */
    inline void clear_line() noexcept {
        printf("\033[2K");
    }

    /**
     * @brief 清空从光标位置到行尾。
     */
    inline void clear_to_eol() noexcept {
        printf("\033[K");
    }

    /**
     * @brief 向左相对移动。
     * @param n 移动的步数，默认为1。
     */
    inline void movel(int n = 1) noexcept {
        printf("\033[%dD", n);
    }

    /**
     * @brief 向右相对移动。
     * @param n 移动的步数，默认为1。
     */
    inline void mover(int n = 1) noexcept {
        printf("\033[%dC", n);
    }

    /**
     * @brief 向上相对移动。
     * @param n 移动的步数，默认为1。
     */
    inline void moveu(int n = 1) noexcept {
        printf("\033[%dA", n);
    }

    /**
     * @brief 向下相对移动。
     * @param n 移动的步数，默认为1。
     */
    inline void moved(int n = 1) noexcept {
        printf("\033[%dB", n);
    }

    /**
     * @brief 保存屏幕内容。
     */
    inline void save_screen() noexcept {
        printf("\033[?47h");
    }

    /**
     * @brief 恢复屏幕内容。
     */
    inline void restore_screen() noexcept {
        printf("\033[?47l");
    }

    /**
     * @brief 清空屏幕并保留历史。
     */
    inline void clear_screen_save() noexcept {
        printf("\033[?47h\033[2J\033[H");
    }

#ifdef _WIN32
    /**
     * @brief 初始化 Windows 虚拟终端支持。
     * @return true 如果成功启用虚拟终端，false 否则。
     */
    inline const bool &init_windows_terminal() noexcept {
        static bool initialized = []() noexcept -> bool {
            HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
            if (hConsole == INVALID_HANDLE_VALUE || hConsole == nullptr)
                return false;
            DWORD mode;
            if (!GetConsoleMode(hConsole, &mode)) return false;
            mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
            mode |= ENABLE_PROCESSED_OUTPUT;

            return SetConsoleMode(hConsole, mode) != 0;
        }();
        return initialized;
    }

    static const bool &supports_ansi = init_windows_terminal();
#elif defined(__unix__) || defined(__APPLE__)
    static const bool &supports_ansi = true;
#else
    static const bool &supports_ansi = false; ///< 可能佯败
#endif
    /**
     * 为什么在此处使用const bool &而非bool？
     * 因为神秘的是，前者不会警告，后者会。
     * 为了避免吵到用户的眼睛，我选择这么做。
     * 毕竟C++11没有[[maybe_unused]] :(
     */
}
