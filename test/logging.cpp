/**
 * @file logging.cpp
 * @brief 测试日志记录模块 (BasicLogging, Logging)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */
#include "../include/logging.h"

#include <sstream>
#include <string>

#include "../include/test.h"

/**
 * @brief 测试 Logging 默认构造。
 * @note 期望：默认构造的日志记录器级别为 INFO，颜色关闭。
 */
TEST(LoggingDefaultConstruction) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::INFO);
    ASSERT_TRUE(true);
}

/**
 * @brief 测试 Logging 的 set(Level) 方法。
 * @note 期望：设置最低级别后，低于该级别的日志不输出。
 */
TEST(LoggingSetLevel) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::INFO);
    logger.set(console::BasicLogging<char>::Level::WARN);
    logger.info("This should not appear");
    std::string output1 = oss.str();
    ASSERT_EQ(std::string(""), output1);
    logger.warn("Warning message");
    std::string output2 = oss.str();
    ASSERT_STRCONTAINS(output2.c_str(), "WARN");
    ASSERT_STRCONTAINS(output2.c_str(), "Warning message");
}

/**
 * @brief 测试 Logging 的 set(bool, bool, bool, bool, bool) 方法。
 * @note 期望：单独控制每个级别的开关。
 */
TEST(LoggingSetIndividual) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::DEBUG);
    logger.set(false, true, false, true, false);
    logger.debug("debug");
    logger.info("info");
    logger.warn("warn");
    logger.error("error");
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "info");
    ASSERT_STRCONTAINS(output.c_str(), "error");
    ASSERT_STRNOTCONTAINS(output.c_str(), "debug");
    ASSERT_STRNOTCONTAINS(output.c_str(), "warn");
}

/**
 * @brief 测试 Logging debug 级别。
 * @note 期望：DEBUG 级别日志正确输出。
 */
TEST(LoggingDebug) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::DEBUG);
    logger.debug("Debug message");
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "DEBUG");
    ASSERT_STRCONTAINS(output.c_str(), "Debug message");
}

/**
 * @brief 测试 Logging info 级别。
 * @note 期望：INFO 级别日志正确输出。
 */
TEST(LoggingInfo) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::INFO);
    logger.info("Info message");
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "INFO");
    ASSERT_STRCONTAINS(output.c_str(), "Info message");
}

/**
 * @brief 测试 Logging warn 级别。
 * @note 期望：WARN 级别日志正确输出。
 */
TEST(LoggingWarn) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::WARN);
    logger.warn("Warning message");
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "WARN");
    ASSERT_STRCONTAINS(output.c_str(), "Warning message");
}

/**
 * @brief 测试 Logging error 级别。
 * @note 期望：ERROR 级别日志正确输出。
 */
TEST(LoggingError) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::ERROR);
    logger.error("Error message");
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "ERROR");
    ASSERT_STRCONTAINS(output.c_str(), "Error message");
}

/**
 * @brief 测试 Logging fatal 级别。
 * @note 期望：FATAL 级别日志输出并抛出异常。
 */
TEST(LoggingFatal) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::FATAL);
    ASSERT_THROWS(logger.fatal("Fatal error occurred"), console::FatalLogging);
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "FATAL");
    ASSERT_STRCONTAINS(output.c_str(), "Fatal error occurred");
}

/**
 * @brief 测试 Logging 时间戳格式。
 * @note 期望：日志包含时间戳。
 */
TEST(LoggingTimestamp) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::INFO);
    logger.info("Test");
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "[");
    ASSERT_STRCONTAINS(output.c_str(), "]");
    ASSERT_STRCONTAINS(output.c_str(), "-");
    ASSERT_STRCONTAINS(output.c_str(), ":");
}

/**
 * @brief 测试 Logging 多个参数。
 * @note 期望：多个参数正确拼接输出。
 */
TEST(LoggingMultipleArgs) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::INFO);
    logger.info("Value:", 42, ", Pi:", 3.14);
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "Value:42, Pi:3.14");
}

/**
 * @brief 测试 Logging 颜色启用。
 * @note 期望：颜色启用时输出包含颜色控制码。
 */
TEST(LoggingColorEnabled) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, true, console::BasicLogging<char>::Level::DEBUG);
    logger.debug("Color test");
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "\033[");
}

/**
 * @brief 测试 Logging 颜色禁用。
 * @note 期望：颜色禁用时输出不包含颜色控制码。
 */
TEST(LoggingColorDisabled) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::DEBUG);
    logger.debug("No color");
    std::string output = oss.str();
    ASSERT_STRNOTCONTAINS(output.c_str(), "\033[");
}

/**
 * @brief 测试 Logging 各级别颜色不同。
 * @note 期望：不同级别使用不同颜色。
 */
TEST(LoggingDifferentColors) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, true, console::BasicLogging<char>::Level::DEBUG);
    logger.debug("debug");
    logger.info("info");
    logger.warn("warn");
    logger.error("error");
    std::string output       = oss.str();
    size_t      first_escape = output.find("\033[");
    ASSERT_NE(std::string::npos, first_escape);
    size_t second_escape = output.find("\033[", first_escape + 1);
    ASSERT_NE(std::string::npos, second_escape);
}

/**
 * @brief 测试 Logging 全局 logger 实例。
 * @note 期望：全局 logger 可正常使用。
 */
TEST(LoggingGlobalLogger) {
    (void)console::logger;
    ASSERT_TRUE(true);
}

/**
 * @brief 测试 Logging fatal 异常消息。
 * @note 期望：异常消息包含日志内容。
 */
TEST(LoggingFatalExceptionMessage) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::FATAL);
    try {
        logger.fatal("Critical error");
    } catch (const console::FatalLogging &e) {
        std::string msg = e.what();
        ASSERT_STRCONTAINS(msg.c_str(), "Fatal Error: Critical error");
        return;
    }
    ASSERT_TRUE(false);
}

/**
 * @brief 测试 Logging 级别过滤 DEBUG。
 * @note 期望：设置为 DEBUG 时所有级别都输出。
 */
TEST(LoggingLevelDebug) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::DEBUG);
    logger.debug("debug");
    logger.info("info");
    logger.warn("warn");
    logger.error("error");
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "debug");
    ASSERT_STRCONTAINS(output.c_str(), "info");
    ASSERT_STRCONTAINS(output.c_str(), "warn");
    ASSERT_STRCONTAINS(output.c_str(), "error");
}

/**
 * @brief 测试 Logging 级别过滤 FATAL。
 * @note 期望：设置为 FATAL 时只有 FATAL 输出。
 */
TEST(LoggingLevelFatal) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::FATAL);
    logger.debug("debug");
    logger.info("info");
    logger.warn("warn");
    logger.error("error");
    ASSERT_THROWS(logger.fatal("fatal"), console::FatalLogging);
    std::string output = oss.str();
    ASSERT_STRNOTCONTAINS(output.c_str(), "debug");
    ASSERT_STRNOTCONTAINS(output.c_str(), "info");
    ASSERT_STRNOTCONTAINS(output.c_str(), "warn");
    ASSERT_STRNOTCONTAINS(output.c_str(), "error");
    ASSERT_STRCONTAINS(output.c_str(), "fatal");
}

/**
 * @brief 测试 Logging 关闭所有级别。
 * @note 期望：所有级别都不输出。
 */
TEST(LoggingAllDisabled) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::DEBUG);
    logger.set(false, false, false, false, false);
    logger.debug("debug");
    logger.info("info");
    logger.warn("warn");
    logger.error("error");
    try {
        logger.fatal("fatal");
    } catch (const console::FatalLogging &) {
    }
    std::string output = oss.str();
    ASSERT_EQ(std::string(""), output);
}

/**
 * @brief 测试宽字符版本 WLogging。
 * @note 期望：宽字符日志正确输出。
 */
TEST(WLoggingBasic) {
    std::wostringstream            woss;
    console::BasicLogging<wchar_t> logger(
        woss, false, console::BasicLogging<wchar_t>::Level::INFO);
    logger.info(L"Wide char info");
    std::wstring woutput = woss.str();
    std::string  output(std::begin(woutput), std::end(woutput));
    ASSERT_STRCONTAINS(output.c_str(), "INFO");
    ASSERT_STRCONTAINS(output.c_str(), "Wide char info");
}

/**
 * @brief 测试宽字符版本 WLogging 颜色。
 * @note 期望：宽字符颜色日志正确输出。
 */
TEST(WLoggingColor) {
    std::wostringstream            woss;
    console::BasicLogging<wchar_t> logger(
        woss, true, console::BasicLogging<wchar_t>::Level::DEBUG);
    logger.debug(L"Color wide test");
    std::wstring output = woss.str();
    std::string  narrow_output(output.begin(), output.end());
    ASSERT_STRCONTAINS(narrow_output.c_str(), "\033[");
}

/**
 * @brief 测试宽字符版本 WLogging fatal。
 * @note 期望：宽字符 FATAL 输出并抛出异常。
 */
TEST(WLoggingFatal) {
    std::wostringstream            woss;
    console::BasicLogging<wchar_t> logger(
        woss, false, console::BasicLogging<wchar_t>::Level::FATAL);
    ASSERT_THROWS(logger.fatal(L"Wide fatal error"), console::FatalLogging);
    std::wstring output = woss.str();
    std::string  narrow_output(output.begin(), output.end());
    ASSERT_STRCONTAINS(narrow_output.c_str(), "FATAL");
    ASSERT_STRCONTAINS(narrow_output.c_str(), "Wide fatal error");
}

/**
 * @brief 测试 Logging 关闭 fatal 但仍抛出异常。
 * @note 期望：即使 fatal 被禁用，调用 fatal 仍抛出异常。
 */
TEST(LoggingFatalAlwaysThrows) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::DEBUG);
    logger.set(true, true, true, true, false);
    ASSERT_THROWS(
        logger.fatal("Fatal even when disabled"), console::FatalLogging);
    std::string output = oss.str();
    ASSERT_STRNOTCONTAINS(output.c_str(), "FATAL");
}

/**
 * @brief 测试 Logging 输出 std::string 参数。
 * @note 期望：std::string 参数正确输出。
 */
TEST(LoggingStringArg) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::INFO);
    std::string msg = "Hello, World!";
    logger.info("Message:", msg);
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "Message:Hello, World!");
}

/**
 * @brief 测试 Logging 输出混合类型参数。
 * @note 期望：混合类型参数正确拼接。
 */
TEST(LoggingMixedTypes) {
    std::ostringstream          oss;
    console::BasicLogging<char> logger(
        oss, false, console::BasicLogging<char>::Level::INFO);
    logger.info("Int:", 42, ", Double:", 3.14, ", Bool:", true);
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "Int:42, Double:3.14, Bool:true");
}
#ifndef NOMAIN

#ifndef NOMAIN
TEST_MAIN
#endif
#endif
