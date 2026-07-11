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
#include <string>
#include <thread>
#include <utility>

#include "csexc.h"
#include "kb.h"

namespace console {
    /**
     * @class Time
     * @brief
     * 表示以纳秒为单位的时间量，支持单位转换、算术运算和自动选择合适的输出单位。
     * @details 内部使用 long long 存储纳秒数，提供
     * ns()、us()、ms()、s()、min()、hr() 等转换函数。
     *          输出流会根据时间大小自动选择最合适的单位（hr/min/s/ms/μs/ns）。
     */
    class Time {
        long long ns_; ///< 纳秒数

    public:
        /**
         * @brief 构造一个 Time 对象。
         * @param ns 纳秒数，默认为 0。
         */
        Time(long long ns = 0) : ns_(ns) {}

        /// @brief 允许隐式转换为 long long（返回纳秒数）。
        operator long long() const { return ns_; }

        /// @brief 返回纳秒数。
        long double ns() const { return ns_; }
        /// @brief 返回微秒数。
        long double us() const { return ns_ / 1e3; }
        /// @brief 返回毫秒数。
        long double ms() const { return ns_ / 1e6; }
        /// @brief 返回秒数。
        long double s() const { return ns_ / 1e9; }
        /// @brief 返回分钟数。
        long double min() const { return ns_ / 6e10; }
        /// @brief 返回小时数。
        long double hr() const { return ns_ / 36e11; }

        /**
         * @brief 输出 Time 对象到流，自动选择合适单位。
         * @param os 输出流。
         * @param t Time 对象。
         * @return std::ostream& 流引用。
         */
        friend std::ostream &operator<<(std::ostream &os, const Time t) {
            if (t.ns_ > 36e11) return os << t.hr() << "hr";
            if (t.ns_ > 6e10) return os << t.min() << "min";
            if (t.ns_ > 1e9) return os << t.s() << "s";
            if (t.ns_ > 1e6) return os << t.ms() << "ms";
            if (t.ns_ > 1e3) return os << t.us() << "μs";
            return os << t.ns() << "ns";
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

        template <class Rep, class Period>
        friend Time operator+(Time t, std::chrono::duration<Rep, Period> d) {
            return t
                   + Time(
                       std::chrono::duration_cast<std::chrono::nanoseconds>(d)
                           .count());
        }

        template <class Rep, class Period>
        friend Time operator-(Time t, std::chrono::duration<Rep, Period> d) {
            return t
                   - Time(
                       std::chrono::duration_cast<std::chrono::nanoseconds>(d)
                           .count());
        }
    };

    /**
     * @brief 静态函数，构造纳秒。
     * @param n 纳秒数。
     * @return Time 构造而来的纳秒。
     */
    inline Time nanoseconds(double n) {
        return Time(n);
    }

    /**
     * @brief 静态函数，构造微秒。
     * @param n 微秒数。
     * @return Time 构造而来的微秒。
     */
    inline Time microseconds(double n) {
        return Time(n * 1e3);
    }

    /**
     * @brief 静态函数，构造毫秒。
     * @param n 毫秒数。
     * @return Time 构造而来的毫秒。
     */
    inline Time milliseconds(double n) {
        return Time(n * 1e6);
    }

    /**
     * @brief 静态函数，构造秒。
     * @param n 秒数。
     * @return Time 构造而来的秒。
     */
    inline Time seconds(double n) {
        return Time(n * 1e9);
    }

    /**
     * @brief 静态函数，构造分钟。
     * @param n 分钟数。
     * @return Time 构造而来的分钟。
     */
    inline Time minutes(double n) {
        return Time(n * 6e10);
    }

    /**
     * @brief 静态函数，构造小时。
     * @param n 小时数。
     * @return Time 构造而来的小时。
     */
    inline Time hours(double n) {
        return Time(n * 3.6e12);
    }

    /**
     * @brief 获取当前时间点（自纪元以来的纳秒数）。
     * @return Time 当前时间点。
     */
    inline Time now() {
        return Time( //
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::system_clock::now().time_since_epoch())
                .count());
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
        thread_local auto last = steady_clock::now(); //于首次调用时初始化
        std::this_thread::sleep_until(last + duration<double>(1.0 / target));
        double actual
            = 1.0 / duration<double>(steady_clock::now() - last).count();
        last = steady_clock::now();
        return actual;
    }

    class DateTime {
        std::tm tm_{};

        void normalize() {
            auto time = std::mktime(&tm_);
            if (time != (std::time_t)-1) {
#ifdef _WIN32
                localtime_s(&tm_, &time);
#else
                localtime_r(&time, &tm_);
#endif
            }
        }

    public:
        DateTime() {
            auto        now  = std::chrono::system_clock::now();
            std::time_t time = std::chrono::system_clock::to_time_t(now);
#ifdef _WIN32
            localtime_s(&tm_, &time);
#else
            localtime_r(&time, &tm_);
#endif
        }

        DateTime(int year,
            int      month,
            int      day,
            int      hour   = 0,
            int      minute = 0,
            int      second = 0) {
            tm_.tm_year  = year - 1900;
            tm_.tm_mon   = month - 1;
            tm_.tm_mday  = day;
            tm_.tm_hour  = hour;
            tm_.tm_min   = minute;
            tm_.tm_sec   = second;
            tm_.tm_isdst = -1;
            normalize();
        }

        DateTime(const std::string &str,
            const std::string      &fmt = "%Y-%m-%d %H:%M:%S") {
            std::istringstream ss(str);
            ss >> std::get_time(&tm_, fmt.c_str());
            if (ss.fail()) throw BadFormat("Failed to parse DateTime");
        }

        DateTime(const std::wstring &str,
            const std::wstring      &fmt = L"%Y-%m-%d %H:%M:%S") {
            std::wistringstream ss(str);
            ss >> std::get_time(&tm_, fmt.c_str());
            if (ss.fail()) throw BadFormat("Failed to parse DateTime");
        }

        int  year() const { return tm_.tm_year + 1900; }
        int  month() const { return tm_.tm_mon + 1; }
        int  day() const { return tm_.tm_mday; }
        int  hour() const { return tm_.tm_hour; }
        int  minute() const { return tm_.tm_min; }
        int  second() const { return tm_.tm_sec; }
        int  weekday() const { return tm_.tm_wday; }
        int  yearday() const { return tm_.tm_yday; }
        bool isdst() const { return tm_.tm_isdst > 0; }

        /**
         * @brief 判断是否为闰年。
         * @param y 年份，默认为当前 year()。
         * @return true 如果是闰年。
         */
        static bool is_leap(int y) {
            return (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
        }

        bool is_leap() const { return is_leap(year()); }

        /**
         * @brief 获取指定月份的天数。
         * @param y 年份。
         * @param m 月份（1-12）。
         * @return 该月的天数。
         */
        static int days_in_month(int y, int m) {
            static const int d[]
                = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
            if (m == 2 && is_leap(y)) return 29;
            return d[m - 1];
        }
        int days_in_month() const { return days_in_month(year(), month()); }

        /**
         * @brief 获取本周第几天（周一=1, ..., 周日=7）。
         */
        int iso_weekday() const {
            int w = tm_.tm_wday;
            return w == 0 ? 7 : w;
        }

        /**
         * @brief 获取 ISO 8601 周数。
         * @return 当前日期所在的 ISO 周数（1-53）。
         */
        int iso_week() const {
            // 取当年 1 月 4 日所在的周一为准
            DateTime jan4(year(), 1, 4);
            int      dow      = jan4.iso_weekday();
            int      week_num = (yearday() - jan4.yearday() + dow + 6) / 7;
            if (week_num < 1)
                week_num = DateTime(year() - 1, 12, 31).iso_week();
            if (week_num > 52 && days_in_month() == 31 && month() == 12
                && iso_weekday() <= 3)
                week_num = 1;
            return week_num;
        }

        /**
         * @brief 获取当月第一天的 DateTime。
         */
        DateTime begin_of_month() const { return DateTime(year(), month(), 1); }

        /**
         * @brief 获取当月最后一天的 DateTime。
         */
        DateTime end_of_month() const {
            return DateTime(year(), month(), days_in_month(), 23, 59, 59);
        }

        /**
         * @brief 获取当天 00:00:00 的 DateTime。
         */
        DateTime begin_of_day() const {
            return DateTime(year(), month(), day());
        }

        /**
         * @brief 获取当天 23:59:59 的 DateTime。
         */
        DateTime end_of_day() const {
            return DateTime(year(), month(), day(), 23, 59, 59);
        }

        /**
         * @brief 判断是否为周末。
         */
        bool is_weekend() const { return tm_.tm_wday == 0 || tm_.tm_wday == 6; }

        /**
         * @brief 获取该年中的第几天（1-based）。
         */
        int day_of_year() const { return tm_.tm_yday + 1; }

        /**
         * @brief 格式化为字符串。
         * @param fmt 格式化字符串，默认为 "%Y-%m-%d %H:%M:%S"。
         * @return std::string 格式化结果。
         */
        std::string str(const std::string &fmt = "%Y-%m-%d %H:%M:%S") const {
            std::ostringstream ss;
            ss << std::put_time(&tm_, fmt.c_str());
            return ss.str();
        }

        /**
         * @brief 格式化为宽字符串。
         * @param fmt 格式化字符串，默认为 L"%Y-%m-%d %H:%M:%S"。
         * @return std::wstring 格式化结果。
         */
        std::wstring
        wstr(const std::wstring &fmt = L"%Y-%m-%d %H:%M:%S") const {
            std::wostringstream ss;
            ss << std::put_time(&tm_, fmt.c_str());
            return ss.str();
        }

        /// @brief 转为 std::time_t。
        std::time_t to_time_t() const {
            return std::mktime(const_cast<std::tm *>(&tm_));
        }

        /// @name 算术运算
        /// @{

        /**
         * @brief 增加指定秒数。
         */
        DateTime &operator+=(Time t) {
            std::time_t sec = static_cast<std::time_t>(t.s());
            auto        tt  = to_time_t() + sec;
#ifdef _WIN32
            localtime_s(&tm_, &tt);
#else
            localtime_r(&tt, &tm_);
#endif
            return *this;
        }

        /**
         * @brief 减少指定秒数。
         */
        DateTime &operator-=(Time t) { return *this += Time(-t.s() * 1e9); }

        friend DateTime operator+(DateTime dt, Time t) { return dt += t; }

        friend DateTime operator-(DateTime dt, Time t) { return dt -= t; }

        /**
         * @brief 计算两个 DateTime 的时间差。
         */
        friend Time operator-(const DateTime &lhs, const DateTime &rhs) {
            return Time(static_cast<long long>(
                std::difftime(lhs.to_time_t(), rhs.to_time_t()) * 1e9));
        }

        /// @}

        /// @name 比较运算
        /// @{
        friend bool operator==(const DateTime &lhs, const DateTime &rhs) {
            return std::difftime(lhs.to_time_t(), rhs.to_time_t()) == 0;
        }
        friend bool operator!=(const DateTime &lhs, const DateTime &rhs) {
            return !(lhs == rhs);
        }
        friend bool operator<(const DateTime &lhs, const DateTime &rhs) {
            return std::difftime(lhs.to_time_t(), rhs.to_time_t()) < 0;
        }
        friend bool operator>(const DateTime &lhs, const DateTime &rhs) {
            return rhs < lhs;
        }
        friend bool operator<=(const DateTime &lhs, const DateTime &rhs) {
            return !(lhs > rhs);
        }
        friend bool operator>=(const DateTime &lhs, const DateTime &rhs) {
            return !(lhs < rhs);
        }
        /// @}

        /**
         * @brief 输出日期时间到流，格式为 "%Y-%m-%d %H:%M:%S"。
         */
        friend std::ostream &operator<<(std::ostream &os, const DateTime &dt) {
            return os << dt.str();
        }

        /**
         * @brief 输出日期时间到宽字符流。
         */
        friend std::wostream &
        operator<<(std::wostream &os, const DateTime &dt) {
            return os << dt.wstr();
        }
    };
}
