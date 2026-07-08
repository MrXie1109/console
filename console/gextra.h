/**
 * @file gextra.h
 * @brief 生成器库扩展功能。
 * @details 为 gen.h 补充额外的生成器、适配器和工具函数。
 * @author MrXie1109
 * @date 2026
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
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>

#include "gen.h"
#include "sfinae.h"

namespace console {
    namespace gen {
        /**
         * @brief 随机数生成器，生成指定范围内的随机数。
         * @tparam T 数值类型。
         */
        template <class T>
        class Random : public Generator<Random<T>, T> {
            std::mt19937              gen_;
            uniform_distribution_t<T> dist_;
            T                         curr_;

        public:
            /**
             * @brief 构造函数。
             * @param min 随机数最小值（包含）。
             * @param max 随机数最大值（包含）。
             * @param seed 随机数种子，默认为随机设备生成。
             */
            Random(T                      min,
                T                         max,
                std::mt19937::result_type seed = std::random_device{}()) :
                gen_(seed), dist_(min, max), curr_(dist_(gen_)) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 始终返回false（无限生成）。
             */
            bool done() { return false; }

            /**
             * @brief 获取当前随机数。
             * @return 生成的随机数。
             */
            T current() { return curr_; }

            /**
             * @brief 向前移动一步。
             */
            void advance() { curr_ = dist_(gen_); }
        };

        /**
         * @brief 文件行生成器，逐行读取文本文件。
         */
        class FileLines : public Generator<FileLines, std::string> {
            std::ifstream file_;
            std::string   current_line_;

        public:
            /**
             * @brief 构造函数，打开文件并读取第一行。
             * @param filename 文件名。
             * @throws FileError 如果文件无法打开。
             */
            explicit FileLines(const std::string &filename) : file_(filename) {
                if (!file_.is_open())
                    throw FileError("Cannot Open File \"" + filename + '"');
                advance();
            }

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果已到达文件末尾则返回true。
             */
            bool done() { return file_.eof(); }

            /**
             * @brief 获取当前行内容。
             * @return 当前行的字符串。
             */
            std::string current() { return current_line_; }

            /**
             * @brief 向前移动一步，读取下一行。
             */
            void advance() { std::getline(file_, current_line_); }
        };

        /**
         * @brief 文件块生成器，按固定大小读取文件块。
         */
        class FileChunks : public Generator<FileChunks, std::string> {
            std::ifstream file_;
            std::string   current_chunk_;
            size_t        chunk_size_;

        public:
            /**
             * @brief 构造函数，打开文件并读取第一个数据块。
             * @param filename 文件名。
             * @param chunk_size 每个数据块的大小，默认为1024字节。
             * @param mode 文件打开模式，默认为std::ios::in。
             * @throws FileError 如果文件无法打开。
             */
            explicit FileChunks(const std::string &filename,
                size_t                             chunk_size = 1024,
                std::ios_base::openmode            mode       = std::ios::in) :
                file_(filename, mode), chunk_size_(chunk_size) {
                if (!file_.is_open())
                    throw FileError("Cannot Open File \"" + filename + '"');
                advance();
            }

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果已到达文件末尾则返回true。
             */
            bool done() { return file_.eof(); }

            /**
             * @brief 获取当前数据块。
             * @return 当前数据块的字符串。
             */
            std::string current() { return current_chunk_; }

            /**
             * @brief 向前移动一步，读取下一个数据块。
             */
            void advance() {
                current_chunk_.resize(chunk_size_);
                file_.read(&current_chunk_[0], chunk_size_);
                current_chunk_.resize(file_.gcount());
            }
        };

        /**
         * @brief 滑动窗口生成器，生成固定大小的滑动窗口。
         * @tparam Gen 源生成器类型。
         */
        template <class Gen>
        class Window : public Generator<Window<Gen>,
                           std::vector<typename Gen::value_type>> {
            Gen                                   gen_;
            size_t                                window_size_;
            std::vector<typename Gen::value_type> window_;
            bool                                  started = false;

            void init() {
                if (!started) {
                    for (size_t i = 0; i < window_size_ && !gen_.done(); ++i) {
                        window_.push_back(gen_.current());
                        gen_.advance();
                    }
                    started = true;
                }
            }

        public:
            /**
             * @brief 构造函数。
             * @param gen 源生成器。
             * @param window_size 窗口大小，默认为16。
             * @throws ValueError 如果窗口大小为0。
             */
            explicit Window(Gen gen, size_t window_size = 16) :
                gen_(gen), window_size_(window_size) {
                if (window_size_ == 0)
                    throw ValueError("Window size must be greater than 0");
            }

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果窗口大小不足则返回true。
             */
            bool done() {
                init();
                return window_.size() < window_size_;
            }

            /**
             * @brief 获取当前窗口内容。
             * @return 包含窗口元素的向量。
             */
            std::vector<typename Gen::value_type> current() {
                init();
                return window_;
            }

            /**
             * @brief 向前移动一步，滑动窗口。
             */
            void advance() {
                init();
                if (!gen_.done()) {
                    window_.erase(window_.begin());
                    window_.push_back(gen_.current());
                    gen_.advance();
                } else
                    window_.erase(window_.begin());
            }
        };

        /**
         * @brief 分块生成器，将元素按块分组。
         * @tparam Gen 源生成器类型。
         */
        template <class Gen>
        class Chunk : public Generator<Chunk<Gen>,
                          std::vector<typename Gen::value_type>> {
            Gen                                   gen_;
            size_t                                chunk_size_;
            std::vector<typename Gen::value_type> chunk_;
            bool                                  started = false;

            void init() {
                if (!started) {
                    for (size_t i = 0; i < chunk_size_ && !gen_.done(); ++i) {
                        chunk_.push_back(gen_.current());
                        gen_.advance();
                    }
                    started = true;
                }
            }

        public:
            /**
             * @brief 构造函数。
             * @param gen 源生成器。
             * @param chunk_size 每个块的大小，默认为16。
             * @throws ValueError 如果块大小为0。
             */
            explicit Chunk(Gen gen, size_t chunk_size = 16) :
                gen_(gen), chunk_size_(chunk_size) {
                if (chunk_size_ == 0)
                    throw ValueError("Chunk size must be greater than 0");
            }

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果当前块为空则返回true。
             */
            bool done() {
                init();
                return chunk_.empty();
            }

            /**
             * @brief 获取当前块内容。
             * @return 包含块元素的向量。
             */
            std::vector<typename Gen::value_type> current() {
                init();
                return chunk_;
            }

            /**
             * @brief 向前移动一步，读取下一个块。
             */
            void advance() {
                init();
                if (!gen_.done()) {
                    chunk_.clear();
                    for (size_t i = 0; i < chunk_size_ && !gen_.done(); ++i) {
                        chunk_.push_back(gen_.current());
                        gen_.advance();
                    }
                } else
                    chunk_.clear();
            }
        };

        /**
         * @brief 链式生成器，将两个生成器首尾连接。
         * @tparam Gen1 第一个生成器类型。
         * @tparam Gen2 第二个生成器类型。
         */
        template <class Gen1, class Gen2>
        class Chain
            : public Generator<Chain<Gen1, Gen2>,
                  typename std::decay<
                      decltype(true ? std::declval<typename Gen1::value_type>()
                                    : std::declval<typename Gen2::
                                              value_type>())>::type> {
            using T = typename std::decay<
                decltype(true ? std::declval<typename Gen1::value_type>()
                              : std::declval<typename Gen2::value_type>())>::
                type;

            Gen1 gen1;
            Gen2 gen2;

        public:
            /**
             * @brief 构造函数。
             * @param g1 第一个生成器。
             * @param g2 第二个生成器。
             */
            Chain(Gen1 g1, Gen2 g2) : gen1(g1), gen2(g2) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果两个生成器都已完成则返回true。
             */
            bool done() { return gen1.done() && gen2.done(); }

            /**
             * @brief 获取当前值。
             * @return 当前活跃生成器的值。
             */
            T current() {
                if (!gen1.done())
                    return gen1.current();
                else
                    return gen2.current();
            }

            /**
             * @brief 向前移动一步。
             */
            void advance() {
                if (!gen1.done())
                    gen1.advance();
                else
                    gen2.advance();
            }
        };

        /**
         * @brief 步进生成器，按指定步长跳过元素。
         * @tparam Gen 源生成器类型。
         */
        template <class Gen>
        class StepBy : public Generator<StepBy<Gen>, typename Gen::value_type> {
            Gen    gen;
            size_t step;

        public:
            /**
             * @brief 构造函数。
             * @param g 源生成器。
             * @param s 步长，默认为1。
             * @throws ValueError 如果步长为0。
             */
            explicit StepBy(Gen g, size_t s = 1) : gen(g), step(s) {
                if (step == 0)
                    throw ValueError("Step size must be greater than 0");
            }

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果源生成器已完成则返回true。
             */
            bool done() { return gen.done(); }

            /**
             * @brief 获取当前值。
             * @return 当前值。
             */
            typename Gen::value_type current() { return gen.current(); }

            /**
             * @brief 向前移动一步，跳过step-1个元素。
             */
            void advance() {
                for (size_t i = 0; i < step && !gen.done(); ++i) gen.advance();
            }
        };

        /**
         * @brief 调试生成器，打印每个元素的值。
         * @tparam Gen 源生成器类型。
         */
        template <class Gen>
        class Debug : public Generator<Debug<Gen>, typename Gen::value_type> {
            Gen           gen;
            std::string   message;
            std::ostream &out;

        public:
            /**
             * @brief 构造函数。
             * @param g 源生成器。
             * @param msg 调试消息前缀，默认为"Current Value: "。
             * @param os 输出流，默认为std::cout。
             */
            explicit Debug(Gen g,
                std::string    msg = "Current Value: ",
                std::ostream &os = std::cout) : gen(g), message(msg), out(os) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果源生成器已完成则返回true。
             */
            bool done() { return gen.done(); }

            /**
             * @brief 获取当前值并打印调试信息。
             * @return 当前值。
             */
            typename Gen::value_type current() {
#ifndef NDEBUG
                out << message << gen.current() << std::endl;
#endif
                return gen.current();
            }

            /**
             * @brief 向前移动一步。
             */
            void advance() { gen.advance(); }
        };

        /**
         * @brief 取元素直到谓词成立（包含第一个满足条件的元素）。
         * @tparam Gen 源生成器类型。
         * @tparam Pred 谓词类型。
         */
        template <class Gen, class Pred>
        class TakeUntil
            : public Generator<TakeUntil<Gen, Pred>, typename Gen::value_type> {
            Gen  gen;
            Pred pred;
            bool stopped = false;

            void try_advance() {
                if (stopped || gen.done()) return;
                if (pred(gen.current())) {
                    stopped = true;
                    gen.advance();
                }
            }

        public:
            /**
             * @brief 构造函数。
             * @param g 源生成器。
             * @param p 谓词函数。
             */
            TakeUntil(Gen g, Pred p) : gen(g), pred(p) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果源生成器已完成或已触发停止条件则返回true。
             */
            bool done() {
                if (gen.done() || stopped) return true;
                return false;
            }

            /**
             * @brief 获取当前值。
             * @return 当前值。
             */
            auto current() -> decltype(gen.current()) { return gen.current(); }

            /**
             * @brief 向前移动一步。
             */
            void advance() {
                gen.advance();
                try_advance();
            }
        };

        /**
         * @brief 跳过元素直到谓词成立（跳过第一个满足条件的元素）。
         * @tparam Gen 源生成器类型。
         * @tparam Pred 谓词类型。
         */
        template <class Gen, class Pred>
        class DropUntil
            : public Generator<DropUntil<Gen, Pred>, typename Gen::value_type> {
            Gen  gen;
            Pred pred;
            bool dropped = false;

            void do_drop() {
                while (!gen.done() && !pred(gen.current())) gen.advance();
                if (!gen.done() && pred(gen.current())) gen.advance();
                dropped = true;
            }

        public:
            /**
             * @brief 构造函数。
             * @param g 源生成器。
             * @param p 谓词函数。
             */
            DropUntil(Gen g, Pred p) : gen(g), pred(p) {}

            /**
             * @brief 检查生成器是否已完成。
             * @return 如果源生成器已完成则返回true。
             */
            bool done() {
                if (!dropped) do_drop();
                return gen.done();
            }

            /**
             * @brief 获取当前值。
             * @return 当前值。
             */
            auto current() -> decltype(gen.current()) { return gen.current(); }

            /**
             * @brief 向前移动一步。
             */
            void advance() { gen.advance(); }
        };

        template <class... Ops>
        class Pipeline;

        template <class Op>
        class Pipeline<Op> {
            Op op;

        public:
            Pipeline(Op o) : op(o) {}

            template <class Gen>
            auto operator()(Gen gen) const -> decltype(gen | op) {
                return gen | op;
            }
        };

        template <class Op, class... Rest>
        class Pipeline<Op, Rest...> {
            Op                op;
            Pipeline<Rest...> rest;

        public:
            Pipeline(Op o, Rest... r) : op(o), rest(r...) {}

            template <class Gen>
            auto operator()(Gen gen) const -> decltype(rest(gen | op)) {
                return rest(gen | op);
            }
        };

        template <class... Ops>
        class pipeline_t {
            Pipeline<Ops...> pipeline;

        public:
            pipeline_t(Ops... ops) : pipeline(ops...) {}

            template <class Gen>
            friend auto
            operator|(Gen gen, pipeline_t pl) -> decltype(pl.pipeline(gen)) {
                return pl.pipeline(gen);
            }
        };

        template <class... Ops>
        pipeline_t<Ops...> pipeline(Ops... ops) {
            return pipeline_t<Ops...>(ops...);
        }

        template <class Op1, class Op2>
        pipeline_t<Op1, Op2> operator>>(Op1 op1, Op2 op2) {
            return pipeline(op1, op2);
        }

        // =====================================================================

        /**
         * @brief 创建随机数生成器。
         * @tparam T 数值类型。
         * @param min 随机数最小值（包含）。
         * @param max 随机数最大值（包含）。
         * @param seed 随机数种子，默认为随机设备生成。
         * @return Random<T> 随机数生成器。
         */
        template <class T>
        Random<T> random(T            min,
            T                         max,
            std::mt19937::result_type seed = std::random_device{}()) {
            return Random<T>(min, max, seed);
        }

        /**
         * @brief 创建文件行生成器。
         * @param filename 文件名。
         * @return FileLines 文件行生成器。
         */
        inline FileLines file_lines(const std::string &filename) {
            return FileLines(filename);
        }

        /**
         * @brief 创建文件块生成器。
         * @param filename 文件名。
         * @param chunk_size 每个数据块的大小，默认为1024字节。
         * @param mode 文件打开模式，默认为std::ios::in。
         * @return FileChunks 文件块生成器。
         */
        inline FileChunks file_chunks(const std::string &filename,
            size_t                                       chunk_size = 1024,
            std::ios_base::openmode                      mode = std::ios::in) {
            return FileChunks(filename, chunk_size, mode);
        }

        /**
         * @brief 窗口适配器（用于管道操作符）。
         */
        class window_t {
            size_t s;

        public:
            /**
             * @brief 构造函数。
             * @param size 窗口大小，默认为16。
             */
            explicit window_t(size_t size = 16) : s(size) {}

            /**
             * @brief 管道操作符，创建窗口生成器。
             * @tparam Gen 源生成器类型。
             * @param g 源生成器。
             * @param w 窗口适配器。
             * @return Window<Gen> 窗口生成器。
             */
            template <class Gen>
            friend Window<Gen> operator|(Gen g, window_t w) {
                return Window<Gen>(g, w.s);
            }
        };

        /**
         * @brief 创建窗口适配器。
         * @param size 窗口大小，默认为16。
         * @return window_t 窗口适配器。
         */
        inline window_t window(size_t size = 16) {
            return window_t(size);
        }

        /**
         * @brief 分块适配器（用于管道操作符）。
         */
        class chunk_t {
            size_t s;

        public:
            /**
             * @brief 构造函数。
             * @param size 块大小，默认为16。
             */
            explicit chunk_t(size_t size = 16) : s(size) {}

            /**
             * @brief 管道操作符，创建分块生成器。
             * @tparam Gen 源生成器类型。
             * @param g 源生成器。
             * @param t 分块适配器。
             * @return Chunk<Gen> 分块生成器。
             */
            template <class Gen>
            friend Chunk<Gen> operator|(Gen g, chunk_t t) {
                return Chunk<Gen>(g, t.s);
            }
        };

        /**
         * @brief 创建分块适配器。
         * @param size 块大小，默认为16。
         * @return chunk_t 分块适配器。
         */
        inline chunk_t chunk(size_t size = 16) {
            return chunk_t(size);
        }

        /**
         * @brief 创建链式生成器，将两个生成器首尾连接。
         * @tparam Gen1 第一个生成器类型。
         * @tparam Gen2 第二个生成器类型。
         * @param g1 第一个生成器。
         * @param g2 第二个生成器。
         * @return Chain<Gen1, Gen2> 链式生成器。
         */
        template <class Gen1, class Gen2>
        Chain<Gen1, Gen2> chain(Gen1 g1, Gen2 g2) {
            return Chain<Gen1, Gen2>(g1, g2);
        }

        /**
         * @brief 创建链式生成器，将多个生成器首尾连接。
         * @tparam Gen 第一个生成器类型。
         * @tparam ...Rest 其余生成器类型。
         * @param g 第一个生成器。
         * @param rest 其余生成器。
         * @return Chain<Gen, decltype(chain(rest...))> 链式生成器。
         */
        template <class Gen, class... Rest>
        auto
        chain(Gen g, Rest... rest) -> Chain<Gen, decltype(chain(rest...))> {
            return {g, chain(rest...)};
        }

        /**
         * @brief 加号操作符重载，用于连接两个生成器。
         * @tparam Gen1 第一个生成器类型。
         * @tparam Gen2 第二个生成器类型。
         * @param g1 第一个生成器。
         * @param g2 第二个生成器。
         * @return Chain<Gen1, Gen2> 链式生成器。
         */
        template <class Gen1, class Gen2>
        Chain<Gen1, Gen2> operator+(Gen1 g1, Gen2 g2) {
            return Chain<Gen1, Gen2>(g1, g2);
        }

        /**
         * @brief 步进适配器（用于管道操作符）。
         */
        class step_by_t {
            size_t s;

        public:
            /**
             * @brief 构造函数。
             * @param size 步长，默认为1。
             */
            explicit step_by_t(size_t size = 1) : s(size) {}

            /**
             * @brief 管道操作符，创建步进生成器。
             * @tparam Gen 源生成器类型。
             * @param g 源生成器。
             * @param s 步进适配器。
             * @return StepBy<Gen> 步进生成器。
             */
            template <class Gen>
            friend StepBy<Gen> operator|(Gen g, step_by_t s) {
                return StepBy<Gen>(g, s.s);
            }
        };

        /**
         * @brief 创建步进适配器。
         * @param size 步长，默认为1。
         * @return step_by_t 步进适配器。
         */
        inline step_by_t step_by(size_t size = 1) {
            return step_by_t(size);
        }

        /**
         * @brief 调试适配器（用于管道操作符）。
         */
        class debug_t {
            std::string   message;
            std::ostream &out;

        public:
            /**
             * @brief 构造函数。
             * @param msg 调试消息前缀，默认为"DEBUG: "。
             * @param os 输出流，默认为std::cout。
             */
            explicit debug_t(std::string msg = "DEBUG: ",
                std::ostream &os = std::cout) : message(msg), out(os) {}

            /**
             * @brief 管道操作符，创建调试生成器。
             * @tparam Gen 源生成器类型。
             * @param g 源生成器。
             * @param d 调试适配器。
             * @return Debug<Gen> 调试生成器。
             */
            template <class Gen>
            friend Debug<Gen> operator|(Gen g, debug_t d) {
                return Debug<Gen>(g, d.message, d.out);
            }
        };

        /**
         * @brief 创建调试适配器。
         * @param msg 调试消息前缀，默认为"DEBUG: "。
         * @param os 输出流，默认为std::cout。
         * @return debug_t 调试适配器。
         */
        inline debug_t
        debug(std::string msg = "DEBUG: ", std::ostream &os = std::cout) {
            return debug_t{msg, os};
        }

        /**
         * @brief 取元素直到谓词成立适配器（用于管道操作符）。
         * @tparam Pred 谓词类型。
         */
        template <class Pred>
        class take_until_t {
            Pred pred;

        public:
            /**
             * @brief 构造函数。
             * @param p 谓词函数。
             */
            explicit take_until_t(Pred p) : pred(p) {}

            /**
             * @brief 管道操作符，创建TakeUntil生成器。
             * @tparam Gen 源生成器类型。
             * @param g 源生成器。
             * @param tu 取元素适配器。
             * @return TakeUntil<Gen, Pred> 取元素生成器。
             */
            template <class Gen>
            friend TakeUntil<Gen, Pred> operator|(Gen g, take_until_t tu) {
                return TakeUntil<Gen, Pred>(g, tu.pred);
            }
        };

        /**
         * @brief 创建取元素直到谓词成立适配器。
         * @tparam Pred 谓词类型。
         * @param p 谓词函数。
         * @return take_until_t<Pred> 取元素适配器。
         */
        template <class Pred>
        take_until_t<Pred> take_until(Pred p) {
            return take_until_t<Pred>(p);
        }

        /**
         * @brief 跳过元素直到谓词成立适配器（用于管道操作符）。
         * @tparam Pred 谓词类型。
         */
        template <class Pred>
        class drop_until_t {
            Pred pred;

        public:
            /**
             * @brief 构造函数。
             * @param p 谓词函数。
             */
            explicit drop_until_t(Pred p) : pred(p) {}

            /**
             * @brief 管道操作符，创建DropUntil生成器。
             * @tparam Gen 源生成器类型。
             * @param g 源生成器。
             * @param du 跳过元素适配器。
             * @return DropUntil<Gen, Pred> 跳过元素生成器。
             */
            template <class Gen>
            friend DropUntil<Gen, Pred> operator|(Gen g, drop_until_t du) {
                return DropUntil<Gen, Pred>(g, du.pred);
            }
        };

        /**
         * @brief 创建跳过元素直到谓词成立适配器。
         * @tparam Pred 谓词类型。
         * @param p 谓词函数。
         * @return drop_until_t<Pred> 跳过元素适配器。
         */
        template <class Pred>
        drop_until_t<Pred> drop_until(Pred p) {
            return drop_until_t<Pred>(p);
        }

        /**
         * @brief 归约适配器，将生成器所有元素归约为单个值。
         * @tparam T 初始值和结果类型。
         * @tparam BinaryOp 二元操作符类型。
         */
        template <class T, class BinaryOp>
        class reduce_t {
            T        init_;
            BinaryOp op_;

        public:
            /**
             * @brief 构造函数。
             * @param init 初始值。
             * @param op 二元操作符。
             */
            reduce_t(T init, BinaryOp op) : init_(init), op_(op) {}

            /**
             * @brief 管道操作符，执行归约。
             * @param gen 源生成器。
             * @param r 归约适配器。
             * @return T 归约结果。
             */
            template <class Gen>
            friend T operator|(Gen gen, reduce_t r) {
                T result = r.init_;
                for (auto &&item : gen) result = r.op_(result, item);
                return result;
            }
        };

        /**
         * @brief 创建归约适配器。
         * @tparam T 初始值类型。
         * @tparam BinaryOp 二元操作符类型。
         * @param init 初始值。
         * @param op 二元操作符。
         * @return reduce_t<T, BinaryOp> 归约适配器。
         */
        template <class T, class BinaryOp>
        reduce_t<T, BinaryOp> reduce(const T &init, BinaryOp bo) {
            return reduce_t<T, BinaryOp>(init, bo);
        }
    }

    namespace ops {
        /**
         * @brief 幂运算变换器。
         * @tparam T 指数类型。
         */
        template <class T>
        struct Pow {
            T exponent;

            /**
             * @brief 计算输入值的指定次幂。
             * @param x 输入值。
             * @return std::pow(x, exponent)。
             */
            template <class U>
            auto operator()(U x) const -> decltype(std::pow(x, exponent)) {
                return std::pow(x, exponent);
            }
        };

        /**
         * @brief 创建幂运算变换器。
         * @tparam T 指数类型（自动推导）。
         * @param exp 指数值。
         * @return Pow<T> 幂运算变换器。
         */
        template <class T>
        Pow<T> pow(T exp) {
            return {exp};
        }

        /**
         * @brief 自然指数变换器。
         */
        struct exp_t {
            /**
             * @brief 计算输入值的自然指数。
             * @param x 输入值。
             * @return std::exp(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::exp(x)) {
                return std::exp(x);
            }
        };

        static constexpr exp_t exp;

        /**
         * @brief 常用对数变换器（以10为底）。
         */
        struct log10_t {
            /**
             * @brief 计算输入值的常用对数。
             * @param x 输入值。
             * @return std::log10(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::log10(x)) {
                return std::log10(x);
            }
        };

        static constexpr log10_t log10;

        /**
         * @brief 正弦变换器。
         */
        struct sin_t {
            /**
             * @brief 计算输入值的正弦值。
             * @param x 输入值。
             * @return std::sin(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::sin(x)) {
                return std::sin(x);
            }
        };

        static constexpr sin_t sin;

        /**
         * @brief 余弦变换器。
         */
        struct cos_t {
            /**
             * @brief 计算输入值的余弦值。
             * @param x 输入值。
             * @return std::cos(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::cos(x)) {
                return std::cos(x);
            }
        };

        static constexpr cos_t cos;

        /**
         * @brief 正切变换器。
         */
        struct tan_t {
            /**
             * @brief 计算输入值的正切值。
             * @param x 输入值。
             * @return std::tan(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::tan(x)) {
                return std::tan(x);
            }
        };

        static constexpr tan_t tan;

        /**
         * @brief 反正弦变换器。
         */
        struct asin_t {
            /**
             * @brief 计算输入值的反正弦值。
             * @param x 输入值。
             * @return std::asin(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::asin(x)) {
                return std::asin(x);
            }
        };

        static constexpr asin_t asin;

        /**
         * @brief 反余弦变换器。
         */
        struct acos_t {
            /**
             * @brief 计算输入值的反余弦值。
             * @param x 输入值。
             * @return std::acos(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::acos(x)) {
                return std::acos(x);
            }
        };

        static constexpr acos_t acos;

        /**
         * @brief 反正切变换器。
         */
        struct atan_t {
            /**
             * @brief 计算输入值的反正切值。
             * @param x 输入值。
             * @return std::atan(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::atan(x)) {
                return std::atan(x);
            }
        };

        static constexpr atan_t atan;

        /**
         * @brief 符号变换器。
         * @details 返回输入值的符号：正数返回1，负数返回-1，零返回0。
         */
        struct sign_t {
            /**
             * @brief 计算输入值的符号。
             * @param x 输入值。
             * @return (x > 0) - (x < 0)。
             */
            template <class T>
            int operator()(T x) const {
                return (x > 0) - (x < 0);
            }
        };

        static constexpr sign_t sign;

        /**
         * @brief 四舍五入变换器。
         */
        struct round_t {
            /**
             * @brief 对输入值进行四舍五入。
             * @param x 输入值。
             * @return std::round(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::round(x)) {
                return std::round(x);
            }
        };

        static constexpr round_t round;

        /**
         * @brief 向上取整变换器。
         */
        struct ceil_t {
            /**
             * @brief 对输入值进行向上取整。
             * @param x 输入值。
             * @return std::ceil(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::ceil(x)) {
                return std::ceil(x);
            }
        };

        static constexpr ceil_t ceil;

        /**
         * @brief 向下取整变换器。
         */
        struct floor_t {
            /**
             * @brief 对输入值进行向下取整。
             * @param x 输入值。
             * @return std::floor(x)。
             */
            template <class T>
            auto operator()(T x) const -> decltype(std::floor(x)) {
                return std::floor(x);
            }
        };

        static constexpr floor_t floor;

        /**
         * @brief 字符串长度变换器。
         */
        struct strlen_t {
            /**
             * @brief 获取字符串的长度。
             * @param s 输入字符串。
             * @return s.size()。
             */
            size_t operator()(const std::string &s) const { return s.size(); }
        };

        static constexpr strlen_t strlen;

        /**
         * @brief 字符串转大写变换器。
         */
        struct to_upper_t {
            /**
             * @brief 将字符串转换为大写。
             * @param s 输入字符串。
             * @return 大写后的字符串。
             */
            std::string operator()(std::string s) const {
                for (auto &c : s) c = std::toupper(c);
                return s;
            }
        };

        static constexpr to_upper_t to_upper;

        /**
         * @brief 字符串转小写变换器。
         */
        struct to_lower_t {
            /**
             * @brief 将字符串转换为小写。
             * @param s 输入字符串。
             * @return 小写后的字符串。
             */
            std::string operator()(std::string s) const {
                for (auto &c : s) c = std::tolower(c);
                return s;
            }
        };

        static constexpr to_lower_t to_lower;

        /**
         * @brief 字符串去空格变换器。
         * @details 去除字符串首尾的空白字符（空格、制表符、换行等）。
         */
        struct trim_t {
            /**
             * @brief 去除字符串首尾的空白字符。
             * @param s 输入字符串。
             * @return 去除首尾空白后的字符串。
             */
            std::string operator()(std::string s) const {
                auto start = s.find_first_not_of(" \t\n\r\f\v");
                if (start == std::string::npos) return "";
                auto end = s.find_last_not_of(" \t\n\r\f\v");
                return s.substr(start, end - start + 1);
            }
        };

        static constexpr trim_t trim;

        /**
         * @brief 字符串反转变换器。
         */
        struct reverse_string_t {
            /**
             * @brief 反转字符串。
             * @param s 输入字符串。
             * @return 反转后的字符串。
             */
            std::string operator()(std::string s) const {
                std::reverse(s.begin(), s.end());
                return s;
            }
        };

        static constexpr reverse_string_t reverse_string;

        /**
         * @brief 容器大小变换器。
         */
        struct size_t_ {
            /**
             * @brief 获取容器的大小。
             * @param c 输入容器。
             * @return c.size()。
             */
            template <class Container>
            auto operator()(const Container &c) const -> decltype(c.size()) {
                return c.size();
            }
        };

        static constexpr size_t_ size;

        /**
         * @brief 容器判空谓词。
         */
        struct empty_t {
            /**
             * @brief 判断容器是否为空。
             * @param c 输入容器。
             * @return c.empty()。
             */
            template <class Container>
            bool operator()(const Container &c) const {
                return c.empty();
            }
        };

        static constexpr empty_t empty;

        /**
         * @brief 容器最大值变换器。
         */
        struct max_element_t {
            /**
             * @brief 获取容器中的最大值。
             * @param c 输入容器。
             * @return *std::max_element(c.begin(), c.end())。
             */
            template <class Container>
            auto operator()(const Container &c) const
                -> decltype(*std::max_element(c.begin(), c.end())) {
                return *std::max_element(c.begin(), c.end());
            }
        };

        static constexpr max_element_t max_element;

        /**
         * @brief 容器最小值变换器。
         */
        struct min_element_t {
            /**
             * @brief 获取容器中的最小值。
             * @param c 输入容器。
             * @return *std::min_element(c.begin(), c.end())。
             */
            template <class Container>
            auto operator()(const Container &c) const
                -> decltype(*std::min_element(c.begin(), c.end())) {
                return *std::min_element(c.begin(), c.end());
            }
        };

        static constexpr min_element_t min_element;

        /**
         * @brief 范围求和变换器。
         */
        struct sum_t {
            /**
             * @brief 计算范围中所有元素的和。
             * @param r 输入范围。
             * @return 元素总和。
             */
            template <class Range>
            auto operator()(Range r) const -> decltype(std::accumulate(
                r.begin(), r.end(), typename Range::value_type{})) {
                using T = typename Range::value_type;
                return std::accumulate(r.begin(), r.end(), T{});
            }
        };

        static constexpr sum_t sum;

        /**
         * @brief 范围平均值变换器。
         */
        struct average_t {
            /**
             * @brief 计算范围中所有元素的平均值。
             * @param r 输入范围。
             * @return 元素平均值。
             */
            template <class Range>
            auto operator()(Range r) const
                -> decltype(std::accumulate(r.begin(),
                                r.end(),
                                typename Range::value_type{})
                            / r.size()) {
                using T = typename Range::value_type;
                return std::accumulate(r.begin(), r.end(), T{}) / r.size();
            }
        };

        static constexpr average_t average;

        /**
         * @brief 区间谓词（闭区间）。
         * @tparam T 元素类型。
         */
        template <class T>
        struct Between {
            T low, high;

            /**
             * @brief 判断输入值是否在闭区间 [low, high] 内。
             * @param x 输入值。
             * @return low <= x && x <= high。
             */
            bool operator()(const T &x) const { return low <= x && x <= high; }
        };

        /**
         * @brief 创建闭区间谓词。
         * @tparam T 元素类型（自动推导）。
         * @param low 区间下界。
         * @param high 区间上界。
         * @return Between<T> 区间谓词。
         */
        template <class T>
        Between<T> between(T low, T high) {
            return {low, high};
        }

        /**
         * @brief 区间谓词（开区间）。
         * @tparam T 元素类型。
         */
        template <class T>
        struct BetweenExclusive {
            T low, high;

            /**
             * @brief 判断输入值是否在开区间 (low, high) 内。
             * @param x 输入值。
             * @return low < x && x < high。
             */
            bool operator()(const T &x) const { return low < x && x < high; }
        };

        /**
         * @brief 创建开区间谓词。
         * @tparam T 元素类型（自动推导）。
         * @param low 区间下界。
         * @param high 区间上界。
         * @return BetweenExclusive<T> 区间谓词。
         */
        template <class T>
        BetweenExclusive<T> between_exclusive(T low, T high) {
            return {low, high};
        }

        /**
         * @brief 整除谓词。
         * @tparam T 元素类型。
         */
        template <class T>
        struct DivisibleBy {
            T n;

            /**
             * @brief 判断输入值是否能被 n 整除。
             * @param x 输入值。
             * @return x % n == 0。
             */
            bool operator()(const T &x) const { return x % n == 0; }
        };

        /**
         * @brief 创建整除谓词。
         * @tparam T 元素类型（自动推导）。
         * @param n 除数。
         * @return DivisibleBy<T> 整除谓词。
         */
        template <class T>
        DivisibleBy<T> divisible_by(T n) {
            return {n};
        }

        /**
         * @brief 2的幂次谓词。
         */
        struct power_of_two_t {
            /**
             * @brief 判断输入值是否为2的幂次。
             * @param n 输入值。
             * @return 是否为2的幂次。
             */
            template <class Int>
            bool operator()(Int n) const {
                return n > 0 && (n & (n - 1)) == 0;
            }
        };

        static constexpr power_of_two_t power_of_two;

        /**
         * @brief 回文数谓词。
         */
        struct palindrome_t {
            /**
             * @brief 判断输入值是否为回文数。
             * @param n 输入值。
             * @return 是否为回文数。
             */
            template <class T>
            bool operator()(T n) const {
                std::string s = std::to_string(n);
                std::string r = s;
                std::reverse(r.begin(), r.end());
                return s == r;
            }
        };

        static constexpr palindrome_t palindrome;

        /**
         * @brief 恒真谓词。
         */
        struct always_true_t {
            /**
             * @brief 对任意输入返回 true。
             * @param args 任意参数。
             * @return true。
             */
            template <class... Args>
            bool operator()(Args &&...) const {
                return true;
            }
        };

        static constexpr always_true_t always_true;

        /**
         * @brief 恒假谓词。
         */
        struct always_false_t {
            /**
             * @brief 对任意输入返回 false。
             * @param args 任意参数。
             * @return false。
             */
            template <class... Args>
            bool operator()(Args &&...) const {
                return false;
            }
        };

        static constexpr always_false_t always_false;

        /**
         * @brief 限幅变换器。
         * @tparam T 元素类型。
         */
        template <class T>
        struct Clamp {
            T low, high;

            /**
             * @brief 将输入值限制在 [low, high] 范围内。
             * @param x 输入值。
             * @return 限幅后的值。
             */
            T operator()(T x) const { return std::max(low, std::min(high, x)); }
        };

        /**
         * @brief 创建限幅变换器。
         * @tparam T 元素类型（自动推导）。
         * @param low 下界。
         * @param high 上界。
         * @return Clamp<T> 限幅变换器。
         */
        template <class T>
        Clamp<T> clamp(T low, T high) {
            return {low, high};
        }

        /**
         * @brief 时间戳生成器。
         * @details 生成当前时间的 Unix 时间戳（秒）。
         */
        struct timestamp_t {
            /**
             * @brief 获取当前时间的 Unix 时间戳。
             * @return 当前时间的秒级时间戳。
             */
            long long operator()() const {
                return std::chrono::duration_cast<std::chrono::seconds>(
                    std::chrono::system_clock::now().time_since_epoch())
                    .count();
            }
        };

        static constexpr timestamp_t timestamp;

        /**
         * @brief 加法函数子。
         * @details 等价于 operator+，支持任意类型（只要定义了 operator+）。
         *          用于 reduce 等需要二元操作的场景。
         */
        struct plus_t {
            /**
             * @brief 对两个值执行加法。
             * @tparam T 左操作数类型。
             * @tparam U 右操作数类型。
             * @param a 左操作数。
             * @param b 右操作数。
             * @return decltype(a + b) 两数之和。
             */
            template <class T, class U>
            auto operator()(T a, U b) const -> decltype(a + b) {
                return a + b;
            }
        };

        static constexpr plus_t plus;

        /**
         * @brief 减法函数子。
         * @details 等价于 operator-，支持任意类型（只要定义了 operator-）。
         *          用于 reduce 等需要二元操作的场景。
         */
        struct minus_t {
            /**
             * @brief 对两个值执行减法。
             * @tparam T 左操作数类型。
             * @tparam U 右操作数类型。
             * @param a 左操作数。
             * @param b 右操作数。
             * @return decltype(a - b) 两数之差。
             */
            template <class T, class U>
            auto operator()(T a, U b) const -> decltype(a - b) {
                return a - b;
            }
        };

        static constexpr minus_t minus;

        /**
         * @brief 乘法函数子。
         * @details 等价于 operator*，支持任意类型（只要定义了 operator*）。
         *          用于 reduce 等需要二元操作的场景。
         */
        struct multiplies_t {
            /**
             * @brief 对两个值执行乘法。
             * @tparam T 左操作数类型。
             * @tparam U 右操作数类型。
             * @param a 左操作数。
             * @param b 右操作数。
             * @return decltype(a * b) 两数之积。
             */
            template <class T, class U>
            auto operator()(T a, U b) const -> decltype(a * b) {
                return a * b;
            }
        };

        static constexpr multiplies_t multiplies;

        /**
         * @brief 除法函数子。
         * @details 等价于 operator/，支持任意类型（只要定义了 operator/）。
         *          用于 reduce 等需要二元操作的场景。
         * @note 若是算术除法语义，调用者需确保除数不为零，否则行为由 operator/ 决定。
         */
        struct divides_t {
            /**
             * @brief 对两个值执行除法。
             * @tparam T 左操作数类型。
             * @tparam U 右操作数类型。
             * @param a 左操作数。
             * @param b 右操作数。
             * @return decltype(a / b) 两数之商。
             */
            template <class T, class U>
            auto operator()(T a, U b) const -> decltype(a / b) {
                return a / b;
            }
        };

        static constexpr divides_t divides;

        /**
         * @brief 取模函数子。
         * @details 等价于 operator%，支持任意类型（只要定义了 operator%）。
         *          用于 reduce 等需要二元操作的场景。
         * @note 若是算术取模语义，调用者需确保除数不为零，否则行为由 operator% 决定。
         */
        struct modulus_t {
            /**
             * @brief 对两个值执行取模。
             * @tparam T 左操作数类型。
             * @tparam U 右操作数类型。
             * @param a 左操作数。
             * @param b 右操作数。
             * @return decltype(a % b) 两数之模。
             */
            template <class T, class U>
            auto operator()(T a, U b) const -> decltype(a % b) {
                return a % b;
            }
        };

        static constexpr modulus_t modulus;
    }
}
