/**
 * @file logging.h
 * @brief 提供带颜色和级别的日志记录功能。
 * @details 包含 Logging 类，支持 DEBUG、INFO、WARN、ERROR、FATAL 五个级别，
 *          可控制输出级别、颜色开关，并自动附加时间戳。全局默认 logger
 * 实例可直接使用。
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
#include <cstdint>
#include <sstream>

#include "colorful.h"
#include "csexc.h"
#include "literals.h"
#include "output.h"
#include "strpp.h"
#include "time.h"

namespace console {
    /**
     * @class BasicLogging
     * @brief 日志记录器，支持多级别、颜色输出和自动时间戳。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @details 使用 BasicOutput
     * 类进行格式化输出，可设置最低日志级别或单独控制每个级别的开关。 FATAL
     * 级别会抛出 FatalLogging 异常。
     */
    template <class CharT = char, class Traits = std::char_traits<CharT>>
    class BasicLogging {
    public:
        using string_type = std::basic_string<CharT, Traits>;

        /**
         * @enum Level
         * @brief 日志级别枚举。
         * @var DEBUG 调试信息
         * @var INFO  一般信息
         * @var WARN  警告
         * @var ERROR 错误
         * @var FATAL 致命错误（会抛出异常）
         */
        enum class Level : int8_t { DEBUG, INFO, WARN, ERROR, FATAL };

    private:
        BasicOutput<CharT, Traits> output; ///< 输出器
        bool                       colorful; ///< 是否启用颜色
        bool settings[5]; ///< 每个级别的开关，索引对应 Level 枚举值

    public:
        /**
         * @brief 设置最低日志级别（低于该级别的日志将被忽略）。
         * @param minLevel 最低级别（包含）。
         */
        void set(Level minLevel) {
            int8_t n = int8_t(minLevel);
            for (int i = 0; i < n; i++) settings[i] = false;
            for (int i = n; i < 5; i++) settings[i] = true;
        }

        /**
         * @brief 单独设置每个级别的开关。
         * @param a DEBUG 开关
         * @param b INFO 开关
         * @param c WARN 开关
         * @param d ERROR 开关
         * @param e FATAL 开关
         */
        void set(bool a, bool b, bool c, bool d, bool e) {
            settings[0] = a;
            settings[1] = b;
            settings[2] = c;
            settings[3] = d;
            settings[4] = e;
        }

        /**
         * @brief 构造 BasicLogging 对象。
         * @param os 输出流，默认为 std::clog。
         * @param cf 是否启用颜色，默认 false。
         * @param lvl 最低日志级别，默认 INFO。
         */
        BasicLogging(std::basic_ostream<CharT, Traits> &os  = std::clog,
            bool                                        cf  = false,
            Level                                       lvl = Level::INFO) :
            output(os, string_type{}, string_type{1, CharT('\n')}, true),
            colorful(cf) {
            set(lvl);
        }

        /**
         * @brief 输出 DEBUG 级别日志。
         * @tparam Args 可变参数类型。
         * @param args 要输出的内容（可多个，自动转换为字符串）。
         */
        template <class... Args>
        void debug(const Args &...args) {
            if (settings[0]) {
                if (colorful)
                    output(color::BrightBlack,
                        CharT('['),
                        datetime(),
                        "] [DEBUG] - ",
                        args...,
                        color::Reset);
                else
                    output(CharT('['), datetime(), "] [DEBUG] - ", args...);
            }
        }

        /**
         * @brief 输出 INFO 级别日志。
         * @tparam Args 可变参数类型。
         * @param args 要输出的内容。
         */
        template <class... Args>
        void info(const Args &...args) {
            if (settings[1]) {
                if (colorful)
                    output(color::BrightCyan,
                        CharT('['),
                        datetime(),
                        "] [.INFO] - ",
                        args...,
                        color::Reset);
                else
                    output(CharT('['), datetime(), "] [.INFO] - ", args...);
            }
        }

        /**
         * @brief 输出 WARN 级别日志。
         * @tparam Args 可变参数类型。
         * @param args 要输出的内容。
         */
        template <class... Args>
        void warn(const Args &...args) {
            if (settings[2]) {
                if (colorful)
                    output(color::BrightYellow,
                        CharT('['),
                        datetime(),
                        "] [.WARN] - ",
                        args...,
                        color::Reset);
                else
                    output(CharT('['), datetime(), "] [.WARN] - ", args...);
            }
        }

        /**
         * @brief 输出 ERROR 级别日志。
         * @tparam Args 可变参数类型。
         * @param args 要输出的内容。
         */
        template <class... Args>
        void error(const Args &...args) {
            if (settings[3]) {
                if (colorful)
                    output(color::BrightRed,
                        CharT('['),
                        datetime(),
                        "] [ERROR] - ",
                        args...,
                        color::Reset);
                else
                    output(CharT('['), datetime(), "] [ERROR] - ", args...);
            }
        }

        /**
         * @brief 输出 FATAL 级别日志并抛出 FatalLogging 异常。
         * @tparam Args 可变参数类型。
         * @param args 要输出的内容。
         * @throw FatalLogging 异常，异常消息包含日志内容。
         */
        template <class... Args>
        void fatal(const Args &...args) {
            std::string error_info(to_string(args...));
            if (settings[4]) {
                if (colorful)
                    output(color::BrightMagenta,
                        CharT('['),
                        datetime(),
                        "] [FATAL] - ",
                        error_info,
                        color::Reset);
                else
                    output(CharT('['), datetime(), "] [FATAL] - ", error_info);
            }
            throw FatalLogging("Fatal Error: " + error_info);
        }
    };

    /** @brief BasicLogging<char> 的类型别名。 */
    using Logging = BasicLogging<char>;

    /** @brief BasicLogging<wchar_t> 的类型别名。 */
    using WLogging = BasicLogging<wchar_t>;

    /**
     * @brief 内部单例（char）。
     */
    inline BasicLogging<> &get_logger() {
        static BasicLogging<> instance(
            std::clog, true, BasicLogging<>::Level::INFO);
        return instance;
    }

    static BasicLogging<> &logger
        = get_logger(); ///< 全局默认 logger 实例，启用颜色，级别 INFO。
}
