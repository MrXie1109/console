/**
 * @file progress.h
 * @brief 提供非侵入式的容器遍历进度条显示。
 * @details
 * 该模块允许你在遍历任何容器（或支持迭代器的范围）时，自动在控制台输出进度条。
 *          进度条样式可通过 ProgressConfig 自定义，也提供了几种预定义样式。
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
#include <iostream>
#include <iterator>
#include <string>

#include "csexc.h"

namespace console {
    /**
     * @struct ProgressConfig
     * @brief 进度条显示配置。
     * @details
     * 该结构体控制进度条的外观和行为，包括输出流、宽度、填充字符、前后缀以及是否显示百分比。
     */
    struct ProgressConfig {
        std::ostream &os; ///< 输出目标流，默认为 std::cout。
        int           width; ///< 进度条的宽度（字符数）。
        std::string fill_char; ///< 已填充部分使用的字符（如 "#"）。
        std::string empty_char; ///< 未填充部分使用的字符（如 "."）。
        std::string prefix; ///< 进度条前缀字符串（如 "["）。
        std::string suffix; ///< 进度条后缀字符串（如 "]"]。
        bool show_percent; ///< 是否在进度条后显示百分比数字。

        /**
         * @brief 构造一个进度条配置对象。
         * @param o   输出流，默认为 std::cout。
         * @param w   进度条宽度（字符数），默认为 50。
         * @param fc  填充字符，默认为 "#"。
         * @param ec  空白字符，默认为 "."。
         * @param pre 前缀字符串，默认为 "["。
         * @param suf 后缀字符串，默认为 "]".
         * @param sp  是否显示百分比，默认为 true。
         */
        ProgressConfig(std::ostream &o   = std::cout,
            int                      w   = 50,
            std::string              fc  = "#",
            std::string              ec  = ".",
            std::string              pre = "[",
            std::string              suf = "]",
            bool                     sp  = true) :
            os(o), width(w), fill_char(std::move(fc)),
            empty_char(std::move(ec)), prefix(std::move(pre)),
            suffix(std::move(suf)), show_percent(sp) {}
    };

    /**
     * @class Progress
     * @brief 进度条迭代器包装器，用于在遍历容器时显示进度。
     * @tparam Iter 底层迭代器类型（通常为容器的迭代器）。
     * @details 该类返回一个特殊的迭代器，在每次递增时更新并重绘进度条。
     *          用法：将 Progress 对象用于范围 for 循环，例如：
     *          for (auto& item : Progress(container)) { ... }
     */
    template <class Iter> class Progress {
    public:
        /**
         * @class iterator
         * @brief 进度条的迭代器，负责绘制进度条。
         * @details
         * 该迭代器包装了底层迭代器，并在递增时根据已完成比例刷新进度条显示。
         *          绘制频率限制为每 50 毫秒一次，以避免刷新过快。
         */
        class iterator {
            const ProgressConfig *config_; ///< 进度条配置（不能为空）
            size_t current_; ///< 当前已处理元素个数
            size_t total_; ///< 总元素个数
            Iter   it_; ///< 底层迭代器
            std::chrono::steady_clock::time_point
                last_draw_; ///< 上次绘制时间点

            /**
             * @brief 绘制进度条（如果满足刷新条件）。
             * @details 根据 current_ / total_
             * 计算百分比，并输出到配置中的输出流。 如果距离上次绘制不足
             * 50ms，则跳过绘制。
             */
            void draw() {
                if (!config_) return;
                auto now = std::chrono::steady_clock::now();
                if (current_ < total_
                    && (now - last_draw_) < std::chrono::milliseconds{50}) {
                    return;
                }
                int percent = current_ * 100 / total_;
                int filled  = percent * config_->width / 100;
                config_->os << '\r' << config_->prefix;
                for (int i = 0; i < filled; ++i)
                    config_->os << config_->fill_char;
                for (int i = filled; i < config_->width; ++i)
                    config_->os << config_->empty_char;
                config_->os << config_->suffix;
                if (config_->show_percent) {
                    config_->os.width(4);
                    config_->os << percent << '%';
                }
                config_->os.flush();
                last_draw_ = now;
            }

        public:
            /**
             * @brief 构造进度条迭代器。
             * @param config 进度条配置指针（不能为 nullptr）。
             * @param current 当前进度索引（0 ~ total）。
             * @param total 总元素个数。
             * @param it 底层迭代器。
             */
            iterator(const ProgressConfig *config,
                size_t                     current,
                size_t                     total,
                Iter                       it) :
                config_(config), current_(current), total_(total), it_(it),
                last_draw_(std::chrono::steady_clock::now()) {}

            /// @brief 前置递增：移动到下一个元素，并更新进度条。
            iterator &operator++() {
                if (current_ < total_) {
                    ++current_;
                    ++it_;
                    draw();
                }
                return *this;
            }

            /// @brief 后置递增（调用前置递增）。
            iterator operator++(int) {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            /// @brief 比较迭代器是否相等（通过当前进度索引）。
            bool operator==(const iterator &other) {
                return current_ == other.current_;
            }

            /// @brief 比较迭代器是否不等。
            bool operator!=(const iterator &other) {
                return current_ != other.current_;
            }

            /// @brief 解引用：返回底层迭代器所指向的元素。
            decltype(*it_) operator*() { return *it_; }
        };

        /**
         * @brief 从 const 容器构造进度条对象。
         * @tparam Cont 容器类型，必须支持 std::begin 和 std::end。
         * @param cont  容器（const 左值引用）。
         * @param config 进度条配置，默认为默认构造的 ProgressConfig。
         */
        template <typename Cont>
        Progress(const Cont &cont, const ProgressConfig &config = {}) :
            config_(config),
            total_(std::distance(std::begin(cont), std::end(cont))),
            begin_(std::begin(cont)), end_(std::end(cont)) {}

        /**
         * @brief 从非 const 容器构造进度条对象。
         * @tparam Cont 容器类型，必须支持 std::begin 和 std::end。
         * @param cont  容器（左值引用）。
         * @param config 进度条配置，默认为默认构造的 ProgressConfig。
         */
        template <typename Cont>
        Progress(Cont &cont, const ProgressConfig &config = {}) :
            config_(config),
            total_(std::distance(std::begin(cont), std::end(cont))),
            begin_(std::begin(cont)), end_(std::end(cont)) {}

        /**
         * @brief 析构函数，确保最后输出换行。
         * @details 进度条完成后输出一个换行，使后续输出另起一行。
         */
        ~Progress() { config_.os << std::endl; }

        /// @brief 返回指向第一个元素的迭代器（进度条起始）。
        iterator begin() { return iterator(&config_, 0, total_, begin_); }

        /// @brief 返回指向末尾的迭代器（进度条结束）。
        iterator end() { return iterator(&config_, total_, total_, end_); }

    private:
        ProgressConfig config_; ///< 进度条配置（副本）
        size_t         total_; ///< 总元素个数
        Iter           begin_; ///< 起始迭代器
        Iter end_; ///< 结束迭代器（当前未使用，但保留以备扩展）
    };

    /**
     * @namespace ProgressStyle
     * @brief 预定义的进度条样式。
     * @details 提供几种常用样式，可直接作为 ProgressConfig 使用。
     */
    namespace ProgressStyle {
        /**
         * @brief 普通样式：宽度 50，填充 '#'，空白 '.'，前后缀
         * "[]"，显示百分比。
         * @return 对应的 ProgressConfig 常量引用。
         */
        inline const ProgressConfig &normal() {
            static ProgressConfig cfg{};
            return cfg;
        }

        /**
         * @brief 简洁样式：宽度 50，填充 '='，空白
         * '-'，无前后缀，不显示百分比。
         * @return 对应的 ProgressConfig 常量引用。
         */
        inline const ProgressConfig &simple() {
            static ProgressConfig cfg{std::cout, 50, "=", "-", "", "", false};
            return cfg;
        }

        /**
         * @brief 美观样式（使用 Unicode 块字符）：宽度 50，填充 '█'，空白
         * '░'，边框 '▒' 和 '▏'，显示百分比。
         * @return 对应的 ProgressConfig 常量引用。
         * @note 需要终端支持 UTF-8 编码。
         */
        inline const ProgressConfig &beautiful() {
            static ProgressConfig cfg{
                std::cout, 50, "\u2588", "\u2591", "\u2595", "\u258F", true};
            return cfg;
        }
    }

    /**
     * @brief 创建进度条对象的辅助函数（自动推导容器类型）。
     * @tparam Cont 容器类型（支持转发引用）。
     * @param cont  容器（左值或右值）。
     * @param pc    进度条配置，默认为默认配置。
     * @return Progress<decltype(std::begin(cont))> 进度条对象。
     * @details 使用示例：
     * @code
     * std::vector<int> v(100);
     * for (auto& x : progress(v)) {
     *     // 处理 x
     * }
     * @endcode
     */
    template <class Cont>
    inline auto progress(Cont &&cont,
        const ProgressConfig &pc = {}) -> Progress<decltype(std::begin(cont))> {
        return {std::forward<Cont>(cont), pc};
    }
}
