/**
 * @file matools.cpp
 * @brief 测试 MultiArray 数学工具模块 (matools)。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 单元测试为 Vibe Coding。
 */

#include "../include/matools.h"

#include <cmath>
#include <sstream>
#include <string>

#include "../include/test.h"

/**
 * @brief 测试 mean 函数。
 * @note 期望：正确计算平均值。
 */
TEST(MatoolsMean) {
    console::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    double                      m = console::mean(arr);
    ASSERT_NEAR(3.0, m, 0.001);
}

/**
 * @brief 测试 variance 函数。
 * @note 期望：正确计算方差。
 */
TEST(MatoolsVariance) {
    console::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    double                      v = console::variance(arr, false);
    ASSERT_NEAR(2.0, v, 0.001);
    double vs = console::variance(arr, true);
    ASSERT_NEAR(2.5, vs, 0.001);
}

/**
 * @brief 测试 stddev 函数。
 * @note 期望：正确计算标准差。
 */
TEST(MatoolsStddev) {
    console::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    double                      s = console::stddev(arr, false);
    ASSERT_NEAR(std::sqrt(2.0), s, 0.001);
    double ss = console::stddev(arr, true);
    ASSERT_NEAR(std::sqrt(2.5), ss, 0.001);
}

/**
 * @brief 测试 dot 函数。
 * @note 期望：正确计算点积。
 */
TEST(MatoolsDot) {
    console::MultiArray<int, 3> a{1, 2, 3};
    console::MultiArray<int, 3> b{4, 5, 6};
    int                         d = console::dot(a, b);
    ASSERT_EQ(32, d);
}

/**
 * @brief 测试 norm 函数。
 * @note 期望：正确计算欧几里得范数。
 */
TEST(MatoolsNorm) {
    console::MultiArray<int, 3> a{3, 4, 0};
    double                      n = console::norm(a);
    ASSERT_NEAR(5.0, n, 0.001);
}

/**
 * @brief 测试 cosine 函数。
 * @note 期望：正确计算余弦相似度。
 */
TEST(MatoolsCosine) {
    console::MultiArray<int, 3> a{1, 0, 0};
    console::MultiArray<int, 3> b{0, 1, 0};
    double                      c = console::cosine(a, b);
    ASSERT_NEAR(0.0, c, 0.001);
    console::MultiArray<int, 3> c2{1, 1, 0};
    double                      c3 = console::cosine(a, c2);
    ASSERT_NEAR(0.7071, c3, 0.001);
}

/**
 * @brief 测试 normalize 函数。
 * @note 期望：正确归一化向量。
 */
TEST(MatoolsNormalize) {
    console::MultiArray<double, 3> a{3, 4, 0};
    auto                           n = console::normalize(a);
    ASSERT_NEAR(0.6, n[0], 0.001);
    ASSERT_NEAR(0.8, n[1], 0.001);
    ASSERT_NEAR(0.0, n[2], 0.001);
}

/**
 * @brief 测试 euclidean 函数。
 * @note 期望：正确计算欧几里得距离。
 */
TEST(MatoolsEuclidean) {
    console::MultiArray<int, 3> a{1, 2, 3};
    console::MultiArray<int, 3> b{4, 5, 6};
    double                      d = console::euclidean(a, b);
    ASSERT_NEAR(std::sqrt(27), d, 0.001);
}

/**
 * @brief 测试 manhattan 函数。
 * @note 期望：正确计算曼哈顿距离。
 */
TEST(MatoolsManhattan) {
    console::MultiArray<int, 3> a{1, 2, 3};
    console::MultiArray<int, 3> b{4, 5, 6};
    double                      d = console::manhattan(a, b);
    ASSERT_EQ(9.0, d);
}

/**
 * @brief 测试 matmul 函数。
 * @note 期望：正确计算矩阵乘法。
 */
TEST(MatoolsMatmul) {
    console::MultiArray<int, 2, 2> A{{1, 2}, {3, 4}};
    console::MultiArray<int, 2, 2> B{{5, 6}, {7, 8}};
    auto                           C = console::matmul(A, B);
    ASSERT_EQ(19, C[0][0]);
    ASSERT_EQ(22, C[0][1]);
    ASSERT_EQ(43, C[1][0]);
    ASSERT_EQ(50, C[1][1]);
}

/**
 * @brief 测试 transpose 函数。
 * @note 期望：正确计算矩阵转置。
 */
TEST(MatoolsTranspose) {
    console::MultiArray<int, 2, 3> A{{1, 2, 3}, {4, 5, 6}};
    auto                           T = console::transpose(A);
    ASSERT_EQ(1, T[0][0]);
    ASSERT_EQ(4, T[0][1]);
    ASSERT_EQ(2, T[1][0]);
    ASSERT_EQ(5, T[1][1]);
    ASSERT_EQ(3, T[2][0]);
    ASSERT_EQ(6, T[2][1]);
}

/**
 * @brief 测试 identity 函数。
 * @note 期望：生成正确的单位矩阵。
 */
TEST(MatoolsIdentity) {
    auto I = console::identity<int, 3>();
    ASSERT_EQ(1, I[0][0]);
    ASSERT_EQ(0, I[0][1]);
    ASSERT_EQ(0, I[0][2]);
    ASSERT_EQ(0, I[1][0]);
    ASSERT_EQ(1, I[1][1]);
    ASSERT_EQ(0, I[1][2]);
    ASSERT_EQ(0, I[2][0]);
    ASSERT_EQ(0, I[2][1]);
    ASSERT_EQ(1, I[2][2]);
}

/**
 * @brief 测试 trace 函数。
 * @note 期望：正确计算矩阵迹。
 */
TEST(MatoolsTrace) {
    console::MultiArray<int, 3, 3> A{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    int                            t = console::trace(A);
    ASSERT_EQ(15, t);
}

/**
 * @brief 测试 cross 函数。
 * @note 期望：正确计算三维叉积。
 */
TEST(MatoolsCross) {
    console::MultiArray<int, 3> a{1, 2, 3};
    console::MultiArray<int, 3> b{4, 5, 6};
    auto                        c = console::cross(a, b);
    ASSERT_EQ(-3, c[0]);
    ASSERT_EQ(6, c[1]);
    ASSERT_EQ(-3, c[2]);
}

/**
 * @brief 测试 clamp 函数。
 * @note 期望：正确裁剪元素到 [low, high] 范围。
 */
TEST(MatoolsClamp) {
    console::MultiArray<int, 5> arr{0, 3, 5, 7, 10};
    auto                        c = console::clamp(arr, 2, 8);
    ASSERT_EQ(2, c[0]);
    ASSERT_EQ(3, c[1]);
    ASSERT_EQ(5, c[2]);
    ASSERT_EQ(7, c[3]);
    ASSERT_EQ(8, c[4]);
}

/**
 * @brief 测试 abs 函数。
 * @note 期望：正确计算绝对值。
 */
TEST(MatoolsAbs) {
    console::MultiArray<int, 5> arr{-3, -1, 0, 2, 5};
    auto                        a = console::abs(arr);
    ASSERT_EQ(3, a[0]);
    ASSERT_EQ(1, a[1]);
    ASSERT_EQ(0, a[2]);
    ASSERT_EQ(2, a[3]);
    ASSERT_EQ(5, a[4]);
}

/**
 * @brief 测试 randomize 函数。
 * @note 期望：填充随机数在指定范围内。
 */
TEST(MatoolsRandomize) {
    console::MultiArray<int, 10> arr;
    console::randomize(arr, 0, 10);
    for (size_t i = 0; i < arr.fsize(); ++i) {
        ASSERT_GE(arr[i], 0);
        ASSERT_LE(arr[i], 10);
    }
}

/**
 * @brief 测试 linspace 函数。
 * @note 期望：生成线性等间距值。
 */
TEST(MatoolsLinspace) {
    console::MultiArray<double, 5> arr;
    console::linspace(arr, 0.0, 4.0);
    ASSERT_NEAR(0.0, arr[0], 0.001);
    ASSERT_NEAR(1.0, arr[1], 0.001);
    ASSERT_NEAR(2.0, arr[2], 0.001);
    ASSERT_NEAR(3.0, arr[3], 0.001);
    ASSERT_NEAR(4.0, arr[4], 0.001);
}

/**
 * @brief 测试 product 函数。
 * @note 期望：正确计算所有元素的乘积。
 */
TEST(MatoolsProduct) {
    console::MultiArray<int, 4> arr{1, 2, 3, 4};
    int                         p = console::product(arr);
    ASSERT_EQ(24, p);
}

/**
 * @brief 测试 kth_smallest 函数。
 * @note 期望：正确返回第 k 小的元素。
 */
TEST(MatoolsKthSmallest) {
    console::MultiArray<int, 5> arr{3, 1, 4, 1, 5};
    int                         k0 = console::kth_smallest(arr, 0);
    int                         k2 = console::kth_smallest(arr, 2);
    int                         k4 = console::kth_smallest(arr, 4);
    ASSERT_EQ(1, k0);
    ASSERT_EQ(3, k2);
    ASSERT_EQ(5, k4);
}

/**
 * @brief 测试 convolve1d 函数。
 * @note 期望：正确计算一维卷积。
 */
TEST(MatoolsConvolve1d) {
    console::MultiArray<int, 4> signal{1, 2, 3, 4};
    console::MultiArray<int, 3> kernel{1, 0, -1};
    auto                        result = console::convolve1d(signal, kernel);
    ASSERT_EQ(1, result[0]);
    ASSERT_EQ(2, result[1]);
    ASSERT_EQ(2, result[2]);
    ASSERT_EQ(2, result[3]);
    ASSERT_EQ(-3, result[4]);
    ASSERT_EQ(-4, result[5]);
}

/**
 * @brief 测试 sin 函数。
 * @note 期望：正确计算正弦值。
 */
TEST(MatoolsSin) {
    console::MultiArray<double, 3> arr{0, M_PI / 2, M_PI};
    auto                           s = console::sin(arr);
    ASSERT_NEAR(0.0, s[0], 0.001);
    ASSERT_NEAR(1.0, s[1], 0.001);
    ASSERT_NEAR(0.0, s[2], 0.001);
}

/**
 * @brief 测试 cos 函数。
 * @note 期望：正确计算余弦值。
 */
TEST(MatoolsCos) {
    console::MultiArray<double, 3> arr{0, M_PI / 2, M_PI};
    auto                           c = console::cos(arr);
    ASSERT_NEAR(1.0, c[0], 0.001);
    ASSERT_NEAR(0.0, c[1], 0.001);
    ASSERT_NEAR(-1.0, c[2], 0.001);
}

/**
 * @brief 测试 exp 函数。
 * @note 期望：正确计算指数。
 */
TEST(MatoolsExp) {
    console::MultiArray<double, 3> arr{0, 1, 2};
    auto                           e = console::exp(arr);
    ASSERT_NEAR(1.0, e[0], 0.001);
    ASSERT_NEAR(M_E, e[1], 0.001);
    ASSERT_NEAR(M_E * M_E, e[2], 0.001);
}

/**
 * @brief 测试 log 函数。
 * @note 期望：正确计算自然对数。
 */
TEST(MatoolsLog) {
    console::MultiArray<double, 3> arr{1, M_E, M_E * M_E};
    auto                           l = console::log(arr);
    ASSERT_NEAR(0.0, l[0], 0.001);
    ASSERT_NEAR(1.0, l[1], 0.001);
    ASSERT_NEAR(2.0, l[2], 0.001);
}

/**
 * @brief 测试 pow 函数（常数指数）。
 * @note 期望：正确计算幂。
 */
TEST(MatoolsPowConstant) {
    console::MultiArray<double, 3> arr{1, 2, 3};
    auto                           p = console::pow(arr, 2.0);
    ASSERT_NEAR(1.0, p[0], 0.001);
    ASSERT_NEAR(4.0, p[1], 0.001);
    ASSERT_NEAR(9.0, p[2], 0.001);
}

/**
 * @brief 测试 floor/ceil/round 函数。
 * @note 期望：正确取整。
 */
TEST(MatoolsFloorCeilRound) {
    console::MultiArray<double, 4> arr{1.2, 1.8, -1.2, -1.8};
    auto                           f = console::floor(arr);
    auto                           c = console::ceil(arr);
    auto                           r = console::round(arr);
    ASSERT_NEAR(1.0, f[0], 0.001);
    ASSERT_NEAR(1.0, f[1], 0.001);
    ASSERT_NEAR(-2.0, f[2], 0.001);
    ASSERT_NEAR(-2.0, f[3], 0.001);
    ASSERT_NEAR(2.0, c[0], 0.001);
    ASSERT_NEAR(2.0, c[1], 0.001);
    ASSERT_NEAR(-1.0, c[2], 0.001);
    ASSERT_NEAR(-1.0, c[3], 0.001);
    ASSERT_NEAR(1.0, r[0], 0.001);
    ASSERT_NEAR(2.0, r[1], 0.001);
    ASSERT_NEAR(-1.0, r[2], 0.001);
    ASSERT_NEAR(-2.0, r[3], 0.001);
}

/**
 * @brief 测试 covariance 函数。
 * @note 期望：正确计算协方差。
 */
TEST(MatoolsCovariance) {
    console::MultiArray<int, 5> a{1, 2, 3, 4, 5};
    console::MultiArray<int, 5> b{2, 4, 6, 8, 10};
    double                      cov = console::covariance(a, b);
    ASSERT_NEAR(5.0, cov, 0.001);
}

/**
 * @brief 测试 correlation 函数。
 * @note 期望：正确计算相关系数。
 */
TEST(MatoolsCorrelation) {
    console::MultiArray<int, 5> a{1, 2, 3, 4, 5};
    console::MultiArray<int, 5> b{2, 4, 6, 8, 10};
    double                      corr = console::correlation(a, b);
    ASSERT_NEAR(1.0, corr, 0.001);
    console::MultiArray<int, 5> c{5, 4, 3, 2, 1};
    double                      corr2 = console::correlation(a, c);
    ASSERT_NEAR(-1.0, corr2, 0.001);
}

/**
 * @brief 测试 median 函数。
 * @note 期望：正确计算中位数。
 */
TEST(MatoolsMedian) {
    console::MultiArray<int, 5> arr{3, 1, 4, 1, 5};
    double                      m = console::median(arr);
    ASSERT_NEAR(3.0, m, 0.001);
    console::MultiArray<int, 4> arr2{3, 1, 4, 2};
    double                      m2 = console::median(arr2);
    ASSERT_GE(m2, 2.0);
    ASSERT_LE(m2, 3.0);
}

/**
 * @brief 测试 quantile 函数。
 * @note 期望：正确计算分位数。
 */
TEST(MatoolsQuantile) {
    console::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    double                      q0   = console::quantile(arr, 0.0);
    double                      q25  = console::quantile(arr, 0.25);
    double                      q50  = console::quantile(arr, 0.5);
    double                      q75  = console::quantile(arr, 0.75);
    double                      q100 = console::quantile(arr, 1.0);
    ASSERT_NEAR(1.0, q0, 0.001);
    ASSERT_NEAR(2.0, q25, 0.001);
    ASSERT_NEAR(3.0, q50, 0.001);
    ASSERT_NEAR(4.0, q75, 0.001);
    ASSERT_NEAR(5.0, q100, 0.001);
}

/**
 * @brief 测试 det2x2 函数。
 * @note 期望：正确计算2x2行列式。
 */
TEST(MatoolsDet2x2) {
    console::MultiArray<int, 2, 2> A{{1, 2}, {3, 4}};
    int                            d = console::det2x2(A);
    ASSERT_EQ(-2, d);
}

/**
 * @brief 测试 det3x3 函数。
 * @note 期望：正确计算3x3行列式。
 */
TEST(MatoolsDet3x3) {
    console::MultiArray<int, 3, 3> A{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    int                            d = console::det3x3(A);
    ASSERT_EQ(0, d);
    console::MultiArray<int, 3, 3> B{{1, 0, 0}, {0, 2, 0}, {0, 0, 3}};
    int                            d2 = console::det3x3(B);
    ASSERT_EQ(6, d2);
}

/**
 * @brief 测试 inv2x2 函数。
 * @note 期望：正确计算2x2逆矩阵。
 */
TEST(MatoolsInv2x2) {
    console::MultiArray<double, 2, 2> A{{1, 2}, {3, 4}};
    auto                              I = console::inv2x2(A);
    ASSERT_NEAR(-2.0, I[0][0], 0.001);
    ASSERT_NEAR(1.0, I[0][1], 0.001);
    ASSERT_NEAR(1.5, I[1][0], 0.001);
    ASSERT_NEAR(-0.5, I[1][1], 0.001);
    auto check = console::matmul(A, I);
    ASSERT_NEAR(1.0, check[0][0], 0.001);
    ASSERT_NEAR(0.0, check[0][1], 0.001);
    ASSERT_NEAR(0.0, check[1][0], 0.001);
    ASSERT_NEAR(1.0, check[1][1], 0.001);
}

/**
 * @brief 测试 inv3x3 函数。
 * @note 期望：正确计算3x3逆矩阵。
 */
TEST(MatoolsInv3x3) {
    console::MultiArray<double, 3, 3> A{{1, 0, 0}, {0, 2, 0}, {0, 0, 3}};
    auto                              I = console::inv3x3(A);
    ASSERT_NEAR(1.0, I[0][0], 0.001);
    ASSERT_NEAR(0.0, I[0][1], 0.001);
    ASSERT_NEAR(0.0, I[0][2], 0.001);
    ASSERT_NEAR(0.0, I[1][0], 0.001);
    ASSERT_NEAR(0.5, I[1][1], 0.001);
    ASSERT_NEAR(0.0, I[1][2], 0.001);
    ASSERT_NEAR(0.0, I[2][0], 0.001);
    ASSERT_NEAR(0.0, I[2][1], 0.001);
    ASSERT_NEAR(1.0 / 3.0, I[2][2], 0.001);
}

/**
 * @brief 测试 sigmoid 函数。
 * @note 期望：正确计算 Sigmoid 激活。
 */
TEST(MatoolsSigmoid) {
    console::MultiArray<double, 3> arr{0, 1, -1};
    auto                           s = console::sigmoid(arr);
    ASSERT_NEAR(0.5, s[0], 0.001);
    ASSERT_NEAR(1.0 / (1.0 + 1.0 / M_E), s[1], 0.001);
    ASSERT_NEAR(1.0 / (1.0 + M_E), s[2], 0.001);
}

/**
 * @brief 测试 relu 函数。
 * @note 期望：正确计算 ReLU 激活。
 */
TEST(MatoolsRelu) {
    console::MultiArray<int, 5> arr{-3, -1, 0, 2, 5};
    auto                        r = console::relu(arr);
    ASSERT_EQ(0, r[0]);
    ASSERT_EQ(0, r[1]);
    ASSERT_EQ(0, r[2]);
    ASSERT_EQ(2, r[3]);
    ASSERT_EQ(5, r[4]);
}

/**
 * @brief 测试 softmax 函数。
 * @note 期望：正确计算 Softmax 激活，输出和为1。
 */
TEST(MatoolsSoftmax) {
    console::MultiArray<double, 3> arr{1, 2, 3};
    auto                           s   = console::softmax(arr);
    double                         sum = 0;
    for (size_t i = 0; i < s.fsize(); ++i) sum += s[i];
    ASSERT_NEAR(1.0, sum, 0.001);
    ASSERT_GT(s[2], s[1]);
    ASSERT_GT(s[1], s[0]);
}

/**
 * @brief 测试 convolve2d 函数。
 * @note 期望：正确计算二维卷积。
 */
TEST(MatoolsConvolve2d) {
    console::MultiArray<int, 3, 3> image{{1, 2, 3}, {4, 5, 6}, {7, 8, 9}};
    console::MultiArray<int, 2, 2> kernel{{1, 1}, {1, 1}};
    auto                           result = console::convolve2d(image, kernel);
    ASSERT_EQ(12, result[0][0]);
    ASSERT_EQ(16, result[0][1]);
    ASSERT_EQ(24, result[1][0]);
    ASSERT_EQ(28, result[1][1]);
}

/**
 * @brief 测试 matvec 函数。
 * @note 期望：正确计算矩阵-向量乘法。
 */
TEST(MatoolsMatvec) {
    console::MultiArray<int, 2, 3> A{{1, 2, 3}, {4, 5, 6}};
    console::MultiArray<int, 3>    x{7, 8, 9};
    auto                           y = console::matvec(A, x);
    ASSERT_EQ(50, y[0]);
    ASSERT_EQ(122, y[1]);
}

/**
 * @brief 测试 outer 函数。
 * @note 期望：正确计算外积。
 */
TEST(MatoolsOuter) {
    console::MultiArray<int, 3> a{1, 2, 3};
    console::MultiArray<int, 4> b{4, 5, 6, 7};
    auto                        O = console::outer(a, b);
    ASSERT_EQ(4, O[0][0]);
    ASSERT_EQ(5, O[0][1]);
    ASSERT_EQ(8, O[1][0]);
    ASSERT_EQ(10, O[1][1]);
    ASSERT_EQ(12, O[2][0]);
    ASSERT_EQ(15, O[2][1]);
}

/**
 * @brief 测试 frobenius_norm 函数。
 * @note 期望：正确计算弗罗贝尼乌斯范数。
 */
TEST(MatoolsFrobeniusNorm) {
    console::MultiArray<int, 2, 2> A{{1, 2}, {3, 4}};
    double                         n = console::frobenius_norm(A);
    ASSERT_NEAR(std::sqrt(30.0), n, 0.001);
}

/**
 * @brief 测试 column_norm 函数。
 * @note 期望：正确计算列范数。
 */
TEST(MatoolsColumnNorm) {
    console::MultiArray<int, 2, 2> A{{1, 2}, {3, 4}};
    int                            n = console::column_norm(A);
    ASSERT_EQ(6, n);
}

/**
 * @brief 测试 row_norm 函数。
 * @note 期望：正确计算行范数。
 */
TEST(MatoolsRowNorm) {
    console::MultiArray<int, 2, 2> A{{1, 2}, {3, 4}};
    int                            n = console::row_norm(A);
    ASSERT_EQ(7, n);
}

/**
 * @brief 测试 hann_window 函数。
 * @note 期望：生成汉宁窗，两端接近0。
 */
TEST(MatoolsHannWindow) {
    auto win = console::hann_window<double, 5>();
    ASSERT_NEAR(0.0, win[0], 0.001);
    ASSERT_NEAR(0.5, win[1], 0.001);
    ASSERT_NEAR(1.0, win[2], 0.001);
    ASSERT_NEAR(0.5, win[3], 0.001);
    ASSERT_NEAR(0.0, win[4], 0.001);
}

/**
 * @brief 测试 hamming_window 函数。
 * @note 期望：生成汉明窗。
 */
TEST(MatoolsHammingWindow) {
    auto win = console::hamming_window<double, 3>();
    ASSERT_NEAR(0.08, win[0], 0.001);
    ASSERT_NEAR(1.0, win[1], 0.001);
    ASSERT_NEAR(0.08, win[2], 0.001);
}

/**
 * @brief 测试 fftshift 函数。
 * @note 期望：正确交换数组前后半部分。
 */
TEST(MatoolsFftshift) {
    console::MultiArray<int, 6> arr{1, 2, 3, 4, 5, 6};
    auto                        shifted = console::fftshift(arr);
    ASSERT_EQ(4, shifted[0]);
    ASSERT_EQ(5, shifted[1]);
    ASSERT_EQ(6, shifted[2]);
    ASSERT_EQ(1, shifted[3]);
    ASSERT_EQ(2, shifted[4]);
    ASSERT_EQ(3, shifted[5]);
}

/**
 * @brief 测试 cross_correlation 函数。
 * @note 期望：正确计算互相关。
 */
TEST(MatoolsCrossCorrelation) {
    console::MultiArray<int, 3> a{1, 2, 3};
    console::MultiArray<int, 2> b{4, 5};
    auto                        r = console::cross_correlation(a, b);
    ASSERT_EQ(4, r[0]);
    ASSERT_EQ(13, r[1]);
    ASSERT_EQ(22, r[2]);
    ASSERT_EQ(15, r[3]);
}

/**
 * @brief 测试 print_stats 函数。
 * @note 期望：正确打印统计信息（验证编译和输出格式）。
 */
TEST(MatoolsPrintStats) {
    console::MultiArray<int, 5> arr{1, 2, 3, 4, 5};
    std::ostringstream          oss;
    console::print_stats(oss, arr, "TestArray");
    std::string output = oss.str();
    ASSERT_STRCONTAINS(output.c_str(), "=== TestArray ===");
    ASSERT_STRCONTAINS(output.c_str(), "sum   : 15");
    ASSERT_STRCONTAINS(output.c_str(), "mean  : 3");
    ASSERT_STRCONTAINS(output.c_str(), "min   : 1");
    ASSERT_STRCONTAINS(output.c_str(), "max   : 5");
}

/**
 * @brief 测试 fft 变换和逆变换。
 * @note 期望：FFT + IFFT 恢复原始信号。
 */
TEST(MatoolsFFTAndIFFT) {
    console::MultiArray<std::complex<double>, 4> x;
    x[0]                 = std::complex<double>(1, 0);
    x[1]                 = std::complex<double>(2, 0);
    x[2]                 = std::complex<double>(3, 0);
    x[3]                 = std::complex<double>(4, 0);
    auto X               = console::fft(x);
    auto x_reconstructed = console::ifft(X);
    for (size_t i = 0; i < 4; ++i) {
        ASSERT_NEAR(x[i].real(), x_reconstructed[i].real(), 0.001);
        ASSERT_NEAR(x[i].imag(), x_reconstructed[i].imag(), 0.001);
    }
}

/**
 * @brief 测试 rfft 和 irfft 函数。
 * @note 期望：实数 FFT 和逆变换恢复原始实数信号。
 */
TEST(MatoolsRFFTAndIRFFT) {
    console::MultiArray<double, 4> x{1, 2, 3, 4};
    auto                           X     = console::rfft(x);
    auto                           x_rec = console::irfft(X);
    for (size_t i = 0; i < 4; ++i) {
        ASSERT_NEAR(x[i], x_rec[i], 0.001);
    }
}

/**
 * @brief 测试 spectrum 函数。
 * @note 期望：正确计算幅值谱。
 */
TEST(MatoolsSpectrum) {
    console::MultiArray<double, 4> x{1, 0, -1, 0};
    auto                           mag = console::spectrum(x);
    ASSERT_NEAR(0.0, mag[0], 0.001);
    ASSERT_NEAR(0.5, mag[1], 0.001);
    ASSERT_NEAR(0.0, mag[2], 0.001);
}

/**
 * @brief 测试 power_spectrum 函数。
 * @note 期望：正确计算功率谱。
 */
TEST(MatoolsPowerSpectrum) {
    console::MultiArray<double, 4> x{1, 0, -1, 0};
    auto                           ps = console::power_spectrum(x);
    ASSERT_NEAR(0.0, ps[0], 0.001);
    ASSERT_NEAR(0.25, ps[1], 0.001);
    ASSERT_NEAR(0.0, ps[2], 0.001);
}

/**
 * @brief 测试 phase_spectrum 函数。
 * @note 期望：正确计算相位谱。
 */
TEST(MatoolsPhaseSpectrum) {
    console::MultiArray<double, 4> x{1, 0, 1, 0};
    auto                           phase = console::phase_spectrum(x);
    ASSERT_NEAR(0.0, phase[0], 0.001);
    ASSERT_NEAR(0.0, phase[1], 0.001);
    ASSERT_NEAR(0.0, phase[2], 0.001);
}

/**
 * @brief 测试 log_spectrum 函数。
 * @note 期望：正确计算对数频谱。
 */
TEST(MatoolsLogSpectrum) {
    console::MultiArray<double, 4> x{1, 0, -1, 0};
    auto                           logmag = console::log_spectrum(x);
    ASSERT_NEAR(-100.0, logmag[0], 0.001);
    ASSERT_NEAR(20.0 * std::log10(0.5), logmag[1], 0.001);
    ASSERT_NEAR(-100.0, logmag[2], 0.001);
}

#ifndef NOMAIN
TEST_MAIN
#endif
