/**
 * @file wav/maker.h
 * @brief WAV 波形文件生成库。
 * @details 生成未压缩的 16-bit PCM WAV 文件，并与 `wav/player.h` 的
 *          `Wave` 结构兼容。写入的小端字节序与平台无关。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 生成格式与 `player.h` 解码格式一致，已互相验证。
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
#include <string>
#include <vector>

#include "../core/csexc.h"
#include "../io/file.h"
#include "../util/time.h"
#include "./base.h"

namespace console {
    /**
     * @brief WAV 生成相关接口。
     */
    namespace wav {
        /**
         * @brief 科学音高记号对应的频率(Hz)。
         * @details 以 A4 = 440 Hz 为基准，采用十二平均律。
         *          升音用 s 表示，例如升 C 记作 Cs4。
         *          八度编号越小频率越低，C4 为中央 C。
         */
        namespace hz {
            static constexpr double C0  = 16.3516;
            static constexpr double Cs0 = 17.3239;
            static constexpr double D0  = 18.3540;
            static constexpr double Ds0 = 19.4454;
            static constexpr double E0  = 20.6017;
            static constexpr double F0  = 21.8268;
            static constexpr double Fs0 = 23.1247;
            static constexpr double G0  = 24.4997;
            static constexpr double Gs0 = 25.9565;
            static constexpr double A0  = 27.5000;
            static constexpr double As0 = 29.1352;
            static constexpr double B0_ = 30.8677; // 沟槽的 termios.h！

            static constexpr double C1  = 32.7032;
            static constexpr double Cs1 = 34.6478;
            static constexpr double D1  = 36.7081;
            static constexpr double Ds1 = 38.8909;
            static constexpr double E1  = 41.2034;
            static constexpr double F1  = 43.6535;
            static constexpr double Fs1 = 46.2493;
            static constexpr double G1  = 48.9994;
            static constexpr double Gs1 = 51.9131;
            static constexpr double A1  = 55.0000;
            static constexpr double As1 = 58.2705;
            static constexpr double B1  = 61.7354;

            static constexpr double C2  = 65.4064;
            static constexpr double Cs2 = 69.2957;
            static constexpr double D2  = 73.4162;
            static constexpr double Ds2 = 77.7817;
            static constexpr double E2  = 82.4069;
            static constexpr double F2  = 87.3071;
            static constexpr double Fs2 = 92.4986;
            static constexpr double G2  = 97.9989;
            static constexpr double Gs2 = 103.8262;
            static constexpr double A2  = 110.0000;
            static constexpr double As2 = 116.5409;
            static constexpr double B2  = 123.4708;

            static constexpr double C3  = 130.8128;
            static constexpr double Cs3 = 138.5913;
            static constexpr double D3  = 146.8324;
            static constexpr double Ds3 = 155.5635;
            static constexpr double E3  = 164.8138;
            static constexpr double F3  = 174.6141;
            static constexpr double Fs3 = 184.9972;
            static constexpr double G3  = 195.9977;
            static constexpr double Gs3 = 207.6523;
            static constexpr double A3  = 220.0000;
            static constexpr double As3 = 233.0819;
            static constexpr double B3  = 246.9417;

            static constexpr double C4  = 261.6256;
            static constexpr double Cs4 = 277.1826;
            static constexpr double D4  = 293.6648;
            static constexpr double Ds4 = 311.1270;
            static constexpr double E4  = 329.6276;
            static constexpr double F4  = 349.2282;
            static constexpr double Fs4 = 369.9944;
            static constexpr double G4  = 391.9954;
            static constexpr double Gs4 = 415.3047;
            static constexpr double A4  = 440.0000;
            static constexpr double As4 = 466.1638;
            static constexpr double B4  = 493.8833;

            static constexpr double C5  = 523.2511;
            static constexpr double Cs5 = 554.3653;
            static constexpr double D5  = 587.3295;
            static constexpr double Ds5 = 622.2540;
            static constexpr double E5  = 659.2551;
            static constexpr double F5  = 698.4565;
            static constexpr double Fs5 = 739.9888;
            static constexpr double G5  = 783.9909;
            static constexpr double Gs5 = 830.6094;
            static constexpr double A5  = 880.0000;
            static constexpr double As5 = 932.3275;
            static constexpr double B5  = 987.7666;

            static constexpr double C6  = 1046.5023;
            static constexpr double Cs6 = 1108.7305;
            static constexpr double D6  = 1174.6591;
            static constexpr double Ds6 = 1244.5079;
            static constexpr double E6  = 1318.5102;
            static constexpr double F6  = 1396.9129;
            static constexpr double Fs6 = 1479.9777;
            static constexpr double G6  = 1567.9817;
            static constexpr double Gs6 = 1661.2188;
            static constexpr double A6  = 1760.0000;
            static constexpr double As6 = 1864.6550;
            static constexpr double B6  = 1975.5332;

            static constexpr double C7  = 2093.0045;
            static constexpr double Cs7 = 2217.4610;
            static constexpr double D7  = 2349.3181;
            static constexpr double Ds7 = 2489.0159;
            static constexpr double E7  = 2637.0205;
            static constexpr double F7  = 2793.8259;
            static constexpr double Fs7 = 2959.9554;
            static constexpr double G7  = 3135.9635;
            static constexpr double Gs7 = 3322.4376;
            static constexpr double A7  = 3520.0000;
            static constexpr double As7 = 3729.3101;
            static constexpr double B7  = 3951.0664;

            static constexpr double C8  = 4186.0090;
            static constexpr double Cs8 = 4434.9221;
            static constexpr double D8  = 4698.6363;
            static constexpr double Ds8 = 4978.0317;
            static constexpr double E8  = 5274.0409;
            static constexpr double F8  = 5587.6517;
            static constexpr double Fs8 = 5919.9108;
            static constexpr double G8  = 6271.9270;
            static constexpr double Gs8 = 6644.8752;
            static constexpr double A8  = 7040.0000;
            static constexpr double As8 = 7458.6202;
            static constexpr double B8  = 7902.1328;
        }

        /**
         * @brief 圆周率。
         */
        static constexpr double k_pi = 3.14159265358979323846;

        /**
         * @brief 写入小端 16 位整数。
         * @param out 输出缓冲区。
         * @param v 待写入的值。
         */
        inline void wr_u16(std::vector<unsigned char> &out, uint16_t v) {
            out.push_back(static_cast<unsigned char>(v & 0xFF));
            out.push_back(static_cast<unsigned char>(v >> 8 & 0xFF));
        }

        /**
         * @brief 写入小端 32 位整数。
         * @param out 输出缓冲区。
         * @param v 待写入的值。
         */
        inline void wr_u32(std::vector<unsigned char> &out, uint32_t v) {
            out.push_back(static_cast<unsigned char>(v & 0xFF));
            out.push_back(static_cast<unsigned char>(v >> 8 & 0xFF));
            out.push_back(static_cast<unsigned char>(v >> 16 & 0xFF));
            out.push_back(static_cast<unsigned char>(v >> 24 & 0xFF));
        }

        /**
         * @brief 写入四字符块标记。
         * @param out 输出缓冲区。
         * @param tag 块标记。
         */
        inline void wr_tag(std::vector<unsigned char> &out, const char *tag) {
            out.insert(out.end(), tag, tag + 4);
        }

        /**
         * @brief 将波形数据编码为 WAV 字节流。
         * @param w 波形数据。
         * @return WAV 文件内容。
         * @throws ValueError 声道数或采样率为 0。
         */
        inline std::vector<unsigned char> encode(const Wave &w) {
            if (w.channels == 0 || w.sample_rate == 0)
                throw ValueError("Invalid Channel Count or Sample Rate");
            const uint32_t frame = w.channels * 2;
            const uint32_t data
                = static_cast<uint32_t>(w.pcm.size() / frame * frame);
            std::vector<unsigned char> info_data;
            auto add_info = [&](const char *tag, const std::string &value) {
                wr_tag(info_data, tag);
                uint32_t len = static_cast<uint32_t>(value.size());
                wr_u32(info_data, len);
                info_data.insert(info_data.end(), value.begin(), value.end());
                if (len & 1) info_data.push_back(0);
            };
            add_info("ISFT", "console::wav");
            add_info(
                "ICMT", "Generated by the wav/maker.h of the Console Library");
            add_info("ICRD", console::datetime("%Y-%m-%d"));
            const uint32_t list_size
                = 4 + static_cast<uint32_t>(info_data.size());
            const uint32_t             list_total = 8 + list_size;
            std::vector<unsigned char> out;
            out.reserve(44 + list_total + data);
            wr_tag(out, "RIFF");
            wr_u32(out, static_cast<uint32_t>(36 + list_total + data));
            wr_tag(out, "WAVE");
            wr_tag(out, "fmt ");
            wr_u32(out, 16);
            wr_u16(out, 1);
            wr_u16(out, static_cast<uint16_t>(w.channels));
            wr_u32(out, w.sample_rate);
            wr_u32(out, w.sample_rate * frame);
            wr_u16(out, static_cast<uint16_t>(frame));
            wr_u16(out, 16);
            wr_tag(out, "LIST");
            wr_u32(out, list_size);
            wr_tag(out, "INFO");
            out.insert(out.end(), info_data.begin(), info_data.end());
            wr_tag(out, "data");
            wr_u32(out, data);
            out.insert(out.end(), w.pcm.begin(), w.pcm.begin() + data);
            return out;
        }

        /**
         * @brief 保存波形数据为 WAV 文件。
         * @param path 文件路径。
         * @param w 波形数据。
         * @throws FileError 文件无法写入。
         * @throws ValueError 声道数或采样率为 0。
         */
        inline void save(const Path &path, const Wave &w) {
            path.write_binary(encode(w));
        }

        /**
         * @brief 保存波形数据为 WAV 文件。
         * @param w 波形数据。
         * @param path 文件路径。
         * @throws FileError 文件无法写入。
         * @throws ValueError 声道数或采样率为 0。
         */
        inline void save(const Wave &w, const Path &path) {
            path.write_binary(encode(w));
        }

        /**
         * @brief 从 16 位样本构造波形数据。
         * @param samples 交错排列的样本。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @return 波形数据。
         * @throws ValueError 声道数或采样率为 0。
         */
        inline Wave make(const std::vector<short> &samples,
            uint32_t                               sample_rate_,
            uint32_t                               channels_ = channels()) {
            if (channels_ == 0 || sample_rate_ == 0)
                throw ValueError("Invalid Channel Count or Sample Rate");
            Wave w;
            w.sample_rate = sample_rate_;
            w.channels    = channels_;
            w.pcm.resize(samples.size() * 2);
            for (size_t i = 0; i < samples.size(); i++) {
                w.pcm[i * 2] = static_cast<unsigned char>(samples[i] & 0xFF);
                w.pcm[i * 2 + 1]
                    = static_cast<unsigned char>(samples[i] >> 8 & 0xFF);
            }
            const size_t frame = channels_ * 2;
            w.pcm.resize(w.pcm.size() / frame * frame);
            return w;
        }

        /**
         * @brief 校验波形生成参数。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @param seconds 时长(秒)。
         * @param amplitude_ 振幅。
         * @throws ValueError 参数超出范围。
         */
        inline void check_wave(uint32_t sample_rate_,
            uint32_t                    channels_,
            double                      seconds,
            double                      amplitude_) {
            if (sample_rate_ == 0 || channels_ == 0)
                throw ValueError("Invalid Channel Count or Sample Rate");
            if (seconds < 0.0)
                throw ValueError("Duration Must Not Be Negative");
            if (amplitude_ < 0.0 || amplitude_ > 1.0)
                throw ValueError("Amplitude Must Be Between 0.0 and 1.0");
        }

        /**
         * @brief 计算淡入淡出包络。
         * @param i 样本下标。
         * @param count 总样本数。
         * @param fade 淡变长度(样本数)。
         * @return 包络值，取值 0.0 至 1.0。
         */
        inline double fade_env(size_t i, size_t count, size_t fade) {
            if (fade == 0 || count <= 2 * fade) return 1.0;
            if (i < fade) return static_cast<double>(i) / fade;
            if (i >= count - fade) return static_cast<double>(count - i) / fade;
            return 1.0;
        }

        /**
         * @brief 将单个样本值写入所有声道。
         * @param w 目标波形。
         * @param i 样本下标。
         * @param v 取值 -1.0 至 1.0 的样本值。
         */
        inline void put_sample(Wave &w, size_t i, double v) {
            if (v > 1.0) v = 1.0;
            if (v < -1.0) v = -1.0;
            const short s = static_cast<short>(v * 32767.0);
            for (uint32_t c = 0; c < w.channels; c++) {
                w.pcm[(i * w.channels + c) * 2]
                    = static_cast<unsigned char>(s & 0xFF);
                w.pcm[(i * w.channels + c) * 2 + 1]
                    = static_cast<unsigned char>(s >> 8 & 0xFF);
            }
        }

        /**
         * @brief 创建空波形。
         * @param seconds 时长(秒)。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @return 已分配未填充的波形数据。
         */
        inline Wave
        alloc_wave(double seconds, uint32_t sample_rate_, uint32_t channels_) {
            Wave w;
            w.sample_rate = sample_rate_;
            w.channels    = channels_;
            w.pcm.resize(
                static_cast<size_t>(sample_rate_ * seconds) * channels_ * 2);
            return w;
        }

        /**
         * @brief 生成正弦波。
         * @param freq 频率(Hz)。
         * @param seconds 时长(秒)。
         * @param amplitude_ 振幅，取值 0.0 至 1.0。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @return 波形数据。
         * @throws ValueError 参数超出范围。
         */
        inline Wave tone(double freq,
            double              seconds,
            double              amplitude_   = amplitude(),
            uint32_t            sample_rate_ = sample_rate(),
            uint32_t            channels_    = channels()) {
            check_wave(sample_rate_, channels_, seconds, amplitude_);
            if (freq < 0.0) throw ValueError("Frequency Must Not Be Negative");
            Wave         w     = alloc_wave(seconds, sample_rate_, channels_);
            const size_t count = w.pcm.size() / (channels_ * 2);
            const size_t fade  = sample_rate_ / 50;
            for (size_t i = 0; i < count; i++) {
                const double t = static_cast<double>(i) / sample_rate_;
                put_sample(w,
                    i,
                    std::sin(2.0 * k_pi * freq * t) * amplitude_
                        * fade_env(i, count, fade));
            }
            return w;
        }

        /**
         * @brief 生成方波。
         * @param freq 频率(Hz)。
         * @param seconds 时长(秒)。
         * @param amplitude_ 振幅，取值 0.0 至 1.0。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @return 波形数据。
         * @throws ValueError 参数超出范围。
         * @note 谐波丰富，振幅过大易削波，建议不超过 0.5。
         */
        inline Wave square(double freq,
            double                seconds,
            double                amplitude_   = amplitude(),
            uint32_t              sample_rate_ = sample_rate(),
            uint32_t              channels_    = channels()) {
            check_wave(sample_rate_, channels_, seconds, amplitude_);
            if (freq < 0.0) throw ValueError("Frequency Must Not Be Negative");
            Wave         w     = alloc_wave(seconds, sample_rate_, channels_);
            const size_t count = w.pcm.size() / (channels_ * 2);
            const size_t fade  = sample_rate_ / 50;
            for (size_t i = 0; i < count; i++) {
                const double t  = static_cast<double>(i) / sample_rate_;
                const double ph = std::fmod(freq * t, 1.0);
                put_sample(w,
                    i,
                    (ph < 0.5 ? 1.0 : -1.0) * amplitude_
                        * fade_env(i, count, fade));
            }
            return w;
        }

        /**
         * @brief 生成锯齿波。
         * @param freq 频率(Hz)。
         * @param seconds 时长(秒)。
         * @param amplitude_ 振幅，取值 0.0 至 1.0。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @return 波形数据。
         * @throws ValueError 参数超出范围。
         */
        inline Wave sawtooth(double freq,
            double                  seconds,
            double                  amplitude_   = amplitude(),
            uint32_t                sample_rate_ = sample_rate(),
            uint32_t                channels_    = channels()) {
            check_wave(sample_rate_, channels_, seconds, amplitude_);
            if (freq < 0.0) throw ValueError("Frequency Must Not Be Negative");
            Wave         w     = alloc_wave(seconds, sample_rate_, channels_);
            const size_t count = w.pcm.size() / (channels_ * 2);
            const size_t fade  = sample_rate_ / 50;
            for (size_t i = 0; i < count; i++) {
                const double t  = static_cast<double>(i) / sample_rate_;
                const double ph = std::fmod(freq * t, 1.0);
                put_sample(w,
                    i,
                    (2.0 * ph - 1.0) * amplitude_ * fade_env(i, count, fade));
            }
            return w;
        }

        /**
         * @brief 生成三角波。
         * @param freq 频率(Hz)。
         * @param seconds 时长(秒)。
         * @param amplitude_ 振幅，取值 0.0 至 1.0。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @return 波形数据。
         * @throws ValueError 参数超出范围。
         */
        inline Wave triangle(double freq,
            double                  seconds,
            double                  amplitude_   = amplitude(),
            uint32_t                sample_rate_ = sample_rate(),
            uint32_t                channels_    = channels()) {
            check_wave(sample_rate_, channels_, seconds, amplitude_);
            if (freq < 0.0) throw ValueError("Frequency Must Not Be Negative");
            Wave         w     = alloc_wave(seconds, sample_rate_, channels_);
            const size_t count = w.pcm.size() / (channels_ * 2);
            const size_t fade  = sample_rate_ / 50;
            for (size_t i = 0; i < count; i++) {
                const double t  = static_cast<double>(i) / sample_rate_;
                const double ph = std::fmod(freq * t, 1.0);
                const double v  = ph < 0.5 ? 4.0 * ph - 1.0 : 3.0 - 4.0 * ph;
                put_sample(w, i, v * amplitude_ * fade_env(i, count, fade));
            }
            return w;
        }

        /**
         * @brief 生成脉冲波。
         * @param freq 频率(Hz)。
         * @param duty 占空比，取值 0.0 至 1.0。
         * @param seconds 时长(秒)。
         * @param amplitude_ 振幅，取值 0.0 至 1.0。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @return 波形数据。
         * @throws ValueError 参数超出范围。
         */
        inline Wave pulse(double freq,
            double               duty,
            double               seconds,
            double               amplitude_   = amplitude(),
            uint32_t             sample_rate_ = sample_rate(),
            uint32_t             channels_    = channels()) {
            check_wave(sample_rate_, channels_, seconds, amplitude_);
            if (freq < 0.0) throw ValueError("Frequency Must Not Be Negative");
            if (duty <= 0.0 || duty >= 1.0)
                throw ValueError("Duty Must Be Between 0.0 and 1.0");
            Wave         w     = alloc_wave(seconds, sample_rate_, channels_);
            const size_t count = w.pcm.size() / (channels_ * 2);
            const size_t fade  = sample_rate_ / 50;
            for (size_t i = 0; i < count; i++) {
                const double t  = static_cast<double>(i) / sample_rate_;
                const double ph = std::fmod(freq * t, 1.0);
                put_sample(w,
                    i,
                    (ph < duty ? 1.0 : -1.0) * amplitude_
                        * fade_env(i, count, fade));
            }
            return w;
        }

        /**
         * @brief 生成扫频信号。
         * @param start 起始频率(Hz)。
         * @param end 终止频率(Hz)。
         * @param seconds 时长(秒)。
         * @param amplitude_ 振幅，取值 0.0 至 1.0。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @return 波形数据。
         * @throws ValueError 参数超出范围。
         * @note 频率按指数变化，各时刻倍频程增量相同。
         */
        inline Wave chirp(double start,
            double               end,
            double               seconds,
            double               amplitude_   = amplitude(),
            uint32_t             sample_rate_ = sample_rate(),
            uint32_t             channels_    = channels()) {
            check_wave(sample_rate_, channels_, seconds, amplitude_);
            if (start <= 0.0 || end <= 0.0)
                throw ValueError("Frequency Must Be Positive");
            Wave         w     = alloc_wave(seconds, sample_rate_, channels_);
            const size_t count = w.pcm.size() / (channels_ * 2);
            const size_t fade  = sample_rate_ / 50;
            const double ratio = end / start;
            double       phase = 0.0;
            for (size_t i = 0; i < count; i++) {
                const double u = count > 1
                                     ? static_cast<double>(i)
                                           / static_cast<double>(count - 1)
                                     : 0.0;
                const double f = start * std::pow(ratio, u);
                put_sample(w,
                    i,
                    std::sin(phase) * amplitude_ * fade_env(i, count, fade));
                phase += 2.0 * k_pi * f / sample_rate_;
            }
            return w;
        }

        /**
         * @brief 生成调幅信号。
         * @param freq 载波频率(Hz)。
         * @param mod_freq 调制频率(Hz)。
         * @param depth 调制深度，取值 0.0 至 1.0。
         * @param seconds 时长(秒)。
         * @param amplitude_ 振幅，取值 0.0 至 1.0。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @return 波形数据。
         * @throws ValueError 参数超出范围。
         */
        inline Wave am(double freq,
            double            mod_freq,
            double            depth,
            double            seconds,
            double            amplitude_   = amplitude(),
            uint32_t          sample_rate_ = sample_rate(),
            uint32_t          channels_    = channels()) {
            check_wave(sample_rate_, channels_, seconds, amplitude_);
            if (freq < 0.0 || mod_freq < 0.0)
                throw ValueError("Frequency Must Not Be Negative");
            if (depth < 0.0 || depth > 1.0)
                throw ValueError("Depth Must Be Between 0.0 and 1.0");
            Wave         w     = alloc_wave(seconds, sample_rate_, channels_);
            const size_t count = w.pcm.size() / (channels_ * 2);
            const size_t fade  = sample_rate_ / 50;
            for (size_t i = 0; i < count; i++) {
                const double t = static_cast<double>(i) / sample_rate_;
                const double env
                    = 1.0 - depth
                      + depth * 0.5
                            * (1.0 + std::sin(2.0 * k_pi * mod_freq * t));
                put_sample(w,
                    i,
                    std::sin(2.0 * k_pi * freq * t) * amplitude_ * env
                        * fade_env(i, count, fade));
            }
            return w;
        }

        /**
         * @brief 生成调频信号。
         * @param carrier 载波频率(Hz)。
         * @param mod_freq 调制频率(Hz)。
         * @param index 调制指数，即最大频偏与调制频率之比。
         * @param seconds 时长(秒)。
         * @param amplitude_ 振幅，取值 0.0 至 1.0。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @return 波形数据。
         * @throws ValueError 参数超出范围。
         */
        inline Wave fm(double carrier,
            double            mod_freq,
            double            index,
            double            seconds,
            double            amplitude_   = amplitude(),
            uint32_t          sample_rate_ = sample_rate(),
            uint32_t          channels_    = channels()) {
            check_wave(sample_rate_, channels_, seconds, amplitude_);
            if (carrier < 0.0 || mod_freq < 0.0)
                throw ValueError("Frequency Must Not Be Negative");
            if (index < 0.0) throw ValueError("Index Must Not Be Negative");
            Wave         w     = alloc_wave(seconds, sample_rate_, channels_);
            const size_t count = w.pcm.size() / (channels_ * 2);
            const size_t fade  = sample_rate_ / 50;
            for (size_t i = 0; i < count; i++) {
                const double t  = static_cast<double>(i) / sample_rate_;
                const double ph = 2.0 * k_pi * carrier * t
                                  + index * std::sin(2.0 * k_pi * mod_freq * t);
                put_sample(
                    w, i, std::sin(ph) * amplitude_ * fade_env(i, count, fade));
            }
            return w;
        }

        /**
         * @brief 生成谐波叠加信号。
         * @param freq 基频(Hz)。
         * @param harmonics 各次谐波的相对幅度，下标 0 为基频。
         * @param seconds 时长(秒)。
         * @param amplitude_ 振幅，取值 0.0 至 1.0。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @return 波形数据。
         * @throws ValueError 参数超出范围，或谐波列表为空。
         * @note 各谐波按幅度之和归一化，因此不会削波。
         */
        inline Wave harmonics(double   freq,
            const std::vector<double> &harmonics,
            double                     seconds,
            double                     amplitude_   = amplitude(),
            uint32_t                   sample_rate_ = sample_rate(),
            uint32_t                   channels_    = channels()) {
            check_wave(sample_rate_, channels_, seconds, amplitude_);
            if (freq < 0.0) throw ValueError("Frequency Must Not Be Negative");
            if (harmonics.empty())
                throw ValueError("Harmonics Must Not Be Empty");
            double total = 0.0;
            for (size_t h = 0; h < harmonics.size(); h++)
                total += std::fabs(harmonics[h]);
            if (total == 0.0)
                throw ValueError("Harmonics Must Not Be All Zero");
            Wave         w     = alloc_wave(seconds, sample_rate_, channels_);
            const size_t count = w.pcm.size() / (channels_ * 2);
            const size_t fade  = sample_rate_ / 50;
            for (size_t i = 0; i < count; i++) {
                const double t = static_cast<double>(i) / sample_rate_;
                double       v = 0.0;
                for (size_t h = 0; h < harmonics.size(); h++)
                    v += harmonics[h]
                         * std::sin(2.0 * k_pi * freq * (h + 1) * t);
                put_sample(
                    w, i, v / total * amplitude_ * fade_env(i, count, fade));
            }
            return w;
        }

        /**
         * @brief 生成静音。
         * @param seconds 时长(秒)。
         * @param sample_rate_ 采样率(Hz)。
         * @param channels_ 声道数。
         * @return 波形数据。
         * @throws ValueError 声道数或采样率为 0。
         */
        inline Wave silence(double seconds,
            uint32_t               sample_rate_ = sample_rate(),
            uint32_t               channels_    = channels()) {
            if (sample_rate_ == 0 || channels_ == 0)
                throw ValueError("Invalid Channel Count or Sample Rate");
            if (seconds < 0.0)
                throw ValueError("Duration Must Not Be Negative");
            Wave w;
            w.sample_rate = sample_rate_;
            w.channels    = channels_;
            w.pcm.assign(
                static_cast<size_t>(sample_rate_ * seconds) * channels_ * 2, 0);
            return w;
        }

        /**
         * @brief 拼接多段波形。
         * @param parts 待拼接的波形。
         * @return 拼接结果。
         * @throws ValueError 格式不一致，或列表为空。
         */
        inline Wave join(const std::vector<Wave> &parts) {
            if (parts.empty()) throw ValueError("Cannot Join an Empty List");
            Wave w;
            w.sample_rate = parts[0].sample_rate;
            w.channels    = parts[0].channels;
            for (size_t i = 1; i < parts.size(); i++) {
                if (parts[i].sample_rate != w.sample_rate
                    || parts[i].channels != w.channels)
                    throw ValueError("All Parts Must Share the Same Format");
            }
            for (size_t i = 0; i < parts.size(); i++)
                w.pcm.insert(
                    w.pcm.end(), parts[i].pcm.begin(), parts[i].pcm.end());
            return w;
        }
    }
}
