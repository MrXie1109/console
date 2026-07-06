/**
 * @file matools.h
 * @brief 提供 MultiArray 多维数组的数学工具函数。
 * @details
 * 包含统计（均值、方差、标准差）、线性代数（点积、范数、余弦、矩阵乘法、转置、迹、叉积）、
 *          元素级运算（clamp、abs、三角函数、指数对数、幂、取整）、随机初始化、卷积等。
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
     * @param sample 若为 true（默认），计算样本方差（除以 n-1）；若为
     * false，计算总体方差（除以 n）。
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
     * @param sample 若为 true，计算样本标准差（除以 n-1
     * 的方差开根）；否则总体标准差。
     * @return double 标准差。
     */
    template <class T, size_t... Dims>
    double stddev(const MultiArray<T, Dims...> &arr, bool sample = true) {
        return std::sqrt(variance(arr, sample));
    }

    // ---------------------------- 向量运算（一维）
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
     * @brief 计算向量的欧几里得范数（L2 范数）。
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
     * @return double 余弦值（范围 [-1, 1]）。
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
     * @return MultiArray<T, N> 归一化后的向量（若原范数为 0，返回原向量）。
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
     * @brief 计算两个向量的曼哈顿距离（L1 距离）。
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

    // ---------------------------- 矩阵运算（二维）
    // ----------------------------
    /**
     * @brief 矩阵乘法（二维）。
     * @tparam T 元素类型。
     * @tparam M 矩阵 A 的行数。
     * @tparam N A 的列数（同时也是 B 的行数）。
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
     * @brief 计算方阵的迹（对角线元素之和）。
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
     * @param a 向量 a（长度为 3）。
     * @param b 向量 b（长度为 3）。
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
     * @tparam T 元素类型（应为有符号数值类型）。
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
     * @param min 最小值（包含）。
     * @param max 最大值（包含）。
     */
    template <class T, size_t... Dims>
    void randomize(MultiArray<T, Dims...> &arr, T min = 0, T max = 100) {
        uniform_distribution_t<T> dis(min, max);
        arr.for_each([&](T &x) { x = dis(default_gen()); });
    }

    /**
     * @brief 用正态分布随机数填充数组。
     * @tparam T 元素类型（通常为浮点数）。
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
     * @param arr 目标数组（其 fsize() 决定点数）。
     * @param start 起始值。
     * @param end 结束值（包含）。
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
     * @brief 查找第 k 小的元素（会修改数组顺序）。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组（将部分排序，顺序改变）。
     * @param k 索引（0-based），若超出范围则取最大索引。
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
     * @return MultiArray<T, N + K - 1> 卷积结果（full 模式）。
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
     * @brief 对数组每个元素应用 std::log（自然对数）。
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
     * @brief 对数组每个元素应用 std::log10（常用对数）。
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
     * @brief 对数组每个元素取幂（常数指数）。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param arr 输入数组。
     * @param exponent 指数（常数）。
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
     * @brief 对数组每个元素取幂（指数也是数组，逐元素计算）。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param base 底数数组。
     * @param exp 指数数组（必须与 base 形状相同）。
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
     * @brief 打印数组的基本统计信息（和、均值、最小值、最大值、标准差）。
     * @tparam T 元素类型。
     * @tparam Dims 维度包。
     * @param os 发送到的输出流。
     * @param arr 输入数组。
     * @param name 数组名称（可选），若提供则打印标题。
     */
    template <class T, size_t... Dims>
    void print_stats(std::ostream    &os,
        const MultiArray<T, Dims...> &arr,
        const char                   *name = "") {
        if (name && *name) os << "=== " << name << " ===" << '\n';
        os << "  sum   : " << sum(arr) << '\n';
        os << "  mean  : " << mean(arr) << '\n';
        os << "  min   : " << min(arr) << '\n';
        os << "  max   : " << max(arr) << '\n';
        os << "  stddev: " << stddev(arr) << std::endl;
    }

    /** @} */ // end of matools group
}
