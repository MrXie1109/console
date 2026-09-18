/**
 * @file base.h
 * @brief WAV 数据结构和参数设置。
 * @details 定义解析后的 WAV 数据结构 Wave，
 *          以及一组全局参数的访问器 (设备名、音量、采样率、声道数、振幅)。
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
#include <string>
#include <vector>

namespace console {
    namespace wav {
        /**
         * @struct Wave
         * @brief 解析后的 WAV 数据。
         */
        struct Wave {
            std::vector<unsigned char> pcm; ///< 交错的 16-bit 小端样本
            uint32_t                   sample_rate = 0; ///< 采样率(Hz)
            uint32_t                   channels    = 0; ///< 声道数

            /**
             * @brief 计算时长。
             * @return 时长(秒)。
             */
            double seconds() const {
                if (channels == 0 || sample_rate == 0) return 0.0;
                return static_cast<double>(pcm.size())
                       / static_cast<double>(channels * 2)
                       / static_cast<double>(sample_rate);
            }
        };

        /**
         * @struct Meta
         * @brief 音频元数据。
         */
        struct Meta {
            uint32_t    sample_rate = 44100;     ///< 采样率(Hz)
            uint32_t    channels    = 1;         ///< 声道数
            std::string device      = "default"; ///< 设备名
            double      volume      = 1.0;       ///< 音量
        };

        /**
         * @struct ADSR
         * @brief 音频 ADSR 效果参数。
         */
        struct ADSR {
            double attack;  ///< 攻击时间(秒)
            double decay;   ///< 衰减时间(秒)
            double sustain; ///< sustain 音量（0.0–1.0 的比例，非时间）
            double release; ///< 释放时间(秒)

            /// @brief 无效果的 ADSR 参数。
            static ADSR none() { return {0.0, 0.0, 1.0, 0.0}; }

            /// @brief 拨弦类。
            static ADSR pluck() { return {0.01, 0.15, 0.1, 0.1}; }

            /// @brief 贝斯类。
            static ADSR bass() { return {0.005, 0.25, 0.6, 0.1}; }

            /// @brief 主音类。
            static ADSR lead() { return {0.02, 0.3, 0.8, 0.3}; }

            /// @brief 铺底类。
            static ADSR pad() { return {1.0, 1.5, 0.9, 3.0}; }

            /// @brief 环境氛围类。
            static ADSR ambient() { return {3.0, 2.0, 0.85, 5.0}; }

            /// @brief 打击乐类。
            static ADSR percussion() { return {0.001, 0.1, 0.0, 0.05}; }

            /// @brief 管风琴类。
            static ADSR organ() { return {0.005, 0.0, 1.0, 0.05}; }
        };

        /*
         * @brief 默认音频元数据。
         * @return 默认音频元数据的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::meta() = { 48000, 2, "hw:0,0", 0.5 };`
         */
        inline Meta &meta() {
            static Meta instance;
            return instance;
        }

        /**
         * @brief 默认音频 ADSR 包络参数。
         * @return 默认 ADSR 参数的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::adsr() = console::wav::ADSR::pad();`
         */
        inline ADSR &adsr() {
            static ADSR instance = {0.02, 0.0, 1.0, 0.02};
            return instance;
        }

        /**
         * @brief 默认音频设备名。
         * @return 默认设备名的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::device() = "hw:0,0";`
         */
        inline std::string &device() {
            return meta().device;
        }

        /**
         * @brief 默认音量倍率。
         * @return 默认音量倍率的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::volume() = 0.5;`
         */
        inline double &volume() {
            return meta().volume;
        }

        /**
         * @brief 默认采样率(Hz)。
         * @return 默认采样率的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::sample_rate() = 48000;`
         */
        inline uint32_t &sample_rate() {
            return meta().sample_rate;
        }

        /**
         * @brief 默认声道数。
         * @return 默认声道数的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::channels() = 2;`
         */
        inline uint32_t &channels() {
            return meta().channels;
        }

        /**
         * @brief 默认振幅。
         * @return 默认振幅的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::amplitude() = 0.8;`
         */
        inline double &amplitude() {
            static double instance = 0.5;
            return instance;
        }

        /**
         * @brief 默认 attack。
         * @return 默认 attack 的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::attack() = 0.02;`
         */
        inline double &attack() {
            return adsr().attack;
        }

        /**
         * @brief 默认 decay。
         * @return 默认 decay 的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::decay() = 1;`
         */
        inline double &decay() {
            return adsr().decay;
        }

        /**
         * @brief 默认 sustain。
         * @return 默认 sustain 的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::sustain() = 0.8;`
         */
        inline double &sustain() {
            return adsr().sustain;
        }

        /**
         * @brief 默认 release。
         * @return 默认 release 的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::release() = 0.5;`
         */
        inline double &release() {
            return adsr().release;
        }
    }
}
