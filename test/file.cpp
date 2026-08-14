/**
 * @file file.cpp
 * @brief 测试文件路径封装模块 (BasicPath)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */
#include "../include/file.h"

#include <cstdio>
#include <cstdlib>

#include "../include/test.h"

static std::string get_temp_filename() {
    static int counter = 0;
    return "file_test_temp_" + std::to_string(rand()) + "_"
           + std::to_string(++counter) + ".tmp";
}

static void remove_temp_file(const std::string &filename) {
    std::remove(filename.c_str());
}

/**
 * @brief 测试 Path 默认构造和字符串转换。
 * @note 期望：构造的 Path 对象存储正确的路径字符串。
 */
TEST(PathConstruction) {
    console::Path p("test.txt");
    ASSERT_EQ(std::string("test.txt"), p.str());
}

/**
 * @brief 测试 Path 路径拼接操作符。
 * @note 期望：两个路径正确拼接。
 */
TEST(PathConcatenation) {
    console::Path p1("dir");
    console::Path p2("file.txt");
    console::Path p3 = p1 / p2;
    ASSERT_EQ(std::string("dir/file.txt"), p3.str());
    console::Path p4 = p1 / "subdir" / p2;
    ASSERT_EQ(std::string("dir/subdir/file.txt"), p4.str());
}

/**
 * @brief 测试 Path exists 方法。
 * @note 期望：存在返回 true，不存在返回 false。
 */
TEST(PathExists) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    ASSERT_FALSE(p.exists());
    p.touch();
    ASSERT_TRUE(p.exists());
    p.remove();
    ASSERT_FALSE(p.exists());
}

/**
 * @brief 测试 Path touch 方法。
 * @note 期望：touch 创建空文件。
 */
TEST(PathTouch) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    ASSERT_FALSE(p.exists());
    p.touch();
    ASSERT_TRUE(p.exists());
    std::string content = p.read_text();
    ASSERT_EQ(std::string(""), content);
    p.remove();
}

/**
 * @brief 测试 Path ensure 方法。
 * @note 期望：ensure 创建文件（如果不存在），不覆盖已有内容。
 */
TEST(PathEnsure) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    ASSERT_FALSE(p.exists());
    p.ensure();
    ASSERT_TRUE(p.exists());
    std::string content = p.read_text();
    ASSERT_EQ(std::string(""), content);
    p.write_text("test");
    p.ensure();
    content = p.read_text();
    ASSERT_EQ(std::string("test"), content);
    p.remove();
}

/**
 * @brief 测试 Path remove 方法。
 * @note 期望：remove 删除文件，不存在时抛出异常。
 */
TEST(PathRemove) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    p.touch();
    ASSERT_TRUE(p.exists());
    p.remove();
    ASSERT_FALSE(p.exists());
    ASSERT_THROWS(p.remove(), console::FileError);
}

/**
 * @brief 测试 Path write_text 和 read_text。
 * @note 期望：写入和读取文本内容一致。
 */
TEST(PathWriteReadText) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    std::string   content = "Hello, World!\nThis is a test.\n";
    p.write_text(content);
    std::string read_content = p.read_text();
    ASSERT_EQ(content, read_content);
    p.remove();
}

/**
 * @brief 测试 Path write_text 空字符串。
 * @note 期望：空字符串写入后读取为空。
 */
TEST(PathWriteReadTextEmpty) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    p.write_text("");
    std::string content = p.read_text();
    ASSERT_EQ(std::string(""), content);
    p.remove();
}

/**
 * @brief 测试 Path write_binary 和 read_binary。
 * @note 期望：写入和读取二进制内容一致。
 */
TEST(PathWriteReadBinary) {
    std::string          filename = get_temp_filename();
    console::Path        p(filename);
    console::Path::Bytes content = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD};
    p.write_binary(content);
    console::Path::Bytes read_content = p.read_binary();
    ASSERT_EQ(content, read_content);
    p.remove();
}

/**
 * @brief 测试 Path write_lines 和 read_lines。
 * @note 期望：写入和读取行内容一致。
 */
TEST(PathWriteReadLines) {
    std::string              filename = get_temp_filename();
    console::Path            p(filename);
    std::vector<std::string> lines = {"Line 1", "Line 2", "Line 3", "Line 4"};
    p.write_lines(lines);
    std::vector<std::string> read_lines = p.read_lines();
    ASSERT_EQ(lines.size(), read_lines.size());
    for (size_t i = 0; i < lines.size(); ++i) {
        ASSERT_EQ(lines[i], read_lines[i]);
    }
    p.remove();
}

/**
 * @brief 测试 Path write_lines 空向量。
 * @note 期望：空向量不写入任何内容，文件保持为空。
 */
TEST(PathWriteLinesEmpty) {
    std::string              filename = get_temp_filename();
    console::Path            p(filename);
    std::vector<std::string> lines;
    p.write_lines(lines);
    std::string content = p.read_text();
    ASSERT_EQ(std::string(""), content);
    p.remove();
}

/**
 * @brief 测试 Path read_lines 空文件。
 * @note 期望：空文件返回空向量。
 */
TEST(PathReadLinesEmpty) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    p.touch();
    std::vector<std::string> lines = p.read_lines();
    ASSERT_TRUE(lines.empty());
    p.remove();
}

/**
 * @brief 测试 Path write_POD 和 read_POD。
 * @note 期望：POD 类型数据读写一致。
 */
TEST(PathWriteReadPOD) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    struct Data {
        int    a;
        double b;
        char   c[4];
    };
    Data original = {42, 3.14, {'A', 'B', 'C', 'D'}};
    p.write_POD(original);
    Data read_data = p.read_POD<Data>();
    ASSERT_EQ(original.a, read_data.a);
    ASSERT_NEAR(original.b, read_data.b, 0.001);
    for (int i = 0; i < 4; ++i) {
        ASSERT_EQ(original.c[i], read_data.c[i]);
    }
    p.remove();
}

/**
 * @brief 测试 Path write_POD 和 read_POD 整数类型。
 * @note 期望：整数类型读写一致。
 */
TEST(PathWriteReadPODInt) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    int           original = 12345;
    p.write_POD(original);
    int read_data = p.read_POD<int>();
    ASSERT_EQ(original, read_data);
    p.remove();
}

/**
 * @brief 测试 Path write_POD 和 read_POD 浮点类型。
 * @note 期望：浮点类型读写一致。
 */
TEST(PathWriteReadPODFloat) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    double        original = 3.141592653589793;
    p.write_POD(original);
    double read_data = p.read_POD<double>();
    ASSERT_NEAR(original, read_data, 0.000001);
    p.remove();
}

/**
 * @brief 测试 Path unsafe_read_POD 和 unsafe_write_POD。
 * @note 期望：不安全版本读写一致。
 */
TEST(PathUnsafeReadWritePOD) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    struct Data {
        int  x;
        long y;
    };
    Data original = {100, 200};
    p.unsafe_write_POD(original);
    Data read_data = p.unsafe_read_POD<Data>();
    ASSERT_EQ(original.x, read_data.x);
    ASSERT_EQ(original.y, read_data.y);
    p.remove();
}

/**
 * @brief 测试 Path read_text 不存在的文件。
 * @note 期望：不存在时抛出 FileError。
 */
TEST(PathReadTextNonexistent) {
    console::Path p("nonexistent_file_12345.tmp");
    ASSERT_THROWS(p.read_text(), console::FileError);
}

/**
 * @brief 测试 Path read_binary 不存在的文件。
 * @note 期望：不存在时抛出 FileError。
 */
TEST(PathReadBinaryNonexistent) {
    console::Path p("nonexistent_file_12345.tmp");
    ASSERT_THROWS(p.read_binary(), console::FileError);
}

/**
 * @brief 测试 Path read_POD 不存在的文件。
 * @note 期望：不存在时抛出 FileError。
 */
TEST(PathReadPODNonexistent) {
    console::Path p("nonexistent_file_12345.tmp");
    ASSERT_THROWS(p.read_POD<int>(), console::FileError);
}

/**
 * @brief 测试 Path stream 方法。
 * @note 期望：返回有效的 fstream 对象。
 */
TEST(PathStream) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    {
        std::fstream fs = p.stream(std::ios::out);
        ASSERT_TRUE(fs.is_open());
        fs << "Hello, Stream!";
        fs.close();
    }
    {
        std::fstream fs = p.stream(std::ios::in);
        ASSERT_TRUE(fs.is_open());
        std::string content;
        std::getline(fs, content);
        ASSERT_EQ(std::string("Hello, Stream!"), content);
    }
    p.remove();
}

/**
 * @brief 测试 Path 宽字符版本 (WPath)。
 * @note 期望：WPath 正确处理宽字符路径。
 */
TEST(WPathConstruction) {
    console::WPath p(L"test.txt");
    ASSERT_EQ(std::wstring(L"test.txt"), p.str());
}

/**
 * @brief 测试 Path 宽字符版本文件操作。
 * @note 期望：WPath 文件读写正常工作。
 */
TEST(WPathFileOperations) {
    std::string    filename = get_temp_filename();
    std::wstring   wfilename(filename.begin(), filename.end());
    console::WPath p(wfilename);
    std::wstring   content = L"Hello, Wide World!\nSecond line.";
    p.write_text(content);
    std::wstring read_content = p.read_text();
    ASSERT_EQ(content, read_content);
    p.remove();
}

/**
 * @brief 测试 Path 宽字符版本路径拼接。
 * @note 期望：WPath 拼接正确。
 */
TEST(WPathConcatenation) {
    console::WPath p1(L"dir");
    console::WPath p2(L"file.txt");
    console::WPath p3 = p1 / p2;
    ASSERT_EQ(std::wstring(L"dir/file.txt"), p3.str());
    p3 = p1 / L"subdir" / p2;
    ASSERT_EQ(std::wstring(L"dir/subdir/file.txt"), p3.str());
}

/**
 * @brief 测试 Path exists 与 touch 结合。
 * @note 期望：touch 后 exists 返回 true，remove 后返回 false。
 */
TEST(PathExistsTouchRemove) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    ASSERT_FALSE(p.exists());
    p.touch();
    ASSERT_TRUE(p.exists());
    p.remove();
    ASSERT_FALSE(p.exists());
}

/**
 * @brief 测试 Path 大文件读写。
 * @note 期望：大文件正确读写。
 */
TEST(PathLargeFile) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    std::string   content(100000, 'X');
    p.write_text(content);
    std::string read_content = p.read_text();
    ASSERT_EQ(content, read_content);
    p.remove();
}

/**
 * @brief 测试 Path 读取 lines 末尾无换行符。
 * @note 期望：正确处理无换行符结尾。
 */
TEST(PathReadLinesNoTrailingNewline) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    p.write_text("Line1\nLine2\nLine3");
    std::vector<std::string> lines = p.read_lines();
    ASSERT_EQ(3, lines.size());
    ASSERT_EQ(std::string("Line1"), lines[0]);
    ASSERT_EQ(std::string("Line2"), lines[1]);
    ASSERT_EQ(std::string("Line3"), lines[2]);
    p.remove();
}

/**
 * @brief 测试 Path 读取 lines 多个连续换行符。
 * @note 期望：连续换行符产生空行。
 */
TEST(PathReadLinesMultipleNewlines) {
    std::string   filename = get_temp_filename();
    console::Path p(filename);
    p.write_text("Line1\n\nLine3\n\n");
    std::vector<std::string> lines = p.read_lines();
    ASSERT_EQ(5, lines.size());
    ASSERT_EQ(std::string("Line1"), lines[0]);
    ASSERT_EQ(std::string(""), lines[1]);
    ASSERT_EQ(std::string("Line3"), lines[2]);
    ASSERT_EQ(std::string(""), lines[3]);
    ASSERT_EQ(std::string(""), lines[4]);
    p.remove();
}

#ifndef NOMAIN
TEST_MAIN
#endif
