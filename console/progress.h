/**
 * @file progress.h
 * @brief 提供非侵入式的容器遍历进度条显示。
 * @details
 * 该模块允许你在遍历任何容器（或支持迭代器的范围）时，自动在控制台输出进度条。
 *          进度条样式可通过 BasicProgressConfig 自定义，也提供了几种预定义样式。
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
     * @struct BasicProgressConfig
     * @brief 进度条显示配置。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @details
     * 该结构体控制进度条的外观和行为，包括输出流、宽度、填充字符、前后缀以及是否显示百分比。
     */
    template <class CharT = char, class Traits = std::char_traits<CharT>>
    struct BasicProgressConfig {
        using string_type  = std::basic_string<CharT, Traits>;
        using ostream_type = std::basic_ostream<CharT, Traits>;

        ostream_type &os; ///< 输出目标流。
        int           width; ///< 进度条的宽度（字符数）。
        string_type   fill_char; ///< 已填充部分使用的字符。
        string_type   empty_char; ///< 未填充部分使用的字符。
        string_type   prefix; ///< 进度条前缀字符串。
        string_type   suffix; ///< 进度条后缀字符串。
        bool          show_percent; ///< 是否在进度条后显示百分比数字。

        /**
         * @brief 构造进度条配置对象。
         * @param o   输出流。
         * @param w   进度条宽度（字符数），默认为 50。
         * @param fc  填充字符，默认为 "#"。
         * @param ec  空白字符，默认为 "."。
         * @param pre 前缀字符串，默认为 "["。
         * @param suf 后缀字符串，默认为 "]"。
         * @param sp  是否显示百分比，默认为 true。
         */
        BasicProgressConfig(ostream_type &o,
            int                           w   = 50,
            string_type                   fc  = string_type{1, CharT('#')},
            string_type                   ec  = string_type{1, CharT('.')},
            string_type                   pre = string_type{1, CharT('[')},
            string_type                   suf = string_type{1, CharT(']')},
            bool                          sp  = true) :
            os(o), width(w), fill_char(std::move(fc)),
            empty_char(std::move(ec)), prefix(std::move(pre)),
            suffix(std::move(suf)), show_percent(sp) {}
    };

    /** @brief BasicProgressConfig<char> 的类型别名。 */
    using ProgressConfig = BasicProgressConfig<char>;

    /** @brief BasicProgressConfig<wchar_t> 的类型别名。 */
    using WProgressConfig = BasicProgressConfig<wchar_t>;

    /**
     * @class BasicProgress
     * @brief 进度条迭代器包装器，用于在遍历容器时显示进度。
     * @tparam CharT 字符类型。
     * @tparam Traits 字符特征类型。
     * @tparam Iter 底层迭代器类型（通常为容器的迭代器）。
     * @details 该类返回一个特殊的迭代器，在每次递增时更新并重绘进度条。
     *          用法：将 BasicProgress 对象用于范围 for 循环，例如：
     *          for (auto& item : BasicProgress(container)) { ... }
     */
    template <class CharT, class Traits, class Iter>
    class BasicProgress {
    public:
        using config_type = BasicProgressConfig<CharT, Traits>;

        /**
         * @class iterator
         * @brief 进度条的迭代器，负责绘制进度条。
         */
        class iterator {
            const config_type                    *config_;
            size_t                                current_;
            size_t                                total_;
            Iter                                  it_;
            std::chrono::steady_clock::time_point last_draw_;

            void draw() {
                if (!config_) return;
                auto now = std::chrono::steady_clock::now();
                if (current_ < total_
                    && (now - last_draw_) < std::chrono::milliseconds{50}) {
                    return;
                }
                int percent = current_ * 100 / total_;
                int filled  = percent * config_->width / 100;
                config_->os << CharT('\r') << config_->prefix;
                for (int i = 0; i < filled; ++i)
                    config_->os << config_->fill_char;
                for (int i = filled; i < config_->width; ++i)
                    config_->os << config_->empty_char;
                config_->os << config_->suffix;
                if (config_->show_percent) {
                    config_->os.width(4);
                    config_->os << percent << CharT('%');
                }
                config_->os.flush();
                last_draw_ = now;
            }

        public:
            iterator(const config_type *config,
                size_t                  current,
                size_t                  total,
                Iter                    it) :
                config_(config), current_(current), total_(total), it_(it),
                last_draw_(std::chrono::steady_clock::now()) {}

            iterator &operator++() {
                if (current_ < total_) {
                    ++current_;
                    ++it_;
                    draw();
                }
                return *this;
            }

            iterator operator++(int) {
                iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const iterator &other) {
                return current_ == other.current_;
            }

            bool operator!=(const iterator &other) {
                return current_ != other.current_;
            }

            decltype(*it_) operator*() { return *it_; }
        };

        template <typename Cont>
        BasicProgress(const Cont &cont, const config_type &config) :
            config_(config),
            total_(std::distance(std::begin(cont), std::end(cont))),
            begin_(std::begin(cont)), end_(std::end(cont)) {}

        template <typename Cont>
        BasicProgress(Cont &cont, const config_type &config) :
            config_(config),
            total_(std::distance(std::begin(cont), std::end(cont))),
            begin_(std::begin(cont)), end_(std::end(cont)) {}

        ~BasicProgress() { config_.os << std::endl; }

        iterator begin() { return iterator(&config_, 0, total_, begin_); }
        iterator end() { return iterator(&config_, total_, total_, end_); }

    private:
        config_type config_;
        size_t      total_;
        Iter        begin_;
        Iter        end_;
    };

    /** @brief BasicProgress<char, std::char_traits<char>, Iter> 的类型别名。 */
    template <class Iter>
    using Progress = BasicProgress<char, std::char_traits<char>, Iter>;

    /** @brief BasicProgress<wchar_t, std::char_traits<wchar_t>, Iter> 的类型别名。 */
    template <class Iter>
    using WProgress = BasicProgress<wchar_t, std::char_traits<wchar_t>, Iter>;

    namespace ProgressStyle {
        inline const BasicProgressConfig<> &normal() {
            static BasicProgressConfig<> cfg{std::cout};
            return cfg;
        }

        inline const BasicProgressConfig<> &simple() {
            static BasicProgressConfig<> cfg{//
                std::cout,
                50,
                std::string(1, '='),
                std::string(1, '-'),
                std::string{},
                std::string{},
                false};
            return cfg;
        }

        inline const BasicProgressConfig<> &beautiful() {
            static BasicProgressConfig<> cfg{//
                std::cout,
                50,
                std::string("\u2588"),
                std::string("\u2591"),
                std::string("\u2595"),
                std::string("\u258F"),
                true};
            return cfg;
        }
    }

    /**
     * @brief 创建进度条对象的辅助函数。
     * @tparam CharT 字符类型，默认为 char。
     * @tparam Traits 字符特征类型。
     * @tparam Cont 容器类型（支持转发引用）。
     * @param cont  容器。
     * @param pc    进度条配置，默认为默认配置。
     * @return BasicProgress<CharT, Traits, decltype(std::begin(cont))>。
     */
    template <class CharT = char,
        class Traits      = std::char_traits<CharT>,
        class Cont>
    inline auto
    progress(Cont &&cont, const BasicProgressConfig<CharT, Traits> &pc)
        -> BasicProgress<CharT, Traits, decltype(std::begin(cont))> {
        return {std::forward<Cont>(cont), pc};
    }
}
