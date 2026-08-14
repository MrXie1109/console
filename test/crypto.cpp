/**
 * @file crypto.cpp
 * @brief 测试加密模块 (SHA256, MD5, Base64, AES)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */
#include "../include/crypto.h"

#include <cstdio>
#include <cstdlib>

#include "../include/test.h"

static std::string create_temp_file(const std::string &content) {
    std::string filename
        = "crypto_test_temp_" + std::to_string(rand()) + ".tmp";
    FILE *f = std::fopen(filename.c_str(), "wb");
    if (!f) {
        throw std::runtime_error("Failed to create temp file");
    }
    std::fwrite(content.c_str(), 1, content.size(), f);
    std::fclose(f);
    return filename;
}

static void remove_temp_file(const std::string &filename) {
    std::remove(filename.c_str());
}

/**
 * @brief 测试 SHA256 空字符串哈希。
 * @note 期望：空字符串的 SHA256 哈希值为已知标准值。
 */
TEST(SHA256EmptyString) {
    std::string result = console::crypto::sha256("");
    ASSERT_EQ(
        std::string(
            "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"),
        result);
}

/**
 * @brief 测试 SHA256 简单字符串。
 * @note 期望：已知字符串的哈希值正确。
 */
TEST(SHA256SimpleString) {
    std::string result = console::crypto::sha256("hello");
    ASSERT_EQ(
        std::string(
            "2cf24dba5fb0a30e26e83b2ac5b9e29e1b161e5c1fa7425e73043362938b9824"),
        result);
}

/**
 * @brief 测试 SHA256 标准测试向量。
 * @note 期望：与已知标准值一致。
 */
TEST(SHA256StandardVectors) {
    ASSERT_EQ(
        std::string(
            "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"),
        console::crypto::sha256("abc"));
    ASSERT_EQ(
        std::string(
            "248d6a61d20638b8e5c026930c3e6039a33ce45964ff2167f6ecedd419db06c1"),
        console::crypto::sha256(
            "abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"));
}

/**
 * @brief 测试 SHA256 包含特殊字符。
 * @note 期望：特殊字符正确处理。
 */
TEST(SHA256SpecialCharacters) {
    std::string result = console::crypto::sha256("Hello, 世界!");
    ASSERT_NE(std::string(""), result);
    ASSERT_EQ(64, result.size());
}

/**
 * @brief 测试文件 SHA256。
 * @note 期望：文件内容哈希值与直接计算字符串一致。
 */
TEST(FileSHA256) {
    std::string content     = "This is a test file content.";
    std::string filename    = create_temp_file(content);
    std::string file_hash   = console::crypto::file_sha256(filename);
    std::string string_hash = console::crypto::sha256(content);
    ASSERT_EQ(string_hash, file_hash);
    remove_temp_file(filename);
}

/**
 * @brief 测试文件 SHA256 空文件。
 * @note 期望：空文件哈希值等于空字符串哈希值。
 */
TEST(FileSHA256EmptyFile) {
    std::string filename    = create_temp_file("");
    std::string file_hash   = console::crypto::file_sha256(filename);
    std::string string_hash = console::crypto::sha256("");
    ASSERT_EQ(string_hash, file_hash);
    remove_temp_file(filename);
}

/**
 * @brief 测试文件 SHA256 大文件。
 * @note 期望：大文件哈希计算正常。
 */
TEST(FileSHA256LargeFile) {
    std::string content(10000, 'A');
    std::string filename    = create_temp_file(content);
    std::string file_hash   = console::crypto::file_sha256(filename);
    std::string string_hash = console::crypto::sha256(content);
    ASSERT_EQ(string_hash, file_hash);
    remove_temp_file(filename);
}

/**
 * @brief 测试文件 SHA256 不存在的文件。
 * @note 期望：返回错误信息。
 */
TEST(FileSHA256Nonexistent) {
    std::string result
        = console::crypto::file_sha256("nonexistent_file_12345.tmp");
    ASSERT_STRCONTAINS(result.c_str(), "Cannot open file");
}

/**
 * @brief 测试 MD5 空字符串。
 * @note 期望：空字符串的 MD5 哈希值为已知标准值。
 */
TEST(MD5EmptyString) {
    std::string result = console::crypto::md5("");
    ASSERT_EQ(std::string("d41d8cd98f00b204e9800998ecf8427e"), result);
}

/**
 * @brief 测试 MD5 简单字符串。
 * @note 期望：已知字符串的哈希值正确。
 */
TEST(MD5SimpleString) {
    std::string result = console::crypto::md5("hello");
    ASSERT_EQ(std::string("5d41402abc4b2a76b9719d911017c592"), result);
}

/**
 * @brief 测试 MD5 标准测试向量。
 * @note 期望：与已知标准值一致。
 */
TEST(MD5StandardVectors) {
    ASSERT_EQ(std::string("900150983cd24fb0d6963f7d28e17f72"),
        console::crypto::md5("abc"));
    ASSERT_EQ(std::string("f96b697d7cb7938d525a2f31aaf161d0"),
        console::crypto::md5("message digest"));
}

/**
 * @brief 测试 MD5 与 SHA256 不同。
 * @note 期望：同一字符串的 MD5 和 SHA256 哈希值不同。
 */
TEST(MD5VsSHA256) {
    std::string input       = "test";
    std::string md5_hash    = console::crypto::md5(input);
    std::string sha256_hash = console::crypto::sha256(input);
    ASSERT_NE(md5_hash, sha256_hash);
    ASSERT_EQ(32, md5_hash.size());
    ASSERT_EQ(64, sha256_hash.size());
}

/**
 * @brief 测试 Base64 编码空字符串。
 * @note 期望：空字符串编码为空。
 */
TEST(Base64EncodeEmpty) {
    std::string result = console::crypto::base64_encode("");
    ASSERT_EQ(std::string(""), result);
}

/**
 * @brief 测试 Base64 编码标准向量。
 * @note 期望：与已知标准值一致。
 */
TEST(Base64EncodeStandard) {
    ASSERT_EQ(std::string("TWFu"), console::crypto::base64_encode("Man"));
    ASSERT_EQ(std::string("TWE="), console::crypto::base64_encode("Ma"));
    ASSERT_EQ(std::string("TQ=="), console::crypto::base64_encode("M"));
    ASSERT_EQ(std::string("SGVsbG8sIFdvcmxkIQ=="),
        console::crypto::base64_encode("Hello, World!"));
}

/**
 * @brief 测试 Base64 解码标准向量。
 * @note 期望：解码结果与原始字符串一致。
 */
TEST(Base64DecodeStandard) {
    ASSERT_EQ(std::string("Man"), console::crypto::base64_decode("TWFu"));
    ASSERT_EQ(std::string("Ma"), console::crypto::base64_decode("TWE="));
    ASSERT_EQ(std::string("M"), console::crypto::base64_decode("TQ=="));
    ASSERT_EQ(std::string("Hello, World!"),
        console::crypto::base64_decode("SGVsbG8sIFdvcmxkIQ=="));
}

/**
 * @brief 测试 Base64 编码和解码往返。
 * @note 期望：编码后再解码得到原始字符串。
 */
TEST(Base64RoundTrip) {
    std::string original = "The quick brown fox jumps over the lazy dog";
    std::string encoded  = console::crypto::base64_encode(original);
    std::string decoded  = console::crypto::base64_decode(encoded);
    ASSERT_EQ(original, decoded);
}

/**
 * @brief 测试 Base64 编码和解码往返（包含特殊字符）。
 * @note 期望：特殊字符编码解码后保持不变。
 */
TEST(Base64RoundTripSpecial) {
    std::string original = "Hello, 世界! @#$%^&*()";
    std::string encoded  = console::crypto::base64_encode(original);
    std::string decoded  = console::crypto::base64_decode(encoded);
    ASSERT_EQ(original, decoded);
}

/**
 * @brief 测试 Base64 解码忽略非 Base64 字符。
 * @note 期望：非 Base64 字符被忽略，正确解码。
 */
TEST(Base64DecodeIgnoreInvalid) {
    std::string encoded = "SGVsbG8s\nIFdvcmxkIQ==";
    std::string decoded = console::crypto::base64_decode(encoded);
    ASSERT_EQ(std::string("Hello, World!"), decoded);
    std::string encoded2 = "SGVsbG8sIFdvcmxkIQ==!@#$";
    std::string decoded2 = console::crypto::base64_decode(encoded2);
    ASSERT_EQ(std::string("Hello, World!"), decoded2);
}

/**
 * @brief 测试 Base64 编码和解码随机数据。
 * @note 期望：随机数据编码解码后保持不变。
 */
TEST(Base64RoundTripRandom) {
    std::string original(256, '\0');
    for (int i = 0; i < 256; ++i) {
        original[i] = static_cast<char>(i);
    }
    std::string encoded = console::crypto::base64_encode(original);
    std::string decoded = console::crypto::base64_decode(encoded);
    ASSERT_EQ(original, decoded);
}

/**
 * @brief 测试 AES 加密解密简单字符串。
 * @note 期望：加密后解密得到原始字符串。
 */
TEST(AESEncryptDecryptSimple) {
    std::string plaintext  = "Hello, World!";
    std::string key        = "secretkey123456";
    std::string ciphertext = console::crypto::aes_encrypt(plaintext, key);
    std::string decrypted  = console::crypto::aes_decrypt(ciphertext, key);
    ASSERT_EQ(plaintext, decrypted);
}

/**
 * @brief 测试 AES 加密解密空字符串。
 * @note 期望：空字符串加密解密后仍为空。
 */
TEST(AESEncryptDecryptEmpty) {
    std::string plaintext  = "";
    std::string key        = "secretkey123456";
    std::string ciphertext = console::crypto::aes_encrypt(plaintext, key);
    std::string decrypted  = console::crypto::aes_decrypt(ciphertext, key);
    ASSERT_EQ(plaintext, decrypted);
}

/**
 * @brief 测试 AES 加密解密长字符串。
 * @note 期望：长字符串加密解密后保持不变。
 */
TEST(AESEncryptDecryptLong) {
    std::string plaintext(1000, 'A');
    for (int i = 0; i < 1000; ++i) {
        plaintext[i] = static_cast<char>('A' + (i % 26));
    }
    std::string key        = "secretkey123456";
    std::string ciphertext = console::crypto::aes_encrypt(plaintext, key);
    std::string decrypted  = console::crypto::aes_decrypt(ciphertext, key);
    ASSERT_EQ(plaintext, decrypted);
}

/**
 * @brief 测试 AES 不同密钥。
 * @note 期望：不同密钥解密相同密文失败。
 */
TEST(AESDifferentKeys) {
    std::string plaintext  = "Hello, World!";
    std::string key1       = "secretkey123456";
    std::string key2       = "anotherkey78901";
    std::string ciphertext = console::crypto::aes_encrypt(plaintext, key1);
    std::string decrypted  = console::crypto::aes_decrypt(ciphertext, key2);
    ASSERT_NE(plaintext, decrypted);
}

/**
 * @brief 测试 AES 短密钥填充。
 * @note 期望：短密钥自动填充到16字节。
 */
TEST(AESShortKey) {
    std::string plaintext  = "Hello, World!";
    std::string key        = "short";
    std::string ciphertext = console::crypto::aes_encrypt(plaintext, key);
    std::string decrypted  = console::crypto::aes_decrypt(ciphertext, key);
    ASSERT_EQ(plaintext, decrypted);
}

/**
 * @brief 测试 AES 长密钥截断。
 * @note 期望：长密钥自动截断到16字节。
 */
TEST(AESLongKey) {
    std::string plaintext  = "Hello, World!";
    std::string key        = "this_is_a_very_long_key_exceeding_16_bytes";
    std::string ciphertext = console::crypto::aes_encrypt(plaintext, key);
    std::string decrypted  = console::crypto::aes_decrypt(ciphertext, key);
    ASSERT_EQ(plaintext, decrypted);
}

/**
 * @brief 测试 AES 无效密文。
 * @note 期望：无效密文返回错误信息。
 */
TEST(AESInvalidCiphertext) {
    std::string key       = "secretkey123456";
    std::string invalid   = "invalid";
    std::string decrypted = console::crypto::aes_decrypt(invalid, key);
    ASSERT_EQ(std::string("Invalid ciphertext"), decrypted);
}

/**
 * @brief 测试 AES 加密后 Base64 格式。
 * @note 期望：加密结果符合 Base64 格式，且长度合理。
 */
TEST(AESEncryptFormat) {
    std::string plaintext  = "Hello";
    std::string key        = "secretkey123456";
    std::string ciphertext = console::crypto::aes_encrypt(plaintext, key);
    ASSERT_GT(ciphertext.size(), 0);
    std::string decoded = console::crypto::base64_decode(ciphertext);
    ASSERT_GE(decoded.size(), 16); // 至少包含 IV
}

/**
 * @brief 测试 AES 多次加密同一明文产生不同密文。
 * @note 期望：由于使用随机 IV，同一明文多次加密产生不同密文。
 */
TEST(AESMultipleEncryptDifferent) {
    std::string plaintext   = "Hello, World!";
    std::string key         = "secretkey123456";
    std::string ciphertext1 = console::crypto::aes_encrypt(plaintext, key);
    std::string ciphertext2 = console::crypto::aes_encrypt(plaintext, key);
    ASSERT_NE(ciphertext1, ciphertext2);
    std::string decrypted1 = console::crypto::aes_decrypt(ciphertext1, key);
    std::string decrypted2 = console::crypto::aes_decrypt(ciphertext2, key);
    ASSERT_EQ(plaintext, decrypted1);
    ASSERT_EQ(plaintext, decrypted2);
}

/**
 * @brief 测试 AES 加密解密包含特殊字符。
 * @note 期望：特殊字符正确处理。
 */
TEST(AESEncryptDecryptSpecial) {
    std::string plaintext  = "Hello, 世界! @#$%^&*()";
    std::string key        = "secretkey123456";
    std::string ciphertext = console::crypto::aes_encrypt(plaintext, key);
    std::string decrypted  = console::crypto::aes_decrypt(ciphertext, key);
    ASSERT_EQ(plaintext, decrypted);
}

/**
 * @brief 测试 AES 加密解密二进制数据。
 * @note 期望：二进制数据正确处理。
 */
TEST(AESEncryptDecryptBinary) {
    std::string plaintext;
    plaintext.resize(128);
    for (int i = 0; i < 128; ++i) {
        plaintext[i] = static_cast<char>(i * 2);
    }
    std::string key        = "secretkey123456";
    std::string ciphertext = console::crypto::aes_encrypt(plaintext, key);
    std::string decrypted  = console::crypto::aes_decrypt(ciphertext, key);
    ASSERT_EQ(plaintext, decrypted);
}

/**
 * @brief 测试 AES 非16倍数长度数据。
 * @note 期望：任意长度数据正确处理。
 */
TEST(AESEncryptDecryptVariousLengths) {
    std::string key = "secretkey123456";
    for (int len = 0; len < 100; ++len) {
        std::string plaintext(len, 'X');
        std::string ciphertext = console::crypto::aes_encrypt(plaintext, key);
        std::string decrypted  = console::crypto::aes_decrypt(ciphertext, key);
        ASSERT_EQ(plaintext, decrypted);
    }
}

#ifndef NOMAIN
TEST_MAIN
#endif
