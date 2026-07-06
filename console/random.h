/**
 * @file random.h
 * @brief 提供随机数生成、随机选择、随机打乱等实用工具。
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
#include <chrono>
#include <cstdint>
#include <initializer_list>
#include <numeric>
#include <random>
#include <utility>

#include "csexc.h" // for ContainerError
#include "sfinae.h" // for uniform_distribution_t

namespace console {
    /**
     * @defgroup random 随机数工具
     * @brief 随机数生成、随机选择、随机打乱等实用函数。
     * @{
     */

    /**
     * @brief 获取一个全局的、以当前时间戳为种子的 Mersenne Twister 随机数引擎。
     * @details 该函数返回一个静态的 std::mt19937
     * 对象的引用，引擎使用高精度时钟的当前时间点进行初始化。
     * @return std::mt19937& 随机数引擎的引用。
     */
    inline std::mt19937 &default_gen() {
        thread_local std::mt19937 gen(std::chrono::high_resolution_clock::now()
                                          .time_since_epoch()
                                          .count());
        return gen;
    }

    /**
     * @brief 修改 default_gen 提供的随机数引擎的种子。
     * @param seed 新的种子。
     */
    inline void seed(unsigned int seed) {
        default_gen().seed(seed);
    }

    /**
     * @brief 生成一个指定范围内的随机整数（均匀分布）。
     * @tparam T 整数类型，默认为 int。
     * @param min 最小值（包含），默认为 0。
     * @param max 最大值（包含），默认为 32767。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return T 在 [min, max] 区间内的随机整数。
     */
    template <class T = int>
    T randint(T min = 0, T max = 32767, std::mt19937 &gen = default_gen()) {
        return std::uniform_int_distribution<T>(min, max)(gen);
    }

    /**
     * @brief 生成一个指定范围内的随机浮点数（均匀分布）。
     * @tparam T 浮点数类型，默认为 double。
     * @param min 最小值（包含），默认为 0.0。
     * @param max 最大值（包含），默认为 1.0。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return T 在 [min, max] 区间内的随机浮点数。
     */
    template <class T = double>
    T uniform(T min = 0.0, T max = 1.0, std::mt19937 &gen = default_gen()) {
        return std::uniform_real_distribution<T>(min, max)(gen);
    }

    /**
     * @brief 生成一个服从伯努利分布（Bernoulli Distribution）的随机布尔值。
     * @param p 生成 true 的概率，默认为 0.5。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return bool 服从伯努利分布的随机布尔值。
     */
    inline bool randbool(double p = 0.5, std::mt19937 &gen = default_gen()) {
        return std::bernoulli_distribution(p)(gen);
    }

    /**
     * @brief 从容器中随机选择一个元素（左值版本，返回引用）。
     * @tparam C 容器类型，必须支持 std::begin 和 std::end 迭代器以及 size()
     * 方法。
     * @param c 要从中选择的容器（左值）。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return typename C::reference 所选元素的引用。
     * @throw ContainerError 如果容器为空。
     */
    template <class C>
    typename C::reference choice(C &c, std::mt19937 &gen = default_gen()) {
        if (std::begin(c) == std::end(c))
            throw ContainerError("Empty container");
        return *std::next(std::begin(c), randint<size_t>(0, c.size() - 1, gen));
    }

    /**
     * @brief 从容器中随机选择一个元素（常量左值版本，返回 const 引用）。
     * @tparam C 容器类型，必须支持 std::begin 和 std::end 迭代器以及 size()
     * 方法。
     * @param c 要从中选择的容器（左值）。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return typename C::const_reference 所选元素的引用。
     * @throw ContainerError 如果容器为空。
     */
    template <class C>
    typename C::const_reference
    choice(const C &c, std::mt19937 &gen = default_gen()) {
        if (std::begin(c) == std::end(c))
            throw ContainerError("Empty container");
        return *std::next(std::begin(c), randint<size_t>(0, c.size() - 1, gen));
    }

    /**
     * @brief 从容器中随机选择一个元素（右值版本，返回值）。
     * @tparam C 容器类型，必须支持 std::begin 和 std::end 迭代器以及 size()
     * 方法。
     * @param c 要从中选择的容器（右值）。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return typename C::value_type 所选元素的副本。
     * @throw ContainerError 如果容器为空。
     */
    template <class C>
    typename C::value_type choice(C &&c, std::mt19937 &gen = default_gen()) {
        if (std::begin(c) == std::end(c))
            throw ContainerError("Empty container");
        return *std::next(std::begin(c), randint<size_t>(0, c.size() - 1, gen));
    }

    /**
     * @brief 从 std::initializer_list 中随机选择一个元素（重载版本）。
     * @tparam T 元素类型。
     * @param init 初始化列表。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return T 所选元素的引值。
     * @throw ContainerError 如果初始化列表为空。
     */
    template <class T>
    T choice(std::initializer_list<T> init, std::mt19937 &gen = default_gen()) {
        return choice<std::initializer_list<T>>(init, gen);
    }

    /**
     * @brief 随机打乱容器中元素的顺序（Fisher-Yates 洗牌算法）。
     * @tparam C 容器类型，必须支持 std::begin 和 std::end 迭代器、size()，
     *         以及存储的元素类型支持 swap 操作。
     * @param c 要打乱的容器（支持左值或右值引用）。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     */
    template <class C>
    void shuffle(C &&c, std::mt19937 &gen = default_gen()) {
        if (std::begin(c) == std::end(c)) return;
        for (size_t i = c.size() - 1; i > 0; i--) {
            auto j = randint<size_t>(0, i, gen);
            std::swap(
                *std::next(std::begin(c), i), *std::next(std::begin(c), j));
        }
    }

    /**
     * @brief 生成 n 个服从正态分布（Normal Distribution）的随机数。
     * @tparam T 浮点数类型，默认为 double。
     * @param n 生成的随机数个数。
     * @param mean 分布的均值 μ，默认为 0.0。
     * @param sd 分布的标准差 σ，默认为 1.0。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return std::vector<T> 包含 n 个随机数的向量。
     */
    template <class T = double>
    std::vector<T> rnorm(
        size_t n, T mean = 0.0, T sd = 1.0, std::mt19937 &gen = default_gen()) {
        std::vector<T>              vec(n);
        std::normal_distribution<T> dist(mean, sd);
        for (T &t : vec) t = dist(gen);
        return vec;
    }

    /**
     * @brief 生成 n 个服从均匀分布（Uniform Distribution）的随机数。
     * @tparam T 浮点数类型，默认为 double。
     * @param n 生成的随机数个数。
     * @param min 下界（包含），默认为 0.0。
     * @param max 上界（包含），默认为 1.0。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return std::vector<T> 包含 n 个随机数的向量。
     */
    template <class T = double>
    std::vector<T> runif(
        size_t n, T min = 0.0, T max = 1.0, std::mt19937 &gen = default_gen()) {
        std::vector<T>            vec(n);
        uniform_distribution_t<T> dist(min, max);
        for (T &t : vec) t = dist(gen);
        return vec;
    }

    /**
     * @brief 生成 n 个服从二项分布（Binomial Distribution）的随机数。
     * @tparam T 整数类型，默认为 int。
     * @param n 生成的随机数个数。
     * @param size 每次试验的次数（trials）。
     * @param prob 每次试验的成功概率（probability），取值范围 [0, 1]。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return std::vector<T> 包含 n 个随机数的向量。
     */
    template <class T = int>
    std::vector<T>
    rbinom(size_t n, T size, double prob, std::mt19937 &gen = default_gen()) {
        std::vector<T>                vec(n);
        std::binomial_distribution<T> dist(size, prob);
        for (T &t : vec) t = dist(gen);
        return vec;
    }

    /**
     * @brief 生成 n 个服从泊松分布（Poisson Distribution）的随机数。
     * @tparam T 整数类型，默认为 int。
     * @param n 生成的随机数个数。
     * @param lambda 分布的均值 λ（同时也是方差），λ > 0。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return std::vector<T> 包含 n 个随机数的向量。
     */
    template <class T = int>
    std::vector<T>
    rpois(size_t n, double lambda, std::mt19937 &gen = default_gen()) {
        std::vector<T>               vec(n);
        std::poisson_distribution<T> dist(lambda);
        for (T &t : vec) t = dist(gen);
        return vec;
    }

    /**
     * @brief 生成 n 个服从指数分布（Exponential Distribution）的随机数。
     * @tparam T 浮点数类型，默认为 double。
     * @param n 生成的随机数个数。
     * @param rate 比率参数 λ（即 1/尺度），rate > 0。默认为 1.0。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return std::vector<T> 包含 n 个随机数的向量。
     */
    template <class T = double>
    std::vector<T>
    rexp(size_t n, T rate = 1.0, std::mt19937 &gen = default_gen()) {
        std::vector<T>                   vec(n);
        std::exponential_distribution<T> dist(rate);
        for (T &t : vec) t = dist(gen);
        return vec;
    }

    /**
     * @brief 生成 n 个服从伽马分布（Gamma Distribution）的随机数。
     * @tparam T 浮点数类型，默认为 double。
     * @param n 生成的随机数个数。
     * @param shape 形状参数 k（α），shape > 0。
     * @param rate 比率参数 β（即 1/尺度），rate > 0。默认为 1.0。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @note 标准库实现使用的是尺度参数（scale = 1/rate），因此内部做了转换。
     * @return std::vector<T> 包含 n 个随机数的向量。
     */
    template <class T = double>
    std::vector<T>
    rgamma(size_t n, T shape, T rate = 1.0, std::mt19937 &gen = default_gen()) {
        std::vector<T>             vec(n);
        std::gamma_distribution<T> dist(shape, 1.0 / rate);
        for (T &t : vec) t = dist(gen);
        return vec;
    }

    /**
     * @brief 生成 n 个服从贝塔分布（Beta Distribution）的随机数。
     * @tparam T 浮点数类型，默认为 double。
     * @param n 生成的随机数个数。
     * @param shape1 α 形状参数，shape1 > 0。
     * @param shape2 β 形状参数，shape2 > 0。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return std::vector<T> 包含 n 个随机数的向量。
     * @note 使用 Gamma 分布的关系：Beta(a, b) = Gamma(a, 1) / (Gamma(a, 1) +
     * Gamma(b, 1))。
     */

    template <class T = double>
    std::vector<T>
    rbeta(size_t n, T shape1, T shape2, std::mt19937 &gen = default_gen()) {
        std::vector<T>             vec(n);
        std::gamma_distribution<T> dist_a(shape1, 1.0);
        std::gamma_distribution<T> dist_b(shape2, 1.0);
        for (T &t : vec) {
            T x = dist_a(gen);
            T y = dist_b(gen);
            t   = x / (x + y);
        }
        return vec;
    }

    /**
     * @brief 生成 n 个服从卡方分布（Chi-squared Distribution）的随机数。
     * @tparam T 浮点数类型，默认为 double。
     * @param n 生成的随机数个数。
     * @param df 自由度（degrees of freedom），df > 0。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @note 卡方分布是伽马分布的一个特例：χ²(k) = Gamma(k/2, 1/2)。
     * @return std::vector<T> 包含 n 个随机数的向量。
     */
    template <class T = double>
    std::vector<T> rchisq(size_t n, T df, std::mt19937 &gen = default_gen()) {
        return rgamma(n, df / 2.0, 0.5, gen);
    }

    /**
     * @brief 生成 n 个服从 t 分布（Student's t-distribution）的随机数。
     * @tparam T 浮点数类型，默认为 double。
     * @param n 生成的随机数个数。
     * @param df 自由度（degrees of freedom），df > 0。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return std::vector<T> 包含 n 个随机数的向量。
     */
    template <class T = double>
    std::vector<T> rt(size_t n, T df, std::mt19937 &gen = default_gen()) {
        std::vector<T>                 vec(n);
        std::student_t_distribution<T> dist(df);
        for (T &t : vec) t = dist(gen);
        return vec;
    }

    /**
     * @brief 生成 n 个服从 F 分布（Fisher–Snedecor F-distribution）的随机数。
     * @tparam T 浮点数类型，默认为 double。
     * @param n 生成的随机数个数。
     * @param df1 分子自由度，df1 > 0。
     * @param df2 分母自由度，df2 > 0。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return std::vector<T> 包含 n 个随机数的向量。
     */
    template <class T = double>
    std::vector<T>
    rf(size_t n, T df1, T df2, std::mt19937 &gen = default_gen()) {
        std::vector<T>                vec(n);
        std::fisher_f_distribution<T> dist(df1, df2);
        for (T &t : vec) t = dist(gen);
        return vec;
    }

    /**
     * @brief 生成 n 个服从对数正态分布（Lognormal Distribution）的随机数。
     * @tparam T 浮点数类型，默认为 double。
     * @param n 生成的随机数个数。
     * @param meanlog 取对数后的均值，默认为 0.0。
     * @param sdlog 取对数后的标准差，sdlog > 0，默认为 1.0。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return std::vector<T> 包含 n 个随机数的向量。
     */
    template <class T = double>
    std::vector<T> rlnorm(size_t n,
        T                        meanlog = 0.0,
        T                        sdlog   = 1.0,
        std::mt19937            &gen     = default_gen()) {
        std::vector<T>                 vec(n);
        std::lognormal_distribution<T> dist(meanlog, sdlog);
        for (T &t : vec) t = dist(gen);
        return vec;
    }

    /**
     * @brief 生成 n 个服从韦布尔分布（Weibull Distribution）的随机数。
     * @tparam T 浮点数类型，默认为 double。
     * @param n 生成的随机数个数。
     * @param shape 形状参数 k，shape > 0。
     * @param scale 尺度参数 λ，scale > 0。默认为 1.0。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return std::vector<T> 包含 n 个随机数的向量。
     */
    template <class T = double>
    std::vector<T> rweibull(
        size_t n, T shape, T scale = 1.0, std::mt19937 &gen = default_gen()) {
        std::vector<T>               vec(n);
        std::weibull_distribution<T> dist(shape, scale);
        for (T &t : vec) t = dist(gen);
        return vec;
    }

    /**
     * @brief 从容器中随机抽取指定数量的元素（有放回或无放回）。
     * @tparam C 容器类型，必须支持 std::begin 和 std::end 迭代器。
     * @param c 要抽样的容器。
     * @param size 抽取的元素数量。
     * @param replace 是否允许重复抽取（有放回），默认为 false。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return std::vector<typename C::value_type> 抽取结果的向量。
     * @throw ContainerError 如果容器为空，或 size 超过容器大小且
     * replace==false。
     * @note 对非随机访问容器效果不佳，这并非设计缺陷。
     */
    template <class C>
    std::vector<typename C::value_type> sample(const C &c,
        size_t                                          size,
        bool                                            replace = false,
        std::mt19937                                   &gen = default_gen()) {
        using value_type = typename C::value_type;
        std::vector<value_type> result;
        result.reserve(size);
        auto   it_begin = std::begin(c);
        auto   it_end   = std::end(c);
        size_t n        = std::distance(it_begin, it_end);
        if (n == 0) throw ContainerError("Cannot sample from empty container");
        if (!replace && size > n)
            throw ContainerError(
                "Sample size exceeds container size when replace=false");
        if (replace)
            for (size_t i = 0; i < size; ++i) {
                result.push_back(*std::next(it_begin,
                    std::uniform_int_distribution<size_t>(0, n - 1)(gen)));
            }
        else {
            std::vector<size_t> indices(n);
            std::iota(indices.begin(), indices.end(), 0);
            shuffle(indices, gen);
            for (size_t i = 0; i < size; ++i)
                result.push_back(*std::next(it_begin, indices[i]));
        }
        return result;
    }

    /**
     * @brief 从 std::initializer_list 中随机抽样（重载版本）。
     * @tparam T 元素类型。
     * @param init 初始化列表。
     * @param size 抽取的元素数量。
     * @param replace 是否允许重复抽取（有放回），默认为 false。
     * @param gen 使用的随机数引擎，默认为 default_gen()。
     * @return std::vector<T> 抽取结果的向量。
     * @throw ContainerError 如果容器为空，或 size 超过容器大小且
     * replace==false。
     * @note 对非随机访问容器效果不佳，这并非设计缺陷。
     */
    template <class T>
    std::vector<T> sample(std::initializer_list<T> init,
        size_t                                     size,
        bool                                       replace = false,
        std::mt19937                              &gen     = default_gen()) {
        return sample<std::initializer_list<T>>(init, size, replace, gen);
    }

    /** @} */ // end of random group
}
