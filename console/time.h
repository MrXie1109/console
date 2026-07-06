/**
 * @file time.h
 * @brief 提供时间度量、计时、休眠和日期时间格式化功能。
 * @details 包含 Time 类（纳秒精度时间量），支持单位转换、算术运算和比较；提供
 * now() 获取当前时间点， timer() 测量函数执行时间，sleep() 休眠，datetime()
 * 获取格式化日期时间字符串。
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
#include <chrono>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <thread>
#include <utility>

namespace console {
    /**
     * @class Time
     * @brief
     * 表示以纳秒为单位的时间量，支持单位转换、算术运算和自动选择合适的输出单位。
     * @details 内部使用 long double 存储纳秒数，提供
     * ns()、us()、ms()、s()、min()、hr() 等转换函数。
     *          输出流会根据时间大小自动选择最合适的单位（hr/min/s/ms/μs/ns）。
     */
    class Time {
        long double ns_; ///< 纳秒数

    public:
        /**
         * @brief 构造一个 Time 对象。
         * @param ns 纳秒数，默认为 0。
         */
        Time(long double ns = 0) : ns_(ns) {}

        /// @brief 允许隐式转换为 long double（返回纳秒数）。
        operator long double() const { return ns_; }

        /// @brief 返回纳秒数。
        long double ns() const { return ns_; }
        /// @brief 返回微秒数（1 微秒 = 1000 纳秒）。
        long double us() const { return ns_ / 1e3; }
        /// @brief 返回毫秒数。
        long double ms() const { return ns_ / 1e6; }
        /// @brief 返回秒数。
        long double s() const { return ns_ / 1e9; }
        /// @brief 返回分钟数。
        long double min() const { return ns_ / 60 / 1e9; }
        /// @brief 返回小时数。
        long double hr() const { return ns_ / 3600 / 1e9; }

        /**
         * @brief 输出 Time 对象到流，自动选择合适单位。
         * @param os 输出流。
         * @param t Time 对象。
         * @return std::ostream& 流引用。
         */
        friend std::ostream &operator<<(std::ostream &os, const Time t) {
            if (t.ns_ > 3600 * 1e9) return os << t.ns_ / 3600 / 1e9 << "hr";
            if (t.ns_ > 60 * 1e9) return os << t.ns_ / 60 / 1e9 << "min";
            if (t.ns_ > 1e9) return os << t.ns_ / 1e9 << "s";
            if (t.ns_ > 1e6) return os << t.ns_ / 1e6 << "ms";
            if (t.ns_ > 1e3) return os << t.ns_ / 1e3 << "μs";
            return os << t.ns_ << "ns";
        }

        /// @name 算术运算
        /// @{
        friend Time operator+(Time t1, Time t2) { return t1.ns_ + t2.ns_; }
        friend Time operator-(Time t1, Time t2) { return t1.ns_ - t2.ns_; }
        Time        operator*(long double d) const { return ns_ * d; }
        Time        operator/(long double d) const { return ns_ / d; }
        /// @}

        /// @name 比较运算
        /// @{
        friend bool operator==(Time t1, Time t2) { return t1.ns_ == t2.ns_; }
        friend bool operator!=(Time t1, Time t2) { return t1.ns_ != t2.ns_; }
        friend bool operator<(Time t1, Time t2) { return t1.ns_ < t2.ns_; }
        friend bool operator>(Time t1, Time t2) { return t1.ns_ > t2.ns_; }
        friend bool operator<=(Time t1, Time t2) { return t1.ns_ <= t2.ns_; }
        friend bool operator>=(Time t1, Time t2) { return t1.ns_ >= t2.ns_; }
        /// @}

        /**
         * @brief 静态函数，构造纳秒。
         * @param n 纳秒数。
         * @return Time 构造而来的纳秒。
         */
        static Time nanosecond(double n) { return Time(n); }

        /**
         * @brief 静态函数，构造微秒。
         * @param n 微秒数。
         * @return Time 构造而来的微秒。
         */
        static Time microsecond(double n) { return Time(n * 1e3); }

        /**
         * @brief 静态函数，构造毫秒。
         * @param n 毫秒数。
         * @return Time 构造而来的毫秒。
         */
        static Time millisecond(double n) { return Time(n * 1e6); }

        /**
         * @brief 静态函数，构造秒。
         * @param n 秒数。
         * @return Time 构造而来的秒。
         */
        static Time second(double n) { return Time(n * 1e9); }

        /**
         * @brief 静态函数，构造分钟。
         * @param n 分钟数。
         * @return Time 构造而来的分钟。
         */
        static Time minute(double n) { return Time(n * 6e10); }

        /**
         * @brief 静态函数，构造小时。
         * @param n 小时数。
         * @return Time 构造而来的小时。
         */
        static Time hour(double n) { return Time(n * 3.6e12); }
    };

    /**
     * @brief 获取当前时间点（自纪元以来的纳秒数）。
     * @return Time 当前时间点。
     */
    inline Time now() {
        return Time(
            (long double)(std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch())
                              .count()));
    }

    /**
     * @brief 测量函数调用的执行时间。
     * @tparam F 可调用对象类型。
     * @tparam Args 参数类型包。
     * @param f 要执行的函数。
     * @param args 传递给函数的参数。
     * @return Time 函数执行所花费的时间。
     */
    template <class F, class... Args>
    inline Time timer(F &&f, Args &&...args) {
        Time start = now();
        f(args...);
        return now() - start;
    }

    /**
     * @brief 休眠指定时间。
     * @param tr 要休眠的时长。
     */
    inline void sleep(const Time &tr) {
        std::this_thread::sleep_for(std::chrono::duration<long double>(tr.s()));
    }

    /**
     * @brief 获取当前日期时间字符串。
     * @param fmt 格式化字符串，默认为 "%Y-%m-%d %H:%M:%S"。
     * @return std::string 格式化后的日期时间字符串。
     * @details 格式化语法与 std::put_time 相同。
     */
    inline std::string datetime(const std::string &fmt = "%Y-%m-%d %H:%M:%S") {
        std::stringstream ss;
        auto              now  = std::chrono::system_clock::now();
        auto              time = std::chrono::system_clock::to_time_t(now);
        std::tm           tm_buffer;
#ifdef _WIN32
        localtime_s(&tm_buffer, &time);
#else
        localtime_r(&time, &tm_buffer);
#endif
        ss << std::put_time(&tm_buffer, fmt.c_str());
        return ss.str();
    }

    /**
     * @brief 控制循环的帧率。
     * @param target 目标帧率。
     * @return double 实际帧率。
     */
    inline double fps(double target) {
        using namespace std::chrono;
        thread_local auto last = steady_clock::now();
        std::this_thread::sleep_until(last + duration<double>(1.0 / target));
        double actual
            = 1.0 / duration<double>(steady_clock::now() - last).count();
        last = steady_clock::now();
        return actual;
    }
}
