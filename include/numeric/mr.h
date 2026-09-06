/**
 * @file numeric/mr.h
 * @brief 提供基于 Miller-Rabin 算法的素数判定功能。
 * @details 本文件实现了针对 uint64_t 类型的高效素数判定，包含模乘、模幂、
 *          Miller-Rabin 素性测试以及确定性小范围素数判断。
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

namespace console {

#ifdef __SIZEOF_INT128__
    /**
     * @brief 使用 128 位整数实现模乘。
     * @details 当编译器支持 __int128 时，使用硬件原生乘法避免溢出。
     * @param a 乘数。
     * @param b 被乘数。
     * @param mod 模数。
     * @return uint64_t (a * b) % mod 的结果。
     */
    inline uint64_t mod_mul(uint64_t a, uint64_t b, uint64_t mod) noexcept {
        return (unsigned __int128)a * b % mod;
    }
#else
    /**
     * @brief 使用二进制加法模拟模乘。
     * @details 当编译器不支持 __int128 时，通过循环加法避免溢出。
     *          (注：如果你在用 MSVC，建议换 MinGW 或 Clang，真的。)
     * @param a 乘数。
     * @param b 被乘数。
     * @param mod 模数。
     * @return uint64_t (a * b) % mod 的结果。
     */
    inline uint64_t mod_mul(uint64_t a, uint64_t b, uint64_t mod) noexcept {
        uint64_t result = 0;
        a %= mod;
        b %= mod;
        while (b > 0) {
            if (b & 1) result = (result + a) % mod;
            a = (a * 2) % mod;
            b >>= 1;
        }
        return result;
    }
#endif

    /**
     * @brief 模幂运算。
     * @details 使用二进制快速幂算法计算 (a^b) % mod。
     * @param a 底数。
     * @param b 指数。
     * @param mod 模数。
     * @return uint64_t (a^b) % mod 的结果。
     */
    inline uint64_t mod_pow(uint64_t a, uint64_t b, uint64_t mod) noexcept {
        uint64_t result = 1;
        a %= mod;
        while (b > 0) {
            if (b & 1) result = mod_mul(result, a, mod);
            a = mod_mul(a, a, mod);
            b >>= 1;
        }
        return result;
    }

    /**
     * @brief 单次 Miller-Rabin 素性测试。
     * @details 以 a 为底数对 n 进行 Miller-Rabin 测试，判定 n 是否为合数。
     * @param n 待测试的数。
     * @param a 底数。
     * @return true 如果 n 通过以 a 为底的测试（可能为素数）。
     * @return false 如果 n 确定是合数。
     */
    inline bool miller_rabin_test(uint64_t n, uint64_t a) noexcept {
        if (n < 2) return false;
        if (n == 2 || n == 3) return true;
        if (n % 2 == 0) return false;
        uint64_t d = n - 1;
        int      s = 0;
        while (d % 2 == 0) {
            d /= 2;
            s++;
        }
        uint64_t x = mod_pow(a, d, n);
        if (x == 1 || x == n - 1) return true;
        for (int r = 1; r < s; r++) {
            x = mod_mul(x, x, n);
            if (x == n - 1) return true;
            if (x == 1) return false;
        }
        return false;
    }

    /**
     * @brief 确定性素数判定。
     * @details 先用小素数试除，再使用一组固定底数进行 Miller-Rabin 测试。
     *          对 uint64_t 范围内的所有数均具有确定性。
     * @param n 待判定的数。
     * @return true 如果 n 是素数。
     * @return false 如果 n 是合数。
     * @note 底数集合对于 2^64 以内的所有数有效。
     */
    inline bool is_prime(uint64_t n) noexcept {
        if (n < 2) return false;
        static const uint64_t small_primes[]
            = {2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37};
        static const uint64_t bases[] = {2ULL,
            325ULL,
            9375ULL,
            28178ULL,
            450775ULL,
            9780504ULL,
            1795265022ULL};
        for (uint64_t p : small_primes) {
            if (n % p == 0) return n == p;
        }
        for (uint64_t a : bases) {
            if (a >= n) continue;
            if (!miller_rabin_test(n, a)) return false;
        }
        return true;
    }
}
