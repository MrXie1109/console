/**
 * @file time.cpp
 * @brief 测试时间模块 (Time, now, timer, sleep, datetime, fps, DateTime)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/time.h"

#include <sstream>
#include <string>
#include <thread>

#include "../include/test.h"

/**
 * @brief 测试 Time 默认构造。
 * @note 期望：默认构造为 0ns。
 */
TEST(TimeDefaultConstruction) {
    console::Time t;
    ASSERT_EQ(0, t.ns());
    ASSERT_EQ(0.0, t.s());
}

/**
 * @brief 测试 Time 纳秒构造。
 * @note 期望：正确存储纳秒值。
 */
TEST(TimeNanosecondConstruction) {
    console::Time t(1000000000LL);
    ASSERT_EQ(1000000000LL, t.ns());
    ASSERT_NEAR(1.0, t.s(), 0.001);
}

/**
 * @brief 测试 Time 单位转换。
 * @note 期望：正确转换各时间单位。
 */
TEST(TimeUnitConversions) {
    console::Time t(1234567890LL);
    ASSERT_NEAR(1.23456789, t.s(), 0.000001);
    ASSERT_NEAR(1234.56789, t.ms(), 0.001);
    ASSERT_NEAR(1234567.89, t.us(), 0.01);
    ASSERT_NEAR(0.020576, t.min(), 0.000001);
    ASSERT_NEAR(0.0003429, t.hr(), 0.0001);
}

/**
 * @brief 测试 Time 流输出自动选择单位。
 * @note 期望：根据大小自动选择合适的单位。
 */
TEST(TimeOutputStream) {
    std::ostringstream oss1, oss2, oss3, oss4, oss5, oss6;
    console::Time      t1(100);             // ns
    console::Time      t2(10000);           // us
    console::Time      t3(10000000);        // ms
    console::Time      t4(2000000000);      // s
    console::Time      t5(120000000000LL);  // min
    console::Time      t6(3600000000000LL); // hr
    oss1 << t1;
    oss2 << t2;
    oss3 << t3;
    oss4 << t4;
    oss5 << t5;
    oss6 << t6;
    auto s1 = oss1.str();
    auto s2 = oss2.str();
    auto s3 = oss3.str();
    auto s4 = oss4.str();
    auto s5 = oss5.str();
    auto s6 = oss6.str();
    ASSERT_STRCONTAINS(s1.c_str(), "ns");
    ASSERT_STRCONTAINS(s2.c_str(), "μs");
    ASSERT_STRCONTAINS(s3.c_str(), "ms");
    ASSERT_STRCONTAINS(s4.c_str(), "s");
    ASSERT_STRCONTAINS(s5.c_str(), "min");
    ASSERT_STRCONTAINS(s6.c_str(), "hr");
}

/**
 * @brief 测试 Time 算术运算。
 * @note 期望：加减乘除正确。
 */
TEST(TimeArithmetic) {
    console::Time t1(1000), t2(2000);
    auto          t3 = t1 + t2;
    ASSERT_EQ(3000, t3.ns());
    auto t4 = t2 - t1;
    ASSERT_EQ(1000, t4.ns());
    auto t5 = t1 * 2.5;
    ASSERT_EQ(2500, t5.ns());
    auto t6 = t2 / 2.0;
    ASSERT_EQ(1000, t6.ns());
}

/**
 * @brief 测试 Time 比较运算。
 * @note 期望：正确比较。
 */
TEST(TimeComparison) {
    console::Time t1(1000), t2(2000), t3(1000);
    ASSERT_TRUE(t1 == t3);
    ASSERT_TRUE(t1 != t2);
    ASSERT_TRUE(t1 < t2);
    ASSERT_TRUE(t2 > t1);
    ASSERT_TRUE(t1 <= t3);
    ASSERT_TRUE(t2 >= t1);
}

/**
 * @brief 测试 Time 与 chrono duration 运算。
 * @note 期望：支持与 std::chrono 混合运算。
 */
TEST(TimeChronoMixed) {
    console::Time t(1000);
    auto          d  = std::chrono::nanoseconds(500);
    auto          t2 = t + d;
    ASSERT_EQ(1500, t2.ns());
    auto t3 = t - d;
    ASSERT_EQ(500, t3.ns());
}

/**
 * @brief 测试 nanoseconds 工厂函数。
 * @note 期望：正确构造 Time。
 */
TEST(TimeFactoryNanoseconds) {
    auto t = console::nanoseconds(123);
    ASSERT_EQ(123, t.ns());
}

/**
 * @brief 测试 microseconds 工厂函数。
 * @note 期望：正确构造 Time。
 */
TEST(TimeFactoryMicroseconds) {
    auto t = console::microseconds(2.5);
    ASSERT_NEAR(2500, t.ns(), 0.001);
}

/**
 * @brief 测试 milliseconds 工厂函数。
 * @note 期望：正确构造 Time。
 */
TEST(TimeFactoryMilliseconds) {
    auto t = console::milliseconds(3.0);
    ASSERT_NEAR(3000000, t.ns(), 0.001);
}

/**
 * @brief 测试 seconds 工厂函数。
 * @note 期望：正确构造 Time。
 */
TEST(TimeFactorySeconds) {
    auto t = console::seconds(1.5);
    ASSERT_NEAR(1500000000, t.ns(), 0.001);
}

/**
 * @brief 测试 minutes 工厂函数。
 * @note 期望：正确构造 Time。
 */
TEST(TimeFactoryMinutes) {
    auto t = console::minutes(2.0);
    ASSERT_NEAR(120000000000LL, t.ns(), 0.001);
}

/**
 * @brief 测试 hours 工厂函数。
 * @note 期望：正确构造 Time。
 */
TEST(TimeFactoryHours) {
    auto t = console::hours(1.0);
    ASSERT_NEAR(3600000000000LL, t.ns(), 0.001);
}

/**
 * @brief 测试 now 获取当前时间。
 * @note 期望：返回非零时间点。
 */
TEST(TimeNow) {
    auto t = console::now();
    ASSERT_GT(t.ns(), 0);
}

/**
 * @brief 测试 timer 测量函数执行时间。
 * @note 期望：测量时间大于 0。
 */
TEST(TimeTimer) {
    auto duration = console::timer(
        []() { std::this_thread::sleep_for(std::chrono::milliseconds(10)); });
    ASSERT_GT(duration.ns(), 0);
    ASSERT_GT(duration.s(), 0.005);
}

/**
 * @brief 测试 timer 带参数。
 * @note 期望：正确传递参数并测量。
 */
TEST(TimeTimerWithArgs) {
    auto func = [](int a, int b) {
        volatile int c = a + b;
        (void)c;
    };
    auto duration = console::timer(func, 10, 20);
    ASSERT_GE(duration.ns(), 0);
}

/**
 * @brief 测试 sleep 休眠。
 * @note 期望：休眠指定时间，实际至少休眠。
 */
TEST(TimeSleep) {
    auto start = console::now();
    console::sleep(console::milliseconds(50));
    auto elapsed = console::now() - start;
    ASSERT_GE(elapsed.s(), 0.04);
}

/**
 * @brief 测试 datetime 格式。
 * @note 期望：返回格式正确的日期时间字符串。
 */
TEST(TimeDatetime) {
    auto dt = console::datetime();
    ASSERT_EQ(19, dt.size()); // YYYY-MM-DD HH:MM:SS
    ASSERT_EQ('-', dt[4]);
    ASSERT_EQ('-', dt[7]);
    ASSERT_EQ(' ', dt[10]);
    ASSERT_EQ(':', dt[13]);
    ASSERT_EQ(':', dt[16]);
}

/**
 * @brief 测试 datetime 自定义格式。
 * @note 期望：按指定格式输出。
 */
TEST(TimeDatetimeCustom) {
    auto dt = console::datetime("%Y/%m/%d");
    ASSERT_EQ(10, dt.size());
    ASSERT_EQ('/', dt[4]);
    ASSERT_EQ('/', dt[7]);
}

/**
 * @brief 测试 fps 帧率控制。
 * @note 期望：实际帧率接近目标帧率。
 */
TEST(TimeFps) {
    double target  = 10.0;
    auto   start   = console::now();
    double actual  = console::fps(target);
    auto   elapsed = console::now() - start;
    // 第一次调用可能由于初始化有偏差，但应大于0
    ASSERT_GT(actual, 0);
    // 休眠时间应接近目标
    ASSERT_GE(elapsed.s(), 0.09);
}

/**
 * @brief 测试 DateTime 默认构造（当前时间）。
 * @note 期望：构造当前时间。
 */
TEST(DateTimeDefaultConstruction) {
    console::DateTime dt;
    ASSERT_GT(dt.year(), 2020);
    ASSERT_GE(dt.month(), 1);
    ASSERT_LE(dt.month(), 12);
    ASSERT_GE(dt.day(), 1);
    ASSERT_LE(dt.day(), 31);
}

/**
 * @brief 测试 DateTime 从年月日构造。
 * @note 期望：正确设置字段。
 */
TEST(DateTimeFromYMD) {
    console::DateTime dt(2026, 8, 14);
    ASSERT_EQ(2026, dt.year());
    ASSERT_EQ(8, dt.month());
    ASSERT_EQ(14, dt.day());
    ASSERT_EQ(0, dt.hour());
    ASSERT_EQ(0, dt.minute());
    ASSERT_EQ(0, dt.second());
}

/**
 * @brief 测试 DateTime 从年月日时分秒构造。
 * @note 期望：正确设置所有字段。
 */
TEST(DateTimeFromYMDHMS) {
    console::DateTime dt(2026, 8, 14, 15, 30, 45);
    ASSERT_EQ(2026, dt.year());
    ASSERT_EQ(8, dt.month());
    ASSERT_EQ(14, dt.day());
    ASSERT_EQ(15, dt.hour());
    ASSERT_EQ(30, dt.minute());
    ASSERT_EQ(45, dt.second());
}

/**
 * @brief 测试 DateTime 解析字符串。
 * @note 期望：正确解析日期时间。
 */
TEST(DateTimeParseString) {
    console::DateTime dt("2026-08-14 15:30:45");
    ASSERT_EQ(2026, dt.year());
    ASSERT_EQ(8, dt.month());
    ASSERT_EQ(14, dt.day());
    ASSERT_EQ(15, dt.hour());
    ASSERT_EQ(30, dt.minute());
    ASSERT_EQ(45, dt.second());
}

/**
 * @brief 测试 DateTime 解析宽字符串。
 * @note 期望：正确解析宽字符日期时间。
 */
TEST(DateTimeParseWString) {
    console::DateTime dt(L"2026-08-14 15:30:45");
    ASSERT_EQ(2026, dt.year());
    ASSERT_EQ(8, dt.month());
    ASSERT_EQ(14, dt.day());
    ASSERT_EQ(15, dt.hour());
    ASSERT_EQ(30, dt.minute());
    ASSERT_EQ(45, dt.second());
}

/**
 * @brief 测试 DateTime 解析自定义格式。
 * @note 期望：按指定格式解析。
 */
TEST(DateTimeParseCustomFormat) {
    console::DateTime dt("2026/08/14", "%Y/%m/%d");
    ASSERT_EQ(2026, dt.year());
    ASSERT_EQ(8, dt.month());
    ASSERT_EQ(14, dt.day());
}

/**
 * @brief 测试 DateTime 解析失败。
 * @note 期望：解析失败抛出 BadFormat。
 */
TEST(DateTimeParseFailure) {
    ASSERT_THROWS(console::DateTime("invalid"), console::BadFormat);
}

/**
 * @brief 测试 DateTime 字段获取。
 * @note 期望：正确返回各字段。
 */
TEST(DateTimeFields) {
    console::DateTime dt(2026, 8, 14, 15, 30, 45);
    ASSERT_EQ(2026, dt.year());
    ASSERT_EQ(8, dt.month());
    ASSERT_EQ(14, dt.day());
    ASSERT_EQ(15, dt.hour());
    ASSERT_EQ(30, dt.minute());
    ASSERT_EQ(45, dt.second());
    // 2026-08-14 是周五，wday=5
    ASSERT_EQ(5, dt.weekday());
    // 该年从1月1日开始到8月14日的天数（1-based）
    ASSERT_EQ(226, dt.day_of_year());
    ASSERT_FALSE(dt.isdst());
}

/**
 * @brief 测试 DateTime is_leap。
 * @note 期望：正确判断闰年。
 */
TEST(DateTimeIsLeap) {
    ASSERT_TRUE(console::DateTime::is_leap(2024));
    ASSERT_FALSE(console::DateTime::is_leap(2023));
    ASSERT_FALSE(console::DateTime::is_leap(2100));
    ASSERT_TRUE(console::DateTime::is_leap(2000));
    console::DateTime dt(2024, 1, 1);
    ASSERT_TRUE(dt.is_leap());
}

/**
 * @brief 测试 DateTime days_in_month。
 * @note 期望：返回正确天数。
 */
TEST(DateTimeDaysInMonth) {
    ASSERT_EQ(31, console::DateTime::days_in_month(2026, 1));
    ASSERT_EQ(28, console::DateTime::days_in_month(2026, 2));
    ASSERT_EQ(29, console::DateTime::days_in_month(2024, 2));
    ASSERT_EQ(31, console::DateTime::days_in_month(2026, 3));
    ASSERT_EQ(30, console::DateTime::days_in_month(2026, 4));
    console::DateTime dt(2026, 8, 14);
    ASSERT_EQ(31, dt.days_in_month());
}

/**
 * @brief 测试 DateTime iso_weekday。
 * @note 期望：周一=1，周日=7。
 */
TEST(DateTimeIsoWeekday) {
    console::DateTime dt(2026, 8, 14); // 周五
    ASSERT_EQ(5, dt.iso_weekday());
    console::DateTime dt2(2026, 8, 10); // 周一
    ASSERT_EQ(1, dt2.iso_weekday());
    console::DateTime dt3(2026, 8, 16); // 周日
    ASSERT_EQ(7, dt3.iso_weekday());
}

/**
 * @brief 测试 DateTime iso_week。
 * @note 期望：返回 ISO 周数。
 */
TEST(DateTimeIsoWeek) {
    console::DateTime dt(2026, 8, 14);
    int               week = dt.iso_week();
    ASSERT_GE(week, 1);
    ASSERT_LE(week, 53);
    // 2026-08-14 应该是第33周
    ASSERT_EQ(33, week);
}

/**
 * @brief 测试 DateTime begin_of_month。
 * @note 期望：返回当月第一天。
 */
TEST(DateTimeBeginOfMonth) {
    console::DateTime dt(2026, 8, 14);
    auto              begin = dt.begin_of_month();
    ASSERT_EQ(2026, begin.year());
    ASSERT_EQ(8, begin.month());
    ASSERT_EQ(1, begin.day());
    ASSERT_EQ(0, begin.hour());
    ASSERT_EQ(0, begin.minute());
    ASSERT_EQ(0, begin.second());
}

/**
 * @brief 测试 DateTime end_of_month。
 * @note 期望：返回当月最后一天。
 */
TEST(DateTimeEndOfMonth) {
    console::DateTime dt(2026, 8, 14);
    auto              end = dt.end_of_month();
    ASSERT_EQ(2026, end.year());
    ASSERT_EQ(8, end.month());
    ASSERT_EQ(31, end.day());
    ASSERT_EQ(23, end.hour());
    ASSERT_EQ(59, end.minute());
    ASSERT_EQ(59, end.second());
}

/**
 * @brief 测试 DateTime begin_of_day。
 * @note 期望：返回当天 00:00:00。
 */
TEST(DateTimeBeginOfDay) {
    console::DateTime dt(2026, 8, 14, 15, 30, 45);
    auto              begin = dt.begin_of_day();
    ASSERT_EQ(2026, begin.year());
    ASSERT_EQ(8, begin.month());
    ASSERT_EQ(14, begin.day());
    ASSERT_EQ(0, begin.hour());
    ASSERT_EQ(0, begin.minute());
    ASSERT_EQ(0, begin.second());
}

/**
 * @brief 测试 DateTime end_of_day。
 * @note 期望：返回当天 23:59:59。
 */
TEST(DateTimeEndOfDay) {
    console::DateTime dt(2026, 8, 14, 15, 30, 45);
    auto              end = dt.end_of_day();
    ASSERT_EQ(2026, end.year());
    ASSERT_EQ(8, end.month());
    ASSERT_EQ(14, end.day());
    ASSERT_EQ(23, end.hour());
    ASSERT_EQ(59, end.minute());
    ASSERT_EQ(59, end.second());
}

/**
 * @brief 测试 DateTime is_weekend。
 * @note 期望：正确判断周末。
 */
TEST(DateTimeIsWeekend) {
    console::DateTime dt1(2026, 8, 14); // 周五
    ASSERT_FALSE(dt1.is_weekend());
    console::DateTime dt2(2026, 8, 15); // 周六
    ASSERT_TRUE(dt2.is_weekend());
    console::DateTime dt3(2026, 8, 16); // 周日
    ASSERT_TRUE(dt3.is_weekend());
}

/**
 * @brief 测试 DateTime day_of_year。
 * @note 期望：返回 1-based 天数。
 */
TEST(DateTimeDayOfYear) {
    console::DateTime dt(2026, 1, 1);
    ASSERT_EQ(1, dt.day_of_year());
    console::DateTime dt2(2026, 12, 31);
    ASSERT_EQ(365, dt2.day_of_year());
    console::DateTime dt3(2024, 12, 31);
    ASSERT_EQ(366, dt3.day_of_year());
}

/**
 * @brief 测试 DateTime str 格式化。
 * @note 期望：返回格式正确的字符串。
 */
TEST(DateTimeStr) {
    console::DateTime dt(2026, 8, 14, 15, 30, 45);
    auto              s = dt.str();
    ASSERT_EQ(std::string("2026-08-14 15:30:45"), s);
    auto s2 = dt.str("%Y/%m/%d");
    ASSERT_EQ(std::string("2026/08/14"), s2);
}

/**
 * @brief 测试 DateTime wstr 宽字符格式化。
 * @note 期望：返回宽字符串。
 */
TEST(DateTimeWStr) {
    console::DateTime dt(2026, 8, 14, 15, 30, 45);
    auto              s = dt.wstr();
    ASSERT_EQ(std::wstring(L"2026-08-14 15:30:45"), s);
}

/**
 * @brief 测试 DateTime 输出流。
 * @note 期望：输出格式正确。
 */
TEST(DateTimeOutputStream) {
    console::DateTime  dt(2026, 8, 14, 15, 30, 45);
    std::ostringstream oss;
    oss << dt;
    ASSERT_EQ(std::string("2026-08-14 15:30:45"), oss.str());
}

/**
 * @brief 测试 DateTime 宽字符输出流。
 * @note 期望：宽字符输出格式正确。
 */
TEST(DateTimeWOutputStream) {
    console::DateTime   dt(2026, 8, 14, 15, 30, 45);
    std::wostringstream woss;
    woss << dt;
    ASSERT_EQ(std::wstring(L"2026-08-14 15:30:45"), woss.str());
}

/**
 * @brief 测试 DateTime to_time_t。
 * @note 期望：转换为 time_t 正确。
 */
TEST(DateTimeToTimeT) {
    console::DateTime dt(2026, 8, 14, 15, 30, 45);
    auto              tt = dt.to_time_t();
    std::tm           tm;
#ifdef _WIN32
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif
    ASSERT_EQ(2026, tm.tm_year + 1900);
    ASSERT_EQ(8, tm.tm_mon + 1);
    ASSERT_EQ(14, tm.tm_mday);
    ASSERT_EQ(15, tm.tm_hour);
    ASSERT_EQ(30, tm.tm_min);
    ASSERT_EQ(45, tm.tm_sec);
}

/**
 * @brief 测试 DateTime 算术运算 +=。
 * @note 期望：正确增加时间。
 */
TEST(DateTimeAddAssign) {
    console::DateTime dt(2026, 8, 14, 0, 0, 0);
    dt += console::seconds(3600);
    ASSERT_EQ(1, dt.hour());
    dt += console::minutes(30);
    ASSERT_EQ(1, dt.hour());
    ASSERT_EQ(30, dt.minute());
}

/**
 * @brief 测试 DateTime 算术运算 -=。
 * @note 期望：正确减少时间。
 */
TEST(DateTimeSubtractAssign) {
    console::DateTime dt(2026, 8, 14, 12, 0, 0);
    dt -= console::seconds(3600);
    ASSERT_EQ(11, dt.hour());
    dt -= console::minutes(30);
    ASSERT_EQ(10, dt.hour());
    ASSERT_EQ(30, dt.minute());
}

/**
 * @brief 测试 DateTime operator+。
 * @note 期望：返回新的 DateTime。
 */
TEST(DateTimeAddition) {
    console::DateTime dt(2026, 8, 14, 0, 0, 0);
    auto              dt2 = dt + console::hours(2);
    ASSERT_EQ(2, dt2.hour());
    // dt 不变
    ASSERT_EQ(0, dt.hour());
}

/**
 * @brief 测试 DateTime operator-。
 * @note 期望：返回新的 DateTime。
 */
TEST(DateTimeSubtraction) {
    console::DateTime dt(2026, 8, 14, 12, 0, 0);
    auto              dt2 = dt - console::hours(2);
    ASSERT_EQ(10, dt2.hour());
    ASSERT_EQ(12, dt.hour());
}

/**
 * @brief 测试 DateTime 时间差。
 * @note 期望：正确计算两个 DateTime 的差值。
 */
TEST(DateTimeDifference) {
    console::DateTime dt1(2026, 8, 14, 12, 0, 0);
    console::DateTime dt2(2026, 8, 14, 13, 30, 0);
    auto              diff = dt2 - dt1;
    ASSERT_NEAR(5400, diff.s(), 0.001);
}

/**
 * @brief 测试 DateTime 比较运算。
 * @note 期望：正确比较。
 */
TEST(DateTimeComparison) {
    console::DateTime dt1(2026, 8, 14, 12, 0, 0);
    console::DateTime dt2(2026, 8, 14, 13, 0, 0);
    console::DateTime dt3(2026, 8, 14, 12, 0, 0);
    ASSERT_TRUE(dt1 == dt3);
    ASSERT_TRUE(dt1 != dt2);
    ASSERT_TRUE(dt1 < dt2);
    ASSERT_TRUE(dt2 > dt1);
    ASSERT_TRUE(dt1 <= dt3);
    ASSERT_TRUE(dt2 >= dt1);
}

#ifndef NOMAIN
TEST_MAIN
#endif
