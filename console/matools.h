/**
 * @file matools.h
 * @brief 提供 MultiArray 多维数组的数学工具函数。
 * @details
 * 包含统计(均值、方差、标准差)、线性代数(点积、范数、余弦、矩阵乘法、转置、迹、叉积)、
 *          元素级运算(clamp、abs、三角函数、指数对数、幂、取整)、随机初始化、卷积等。
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
#define _USE_MATH_DEFINES
#include <cmath>
#include <complex>
#include <iostream>
#include <numeric>

#include "multiarray.h"
#include "random.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace console {
    /**
     * @defgroup matools 数学工具
     * @brief MultiArray 的数学运算函数集合。
     * @{
     */

    // ---------------------------- 统计 ----------------------------
    /**
     * @brief 计算 MultiArray 中所有元素的算术平均值。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return double 平均值。
     */
    template <class T, size_t... Dims>
    double mean(const MultiArray<T, Dims...> &arr) {
        return double(sum(arr)) / arr.fsize();
    }

    /**
     * @brief 计算方差。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @param sample 若为 true(默认)，计算样本方差(除以 n-1)；若为
     * false，计算总体方差(除以 n)。
     * @return double 方差。
     */
    template <class T, size_t... Dims>
    double variance(const MultiArray<T, Dims...> &arr, bool sample = true) {
        double m      = mean(arr);
        double sq_sum = 0;
        arr.for_each([&](const T &x) {
            double d = double(x) - m;
            sq_sum += d * d;
        });
        return sq_sum / (arr.fsize() - (sample ? 1 : 0));
    }

    /**
     * @brief 计算标准差。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @param sample 若为 true，计算样本标准差(除以 n-1
     * 的方差开根)；否则总体标准差。
     * @return double 标准差。
     */
    template <class T, size_t... Dims>
    double stddev(const MultiArray<T, Dims...> &arr, bool sample = true) {
        return std::sqrt(variance(arr, sample));
    }

    // ---------------------------- 向量运算(一维)
    // ----------------------------
    /**
     * @brief 计算两个一维向量的点积。
     * @tparam T 元素类型。
     * @tparam N 向量长度。
     * @param a 向量 a。
     * @param b 向量 b。
     * @return T 点积结果。
     */
    template <class T, size_t N>
    T dot(const MultiArray<T, N> &a, const MultiArray<T, N> &b) {
        return std::inner_product(a.fbegin(), a.fend(), b.fbegin(), T{});
    }

    /**
     * @brief 计算向量的欧几里得范数(L2 范数)。
     * @tparam T 元素类型。
     * @tparam N 向量长度。
     * @param a 输入向量。
     * @return double 范数值。
     */
    template <class T, size_t N>
    double norm(const MultiArray<T, N> &a) {
        return std::sqrt(double(dot(a, a)));
    }

    /**
     * @brief 计算两个向量的余弦相似度。
     * @tparam T 元素类型。
     * @tparam N 向量长度。
     * @param a 向量 a。
     * @param b 向量 b。
     * @return double 余弦值(范围 [-1, 1])。
     */
    template <class T, size_t N>
    double cosine(const MultiArray<T, N> &a, const MultiArray<T, N> &b) {
        return dot(a, b) / (norm(a) * norm(b));
    }

    /**
     * @brief 将向量归一化为单位向量。
     * @tparam T 元素类型。
     * @tparam N 向量长度。
     * @param a 输入向量。
     * @return MultiArray<T, N> 归一化后的向量(若原范数为 0，返回原向量)。
     */
    template <class T, size_t N>
    MultiArray<T, N> normalize(const MultiArray<T, N> &a) {
        double len = norm(a);
        if (len == 0) return a;
        MultiArray<T, N> result;
        auto             ai = a.fbegin();
        auto             ri = result.fbegin();
        while (ri != result.fend()) *ri++ = *ai++ / len;
        return result;
    }

    /**
     * @brief 计算两个向量的欧几里得距离。
     * @tparam T 元素类型。
     * @tparam N 向量长度。
     * @param a 向量 a。
     * @param b 向量 b。
     * @return double 欧氏距离。
     */
    template <class T, size_t N>
    double euclidean(const MultiArray<T, N> &a, const MultiArray<T, N> &b) {
        double sum = 0;
        auto   ai  = a.fbegin();
        auto   bi  = b.fbegin();
        while (ai != a.fend()) {
            double d = *ai++ - *bi++;
            sum += d * d;
        }
        return std::sqrt(sum);
    }

    /**
     * @brief 计算两个向量的曼哈顿距离(L1 距离)。
     * @tparam T 元素类型。
     * @tparam N 向量长度。
     * @param a 向量 a。
     * @param b 向量 b。
     * @return double 曼哈顿距离。
     */
    template <class T, size_t N>
    double manhattan(const MultiArray<T, N> &a, const MultiArray<T, N> &b) {
        double sum = 0;
        auto   ai  = a.fbegin();
        auto   bi  = b.fbegin();
        while (ai != a.fend()) sum += std::abs(*ai++ - *bi++);
        return sum;
    }

    // ---------------------------- 矩阵运算(二维)
    // ----------------------------
    /**
     * @brief 矩阵乘法(二维)。
     * @tparam T 元素类型。
     * @tparam M 矩阵 A 的行数。
     * @tparam N A 的列数(同时也是 B 的行数)。
     * @tparam K B 的列数。
     * @param A 左矩阵，尺寸 M×N。
     * @param B 右矩阵，尺寸 N×K。
     * @return MultiArray<T, M, K> 乘积矩阵。
     */
    template <class T, size_t M, size_t N, size_t K>
    MultiArray<T, M, K>
    matmul(const MultiArray<T, M, N> &A, const MultiArray<T, N, K> &B) {
        MultiArray<T, M, K> C{};
        for (size_t i = 0; i < M; ++i)
            for (size_t j = 0; j < N; ++j)
                for (size_t k = 0; k < K; ++k) C[i][k] += A[i][j] * B[j][k];
        return C;
    }

    /**
     * @brief 矩阵转置。
     * @tparam T 元素类型。
     * @tparam M 原矩阵行数。
     * @tparam N 原矩阵列数。
     * @param A 输入矩阵，尺寸 M×N。
     * @return MultiArray<T, N, M> 转置矩阵，尺寸 N×M。
     */
    template <class T, size_t M, size_t N>
    MultiArray<T, N, M> transpose(const MultiArray<T, M, N> &A) {
        MultiArray<T, N, M> B;
        for (size_t i = 0; i < M; i++)
            for (size_t j = 0; j < N; j++) B[j][i] = A[i][j];
        return B;
    }

    /**
     * @brief 生成 N×N 的单位矩阵。
     * @tparam T 元素类型。
     * @tparam N 矩阵阶数。
     * @return MultiArray<T, N, N> 单位矩阵。
     */
    template <class T, size_t N>
    MultiArray<T, N, N> identity() {
        MultiArray<T, N, N> I{};
        for (size_t i = 0; i < N; i++) I[i][i] = T{1};
        return I;
    }

    /**
     * @brief 计算方阵的迹(对角线元素之和)。
     * @tparam T 元素类型。
     * @tparam N 矩阵阶数。
     * @param A 方阵。
     * @return T 迹。
     */
    template <class T, size_t N>
    T trace(const MultiArray<T, N, N> &A) {
        T result{};
        for (size_t i = 0; i < N; i++) result += A[i][i];
        return result;
    }

    /**
     * @brief 三维向量的叉积。
     * @tparam T 元素类型。
     * @param a 向量 a(长度为 3)。
     * @param b 向量 b(长度为 3)。
     * @return MultiArray<T, 3> 叉积结果。
     */
    template <class T>
    MultiArray<T, 3>
    cross(const MultiArray<T, 3> &a, const MultiArray<T, 3> &b) {
        return MultiArray<T, 3>{a[1] * b[2] - a[2] * b[1],
            a[2] * b[0] - a[0] * b[2],
            a[0] * b[1] - a[1] * b[0]};
    }

    // ---------------------------- 元素级运算 ----------------------------
    /**
     * @brief 将数组每个元素限制在 [low, high] 范围内。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @param low 下限。
     * @param high 上限。
     * @return MultiArray<T, Dims...> 裁剪后的新数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...>
    clamp(const MultiArray<T, Dims...> &arr, T low, T high) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) {
            *ri = *ai < low ? low : (*ai > high ? high : *ai);
            ++ri;
            ++ai;
        }
        return result;
    }

    /**
     * @brief 计算数组中每个元素的绝对值。
     * @tparam T 元素类型(应为有符号数值类型)。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 绝对值数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> abs(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::abs(*ai++);
        return result;
    }

    // ---------------------------- 随机初始化 ----------------------------
    /**
     * @brief 用均匀分布随机数填充数组。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 目标数组。
     * @param min 最小值(包含)。
     * @param max 最大值(包含)。
     */
    template <class T, size_t... Dims>
    void randomize(MultiArray<T, Dims...> &arr, T min = 0, T max = 100) {
        uniform_distribution_t<T> dis(min, max);
        arr.for_each([&](T &x) { x = dis(default_gen()); });
    }

    /**
     * @brief 用正态分布随机数填充数组。
     * @tparam T 元素类型(通常为浮点数)。
     * @tparam Dims 维度包。
     * @param arr 目标数组。
     * @param mean 均值。
     * @param stddev 标准差。
     */
    template <class T, size_t... Dims>
    void
    randomize_normal(MultiArray<T, Dims...> &arr, T mean = 0, T stddev = 1) {
        std::normal_distribution<T> dis(mean, stddev);
        arr.for_each([&](T &x) { x = dis(default_gen()); });
    }

    /**
     * @brief 用线性等间距值填充数组。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 目标数组(其 fsize() 决定点数)。
     * @param start 起始值。
     * @param end 结束值(包含)。
     */
    template <class T, size_t... Dims>
    void linspace(MultiArray<T, Dims...> &arr, T start, T end) {
        size_t n = arr.fsize();
        for (size_t i = 0; i < n; i++)
            arr.fbegin()[i] = start + (end - start) * i / (n - 1);
    }

    // ---------------------------- 其他 ----------------------------
    /**
     * @brief 计算所有元素的乘积。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return T 乘积。
     */
    template <class T, size_t... Dims>
    T product(const MultiArray<T, Dims...> &arr) {
        return std::accumulate(
            arr.fbegin(), arr.fend(), T{1}, std::multiplies<T>());
    }

    /**
     * @brief 查找第 k 小的元素(会修改数组顺序)。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组(将部分排序，顺序改变)。
     * @param k 索引(0-based)，若超出范围则取最大索引。
     * @return T 第 k 小的元素值。
     */
    template <class T, size_t... Dims>
    T kth_smallest(MultiArray<T, Dims...> arr, size_t k) {
        if (k >= arr.fsize()) k = arr.fsize() - 1;
        std::nth_element(arr.fbegin(), arr.fbegin() + k, arr.fend());
        return arr.fbegin()[k];
    }

    /**
     * @brief 一维卷积。
     * @tparam T 元素类型。
     * @tparam N 信号长度。
     * @tparam K 卷积核长度。
     * @param signal 信号数组。
     * @param kernel 卷积核数组。
     * @return MultiArray<T, N + K - 1> 卷积结果(full 模式)。
     */
    template <class T, size_t N, size_t K>
    MultiArray<T, N + K - 1>
    convolve1d(const MultiArray<T, N> &signal, const MultiArray<T, K> &kernel) {
        MultiArray<T, N + K - 1> result{};
        for (size_t i = 0; i < N; i++)
            for (size_t j = 0; j < K; j++)
                result[i + j] += signal[i] * kernel[j];
        return result;
    }

    // ---------------------------- 三角函数 ----------------------------
    /**
     * @brief 对数组每个元素应用 std::sin。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> sin(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::sin(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素应用 std::cos。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> cos(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::cos(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素应用 std::tan。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> tan(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::tan(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素应用 std::asin。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> asin(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::asin(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素应用 std::acos。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> acos(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::acos(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素应用 std::atan。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> atan(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::atan(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素应用 std::sinh。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> sinh(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::sinh(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素应用 std::cosh。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> cosh(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::cosh(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素应用 std::tanh。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> tanh(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::tanh(*ai++);
        return result;
    }

    // ---------------------------- 指数对数 ----------------------------
    /**
     * @brief 对数组每个元素应用 std::exp。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> exp(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::exp(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素应用 std::log(自然对数)。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> log(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::log(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素应用 std::log10(常用对数)。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> log10(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::log10(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素取幂(常数指数)。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @param exponent 指数(常数)。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> pow(const MultiArray<T, Dims...> &arr, T exponent) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::pow(*ai++, exponent);
        return result;
    }

    /**
     * @brief 对数组每个元素取幂(指数也是数组，逐元素计算)。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param base 底数数组。
     * @param exp 指数数组(必须与 base 形状相同)。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...>
    pow(const MultiArray<T, Dims...> &base, const MultiArray<T, Dims...> &exp) {
        MultiArray<T, Dims...> result;
        auto                   bi = base.fbegin();
        auto                   ei = exp.fbegin();
        auto                   ri = result.fbegin();
        while (ri != result.fend()) *ri++ = std::pow(*bi++, *ei++);
        return result;
    }

    // ---------------------------- 取整函数 ----------------------------
    /**
     * @brief 对数组每个元素向下取整。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> floor(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::floor(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素向上取整。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> ceil(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::ceil(*ai++);
        return result;
    }

    /**
     * @brief 对数组每个元素四舍五入。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return MultiArray<T, Dims...> 结果数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> round(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) *ri++ = std::round(*ai++);
        return result;
    }

    /**
     * @brief 打印数组的基本统计信息(和、均值、最小值、最大值、标准差)。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param os 发送到的输出流。
     * @param arr 输入数组。
     * @param name 数组名称(可选)，若提供则打印标题。
     */
    template < //
        class T,
        size_t... Dims,
        class CharT,
        class Traits = std::char_traits<CharT>>
    void print_stats(std::basic_ostream<CharT, Traits> &os,
        const MultiArray<T, Dims...>                   &arr,
        const std::string                              &name = "") {
        if (!name.empty()) os << "=== " << name << " ===" << '\n';
        os << "  sum   : " << sum(arr) << '\n';
        os << "  mean  : " << mean(arr) << '\n';
        os << "  min   : " << min(arr) << '\n';
        os << "  max   : " << max(arr) << '\n';
        os << "  stddev: " << stddev(arr) << std::endl;
    }

    /**
     * @brief 计算两个数组的协方差。
     * @tparam T 浮点类型。
     * @tparam Dims 维度包。
     * @param a 第一个数组。
     * @param b 第二个数组。
     * @return 协方差值。
     */
    template <class T, size_t... Dims>
    double covariance(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        double mean_a = mean(a);
        double mean_b = mean(b);
        double sum    = 0;
        auto   ai     = a.fbegin();
        auto   bi     = b.fbegin();
        while (ai != a.fend()) {
            sum += (double(*ai) - mean_a) * (double(*bi) - mean_b);
            ++ai;
            ++bi;
        }
        return sum / (a.fsize() - 1);
    }

    /**
     * @brief 计算两个数组的相关系数。
     * @tparam T 浮点类型。
     * @tparam Dims 维度包。
     * @param a 第一个数组。
     * @param b 第二个数组。
     * @return 相关系数值。
     */
    template <class T, size_t... Dims>
    double correlation(
        const MultiArray<T, Dims...> &a, const MultiArray<T, Dims...> &b) {
        double cov   = covariance(a, b);
        double std_a = stddev(a);
        double std_b = stddev(b);
        if (std_a == 0 || std_b == 0) return 0;
        return cov / (std_a * std_b);
    }

    /**
     * @brief 计算数组的中位数。
     * @tparam T 浮点类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return 中位数值。
     */
    template <class T, size_t... Dims>
    double median(MultiArray<T, Dims...> arr) {
        size_t n     = arr.fsize();
        auto   begin = arr.fbegin();
        auto   end   = arr.fend();
        std::nth_element(begin, begin + n / 2, end);
        if (n % 2 == 1) {
            return double(arr.fbegin()[n / 2]);
        } else {
            double a = double(arr.fbegin()[n / 2 - 1]);
            double b = double(arr.fbegin()[n / 2]);
            return (a + b) / 2;
        }
    }

    /**
     * @enum QuantileMethod 分位数计算方法。
     */
    enum class QuantileMethod {
        Nearest,  ///< 取最近的元素（默认）
        Linear,   ///< 线性插值（NumPy 默认）
        Midpoint, ///< 取两个值的中点
        Lower,    ///< 取下界
        Upper,    ///< 取上界
        ExcelInc, ///< PERCENTILE.INC
        ExcelExc  ///< PERCENTILE.EXC
    };

    /**
     * @brief 计算数组的分位数。
     * @tparam T 浮点类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @param q 分位数位置（0 到 1 之间）。
     * @param method 分位数计算方法（默认为 Nearest）。
     * @return 分位数值。
     */
    template <class T, size_t... Dims>
    double quantile(MultiArray<T, Dims...> arr,
        double                             q,
        QuantileMethod                     method = QuantileMethod::Nearest) {
        size_t n = arr.fsize();
        if (n == 0) return 0;
        if (q < 0) q = 0;
        if (q > 1) q = 1;
        auto begin = arr.fbegin();
        auto end   = arr.fend();
        switch (method) {
        case QuantileMethod::Nearest: {
            size_t idx = size_t(q * (n - 1) + 0.5);
            if (idx >= n) idx = n - 1;
            std::nth_element(begin, begin + idx, end);
            return double(arr.fbegin()[idx]);
        }
        case QuantileMethod::Linear: {
            double pos  = q * (n - 1);
            size_t idx  = size_t(pos);
            double frac = pos - idx;
            if (idx >= n - 1) {
                std::nth_element(begin, begin + n - 1, end);
                return double(arr.fbegin()[n - 1]);
            }
            std::nth_element(begin, begin + idx + 1, end);
            double a = double(arr.fbegin()[idx]);
            double b = double(arr.fbegin()[idx + 1]);
            return a + (b - a) * frac;
        }
        case QuantileMethod::Midpoint: {
            double pos = q * (n - 1);
            size_t idx = size_t(pos);
            if (idx >= n - 1) {
                std::nth_element(begin, begin + n - 1, end);
                return double(arr.fbegin()[n - 1]);
            }
            std::nth_element(begin, begin + idx + 1, end);
            double a = double(arr.fbegin()[idx]);
            double b = double(arr.fbegin()[idx + 1]);
            return (a + b) / 2;
        }
        case QuantileMethod::Lower: {
            size_t idx = size_t(q * (n - 1));
            if (idx >= n) idx = n - 1;
            std::nth_element(begin, begin + idx, end);
            return double(arr.fbegin()[idx]);
        }
        case QuantileMethod::Upper: {
            size_t idx = size_t(q * (n - 1));
            if (idx >= n - 1) return double(arr.fbegin()[n - 1]);
            idx++;
            std::nth_element(begin, begin + idx, end);
            return double(arr.fbegin()[idx]);
        }
        case QuantileMethod::ExcelInc: {
            double pos  = (n - 1) * q;
            size_t idx  = size_t(pos);
            double frac = pos - idx;
            if (idx >= n - 1) {
                return double(arr.fbegin()[n - 1]);
            }
            std::nth_element(begin, begin + idx + 1, end);
            double a = double(arr.fbegin()[idx]);
            double b = double(arr.fbegin()[idx + 1]);
            return a + (b - a) * frac;
        }
        case QuantileMethod::ExcelExc: {
            double pos  = (n + 1) * q;
            size_t idx  = size_t(pos);
            double frac = pos - idx;
            if (idx < 1) idx = 1;
            if (idx >= n) {
                if (idx == n) {
                    std::nth_element(begin, begin + n - 1, end);
                    return double(arr.fbegin()[n - 1]);
                }
                return double(arr.fbegin()[n - 1]);
            }
            std::nth_element(begin, begin + idx, end);
            double a = double(arr.fbegin()[idx - 1]);
            double b = double(arr.fbegin()[idx]);
            return a + (b - a) * frac;
        }
        default:
            return quantile(arr, q, QuantileMethod::Nearest);
        }
    }

    /**
     * @brief 计算 2x2 矩阵的行列式。
     * @tparam T 浮点类型。
     * @param A 输入矩阵。
     * @return 行列式值。
     */
    template <class T>
    T det2x2(const MultiArray<T, 2, 2> &A) {
        return A[0][0] * A[1][1] - A[0][1] * A[1][0];
    }

    /**
     * @brief 计算 3x3 矩阵的行列式。
     * @tparam T 浮点类型。
     * @param A 输入矩阵。
     * @return 行列式值。
     */
    template <class T>
    T det3x3(const MultiArray<T, 3, 3> &A) {
        return A[0][0] * (A[1][1] * A[2][2] - A[1][2] * A[2][1])
               - A[0][1] * (A[1][0] * A[2][2] - A[1][2] * A[2][0])
               + A[0][2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);
    }

    /**
     * @brief 计算 2x2 矩阵的逆矩阵。
     * @tparam T 浮点类型。
     * @param A 输入矩阵。
     * @return 逆矩阵。
     */
    template <class T>
    MultiArray<T, 2, 2> inv2x2(const MultiArray<T, 2, 2> &A) {
        T d = det2x2(A);
        if (d == 0) return MultiArray<T, 2, 2>{};
        return MultiArray<T, 2, 2>{
            A[1][1] / d, -A[0][1] / d, -A[1][0] / d, A[0][0] / d};
    }

    /**
     * @brief 计算 3x3 矩阵的逆矩阵。
     * @tparam T 浮点类型。
     * @param A 输入矩阵。
     * @return 逆矩阵。
     */
    template <class T>
    MultiArray<T, 3, 3> inv3x3(const MultiArray<T, 3, 3> &A) {
        T d = det3x3(A);
        if (d == 0) return MultiArray<T, 3, 3>{};
        MultiArray<T, 3, 3> B;
        B[0][0] = (A[1][1] * A[2][2] - A[1][2] * A[2][1]) / d;
        B[0][1] = (A[0][2] * A[2][1] - A[0][1] * A[2][2]) / d;
        B[0][2] = (A[0][1] * A[1][2] - A[0][2] * A[1][1]) / d;
        B[1][0] = (A[1][2] * A[2][0] - A[1][0] * A[2][2]) / d;
        B[1][1] = (A[0][0] * A[2][2] - A[0][2] * A[2][0]) / d;
        B[1][2] = (A[0][2] * A[1][0] - A[0][0] * A[1][2]) / d;
        B[2][0] = (A[1][0] * A[2][1] - A[1][1] * A[2][0]) / d;
        B[2][1] = (A[0][1] * A[2][0] - A[0][0] * A[2][1]) / d;
        B[2][2] = (A[0][0] * A[1][1] - A[0][1] * A[1][0]) / d;
        return B;
    }

    /**
     * @brief 计算 Sigmoid 激活函数。
     * @tparam T 浮点类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return 激活后的数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> sigmoid(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) {
            *ri++ = T(1) / (T(1) + std::exp(-*ai++));
        }
        return result;
    }

    /**
     * @brief 计算 ReLU 激活函数。
     * @tparam T 浮点类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return 激活后的数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> relu(const MultiArray<T, Dims...> &arr) {
        MultiArray<T, Dims...> result;
        auto                   ri = result.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != result.fend()) {
            *ri++ = *ai < 0 ? 0 : *ai;
            ++ai;
        }
        return result;
    }

    /**
     * @brief 计算 Softmax 激活函数。
     * @tparam T 浮点类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @return 激活后的数组。
     */
    template <class T, size_t... Dims>
    MultiArray<T, Dims...> softmax(const MultiArray<T, Dims...> &arr) {
        auto                   max_val = max(arr);
        MultiArray<T, Dims...> exp_arr;
        auto                   ri = exp_arr.fbegin();
        auto                   ai = arr.fbegin();
        while (ri != exp_arr.fend()) {
            *ri++ = std::exp(*ai++ - max_val);
        }
        T                      sum_exp = sum(exp_arr);
        MultiArray<T, Dims...> result;
        ri = result.fbegin();
        ai = exp_arr.fbegin();
        while (ri != result.fend()) {
            *ri++ = *ai++ / sum_exp;
        }
        return result;
    }

    /**
     * @brief 执行二维卷积操作。
     * @tparam T 浮点类型。
     * @tparam H 输入图像高度。
     * @tparam W 输入图像宽度。
     * @tparam KH 卷积核高度。
     * @tparam KW 卷积核宽度。
     * @param image 输入图像。
     * @param kernel 卷积核。
     * @return 卷积结果。
     */
    template <class T, size_t H, size_t W, size_t KH, size_t KW>
    MultiArray<T, H - KH + 1, W - KW + 1> convolve2d(
        const MultiArray<T, H, W> &image, const MultiArray<T, KH, KW> &kernel) {
        constexpr size_t      OH = H - KH + 1;
        constexpr size_t      OW = W - KW + 1;
        MultiArray<T, OH, OW> result{};
        for (size_t i = 0; i < OH; ++i) {
            for (size_t j = 0; j < OW; ++j) {
                T sum{};
                for (size_t ki = 0; ki < KH; ++ki) {
                    for (size_t kj = 0; kj < KW; ++kj) {
                        sum += image[i + ki][j + kj] * kernel[ki][kj];
                    }
                }
                result[i][j] = sum;
            }
        }
        return result;
    }

    /**
     * @brief 打印矩阵。
     * @tparam T 浮点类型。
     * @tparam M 矩阵行数。
     * @tparam N 矩阵列数。
     * @param os 输出流。
     * @param A 矩阵。
     * @param name 矩阵名称（可选）。
     */
    template <class T, size_t M, size_t N>
    void print_matrix(std::ostream &os,
        const MultiArray<T, M, N>  &A,
        const char                 *name = nullptr) {
        if (name) os << name << " = \n";
        for (size_t i = 0; i < M; ++i) {
            os << "  [";
            for (size_t j = 0; j < N; ++j) {
                repr(A[i][j], os);
                if (j < N - 1) os << ", ";
            }
            os << "]\n";
        }
    }

    /**
     * @brief 一维 FFT（Cooley-Tukey 算法）。
     * @tparam T 浮点类型（float/double）。
     * @param x 输入信号（时域），长度必须是 2 的幂。
     * @return MultiArray<std::complex<T>, N> 频域结果。
     */
    template <class T, size_t N>
    MultiArray<std::complex<T>, N>
    fft(const MultiArray<std::complex<T>, N> &x) {
        static_assert((N & (N - 1)) == 0, "FFT length must be a power of 2");
        MultiArray<std::complex<T>, N> result = x;
        for (size_t i = 0; i < N; ++i) {
            size_t j = 0;
            for (size_t k = 1; k < N; k <<= 1) {
                if (i & k)
                    j = (j << 1) | 1;
                else
                    j <<= 1;
            }
            if (i < j) std::swap(result[i], result[j]);
        }
        for (size_t len = 2; len <= N; len <<= 1) {
            T               angle = T(-2 * M_PI) / len;
            std::complex<T> wlen(std::cos(angle), std::sin(angle));
            for (size_t i = 0; i < N; i += len) {
                std::complex<T> w = 1;
                for (size_t j = 0; j < len / 2; ++j) {
                    auto u                  = result[i + j];
                    auto v                  = result[i + j + len / 2] * w;
                    result[i + j]           = u + v;
                    result[i + j + len / 2] = u - v;
                    w *= wlen;
                }
            }
        }
        return result;
    }

    /**
     * @brief 一维 IFFT（逆快速傅里叶变换）。
     * @tparam T 浮点类型（float/double）。
     * @param X 输入信号（频域）。
     * @return MultiArray<std::complex<T>, N> 时域结果。
     */
    template <class T, size_t N>
    MultiArray<std::complex<T>, N>
    ifft(const MultiArray<std::complex<T>, N> &X) {
        MultiArray<std::complex<T>, N> conj_X;
        for (size_t i = 0; i < N; ++i) conj_X[i] = std::conj(X[i]);

        auto result = fft(conj_X);

        for (size_t i = 0; i < N; ++i) {
            result[i] = std::conj(result[i]) / T(N);
        }

        return result;
    }

    /**
     * @brief 实数 FFT（输入为实数，输出为复数频域）。
     * @tparam T 浮点类型（float/double）。
     * @param x 输入实数信号。
     * @return MultiArray<std::complex<T>, N> 频域结果。
     */
    template <class T, size_t N>
    MultiArray<std::complex<T>, N> rfft(const MultiArray<T, N> &x) {
        MultiArray<std::complex<T>, N> cx;
        for (size_t i = 0; i < N; ++i) cx[i] = std::complex<T>(x[i], 0);
        return fft(cx);
    }

    /**
     * @brief 实数 IFFT（输入为复数频域，输出为实数时域）。
     * @tparam T 浮点类型（float/double）。
     * @param X 输入频域信号。
     * @return MultiArray<T, N> 时域结果（实数部分）。
     */
    template <class T, size_t N>
    MultiArray<T, N> irfft(const MultiArray<std::complex<T>, N> &X) {
        auto             cx = ifft(X);
        MultiArray<T, N> result;
        for (size_t i = 0; i < N; ++i) result[i] = cx[i].real();
        return result;
    }

    /**
     * @brief 频谱（幅值谱），归一化到真实幅度。
     * @tparam T 浮点类型。
     * @tparam N 长度。
     * @param x 输入信号。
     * @return MultiArray<T, N / 2 + 1> 幅值谱（只返回正频率部分）。
     * @note 直流分量和奈奎斯特频率除以 N，其他正频率除以 N/2。
     */
    template <class T, size_t N>
    MultiArray<T, N / 2 + 1> spectrum(const MultiArray<T, N> &x) {
        auto                     f = rfft(x);
        MultiArray<T, N / 2 + 1> mag;
        const T                  norm = T(1) / T(N);
        for (size_t i = 0; i <= N / 2; ++i) {
            T val = std::abs(f[i]) * norm;
            if (i != 0 && i != N / 2) val *= 2; // 正频率部分
            mag[i] = val;
        }
        return mag;
    }

    /**
     * @brief 功率谱（幅值平方）。
     * @tparam T 浮点类型。
     * @tparam N 长度。
     * @param x 输入信号。
     * @return MultiArray<T, N / 2 + 1> 功率谱。
     */
    template <class T, size_t N>
    MultiArray<T, N / 2 + 1> power_spectrum(const MultiArray<T, N> &x) {
        auto mag = spectrum(x);
        for (size_t i = 0; i < mag.fsize(); ++i) {
            mag[i] = mag[i] * mag[i];
        }
        return mag;
    }

    /**
     * @brief 相位谱（弧度）。
     * @tparam T 浮点类型。
     * @tparam N 长度。
     * @param x 输入信号。
     * @return MultiArray<T, N / 2 + 1> 相位谱（弧度）。
     */
    template <class T, size_t N>
    MultiArray<T, N / 2 + 1> phase_spectrum(const MultiArray<T, N> &x) {
        auto                     f = rfft(x);
        MultiArray<T, N / 2 + 1> phase;
        for (size_t i = 0; i <= N / 2; ++i) {
            phase[i] = std::arg(f[i]);
        }
        return phase;
    }

    /**
     * @brief 对数频谱（分贝）。
     * @tparam T 浮点类型。
     * @tparam N 长度。
     * @param x 输入信号。
     * @return MultiArray<T, N / 2 + 1> 对数频谱（dB）。
     */
    template <class T, size_t N>
    MultiArray<T, N / 2 + 1> log_spectrum(const MultiArray<T, N> &x) {
        auto                     mag = spectrum(x);
        MultiArray<T, N / 2 + 1> logmag;
        for (size_t i = 0; i < mag.fsize(); ++i) {
            T val     = mag[i];
            logmag[i] = (val > 0) ? T(20) * std::log10(val) : T(-100);
        }
        return logmag;
    }

    /**
     * @brief 矩阵-向量乘法。
     * @tparam T 浮点类型。
     * @tparam M 矩阵行数。
     * @tparam N 矩阵列数。
     * @param A 矩阵。
     * @param x 向量。
     * @return 结果向量。
     */
    template <class T, size_t M, size_t N>
    MultiArray<T, M>
    matvec(const MultiArray<T, M, N> &A, const MultiArray<T, N> &x) {
        MultiArray<T, M> y{};
        for (size_t i = 0; i < M; ++i) {
            T sum{};
            for (size_t j = 0; j < N; ++j) {
                sum += A[i][j] * x[j];
            }
            y[i] = sum;
        }
        return y;
    }

    /**
     * @brief 外积。
     * @tparam T 浮点类型。
     * @tparam M 矩阵行数。
     * @tparam N 矩阵列数。
     * @param a 向量。
     * @param b 向量。
     * @return 结果矩阵。
     */
    template <class T, size_t M, size_t N>
    MultiArray<T, M, N>
    outer(const MultiArray<T, M> &a, const MultiArray<T, N> &b) {
        MultiArray<T, M, N> result{};
        for (size_t i = 0; i < M; ++i) {
            for (size_t j = 0; j < N; ++j) {
                result[i][j] = a[i] * b[j];
            }
        }
        return result;
    }

    /**
     * @brief 矩阵范数（弗罗贝尼乌斯范数）。
     * @tparam T 浮点类型。
     * @tparam M 矩阵行数。
     * @tparam N 矩阵列数。
     * @param A 矩阵。
     * @return 范数值。
     */
    template <class T, size_t M, size_t N>
    double frobenius_norm(const MultiArray<T, M, N> &A) {
        double sum{};
        for (auto it = A.fbegin(); it != A.fend(); ++it) {
            double v = double(*it);
            sum += v * v;
        }
        return std::sqrt(sum);
    }

    /**
     * @brief 列范数（最大列和范数）。
     * @tparam T 浮点类型。
     * @tparam M 矩阵行数。
     * @tparam N 矩阵列数。
     * @param A 矩阵。
     * @return 范数值。
     */
    template <class T, size_t M, size_t N>
    T column_norm(const MultiArray<T, M, N> &A) {
        T max_col{};
        for (size_t j = 0; j < N; ++j) {
            T col_sum{};
            for (size_t i = 0; i < M; ++i) {
                col_sum += std::abs(A[i][j]);
            }
            if (j == 0 || col_sum > max_col) max_col = col_sum;
        }
        return max_col;
    }

    /**
     * @brief 行范数（最大行和范数）。
     * @tparam T 浮点类型。
     * @tparam M 矩阵行数。
     * @tparam N 矩阵列数。
     * @param A 矩阵。
     * @return 范数值。
     */
    template <class T, size_t M, size_t N>
    T row_norm(const MultiArray<T, M, N> &A) {
        T max_row{};
        for (size_t i = 0; i < M; ++i) {
            T row_sum{};
            for (size_t j = 0; j < N; ++j) {
                row_sum += std::abs(A[i][j]);
            }
            if (i == 0 || row_sum > max_row) max_row = row_sum;
        }
        return max_row;
    }

    /**
     * @brief 窗函数。
     * @tparam T 浮点类型。
     * @tparam N 窗函数长度。
     * @return 窗函数。
     */
    template <class T, size_t N>
    MultiArray<T, N> hann_window() {
        MultiArray<T, N> win;
        for (size_t i = 0; i < N; ++i) {
            win[i] = T(0.5) * (T(1) - std::cos(T(2 * M_PI) * i / (N - 1)));
        }
        return win;
    }

    /**
     * @brief 汉明窗。
     * @tparam T 浮点类型。
     * @tparam N 窗函数长度。
     * @return 窗函数。
     */
    template <class T, size_t N>
    MultiArray<T, N> hamming_window() {
        MultiArray<T, N> win;
        for (size_t i = 0; i < N; ++i) {
            win[i] = T(0.54) - T(0.46) * std::cos(T(2 * M_PI) * i / (N - 1));
        }
        return win;
    }

    /**
     * @brief 布莱克曼窗。
     * @tparam T 浮点类型。
     * @tparam N 窗函数长度。
     * @return 窗函数。
     */
    template <class T, size_t N>
    MultiArray<T, N> blackman_window() {
        MultiArray<T, N> win;
        for (size_t i = 0; i < N; ++i) {
            T a    = T(2 * M_PI) * i / (N - 1);
            win[i] = T(0.42) - T(0.5) * std::cos(a) + T(0.08) * std::cos(2 * a);
        }
        return win;
    }

    /**
     * @brief FFT 移位。
     * @tparam T 浮点类型。
     * @tparam N 数组长度。
     * @param x 数组。
     * @return 移位后的数组。
     */
    template <class T, size_t N>
    MultiArray<T, N> fftshift(const MultiArray<T, N> &x) {
        MultiArray<T, N> result;
        size_t           half = N / 2;
        for (size_t i = 0; i < half; ++i) {
            result[i]        = x[i + half];
            result[i + half] = x[i];
        }
        return result;
    }

    /**
     * @brief 互相关。
     * @tparam T 浮点类型。
     * @tparam N 数组长度。
     * @tparam M 数组长度。
     * @param a 数组。
     * @param b 数组。
     * @return 互相关结果。
     */
    template <class T, size_t N, size_t M>
    MultiArray<T, N + M - 1>
    cross_correlation(const MultiArray<T, N> &a, const MultiArray<T, M> &b) {
        MultiArray<T, N + M - 1> result{};
        for (size_t i = 0; i < N; ++i) {
            for (size_t j = 0; j < M; ++j) {
                result[i + j] += a[i] * b[j];
            }
        }
        return result;
    }

    /** @} */ // end of matools group
}
