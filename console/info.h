/**
 * @file info.h
 * @brief 提供库的版本信息、许可证、编译器及平台识别函数。
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
#include <string>

namespace console {
    /**
     * @brief 返回 MIT 许可证全文。
     * @return std::string 许可证文本。
     */
    inline std::string license() {
        return R"(Copyright (c) 2026 MrXie1109

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
SOFTWARE.)";
    }

    /**
     * @brief 返回当前操作系统平台名称。
     * @details 根据预定义宏识别 Windows、Linux、macOS 或 Unknown。
     * @return std::string 平台名称。
     */
    inline std::string platform() {
#ifdef _WIN32
        return "Windows";
#elif defined(__linux__)
        return "Linux";
#elif defined(__APPLE__)
        return "macOS";
#else
        return "Unknown";
#endif
    }

    /**
     * @brief 返回当前使用的编译器名称和版本。
     * @details 支持 GCC（版本号）、MSVC（版本号）、Clang（仅名称）以及其他。
     * @return std::string 编译器信息。
     */
    inline std::string compiler() {
#ifdef __GNUC__
        return "GCC " + std::to_string(__GNUC__) + "."
               + std::to_string(__GNUC_MINOR__);
#elif defined(_MSC_VER)
        return "MSVC " + std::to_string(_MSC_VER);
#elif defined(__clang__)
        return "Clang";
#else
        return "Unknown";
#endif
    }

    /**
     * @brief 返回库的版本字符串。
     * @return std::string 格式为 console version Major.Minor.Patch (YYYY-MM-DD)
     * -- "Commit Message"。
     */
    inline std::string version() {
        return "console version 5.8.3 (2026-07-05) -- \"BUG?!\"";
    }

    /**
     * @brief 返回库的作者。
     * @return std::string "MrXie1109"。
     */
    inline std::string author() {
        return "MrXie1109";
    }
}
