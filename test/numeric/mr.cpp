/**
 * @file mr.cpp
 * @brief 测试 Miller-Rabin 素数判定模块。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../../include/numeric/mr.h"

#include <vector>

#include "../../include/util/test.h"

/**
 * @brief 测试小质数判定。
 * @note 期望：正确识别小质数。
 */
TEST(PrimeSmallPrimes) {
    std::vector<uint64_t> primes = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
    for (uint64_t p : primes) {
        ASSERT_TRUE(console::is_prime(p));
    }
}

/**
 * @brief 测试小合数判定。
 * @note 期望：正确识别小合数。
 */
TEST(PrimeSmallComposites) {
    std::vector<uint64_t> composites = {//
        1,
        4,
        6,
        8,
        9,
        10,
        12,
        14,
        15,
        16,
        18,
        20,
        21,
        22,
        24,
        25,
        26,
        27,
        28,
        30,
        32,
        33,
        34,
        35,
        36,
        38,
        39,
        40};
    for (uint64_t n : composites) {
        ASSERT_FALSE(console::is_prime(n));
    }
}

/**
 * @brief 测试常见质数。
 * @note 期望：正确识别常见质数。
 */
TEST(PrimeCommonPrimes) {
    std::vector<uint64_t> primes = {//
        41,
        43,
        47,
        53,
        59,
        61,
        67,
        71,
        73,
        79,
        83,
        89,
        97,
        101,
        103,
        107,
        109,
        113,
        127,
        131,
        137,
        139,
        149,
        151,
        157,
        163,
        167,
        173,
        179,
        181,
        191,
        193,
        197,
        199,
        211,
        223,
        227,
        229,
        233,
        239,
        241,
        251,
        257,
        263,
        269,
        271,
        277,
        281,
        283,
        293};
    for (uint64_t p : primes) {
        ASSERT_TRUE(console::is_prime(p));
    }
}

/**
 * @brief 测试已知的大质数。
 * @note 期望：正确识别已知的大质数 (使用可靠的已知质数)。
 */
TEST(PrimeLargeKnownPrimes) {
    std::vector<uint64_t> primes = {
        1000000007ULL,          // 常用质数
        1000000009ULL,          // 常用质数
        1000000033ULL,          // 常用质数
        2305843009213693951ULL, // 2^61 - 1，梅森质数
        18446744073709551557ULL // 2^64 - 59，已知质数
    };
    for (uint64_t p : primes) {
        ASSERT_TRUE(console::is_prime(p));
    }
}

/**
 * @brief 测试大合数判定。
 * @note 期望：正确识别大合数。
 */
TEST(PrimeLargeComposites) {
    std::vector<uint64_t> composites = {
        1000000000ULL,
        1000000008ULL,
        1000000010ULL,
        1000000012ULL,
        1000000014ULL,
        1000000016ULL,
        1000000018ULL,
        1000000020ULL,
        1000000022ULL,
        1000000024ULL,
        1000000026ULL,
        1000000028ULL,
        1000000030ULL,
        1000000032ULL,
        1000000034ULL,
        1000000036ULL,
        1000000038ULL,
        1000000040ULL,
        1000000042ULL,
        1000000044ULL,
        18446744073709551615ULL // 2^64 - 1，合数
    };
    for (uint64_t n : composites) {
        ASSERT_FALSE(console::is_prime(n));
    }
}

/**
 * @brief 测试模乘溢出安全。
 * @note 期望：大数模乘不溢出。
 */
TEST(PrimeModMul) {
    uint64_t a      = 0xFFFFFFFFFFFFFFFFULL;
    uint64_t b      = 0xFFFFFFFFFFFFFFFFULL;
    uint64_t mod    = 1000000007ULL;
    uint64_t result = console::mod_mul(a, b, mod);
    ASSERT_LT(result, mod);
}

/**
 * @brief 测试模幂运算。
 * @note 期望：模幂结果正确。
 */
TEST(PrimeModPow) {
    uint64_t result = console::mod_pow(2, 10, 1000);
    ASSERT_EQ(24, result);
    result = console::mod_pow(3, 5, 100);
    ASSERT_EQ(43, result);
}

/**
 * @brief 测试质数判定边界值。
 * @note 期望：正确处理边界值。
 */
TEST(PrimeBoundary) {
    ASSERT_FALSE(console::is_prime(0));
    ASSERT_FALSE(console::is_prime(1));
    ASSERT_TRUE(console::is_prime(2));
    ASSERT_TRUE(console::is_prime(3));
    ASSERT_FALSE(console::is_prime(4));
}

/**
 * @brief 测试偶数判定。
 * @note 期望：所有偶数 (除2外) 为合数。
 */
TEST(PrimeEvenNumbers) {
    for (uint64_t n = 4; n < 100; n += 2) {
        ASSERT_FALSE(console::is_prime(n));
    }
    ASSERT_TRUE(console::is_prime(2));
}

/**
 * @brief 测试接近 2^64 的已知质数和合数。
 * @note 期望：正确识别。
 */
TEST(PrimeNearMax) {
    uint64_t prime     = 18446744073709551557ULL; // 2^64 - 59
    uint64_t composite = 18446744073709551615ULL; // 2^64 - 1
    ASSERT_TRUE(console::is_prime(prime));
    ASSERT_FALSE(console::is_prime(composite));
}

#ifndef NOMAIN
TEST_MAIN
#endif
