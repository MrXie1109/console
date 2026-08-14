/**
 * @file config.cpp
 * @brief 测试 INI 配置文件解析模块 (BasicINIConfig, INIConfig)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */
#include "../include/config.h"

#include <cstdio>
#include <fstream>
#include <sstream>

#include "../include/test.h"

static std::string create_temp_ini(const std::string &content) {
    char temp_file[] = "/tmp/config_test_XXXXXX";
    int  fd          = mkstemp(temp_file);
    if (fd == -1) {
        throw std::runtime_error("Failed to create temp file");
    }
    FILE *f = fdopen(fd, "w");
    if (!f) {
        close(fd);
        throw std::runtime_error("Failed to open temp file");
    }
    fwrite(content.c_str(), 1, content.size(), f);
    fclose(f);
    return std::string(temp_file);
}

static void remove_temp_file(const std::string &filename) {
    std::remove(filename.c_str());
}

static std::string read_file(const std::string &filename) {
    std::ifstream ifs(filename);
    std::string   content((std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>());
    return content;
}

/**
 * @brief 测试 INIConfig 的默认构造。
 * @note 期望：默认构造的配置为空。
 */
TEST(INIConfigDefaultConstruction) {
    console::INIConfig config;
    ASSERT_TRUE(config.data().empty());
}

/**
 * @brief 测试 INIConfig 从流解析简单配置。
 * @note 期望：正确解析节和键值对。
 */
TEST(INIConfigParseSimple) {
    std::istringstream iss("[server]\n"
                           "host = localhost\n"
                           "port = 8080\n"
                           "\n"
                           "[database]\n"
                           "user = admin\n"
                           "password = secret\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_TRUE(config.has("server"));
    ASSERT_TRUE(config.has("server.host"));
    ASSERT_TRUE(config.has("server.port"));
    ASSERT_TRUE(config.has("database"));
    ASSERT_TRUE(config.has("database.user"));
    ASSERT_TRUE(config.has("database.password"));
    ASSERT_EQ(std::string("localhost"), config.get<std::string>("server.host"));
    ASSERT_EQ(8080, config.get<int>("server.port"));
    ASSERT_EQ(std::string("admin"), config.get<std::string>("database.user"));
    ASSERT_EQ(
        std::string("secret"), config.get<std::string>("database.password"));
}

/**
 * @brief 测试 INIConfig 解析空节和空键。
 * @note 期望：支持空节头和空键。
 */
TEST(INIConfigParseEmptySectionAndKey) {
    std::istringstream iss("key1 = value1\n"
                           "key2 = value2\n"
                           "\n"
                           "[]\n"
                           "empty_section_key = value3\n"
                           "[section]\n"
                           " = value4\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_TRUE(config.has(".key1"));
    ASSERT_TRUE(config.has(".key2"));
    ASSERT_EQ(std::string("value1"), config.get<std::string>(".key1"));
    ASSERT_EQ(std::string("value2"), config.get<std::string>(".key2"));
    ASSERT_TRUE(config.has(""));
    ASSERT_TRUE(config.has(".empty_section_key"));
    ASSERT_EQ(
        std::string("value3"), config.get<std::string>(".empty_section_key"));
    ASSERT_TRUE(config.has("section."));
    ASSERT_EQ(std::string("value4"), config.get<std::string>("section."));
}

/**
 * @brief 测试 INIConfig 解析注释行。
 * @note 期望：以 ; 或 # 开头的行被忽略。
 */
TEST(INIConfigParseComments) {
    std::istringstream iss("; This is a comment\n"
                           "# Another comment\n"
                           "[server]\n"
                           "; Comment inside section\n"
                           "host = localhost\n"
                           "# Another comment\n"
                           "port = 8080\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_TRUE(config.has("server.host"));
    ASSERT_TRUE(config.has("server.port"));
    ASSERT_EQ(std::string("localhost"), config.get<std::string>("server.host"));
    ASSERT_EQ(8080, config.get<int>("server.port"));
    ASSERT_FALSE(config.has("This is a comment"));
    ASSERT_FALSE(config.has("Another comment"));
}

/**
 * @brief 测试 INIConfig 解析空行和空白。
 * @note 期望：空行被忽略，键值对两端空格被修剪。
 */
TEST(INIConfigParseWhitespace) {
    std::istringstream iss("\n"
                           "  \n"
                           "\t  \n"
                           "[server]  \n"
                           "  host  =  localhost  \n"
                           "  port  =  8080  \n"
                           "\n"
                           "[database]\n"
                           "  user  =  admin  \n");
    console::INIConfig config;
    iss >> config;
    ASSERT_EQ(std::string("localhost"), config.get<std::string>("server.host"));
    ASSERT_EQ(8080, config.get<int>("server.port"));
    ASSERT_EQ(std::string("admin"), config.get<std::string>("database.user"));
}

/**
 * @brief 测试 INIConfig 的 get 方法。
 * @note 期望：正确获取各种类型的值。
 */
TEST(INIConfigGet) {
    std::istringstream iss("[data]\n"
                           "int_val = 42\n"
                           "double_val = 3.14\n"
                           "string_val = hello\n"
                           "bool_true = true\n"
                           "bool_yes = yes\n"
                           "bool_on = on\n"
                           "bool_1 = 1\n"
                           "bool_false = false\n"
                           "bool_no = no\n"
                           "bool_off = off\n"
                           "bool_0 = 0\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_EQ(42, config.get<int>("data.int_val"));
    ASSERT_NEAR(3.14, config.get<double>("data.double_val"), 0.001);
    ASSERT_EQ(std::string("hello"), config.get<std::string>("data.string_val"));
    ASSERT_TRUE(config.get<bool>("data.bool_true"));
    ASSERT_TRUE(config.get<bool>("data.bool_yes"));
    ASSERT_TRUE(config.get<bool>("data.bool_on"));
    ASSERT_TRUE(config.get<bool>("data.bool_1"));
    ASSERT_FALSE(config.get<bool>("data.bool_false"));
    ASSERT_FALSE(config.get<bool>("data.bool_no"));
    ASSERT_FALSE(config.get<bool>("data.bool_off"));
    ASSERT_FALSE(config.get<bool>("data.bool_0"));
}

/**
 * @brief 测试 INIConfig 的 get 方法（节不存在）。
 * @note 期望：节不存在时抛出 IndexError。
 */
TEST(INIConfigGetSectionNotFound) {
    std::istringstream iss("[server]\nhost = localhost\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_THROWS(config.get("nonexistent.host"), console::IndexError);
}

/**
 * @brief 测试 INIConfig 的 get 方法（键不存在）。
 * @note 期望：键不存在时抛出 IndexError。
 */
TEST(INIConfigGetKeyNotFound) {
    std::istringstream iss("[server]\nhost = localhost\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_THROWS(config.get("server.port"), console::IndexError);
}

/**
 * @brief 测试 INIConfig 的 get 方法（格式无效）。
 * @note 期望：格式无效时抛出 IndexError。
 */
TEST(INIConfigGetInvalidFormat) {
    std::istringstream iss("[server]\nhost = localhost\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_THROWS(config.get("invalid_format"), console::IndexError);
    ASSERT_THROWS(config.get(""), console::IndexError);
    ASSERT_THROWS(config.get("."), console::IndexError);
}

/**
 * @brief 测试 INIConfig 的 get 带默认值。
 * @note 期望：存在时返回值，不存在时返回默认值。
 */
TEST(INIConfigGetWithDefault) {
    std::istringstream iss("[server]\nhost = localhost\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_EQ(std::string("localhost"),
        config.get("server.host", std::string("default")));
    ASSERT_EQ(std::string("default"),
        config.get("server.port", std::string("default")));
    ASSERT_EQ(8080, config.get("server.port", 8080));
    ASSERT_EQ(42, config.get("nonexistent.key", 42));
}

/**
 * @brief 测试 INIConfig 的 set 方法。
 * @note 期望：设置新值或更新现有值。
 */
TEST(INIConfigSet) {
    console::INIConfig config;
    config.set("server.host", "localhost");
    config.set("server.port", "8080");
    config.set("database.user", "admin");
    ASSERT_TRUE(config.has("server.host"));
    ASSERT_TRUE(config.has("server.port"));
    ASSERT_TRUE(config.has("database.user"));
    ASSERT_EQ(std::string("localhost"), config.get<std::string>("server.host"));
    ASSERT_EQ(8080, config.get<int>("server.port"));
    ASSERT_EQ(std::string("admin"), config.get<std::string>("database.user"));
    config.set("server.host", "127.0.0.1");
    ASSERT_EQ(std::string("127.0.0.1"), config.get<std::string>("server.host"));
}

/**
 * @brief 测试 INIConfig 的 has 方法。
 * @note 期望：存在时返回 true，不存在时返回 false。
 */
TEST(INIConfigHas) {
    std::istringstream iss("[server]\nhost = localhost\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_TRUE(config.has("server"));
    ASSERT_TRUE(config.has("server.host"));
    ASSERT_FALSE(config.has("server.port"));
    ASSERT_FALSE(config.has("database"));
    ASSERT_FALSE(config.has("database.user"));
    ASSERT_FALSE(config.has("invalid"));
}

/**
 * @brief 测试 INIConfig 的 remove 方法。
 * @note 期望：成功删除配置项或节。
 */
TEST(INIConfigRemove) {
    std::istringstream iss("[server]\n"
                           "host = localhost\n"
                           "port = 8080\n"
                           "[database]\n"
                           "user = admin\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_TRUE(config.remove("server.port"));
    ASSERT_FALSE(config.has("server.port"));
    ASSERT_TRUE(config.has("server.host"));
    ASSERT_FALSE(config.remove("server.nonexistent"));
    ASSERT_TRUE(config.remove("database"));
    ASSERT_FALSE(config.has("database"));
    ASSERT_FALSE(config.has("database.user"));
    ASSERT_FALSE(config.remove("nonexistent"));
}

/**
 * @brief 测试 INIConfig 的流输出运算符。
 * @note 期望：输出符合 INI 格式。
 */
TEST(INIConfigOutputStream) {
    console::INIConfig config;
    config.set("server.host", "localhost");
    config.set("server.port", "8080");
    config.set("database.user", "admin");
    config.set("database.password", "secret");
    std::ostringstream oss;
    oss << config;
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "[server]");
    ASSERT_STRCONTAINS(output.c_str(), "host = localhost");
    ASSERT_STRCONTAINS(output.c_str(), "port = 8080");
    ASSERT_STRCONTAINS(output.c_str(), "[database]");
    ASSERT_STRCONTAINS(output.c_str(), "user = admin");
    ASSERT_STRCONTAINS(output.c_str(), "password = secret");
}

/**
 * @brief 测试 INIConfig 从文件加载。
 * @note 期望：正确从文件加载配置。
 */
TEST(INIConfigLoadFromFile) {
    std::string        content  = "[server]\n"
                                  "host = localhost\n"
                                  "port = 8080\n";
    std::string        filename = create_temp_ini(content);
    console::INIConfig config(filename);
    ASSERT_EQ(std::string("localhost"), config.get<std::string>("server.host"));
    ASSERT_EQ(8080, config.get<int>("server.port"));
    remove_temp_file(filename);
}

/**
 * @brief 测试 INIConfig 保存到文件。
 * @note 期望：正确保存配置到文件。
 */
TEST(INIConfigSaveToFile) {
    console::INIConfig config;
    config.set("server.host", "localhost");
    config.set("server.port", "8080");
    std::string filename = create_temp_ini("");
    config.save(filename);
    std::string content = read_file(filename);
    ASSERT_STRCONTAINS(content.c_str(), "[server]");
    ASSERT_STRCONTAINS(content.c_str(), "host = localhost");
    ASSERT_STRCONTAINS(content.c_str(), "port = 8080");
    remove_temp_file(filename);
}

/**
 * @brief 测试 INIConfig 保存后重新加载。
 * @note 期望：保存和加载后配置一致。
 */
TEST(INIConfigSaveAndReload) {
    console::INIConfig config1;
    config1.set("server.host", "localhost");
    config1.set("server.port", "8080");
    config1.set("database.user", "admin");
    config1.set("database.password", "secret");
    std::string filename = create_temp_ini("");
    config1.save(filename);
    console::INIConfig config2(filename);
    ASSERT_EQ(
        std::string("localhost"), config2.get<std::string>("server.host"));
    ASSERT_EQ(8080, config2.get<int>("server.port"));
    ASSERT_EQ(std::string("admin"), config2.get<std::string>("database.user"));
    ASSERT_EQ(
        std::string("secret"), config2.get<std::string>("database.password"));
    remove_temp_file(filename);
}

/**
 * @brief 测试 INIConfig 的 Item 隐式类型转换（无效转换）。
 * @note 期望：无效类型转换抛出 TypeError。
 */
TEST(INIConfigItemInvalidConversion) {
    std::istringstream iss("[data]\ninvalid_int = abc\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_THROWS(config.get<int>("data.invalid_int"), console::TypeError);
}

/**
 * @brief 测试 INIConfig 的 Item 布尔转换（无效值）。
 * @note 期望：无效布尔值抛出 TypeError。
 */
TEST(INIConfigItemInvalidBool) {
    std::istringstream iss("[data]\ninvalid_bool = maybe\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_THROWS(config.get<bool>("data.invalid_bool"), console::TypeError);
}

/**
 * @brief 测试 INIConfig 多个节相同名称合并。
 * @note 期望：相同节名合并，后出现的覆盖先出现的。
 */
TEST(INIConfigDuplicateSection) {
    std::istringstream iss("[server]\n"
                           "host = localhost\n"
                           "[server]\n"
                           "port = 8080\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_TRUE(config.has("server.host"));
    ASSERT_TRUE(config.has("server.port"));
    ASSERT_EQ(std::string("localhost"), config.get<std::string>("server.host"));
    ASSERT_EQ(8080, config.get<int>("server.port"));
}

/**
 * @brief 测试 INIConfig 相同键重复出现。
 * @note 期望：后出现的覆盖先出现的。
 */
TEST(INIConfigDuplicateKey) {
    std::istringstream iss("[server]\n"
                           "host = localhost\n"
                           "host = 127.0.0.1\n");
    console::INIConfig config;
    iss >> config;
    ASSERT_EQ(std::string("127.0.0.1"), config.get<std::string>("server.host"));
}

/**
 * @brief 测试 INIConfig 的 get 返回 Item 类型。
 * @note 期望：Item 可隐式转换为目标类型。
 */
TEST(INIConfigGetItemType) {
    std::istringstream iss("[data]\nvalue = 123\n");
    console::INIConfig config;
    iss >> config;
    console::INIConfig::Item item    = config.get("data.value");
    int                      int_val = item;
    ASSERT_EQ(123, int_val);
    std::string str_val = item;
    ASSERT_EQ(std::string("123"), str_val);
}

/**
 * @brief 测试 INIConfig 的 data 方法。
 * @note 期望：返回原始数据结构的只读引用。
 */
TEST(INIConfigDataMethod) {
    std::istringstream iss("[server]\nhost = localhost\n");
    console::INIConfig config;
    iss >> config;
    const auto &data = config.data();
    ASSERT_EQ(1, data.size());
    ASSERT_TRUE(data.find("server") != data.end());
    const auto &section = data.at("server");
    ASSERT_TRUE(section.find("host") != section.end());
    ASSERT_EQ(std::string("localhost"), section.at("host"));
}

#ifndef NOMAIN
TEST_MAIN
#endif
