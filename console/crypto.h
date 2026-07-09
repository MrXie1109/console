/**
 * @file crypto.h
 * @brief 提供SHA256、MD5、Base64和AES加密功能的加密库。
 * @details
 * 该库实现了常用的密码学算法，包括哈希函数（SHA256、MD5）、编码转换（Base64）以及对称加密（AES-128-CTR）。
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
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <random>
#include <sstream>
#include <stdexcept>
#include <vector>

namespace console {
    /**
     * @brief 密码学相关的命名空间。
     */
    namespace crypto {
        /**
         * @brief SHA256哈希算法的内部实现细节。
         * @note 该命名空间中的函数和常量仅供内部使用，不建议直接调用。
         */
        namespace sha256_impl {
            /**
             * @brief SHA256算法使用的常量K数组。
             */
            static constexpr uint32_t K[64] = { //
                0x428a2f98,
                0x71374491,
                0xb5c0fbcf,
                0xe9b5dba5,
                0x3956c25b,
                0x59f111f1,
                0x923f82a4,
                0xab1c5ed5,
                0xd807aa98,
                0x12835b01,
                0x243185be,
                0x550c7dc3,
                0x72be5d74,
                0x80deb1fe,
                0x9bdc06a7,
                0xc19bf174,
                0xe49b69c1,
                0xefbe4786,
                0x0fc19dc6,
                0x240ca1cc,
                0x2de92c6f,
                0x4a7484aa,
                0x5cb0a9dc,
                0x76f988da,
                0x983e5152,
                0xa831c66d,
                0xb00327c8,
                0xbf597fc7,
                0xc6e00bf3,
                0xd5a79147,
                0x06ca6351,
                0x14292967,
                0x27b70a85,
                0x2e1b2138,
                0x4d2c6dfc,
                0x53380d13,
                0x650a7354,
                0x766a0abb,
                0x81c2c92e,
                0x92722c85,
                0xa2bfe8a1,
                0xa81a664b,
                0xc24b8b70,
                0xc76c51a3,
                0xd192e819,
                0xd6990624,
                0xf40e3585,
                0x106aa070,
                0x19a4c116,
                0x1e376c08,
                0x2748774c,
                0x34b0bcb5,
                0x391c0cb3,
                0x4ed8aa4a,
                0x5b9cca4f,
                0x682e6ff3,
                0x748f82ee,
                0x78a5636f,
                0x84c87814,
                0x8cc70208,
                0x90befffa,
                0xa4506ceb,
                0xbef9a3f7,
                0xc67178f2};

            /**
             * @brief 对32位无符号整数进行循环右移。
             * @param x 待移位的整数。
             * @param n 右移的位数。
             * @return uint32_t 循环右移后的结果。
             */
            inline uint32_t rotr(uint32_t x, uint32_t n) {
                return (x >> n) | (x << (32 - n));
            }

            /**
             * @brief SHA256中的选择函数Ch。
             * @param x 输入值。
             * @param y 输入值。
             * @param z 输入值。
             * @return uint32_t 计算结果。
             */
            inline uint32_t ch(uint32_t x, uint32_t y, uint32_t z) {
                return (x & y) ^ (~x & z);
            }

            /**
             * @brief SHA256中的多数函数Maj。
             * @param x 输入值。
             * @param y 输入值。
             * @param z 输入值。
             * @return uint32_t 计算结果。
             */
            inline uint32_t maj(uint32_t x, uint32_t y, uint32_t z) {
                return (x & y) ^ (x & z) ^ (y & z);
            }

            /**
             * @brief SHA256中的大西格玛0函数。
             * @param x 输入值。
             * @return uint32_t 计算结果。
             */
            inline uint32_t sigma0(uint32_t x) {
                return rotr(x, 2) ^ rotr(x, 13) ^ rotr(x, 22);
            }

            /**
             * @brief SHA256中的大西格玛1函数。
             * @param x 输入值。
             * @return uint32_t 计算结果。
             */
            inline uint32_t sigma1(uint32_t x) {
                return rotr(x, 6) ^ rotr(x, 11) ^ rotr(x, 25);
            }

            /**
             * @brief SHA256中的小西格玛0函数。
             * @param x 输入值。
             * @return uint32_t 计算结果。
             */
            inline uint32_t gamma0(uint32_t x) {
                return rotr(x, 7) ^ rotr(x, 18) ^ (x >> 3);
            }

            /**
             * @brief SHA256中的小西格玛1函数。
             * @param x 输入值。
             * @return uint32_t 计算结果。
             */
            inline uint32_t gamma1(uint32_t x) {
                return rotr(x, 17) ^ rotr(x, 19) ^ (x >> 10);
            }

            /**
             * @brief 对64字节的数据块执行SHA256变换。
             * @param state 当前哈希状态，长度为8的数组。
             * @param data 要处理的数据块，长度必须为64字节。
             */
            inline void transform(uint32_t *state, const uint8_t *data) {
                uint32_t W[64];
                uint32_t A, B, C, D, E, F, G, H, T1, T2;
                for (int i = 0; i < 16; i++)
                    W[i] = (data[i * 4] << 24) | (data[i * 4 + 1] << 16)
                           | (data[i * 4 + 2] << 8) | data[i * 4 + 3];
                for (int i = 16; i < 64; i++)
                    W[i] = gamma1(W[i - 2]) + W[i - 7] + gamma0(W[i - 15])
                           + W[i - 16];
                A = state[0];
                B = state[1];
                C = state[2];
                D = state[3];
                E = state[4];
                F = state[5];
                G = state[6];
                H = state[7];
                for (int i = 0; i < 64; i++) {
                    T1 = H + sigma1(E) + ch(E, F, G) + K[i] + W[i];
                    T2 = sigma0(A) + maj(A, B, C);
                    H  = G;
                    G  = F;
                    F  = E;
                    E  = D + T1;
                    D  = C;
                    C  = B;
                    B  = A;
                    A  = T1 + T2;
                }
                state[0] += A;
                state[1] += B;
                state[2] += C;
                state[3] += D;
                state[4] += E;
                state[5] += F;
                state[6] += G;
                state[7] += H;
            }
        }

        /**
         * @brief 计算输入字符串的SHA256哈希值。
         * @param input 要计算哈希的输入字符串。
         * @return std::string 64位十六进制字符串格式的SHA256哈希值。
         */
        inline std::string sha256(const std::string &input) {
            using namespace sha256_impl;
            uint32_t             state[8] = {//
                0x6a09e667,
                0xbb67ae85,
                0x3c6ef372,
                0xa54ff53a,
                0x510e527f,
                0x9b05688c,
                0x1f83d9ab,
                0x5be0cd19};
            std::vector<uint8_t> data(input.begin(), input.end());
            uint64_t             bit_len = data.size() * 8;
            data.push_back(0x80);
            while ((data.size() % 64) != 56) data.push_back(0x00);
            for (int i = 7; i >= 0; i--)
                data.push_back((bit_len >> (i * 8)) & 0xFF);
            for (size_t i = 0; i < data.size(); i += 64)
                transform(state, &data[i]);
            std::stringstream ss;
            for (int i = 0; i < 8; i++)
                ss << std::hex << std::setfill('0') << std::setw(8) << state[i];
            return ss.str();
        }

        /**
         * @brief 计算文件的SHA256哈希值。
         * @param filename 要计算哈希的文件路径。
         * @return std::string 64位十六进制字符串格式的SHA256哈希值。
         * @warning 对于大文件，该函数会逐块读取，内存占用较小，但处理时间较长。
         */
        inline std::string file_sha256(const std::string &filename) {
            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open()) return "Cannot open file: " + filename;
            using namespace sha256_impl;
            uint32_t             state[8] = {//
                0x6a09e667,
                0xbb67ae85,
                0x3c6ef372,
                0xa54ff53a,
                0x510e527f,
                0x9b05688c,
                0x1f83d9ab,
                0x5be0cd19};
            std::vector<uint8_t> buffer(64);
            uint64_t             total_bits = 0;
            while (file.read(reinterpret_cast<char *>(buffer.data()), 64)) {
                transform(state, buffer.data());
                total_bits += 512;
            }
            std::streamsize last_size = file.gcount();
            total_bits += last_size * 8;
            std::vector<uint8_t> last_block(
                buffer.begin(), buffer.begin() + last_size);
            last_block.push_back(0x80);
            while ((last_block.size() % 64) != 56) last_block.push_back(0x00);
            for (int i = 7; i >= 0; i--)
                last_block.push_back((total_bits >> (i * 8)) & 0xFF);
            for (size_t i = 0; i < last_block.size(); i += 64)
                transform(state, &last_block[i]);
            std::stringstream ss;
            for (int i = 0; i < 8; i++)
                ss << std::hex << std::setfill('0') << std::setw(8) << state[i];
            return ss.str();
        }

        /**
         * @brief MD5哈希算法的内部实现细节。
         * @warning
         * MD5算法已被证明存在碰撞漏洞，不建议用于安全敏感场景，仅用于兼容遗留系统。
         */
        namespace md5_impl {
            /**
             * @brief MD5算法中每轮循环使用的左移位数数组。
             */
            static constexpr uint32_t S[64] = { //
                7,
                12,
                17,
                22,
                7,
                12,
                17,
                22,
                7,
                12,
                17,
                22,
                7,
                12,
                17,
                22,
                5,
                9,
                14,
                20,
                5,
                9,
                14,
                20,
                5,
                9,
                14,
                20,
                5,
                9,
                14,
                20,
                4,
                11,
                16,
                23,
                4,
                11,
                16,
                23,
                4,
                11,
                16,
                23,
                4,
                11,
                16,
                23,
                6,
                10,
                15,
                21,
                6,
                10,
                15,
                21,
                6,
                10,
                15,
                21,
                6,
                10,
                15,
                21};

            /**
             * @brief MD5算法中使用的常量K数组。
             */
            static constexpr uint32_t K[64] = { //
                0xd76aa478,
                0xe8c7b756,
                0x242070db,
                0xc1bdceee,
                0xf57c0faf,
                0x4787c62a,
                0xa8304613,
                0xfd469501,
                0x698098d8,
                0x8b44f7af,
                0xffff5bb1,
                0x895cd7be,
                0x6b901122,
                0xfd987193,
                0xa679438e,
                0x49b40821,
                0xf61e2562,
                0xc040b340,
                0x265e5a51,
                0xe9b6c7aa,
                0xd62f105d,
                0x02441453,
                0xd8a1e681,
                0xe7d3fbc8,
                0x21e1cde6,
                0xc33707d6,
                0xf4d50d87,
                0x455a14ed,
                0xa9e3e905,
                0xfcefa3f8,
                0x676f02d9,
                0x8d2a4c8a,
                0xfffa3942,
                0x8771f681,
                0x6d9d6122,
                0xfde5380c,
                0xa4beea44,
                0x4bdecfa9,
                0xf6bb4b60,
                0xbebfbc70,
                0x289b7ec6,
                0xeaa127fa,
                0xd4ef3085,
                0x04881d05,
                0xd9d4d039,
                0xe6db99e5,
                0x1fa27cf8,
                0xc4ac5665,
                0xf4292244,
                0x432aff97,
                0xab9423a7,
                0xfc93a039,
                0x655b59c3,
                0x8f0ccc92,
                0xffeff47d,
                0x85845dd1,
                0x6fa87e4f,
                0xfe2ce6e0,
                0xa3014314,
                0x4e0811a1,
                0xf7537e82,
                0xbd3af235,
                0x2ad7d2bb,
                0xeb86d391};

            /**
             * @brief MD5第一轮逻辑函数F。
             * @param x 输入值。
             * @param y 输入值。
             * @param z 输入值。
             * @return uint32_t 计算结果。
             */
            inline uint32_t F(uint32_t x, uint32_t y, uint32_t z) {
                return (x & y) | (~x & z);
            }

            /**
             * @brief MD5第二轮逻辑函数G。
             * @param x 输入值。
             * @param y 输入值。
             * @param z 输入值。
             * @return uint32_t 计算结果。
             */
            inline uint32_t G(uint32_t x, uint32_t y, uint32_t z) {
                return (x & z) | (y & ~z);
            }

            /**
             * @brief MD5第三轮逻辑函数H。
             * @param x 输入值。
             * @param y 输入值。
             * @param z 输入值。
             * @return uint32_t 计算结果。
             */
            inline uint32_t H(uint32_t x, uint32_t y, uint32_t z) {
                return x ^ y ^ z;
            }

            /**
             * @brief MD5第四轮逻辑函数I。
             * @param x 输入值。
             * @param y 输入值。
             * @param z 输入值。
             * @return uint32_t 计算结果。
             */
            inline uint32_t I(uint32_t x, uint32_t y, uint32_t z) {
                return y ^ (x | ~z);
            }

            /**
             * @brief 对32位无符号整数进行循环左移。
             * @param x 待移位的整数。
             * @param n 左移的位数。
             * @return uint32_t 循环左移后的结果。
             */
            inline uint32_t rotate_left(uint32_t x, uint32_t n) {
                return (x << n) | (x >> (32 - n));
            }

            /**
             * @brief 对64字节的数据块执行MD5变换。
             * @param state 当前哈希状态，长度为4的数组。
             * @param block 要处理的数据块，长度必须为64字节。
             */
            inline void transform(uint32_t *state, const uint8_t *block) {
                uint32_t a = state[0], b = state[1], c = state[2], d = state[3];
                uint32_t M[16];
                for (int i = 0; i < 16; i++) {
                    M[i] = block[i * 4] | (block[i * 4 + 1] << 8)
                           | (block[i * 4 + 2] << 16)
                           | (block[i * 4 + 3] << 24);
                }
                for (int i = 0; i < 64; i++) {
                    uint32_t f, g;
                    if (i < 16) {
                        f = F(b, c, d);
                        g = i;
                    } else if (i < 32) {
                        f = G(b, c, d);
                        g = (5 * i + 1) % 16;
                    } else if (i < 48) {
                        f = H(b, c, d);
                        g = (3 * i + 5) % 16;
                    } else {
                        f = I(b, c, d);
                        g = (7 * i) % 16;
                    }
                    uint32_t temp = d;
                    d             = c;
                    c             = b;
                    b             = b + rotate_left(a + f + K[i] + M[g], S[i]);
                    a             = temp;
                }
                state[0] += a;
                state[1] += b;
                state[2] += c;
                state[3] += d;
            }
        }

        /**
         * @brief 计算输入字符串的MD5哈希值。
         * @param input 要计算哈希的输入字符串。
         * @return std::string 32位十六进制字符串格式的MD5哈希值。
         * @warning
         * MD5算法已被证明存在碰撞漏洞，不建议用于安全敏感场景，请考虑使用SHA256替代。
         */
        inline std::string md5(const std::string &input) {
            using namespace md5_impl;
            uint32_t state[4]
                = {0x67452301, 0xefcdab89, 0x98badcfe, 0x10325476};
            std::vector<uint8_t> data(input.begin(), input.end());
            uint64_t             bit_len = data.size() * 8;
            data.push_back(0x80);
            while ((data.size() % 64) != 56) data.push_back(0x00);
            for (int i = 0; i < 8; i++)
                data.push_back((bit_len >> (i * 8)) & 0xFF);
            for (size_t i = 0; i < data.size(); i += 64)
                transform(state, &data[i]);
            std::stringstream ss;
            for (int i = 0; i < 4; i++)
                for (int j = 0; j < 4; j++)
                    ss << std::hex << std::setfill('0') << std::setw(2)
                       << ((state[i] >> (j * 8)) & 0xFF);
            return ss.str();
        }

        /**
         * @brief 对输入字符串进行Base64编码。
         * @param input 要编码的原始字符串。
         * @return std::string Base64编码后的字符串。
         * @note 编码结果末尾会自动添加填充字符'='以确保长度为4的倍数。
         */
        inline std::string base64_encode(const std::string &input) {
            const char *base64_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklm"
                                       "nopqrstuvwxyz0123456789+/";
            std::string result;
            result.reserve(((input.size() + 2) / 3) * 4);
            uint32_t buffer         = 0;
            int      bits_collected = 0;
            for (char c : input) {
                buffer = (buffer << 8) | static_cast<uint8_t>(c);
                bits_collected += 8;
                while (bits_collected >= 6) {
                    bits_collected -= 6;
                    result.push_back(
                        base64_chars[(buffer >> bits_collected) & 0x3F]);
                }
            }
            if (bits_collected > 0) {
                buffer <<= (6 - bits_collected);
                result.push_back(base64_chars[buffer & 0x3F]);
            }
            while (result.size() % 4) result.push_back('=');
            return result;
        }

        /**
         * @brief 对Base64编码的字符串进行解码。
         * @param input Base64编码的字符串。
         * @return std::string 解码后的原始字符串。
         * @note 解码时遇到'='字符会停止处理，非Base64字符会被忽略。
         */
        inline std::string base64_decode(const std::string &input) {
            static const std::string base64_chars
                = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" //
                  "abcdefghijklmnopqrstuvwxyz" //
                  "0123456789+/";
            std::string result;
            result.reserve(input.size() / 4 * 3);
            uint32_t buffer         = 0;
            int      bits_collected = 0;
            for (char c : input) {
                if (c == '=') break;
                size_t index = base64_chars.find(c);
                if (index == std::string::npos) continue;
                buffer = (buffer << 6) | index;
                bits_collected += 6;
                if (bits_collected >= 8) {
                    bits_collected -= 8;
                    result.push_back((buffer >> bits_collected) & 0xFF);
                }
            }
            return result;
        }

        /**
         * @brief AES加密算法的内部实现细节。
         * @details 实现了AES-128算法，使用CTR模式进行加密。
         * @note 该命名空间中的函数和常量仅供内部使用，不建议直接调用。
         */
        namespace aes_impl {
            /**
             * @brief AES算法的S盒，用于字节替换。
             */
            static constexpr uint8_t Sbox[256] = { //
                0x63,
                0x7c,
                0x77,
                0x7b,
                0xf2,
                0x6b,
                0x6f,
                0xc5,
                0x30,
                0x01,
                0x67,
                0x2b,
                0xfe,
                0xd7,
                0xab,
                0x76,
                0xca,
                0x82,
                0xc9,
                0x7d,
                0xfa,
                0x59,
                0x47,
                0xf0,
                0xad,
                0xd4,
                0xa2,
                0xaf,
                0x9c,
                0xa4,
                0x72,
                0xc0,
                0xb7,
                0xfd,
                0x93,
                0x26,
                0x36,
                0x3f,
                0xf7,
                0xcc,
                0x34,
                0xa5,
                0xe5,
                0xf1,
                0x71,
                0xd8,
                0x31,
                0x15,
                0x04,
                0xc7,
                0x23,
                0xc3,
                0x18,
                0x96,
                0x05,
                0x9a,
                0x07,
                0x12,
                0x80,
                0xe2,
                0xeb,
                0x27,
                0xb2,
                0x75,
                0x09,
                0x83,
                0x2c,
                0x1a,
                0x1b,
                0x6e,
                0x5a,
                0xa0,
                0x52,
                0x3b,
                0xd6,
                0xb3,
                0x29,
                0xe3,
                0x2f,
                0x84,
                0x53,
                0xd1,
                0x00,
                0xed,
                0x20,
                0xfc,
                0xb1,
                0x5b,
                0x6a,
                0xcb,
                0xbe,
                0x39,
                0x4a,
                0x4c,
                0x58,
                0xcf,
                0xd0,
                0xef,
                0xaa,
                0xfb,
                0x43,
                0x4d,
                0x33,
                0x85,
                0x45,
                0xf9,
                0x02,
                0x7f,
                0x50,
                0x3c,
                0x9f,
                0xa8,
                0x51,
                0xa3,
                0x40,
                0x8f,
                0x92,
                0x9d,
                0x38,
                0xf5,
                0xbc,
                0xb6,
                0xda,
                0x21,
                0x10,
                0xff,
                0xf3,
                0xd2,
                0xcd,
                0x0c,
                0x13,
                0xec,
                0x5f,
                0x97,
                0x44,
                0x17,
                0xc4,
                0xa7,
                0x7e,
                0x3d,
                0x64,
                0x5d,
                0x19,
                0x73,
                0x60,
                0x81,
                0x4f,
                0xdc,
                0x22,
                0x2a,
                0x90,
                0x88,
                0x46,
                0xee,
                0xb8,
                0x14,
                0xde,
                0x5e,
                0x0b,
                0xdb,
                0xe0,
                0x32,
                0x3a,
                0x0a,
                0x49,
                0x06,
                0x24,
                0x5c,
                0xc2,
                0xd3,
                0xac,
                0x62,
                0x91,
                0x95,
                0xe4,
                0x79,
                0xe7,
                0xc8,
                0x37,
                0x6d,
                0x8d,
                0xd5,
                0x4e,
                0xa9,
                0x6c,
                0x56,
                0xf4,
                0xea,
                0x65,
                0x7a,
                0xae,
                0x08,
                0xba,
                0x78,
                0x25,
                0x2e,
                0x1c,
                0xa6,
                0xb4,
                0xc6,
                0xe8,
                0xdd,
                0x74,
                0x1f,
                0x4b,
                0xbd,
                0x8b,
                0x8a,
                0x70,
                0x3e,
                0xb5,
                0x66,
                0x48,
                0x03,
                0xf6,
                0x0e,
                0x61,
                0x35,
                0x57,
                0xb9,
                0x86,
                0xc1,
                0x1d,
                0x9e,
                0xe1,
                0xf8,
                0x98,
                0x11,
                0x69,
                0xd9,
                0x8e,
                0x94,
                0x9b,
                0x1e,
                0x87,
                0xe9,
                0xce,
                0x55,
                0x28,
                0xdf,
                0x8c,
                0xa1,
                0x89,
                0x0d,
                0xbf,
                0xe6,
                0x42,
                0x68,
                0x41,
                0x99,
                0x2d,
                0x0f,
                0xb0,
                0x54,
                0xbb,
                0x16};

            /**
             * @brief AES算法的逆S盒，用于解密时的字节替换。
             */
            static constexpr uint8_t InvSbox[256] = { //
                0x52,
                0x09,
                0x6a,
                0xd5,
                0x30,
                0x36,
                0xa5,
                0x38,
                0xbf,
                0x40,
                0xa3,
                0x9e,
                0x81,
                0xf3,
                0xd7,
                0xfb,
                0x7c,
                0xe3,
                0x39,
                0x82,
                0x9b,
                0x2f,
                0xff,
                0x87,
                0x34,
                0x8e,
                0x43,
                0x44,
                0xc4,
                0xde,
                0xe9,
                0xcb,
                0x54,
                0x7b,
                0x94,
                0x32,
                0xa6,
                0xc2,
                0x23,
                0x3d,
                0xee,
                0x4c,
                0x95,
                0x0b,
                0x42,
                0xfa,
                0xc3,
                0x4e,
                0x08,
                0x2e,
                0xa1,
                0x66,
                0x28,
                0xd9,
                0x24,
                0xb2,
                0x76,
                0x5b,
                0xa2,
                0x49,
                0x6d,
                0x8b,
                0xd1,
                0x25,
                0x72,
                0xf8,
                0xf6,
                0x64,
                0x86,
                0x68,
                0x98,
                0x16,
                0xd4,
                0xa4,
                0x5c,
                0xcc,
                0x5d,
                0x65,
                0xb6,
                0x92,
                0x6c,
                0x70,
                0x48,
                0x50,
                0xfd,
                0xed,
                0xb9,
                0xda,
                0x5e,
                0x15,
                0x46,
                0x57,
                0xa7,
                0x8d,
                0x9d,
                0x84,
                0x90,
                0xd8,
                0xab,
                0x00,
                0x8c,
                0xbc,
                0xd3,
                0x0a,
                0xf7,
                0xe4,
                0x58,
                0x05,
                0xb8,
                0xb3,
                0x45,
                0x06,
                0xd0,
                0x2c,
                0x1e,
                0x8f,
                0xca,
                0x3f,
                0x0f,
                0x02,
                0xc1,
                0xaf,
                0xbd,
                0x03,
                0x01,
                0x13,
                0x8a,
                0x6b,
                0x3a,
                0x91,
                0x11,
                0x41,
                0x4f,
                0x67,
                0xdc,
                0xea,
                0x97,
                0xf2,
                0xcf,
                0xce,
                0xf0,
                0xb4,
                0xe6,
                0x73,
                0x96,
                0xac,
                0x74,
                0x22,
                0xe7,
                0xad,
                0x35,
                0x85,
                0xe2,
                0xf9,
                0x37,
                0xe8,
                0x1c,
                0x75,
                0xdf,
                0x6e,
                0x47,
                0xf1,
                0x1a,
                0x71,
                0x1d,
                0x29,
                0xc5,
                0x89,
                0x6f,
                0xb7,
                0x62,
                0x0e,
                0xaa,
                0x18,
                0xbe,
                0x1b,
                0xfc,
                0x56,
                0x3e,
                0x4b,
                0xc6,
                0xd2,
                0x79,
                0x20,
                0x9a,
                0xdb,
                0xc0,
                0xfe,
                0x78,
                0xcd,
                0x5a,
                0xf4,
                0x1f,
                0xdd,
                0xa8,
                0x33,
                0x88,
                0x07,
                0xc7,
                0x31,
                0xb1,
                0x12,
                0x10,
                0x59,
                0x27,
                0x80,
                0xec,
                0x5f,
                0x60,
                0x51,
                0x7f,
                0xa9,
                0x19,
                0xb5,
                0x4a,
                0x0d,
                0x2d,
                0xe5,
                0x7a,
                0x9f,
                0x93,
                0xc9,
                0x9c,
                0xef,
                0xa0,
                0xe0,
                0x3b,
                0x4d,
                0xae,
                0x2a,
                0xf5,
                0xb0,
                0xc8,
                0xeb,
                0xbb,
                0x3c,
                0x83,
                0x53,
                0x99,
                0x61,
                0x17,
                0x2b,
                0x04,
                0x7e,
                0xba,
                0x77,
                0xd6,
                0x26,
                0xe1,
                0x69,
                0x14,
                0x63,
                0x55,
                0x21,
                0x0c,
                0x7d};

            /**
             * @brief 字节替换操作，使用S盒替换状态矩阵中的每个字节。
             * @param state 16字节的状态矩阵。
             */
            inline void sub_bytes(uint8_t *state) {
                for (int i = 0; i < 16; i++) state[i] = Sbox[state[i]];
            }

            /**
             * @brief 逆字节替换操作，使用逆S盒替换状态矩阵中的每个字节。
             * @param state 16字节的状态矩阵。
             */
            inline void inv_sub_bytes(uint8_t *state) {
                for (int i = 0; i < 16; i++) state[i] = InvSbox[state[i]];
            }

            /**
             * @brief 行移位操作，将状态矩阵的各行循环左移不同偏移量。
             * @param state 16字节的状态矩阵。
             */
            inline void shift_rows(uint8_t *state) {
                uint8_t tmp;
                tmp       = state[1];
                state[1]  = state[5];
                state[5]  = state[9];
                state[9]  = state[13];
                state[13] = tmp;
                tmp       = state[2];
                state[2]  = state[10];
                state[10] = tmp;
                tmp       = state[6];
                state[6]  = state[14];
                state[14] = tmp;
                tmp       = state[3];
                state[3]  = state[15];
                state[15] = state[11];
                state[11] = state[7];
                state[7]  = tmp;
            }

            /**
             * @brief 逆行移位操作，将状态矩阵的各行循环右移不同偏移量。
             * @param state 16字节的状态矩阵。
             */
            inline void inv_shift_rows(uint8_t *state) {
                uint8_t tmp;
                tmp       = state[13];
                state[13] = state[9];
                state[9]  = state[5];
                state[5]  = state[1];
                state[1]  = tmp;
                tmp       = state[10];
                state[10] = state[2];
                state[2]  = tmp;
                tmp       = state[14];
                state[14] = state[6];
                state[6]  = tmp;
                tmp       = state[7];
                state[7]  = state[11];
                state[11] = state[15];
                state[15] = state[3];
                state[3]  = tmp;
            }

            /**
             * @brief 在GF(2^8)有限域中执行乘法运算。
             * @param a 乘数。
             * @param b 乘数。
             * @return uint8_t 乘积结果。
             */
            inline uint8_t gmul(uint8_t a, uint8_t b) {
                uint8_t p = 0;
                for (int i = 0; i < 8; i++) {
                    if (b & 1) p ^= a;
                    bool high = a & 0x80;
                    a <<= 1;
                    if (high) a ^= 0x1b;
                    b >>= 1;
                }
                return p;
            }

            /**
             * @brief 列混合操作，对状态矩阵的每一列进行线性变换。
             * @param state 16字节的状态矩阵。
             */
            inline void mix_columns(uint8_t *state) {
                uint8_t temp[4];
                for (int i = 0; i < 4; i++) {
                    int idx = i * 4;
                    temp[0] = gmul(state[idx], 2) ^ gmul(state[idx + 1], 3)
                              ^ state[idx + 2] ^ state[idx + 3];
                    temp[1] = state[idx] ^ gmul(state[idx + 1], 2)
                              ^ gmul(state[idx + 2], 3) ^ state[idx + 3];
                    temp[2] = state[idx] ^ state[idx + 1]
                              ^ gmul(state[idx + 2], 2)
                              ^ gmul(state[idx + 3], 3);
                    temp[3] = gmul(state[idx], 3) ^ state[idx + 1]
                              ^ state[idx + 2] ^ gmul(state[idx + 3], 2);
                    std::memcpy(&state[idx], temp, 4);
                }
            }

            /**
             * @brief 逆列混合操作，对状态矩阵的每一列进行逆线性变换。
             * @param state 16字节的状态矩阵。
             */
            inline void inv_mix_columns(uint8_t *state) {
                uint8_t temp[4];
                for (int i = 0; i < 4; i++) {
                    int idx = i * 4;
                    temp[0] = gmul(state[idx], 0x0e)
                              ^ gmul(state[idx + 1], 0x0b)
                              ^ gmul(state[idx + 2], 0x0d)
                              ^ gmul(state[idx + 3], 0x09);
                    temp[1] = gmul(state[idx], 0x09)
                              ^ gmul(state[idx + 1], 0x0e)
                              ^ gmul(state[idx + 2], 0x0b)
                              ^ gmul(state[idx + 3], 0x0d);
                    temp[2] = gmul(state[idx], 0x0d)
                              ^ gmul(state[idx + 1], 0x09)
                              ^ gmul(state[idx + 2], 0x0e)
                              ^ gmul(state[idx + 3], 0x0b);
                    temp[3] = gmul(state[idx], 0x0b)
                              ^ gmul(state[idx + 1], 0x0d)
                              ^ gmul(state[idx + 2], 0x09)
                              ^ gmul(state[idx + 3], 0x0e);
                    std::memcpy(&state[idx], temp, 4);
                }
            }

            /**
             * @brief 轮密钥加操作，将状态矩阵与轮密钥进行异或。
             * @param state 16字节的状态矩阵。
             * @param key 16字节的轮密钥。
             */
            inline void add_round_key(uint8_t *state, const uint8_t *key) {
                for (int i = 0; i < 16; i++) state[i] ^= key[i];
            }

            /**
             * @brief AES密钥扩展算法，将原始密钥扩展为11轮使用的轮密钥。
             * @param key 原始密钥，长度为16字节。
             * @param round_keys
             * 输出缓冲区，用于存储扩展后的轮密钥，需要至少176字节。
             */
            inline void key_expansion(const uint8_t *key, uint8_t *round_keys) {
                uint8_t temp[4];
                uint8_t rcon[4];
                for (int i = 0; i < 16; i++) round_keys[i] = key[i];
                for (int i = 4; i < 44; i++) {
                    std::memcpy(temp, &round_keys[(i - 1) * 4], 4);
                    if (i % 4 == 0) {
                        uint8_t t = temp[0];
                        temp[0]   = Sbox[temp[1]];
                        temp[1]   = Sbox[temp[2]];
                        temp[2]   = Sbox[temp[3]];
                        temp[3]   = Sbox[t];
                        rcon[0]   = 0x01 << ((i / 4) - 1);
                        rcon[1] = rcon[2] = rcon[3] = 0;
                        if (rcon[0] == 0) rcon[0] = 0x1b;
                        for (int j = 0; j < 4; j++) temp[j] ^= rcon[j];
                    }
                    for (int j = 0; j < 4; j++) {
                        round_keys[i * 4 + j]
                            = round_keys[(i - 4) * 4 + j] ^ temp[j];
                    }
                }
            }

            /**
             * @brief AES加密单个块（128位）。
             * @param input 输入的16字节明文块。
             * @param output 输出的16字节密文块。
             * @param round_keys 扩展后的轮密钥。
             */
            inline void aes_encrypt_block(const uint8_t *input,
                uint8_t                                 *output,
                const uint8_t                           *round_keys) {
                uint8_t state[16];
                std::memcpy(state, input, 16);
                add_round_key(state, round_keys);
                for (int round = 1; round < 10; round++) {
                    sub_bytes(state);
                    shift_rows(state);
                    mix_columns(state);
                    add_round_key(state, round_keys + round * 16);
                }
                sub_bytes(state);
                shift_rows(state);
                add_round_key(state, round_keys + 10 * 16);
                std::memcpy(output, state, 16);
            }

            /**
             * @brief AES解密单个块（128位）。
             * @param input 输入的16字节密文块。
             * @param output 输出的16字节明文块。
             * @param round_keys 扩展后的轮密钥。
             */
            inline void aes_decrypt_block(const uint8_t *input,
                uint8_t                                 *output,
                const uint8_t                           *round_keys) {
                uint8_t state[16];
                std::memcpy(state, input, 16);
                add_round_key(state, round_keys + 10 * 16);
                for (int round = 9; round > 0; round--) {
                    inv_shift_rows(state);
                    inv_sub_bytes(state);
                    add_round_key(state, round_keys + round * 16);
                    inv_mix_columns(state);
                }
                inv_shift_rows(state);
                inv_sub_bytes(state);
                add_round_key(state, round_keys);
                std::memcpy(output, state, 16);
            }

            /**
             * @brief 将密钥填充到16字节（AES-128要求的密钥长度）。
             * @param key 原始密钥字符串。
             * @return std::string 填充后的16字节密钥，不足部分补零。
             * @warning 如果密钥长度超过16字节，多余部分将被截断。
             */
            inline std::string pad_key(const std::string &key) {
                std::string padded = key;
                padded.resize(16, 0);
                return padded;
            }

            /**
             * @brief 生成随机字节序列。
             * @return std::string 字节序列。
             * @note 尽可能确保了序列的随机。
             */
            inline std::string random_bytes() {
                static std::random_device rd;
                static std::mt19937       gen(
                    std::chrono::steady_clock::now().time_since_epoch().count()
                    + rd());
                static std::uniform_int_distribution<> dis(0, 255);
                std::string                            bytes;
                bytes.reserve(16);
                for (int i = 0; i < 16; i++)
                    bytes.push_back(static_cast<char>(dis(gen)));
                return bytes;
            }
        }

        /**
         * @brief 使用AES-128-CTR模式加密明文。
         * @details
         * 加密过程使用CTR计数器模式，初始化向量通过对密钥进行SHA256哈希后取前16字节生成。
         * @param plaintext 要加密的明文字符串。
         * @param key 加密密钥，将自动填充或截断至16字节。
         * @return std::string
         * Base64编码后的密文，包含前16字节的初始化向量和加密数据。
         * @note 返回的密文格式为：初始化向量（16字节）+
         * 密文数据，整体经过Base64编码。
         */
        inline std::string
        aes_encrypt(const std::string &plaintext, const std::string &key) {
            std::string padded_key = aes_impl::pad_key(key);
            uint8_t     round_keys[44 * 4];
            aes_impl::key_expansion(
                reinterpret_cast<const uint8_t *>(padded_key.c_str()),
                round_keys);
            std::string iv(aes_impl::random_bytes());
            std::string result;
            result.reserve(plaintext.size() + 16);
            result = iv;
            uint8_t counter[16];
            std::memcpy(counter, iv.c_str(), 16);
            for (size_t i = 0; i < plaintext.size(); i += 16) {
                uint8_t keystream[16];
                aes_impl::aes_encrypt_block(counter, keystream, round_keys);
                for (int j = 0; j < 16 && i + j < plaintext.size(); j++)
                    result.push_back(plaintext[i + j] ^ keystream[j]);
                for (int j = 15; j >= 0; j--) {
                    if (++counter[j] != 0) break;
                }
            }
            return base64_encode(result);
        }

        /**
         * @brief 使用AES-128-CTR模式解密密文。
         * @param ciphertext Base64编码的密文字符串，包含前16字节的初始化向量。
         * @param key 解密密钥，必须与加密时使用的密钥相同。
         * @return std::string 解密后的明文字符串，输入错误时返回 "Invalid
         * ciphertext"。
         * @note 密文格式必须与aes_encrypt函数生成的格式一致。
         */
        inline std::string
        aes_decrypt(const std::string &ciphertext, const std::string &key) {
            std::string decoded = base64_decode(ciphertext);
            if (decoded.size() < 16) return "Invalid ciphertext";
            std::string padded_key = aes_impl::pad_key(key);
            uint8_t     round_keys[44 * 4];
            aes_impl::key_expansion(
                reinterpret_cast<const uint8_t *>(padded_key.c_str()),
                round_keys);
            std::string iv             = decoded.substr(0, 16);
            std::string encrypted_data = decoded.substr(16);
            std::string result;
            result.reserve(encrypted_data.size());
            uint8_t counter[16];
            std::memcpy(counter, iv.c_str(), 16);
            for (size_t i = 0; i < encrypted_data.size(); i += 16) {
                uint8_t keystream[16];
                aes_impl::aes_encrypt_block(counter, keystream, round_keys);
                for (int j = 0; j < 16 && i + j < encrypted_data.size(); j++)
                    result.push_back(encrypted_data[i + j] ^ keystream[j]);
                for (int j = 15; j >= 0; j--)
                    if (++counter[j] != 0) break;
            }
            return result;
        }
    }
}
