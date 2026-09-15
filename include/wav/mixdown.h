/**
 * @file wav/mixdown.h
 * @brief WAV 波形处理库。
 * @details 提供混合、归一化、量化、增益、限幅、反相等操作，
 *          与 `wav/player.h` 的 `Wave` 结构兼容。
 *          所有函数以 16-bit PCM 为输入输出基准。
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
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <vector>

#include "../core/csexc.h"
#include "../numeric/random.h"
#include "./base.h"

namespace console {
    namespace wav {
        /** @cond INTERNAL */
        namespace detail {
            /**
             * @brief 读取交错 PCM 中第 i 个 16-bit 样本。
             * @param pcm 交错小端 PCM 数据。
             * @param i 样本下标(以样本为单位，非字节)。
             * @return 有符号样本值。
             */
            inline short
            get16(const std::vector<unsigned char> &pcm, size_t i) {
                return static_cast<short>(
                    static_cast<uint16_t>(pcm[i * 2])
                    | static_cast<uint16_t>(pcm[i * 2 + 1]) << 8);
            }

            /**
             * @brief 写入交错 PCM 中第 i 个 16-bit 样本。
             * @param pcm 交错小端 PCM 数据。
             * @param i 样本下标(以样本为单位，非字节)。
             * @param s 有符号样本值。
             */
            inline void
            set16(std::vector<unsigned char> &pcm, size_t i, short s) {
                pcm[i * 2]     = static_cast<unsigned char>(s & 0xFF);
                pcm[i * 2 + 1] = static_cast<unsigned char>(s >> 8 & 0xFF);
            }

            /**
             * @brief 将浮点样本限幅并量化为 16-bit。
             * @param v 浮点样本值。
             * @return 量化后的 16-bit 样本。
             * @note 采用四舍五入，正负半轴对称。
             */
            inline short quant16(double v) {
                if (v > 32767.0) v = 32767.0;
                if (v < -32768.0) v = -32768.0;
                return static_cast<short>(v >= 0.0 ? v + 0.5 : v - 0.5);
            }
        }
        /** @endcond */

        /**
         * @brief 混合多条波形。
         * @param tracks 待混合的波形列表。
         * @param gains 各轨线性增益，为空时全部按 1.0 处理。
         * @return 混合后的波形。
         * @throws ValueError 列表为空、格式不一致、增益数量不匹配、
         *         声道数或采样率为 0。
         * @note 采用浮点累加，最后统一限幅到 16-bit，避免中间削波。
         *       长度以最长轨为准，较短轨视为静音补零。
         */
        inline Wave mixdown(const std::vector<Wave> &tracks,
            const std::vector<double>               &gains = {}) {
            if (tracks.empty())
                throw ValueError("Cannot Mixdown an Empty List");
            const uint32_t rate     = tracks[0].sample_rate;
            const uint32_t channels = tracks[0].channels;
            if (rate == 0 || channels == 0)
                throw ValueError("Invalid Channel Count or Sample Rate");
            if (!gains.empty() && gains.size() != tracks.size())
                throw ValueError("Gain Count Must Match Track Count");
            const size_t frame      = channels * 2;
            size_t       max_frames = 0;
            for (size_t t = 0; t < tracks.size(); t++) {
                if (tracks[t].sample_rate != rate
                    || tracks[t].channels != channels)
                    throw ValueError("All Tracks Must Share the Same Format");
                const size_t frames = tracks[t].pcm.size() / frame;
                if (frames > max_frames) max_frames = frames;
            }
            const size_t        sample_count = max_frames * channels;
            std::vector<double> acc(sample_count, 0.0);
            for (size_t t = 0; t < tracks.size(); t++) {
                const double g = gains.empty() ? 1.0 : gains[t];
                if (g == 0.0) continue;
                const std::vector<unsigned char> &src    = tracks[t].pcm;
                const size_t                      frames = src.size() / frame;
                for (size_t i = 0; i < frames; i++)
                    for (uint32_t c = 0; c < channels; c++)
                        acc[i * channels + c]
                            += static_cast<double>(
                                   detail::get16(src, i * channels + c))
                               * g;
            }
            Wave out;
            out.sample_rate = rate;
            out.channels    = channels;
            out.pcm.resize(sample_count * 2);
            for (size_t i = 0; i < sample_count; i++)
                detail::set16(out.pcm, i, detail::quant16(acc[i]));
            return out;
        }

        /**
         * @brief 查找波形峰值。
         * @param w 波形数据。
         * @return 峰值绝对值，以 16-bit 样本为单位。
         */
        inline double peak_of(const Wave &w) {
            double       peak  = 0.0;
            const size_t count = w.pcm.size() / 2;
            for (size_t i = 0; i < count; i++) {
                const double v
                    = std::fabs(static_cast<double>(detail::get16(w.pcm, i)));
                if (v > peak) peak = v;
            }
            return peak;
        }

        /**
         * @brief 将波形整体缩放到指定峰值。
         * @param w 源波形。
         * @param target 目标峰值，取值 0.0 至 32767.0。
         * @return 缩放后的波形。
         * @throws ValueError 目标峰值超出范围。
         * @note 若源峰值为 0，则原样返回。
         */
        inline Wave minimize(const Wave &w, double target = 32767.0) {
            if (target < 0.0 || target > 32767.0)
                throw ValueError("Target Peak Must Be Between 0 and 32767");
            const double peak = peak_of(w);
            if (peak == 0.0) return w;
            const double k = target / peak;
            Wave         out;
            out.sample_rate = w.sample_rate;
            out.channels    = w.channels;
            out.pcm.resize(w.pcm.size());
            const size_t count = w.pcm.size() / 2;
            for (size_t i = 0; i < count; i++)
                detail::set16(out.pcm,
                    i,
                    detail::quant16(
                        static_cast<double>(detail::get16(w.pcm, i)) * k));
            return out;
        }

        /**
         * @brief 将波形峰值归一化到指定分贝值。
         * @param w 源波形。
         * @param db 目标峰值(dBFS)，默认 0.0 即满刻度。
         * @return 归一化后的波形。
         * @note 若源峰值为 0，则原样返回。
         */
        inline Wave normalize(const Wave &w, double db = 0.0) {
            const double target = 32767.0 * std::pow(10.0, db / 20.0);
            return minimize(w, target);
        }

        /**
         * @brief 将波形整体乘以线性增益。
         * @param w 源波形。
         * @param gain 线性增益倍率。
         * @return 增益后的波形。
         * @note 结果直接限幅到 16-bit，可能产生硬削波。
         */
        inline Wave amplify(const Wave &w, double gain) {
            Wave out;
            out.sample_rate = w.sample_rate;
            out.channels    = w.channels;
            out.pcm.resize(w.pcm.size());
            const size_t count = w.pcm.size() / 2;
            for (size_t i = 0; i < count; i++)
                detail::set16(out.pcm,
                    i,
                    detail::quant16(
                        static_cast<double>(detail::get16(w.pcm, i)) * gain));
            return out;
        }

        /**
         * @brief 将波形样本硬限幅到 16-bit 范围。
         * @param w 源波形。
         * @param low 下限，默认 -32768。
         * @param high 上限，默认 32767。
         * @return 限幅后的波形。
         * @throws ValueError 上下限关系错误或超出 16-bit 范围。
         */
        inline Wave
        clamp(const Wave &w, double low = -32768.0, double high = 32767.0) {
            if (low > high)
                throw ValueError("Low Bound Must Not Exceed High Bound");
            if (low < -32768.0 || high > 32767.0)
                throw ValueError("Bounds Must Be Within 16-bit Range");
            Wave out;
            out.sample_rate = w.sample_rate;
            out.channels    = w.channels;
            out.pcm.resize(w.pcm.size());
            const size_t count = w.pcm.size() / 2;
            for (size_t i = 0; i < count; i++) {
                double v = static_cast<double>(detail::get16(w.pcm, i));
                if (v < low) v = low;
                if (v > high) v = high;
                detail::set16(out.pcm, i, detail::quant16(v));
            }
            return out;
        }

        /**
         * @brief 反相波形，样本取负。
         * @param w 源波形。
         * @return 反相后的波形。
         * @note 样本 -32768 取负后仍为 -32768，这是 16-bit 补码的不对称性。
         */
        inline Wave invert(const Wave &w) {
            Wave out;
            out.sample_rate = w.sample_rate;
            out.channels    = w.channels;
            out.pcm.resize(w.pcm.size());
            const size_t count = w.pcm.size() / 2;
            for (size_t i = 0; i < count; i++) {
                const int v = -static_cast<int>(detail::get16(w.pcm, i));
                detail::set16(out.pcm, i, static_cast<short>(v));
            }
            return out;
        }

        /**
         * @brief 量化到指定目标位深。
         * @param w 源波形。
         * @param bits 目标位深，取值 8、16、24、32。
         * @param dither 是否启用 TPDF 抖动，默认关闭。
         * @return 量化后的波形，仍以 16-bit 存储。
         * @throws ValueError 位深不受支持。
         * @note 目标位深高于 16 时不做提升，直接返回副本。
         *       抖动仅在降位深时有意义。
         */
        inline Wave
        quantize(const Wave &w, uint32_t bits, bool dither = false) {
            if (bits != 8 && bits != 16 && bits != 24 && bits != 32)
                throw ValueError("Unsupported Bit Depth");
            if (bits >= 16) return w;
            const int    levels = 1 << bits;
            const double step   = 65536.0 / levels;
            Wave         out;
            out.sample_rate = w.sample_rate;
            out.channels    = w.channels;
            out.pcm.resize(w.pcm.size());
            const size_t count = w.pcm.size() / 2;
            auto         dis
                = std::uniform_real_distribution<>(-step * 0.5, step * 0.5);
            for (size_t i = 0; i < count; i++) {
                double v = static_cast<double>(detail::get16(w.pcm, i));
                if (dither) {
                    const double d = (dis(thread_rng()) - dis(thread_rng()));
                    v += d;
                }
                v = std::floor(v / step + 0.5) * step;
                detail::set16(out.pcm, i, detail::quant16(v));
            }
            return out;
        }
    }
}
