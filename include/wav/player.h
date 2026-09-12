/**
 * @file wav/player.h
 * @brief WAV 波形文件播放库。
 * @details 解析并播放未压缩的 16-bit PCM WAV 文件。
 *          支持 Windows(waveOut)、macOS(AudioQueue)、Linux(ALSA) 后端。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @warning 由于我的电脑限制，Windows 与 macOS 后端均未实测，
 *          仅通过语法检查。Linux 后端已实测。
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
#include <cstring>
#include <string>
#include <vector>

#include "../core/csexc.h"
#include "../io/file.h"

/** @cond INTERNAL */
#if defined(_WIN32)
#define WAVPLAY_PLATFORM_WINDOWS 1
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#elif defined(__APPLE__)
#define WAVPLAY_PLATFORM_MACOS 1
#include <dlfcn.h>
#else
#define WAVPLAY_PLATFORM_LINUX 1
#include <dlfcn.h>
#endif
/** @endcond */

namespace console {
    /**
     * @brief WAV 播放相关接口。
     */
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
         * @brief 读取小端 16 位无符号整数。
         * @param p 缓冲区。
         * @return 读取结果。
         */
        inline uint16_t rd_u16(const unsigned char *p) {
            return static_cast<uint16_t>(
                static_cast<uint16_t>(p[0]) | static_cast<uint16_t>(p[1]) << 8);
        }

        /**
         * @brief 读取小端 32 位无符号整数。
         * @param p 缓冲区。
         * @return 读取结果。
         */
        inline uint32_t rd_u32(const unsigned char *p) {
            return static_cast<uint32_t>(p[0])
                   | static_cast<uint32_t>(p[1]) << 8
                   | static_cast<uint32_t>(p[2]) << 16
                   | static_cast<uint32_t>(p[3]) << 24;
        }

#if defined(WAVPLAY_PLATFORM_WINDOWS)
        /** @cond INTERNAL */
        namespace detail {
            /**
             * @struct WaveHeader
             * @brief WAVEHDR 的公开布局。
             */
#pragma pack(push, 1)
            struct WaveHeader {
                char       *lpData;          ///< 数据指针
                uint32_t    dwBufferLength;  ///< 数据长度
                uint32_t    dwBytesRecorded; ///< 已录制字节数
                uint32_t    dwUser;          ///< 用户数据
                uint32_t    dwFlags;         ///< 标志位
                uint32_t    dwLoops;         ///< 循环次数
                WaveHeader *lpNext;          ///< 下一个头
                uint32_t    reserved;        ///< 保留
            };
#pragma pack(pop)
            // Win64 下 WAVEHDR 以 1 字节对齐，为 40 字节。
            // 不指定对齐会得到 48 字节，与 Windows ABI 不符。
            static_assert(sizeof(WaveHeader) == 40,
                "WAVEHDR layout does not match the expected size, "
                "please verify against the Windows SDK");

            /**
             * @struct Winmm
             * @brief winmm.dll 的函数表。
             */
            struct Winmm {
                uint32_t(WINAPI *waveOutOpen)(void **,
                    uint32_t,
                    const void *,
                    uint32_t,
                    uint32_t,
                    uint32_t);
                uint32_t(WINAPI *waveOutPrepareHeader)(
                    void *, void *, uint32_t);
                uint32_t(WINAPI *waveOutWrite)(void *, void *, uint32_t);
                uint32_t(WINAPI *waveOutUnprepareHeader)(
                    void *, void *, uint32_t);
                uint32_t(WINAPI *waveOutClose)(void *);
                uint32_t(WINAPI *waveOutGetNumDevs)(void);
                uint32_t(WINAPI *waveOutGetDevCapsA)(
                    uint32_t, void *, uint32_t);
            };

            /**
             * @brief 加载 winmm.dll。
             * @return 函数表。
             * @throws WavError 无法加载或缺少符号。
             */
            inline Winmm &winmm() {
                static Winmm w{};
                static bool  loaded = false;
                if (!loaded) {
                    void *h = LoadLibraryA("winmm.dll");
                    if (!h) throw WavError("Cannot Load winmm.dll");
#define WAVPLAY_WSYM(field, name)                                              \
    w.field = reinterpret_cast<decltype(w.field)>(                             \
        GetProcAddress(static_cast<void *>(h), name));                         \
    if (!w.field) throw WavError("winmm.dll Is Missing Symbol " name)
                    WAVPLAY_WSYM(waveOutOpen, "waveOutOpen");
                    WAVPLAY_WSYM(waveOutPrepareHeader, "waveOutPrepareHeader");
                    WAVPLAY_WSYM(waveOutWrite, "waveOutWrite");
                    WAVPLAY_WSYM(
                        waveOutUnprepareHeader, "waveOutUnprepareHeader");
                    WAVPLAY_WSYM(waveOutClose, "waveOutClose");
                    WAVPLAY_WSYM(waveOutGetNumDevs, "waveOutGetNumDevs");
                    WAVPLAY_WSYM(waveOutGetDevCapsA, "waveOutGetDevCapsA");
#undef WAVPLAY_WSYM
                    loaded = true;
                }
                return w;
            }

            /**
             * @brief 按名称查找播放设备编号。
             * @param device 设备名。
             * @return 设备编号，未找到时返回 WAVE_MAPPER。
             */
            inline uint32_t find_device(const std::string &device) {
                if (device.empty() || device == "default") return WAVE_MAPPER;
                Winmm         &w        = winmm();
                const uint32_t n        = w.waveOutGetNumDevs();
                char           caps[64] = {0};
                for (uint32_t i = 0; i < n; i++) {
                    if (w.waveOutGetDevCapsA(i, caps, sizeof(caps)) != 0)
                        continue;
                    if (device == caps) return i;
                }
                throw WavError("Cannot Find Device \"" + device + "\"");
            }

            /**
             * @class WaveOut
             * @brief waveOut 句柄的 RAII 封装。
             */
            class WaveOut {
                void             *handle_ = nullptr; ///< 句柄
                std::vector<char> header_;           ///< WAVEHDR 存储

            public:
                /**
                 * @brief 打开设备。
                 * @param device 设备名。
                 * @param w 波形数据。
                 * @throws WavError 打开失败。
                 */
                WaveOut(const std::string &device, const Wave &w) {
                    // SDK 中以 1 字节对齐声明，共 18 字节。
#pragma pack(push, 1)
                    struct {
                        uint16_t tag;
                        uint16_t channels;
                        uint32_t rate;
                        uint32_t bps;
                        uint16_t align;
                        uint16_t bits;
                        uint16_t extra;
                    } fmt;
#pragma pack(pop)
                    static_assert(
                        sizeof(fmt) == 18, "WAVEFORMATEX size mismatch");

                    fmt.tag           = 1;
                    fmt.channels      = static_cast<uint16_t>(w.channels);
                    fmt.rate          = w.sample_rate;
                    fmt.align         = static_cast<uint16_t>(w.channels * 2);
                    fmt.bps           = w.sample_rate * fmt.align;
                    fmt.bits          = 16;
                    fmt.extra         = 0;
                    const uint32_t id = find_device(device);
                    if (winmm().waveOutOpen(&handle_, id, &fmt, 0, 0, 0) != 0)
                        throw WavError("Cannot Open Device \"" + device + "\"");
                }

                /**
                 * @brief 关闭设备。
                 */
                ~WaveOut() {
                    if (handle_) winmm().waveOutClose(handle_);
                }

                WaveOut(const WaveOut &)            = delete;
                WaveOut &operator=(const WaveOut &) = delete;

                /**
                 * @brief 获取设备句柄。
                 * @return 句柄。
                 */
                void *handle() const { return handle_; }
            };
        }
        /** @endcond */
#endif

        /**
         * @brief 解析 WAV 文件。
         * @param path 文件路径。
         * @return 解析结果。
         * @throws FileError 文件无法读取。
         * @throws ValueError 文件不是合法的 16-bit PCM WAV。
         */
        inline Wave load(const Path &path) {
            const Path::Bytes buf = path.read_binary();
            if (buf.size() < 44)
                throw ValueError("Not a Valid WAV File: Too Short");
            if (std::memcmp(buf.data(), "RIFF", 4) != 0
                || std::memcmp(buf.data() + 8, "WAVE", 4) != 0)
                throw ValueError("Not a RIFF/WAVE File");
            Wave     out;
            bool     has_fmt  = false;
            bool     has_data = false;
            uint16_t format   = 0;
            uint16_t bits     = 0;
            size_t   pos      = 12;
            while (pos + 8 <= buf.size()) {
                const unsigned char *ck      = buf.data() + pos;
                const uint32_t       ck_size = rd_u32(ck + 4);
                const unsigned char *body    = ck + 8;
                const size_t         avail   = buf.size() - (pos + 8);
                const size_t         n = ck_size < avail ? ck_size : avail;
                if (std::memcmp(ck, "fmt ", 4) == 0 && n >= 16) {
                    format          = rd_u16(body + 0);
                    out.channels    = rd_u16(body + 2);
                    out.sample_rate = rd_u32(body + 4);
                    bits            = rd_u16(body + 14);
                    has_fmt         = true;
                } else if (std::memcmp(ck, "data", 4) == 0) {
                    out.pcm.assign(body, body + n);
                    has_data = true;
                }
                if (has_fmt && has_data) break;
                pos += 8 + ck_size + (ck_size & 1);
            }
            if (!has_fmt) throw ValueError("Missing 'fmt ' Chunk");
            if (!has_data) throw ValueError("Missing 'data' Chunk");
            if (format != 1)
                throw ValueError(
                    "Unsupported Format: Only Uncompressed PCM Is Supported");
            if (bits != 16)
                throw ValueError(
                    "Unsupported Bit Depth: Only 16-bit Is Supported");
            if (out.channels == 0 || out.sample_rate == 0)
                throw ValueError("Invalid Channel Count or Sample Rate");
            const size_t frame = out.channels * 2;
            out.pcm.resize(out.pcm.size() / frame * frame);
            return out;
        }

        /**
         * @brief 按音量缩放 PCM 数据。
         * @param w 源数据。
         * @param volume 线性音量倍率。
         * @return 缩放后的数据。
         */
        inline std::vector<unsigned char>
        volume_of(const Wave &w, double volume) {
            if (volume == 1.0) return w.pcm;
            std::vector<unsigned char> out(w.pcm.size());
            const size_t               count = w.pcm.size() / 2;
            for (size_t i = 0; i < count; i++) {
                const short s = static_cast<short>(
                    static_cast<uint16_t>(w.pcm[i * 2])
                    | static_cast<uint16_t>(w.pcm[i * 2 + 1]) << 8);
                double v = static_cast<double>(s) * volume;
                if (v > 32767.0) v = 32767.0;
                if (v < -32768.0) v = -32768.0;
                const short r  = static_cast<short>(v);
                out[i * 2]     = static_cast<unsigned char>(r & 0xFF);
                out[i * 2 + 1] = static_cast<unsigned char>(r >> 8 & 0xFF);
            }
            return out;
        }

#if defined(WAVPLAY_PLATFORM_MACOS)
        /** @cond INTERNAL */
        namespace detail {
            /// AudioStreamBasicDescription 的公开布局。
            struct StreamFormat {
                double   sampleRate; ///< 采样率
                uint32_t formatID;
                uint32_t formatFlags;
                uint32_t bytesPerPacket;
                uint32_t framesPerPacket;
                uint32_t bytesPerFrame;
                uint32_t channelsPerFrame;
                uint32_t bitsPerChannel;
                uint32_t reserved;
            };
            static_assert(sizeof(StreamFormat) == 40,
                "AudioStreamBasicDescription layout does not match the "
                "expected size");

            /// AudioQueueBuffer 的公开布局。
            struct QueueBuffer {
                uint32_t mAudioDataBytesCapacity; ///< 缓冲区容量(字节)
                void    *mAudioData;              ///< 数据区
                uint32_t mAudioDataByteSize;      ///< 有效字节数
                void    *mUserData;               ///< 用户数据
                uint32_t mPacketDescriptionCapacity; ///< 描述符容量
                void    *mPacketDescriptions;        ///< 描述符
                uint32_t mPacketDescriptionCount;    ///< 描述符数量
            };
            static_assert(
                sizeof(QueueBuffer) == 56, "AudioQueueBuffer layout mismatch");

            /**
             * @struct AudioToolbox
             * @brief AudioToolbox 的函数表。
             */
            struct AudioToolbox {
                int (*AudioQueueNewOutput)(const StreamFormat *,
                    void (*)(void *, void *, void *),
                    void *,
                    void *,
                    void *,
                    uint32_t,
                    void **);
                int (*AudioQueueAllocateBuffer)(void *, uint32_t, void **);
                int (*AudioQueueEnqueueBuffer)(
                    void *, void *, uint32_t, const void *);
                int (*AudioQueueStart)(void *, const void *);
                int (*AudioQueueStop)(void *, unsigned char);
                int (*AudioQueueDispose)(void *, unsigned char);
            };

            /**
             * @brief 加载 AudioToolbox 框架。
             * @return 函数表。
             * @throws WavError 无法加载或缺少符号。
             */
            inline AudioToolbox &atb() {
                static AudioToolbox a{};
                static bool         loaded = false;
                if (!loaded) {
                    void *h = dlopen(
                        "/System/Library/Frameworks/AudioToolbox.framework/"
                        "AudioToolbox",
                        RTLD_NOW | RTLD_GLOBAL);
                    if (!h)
                        throw WavError(std::string("Cannot Load AudioToolbox: ")
                                       + dlerror());
#define WAVPLAY_ASYM(field, name)                                              \
    a.field = reinterpret_cast<decltype(a.field)>(dlsym(h, name));             \
    if (!a.field) throw WavError("AudioToolbox Is Missing Symbol " name)
                    WAVPLAY_ASYM(AudioQueueNewOutput, "AudioQueueNewOutput");
                    WAVPLAY_ASYM(
                        AudioQueueAllocateBuffer, "AudioQueueAllocateBuffer");
                    WAVPLAY_ASYM(
                        AudioQueueEnqueueBuffer, "AudioQueueEnqueueBuffer");
                    WAVPLAY_ASYM(AudioQueueStart, "AudioQueueStart");
                    WAVPLAY_ASYM(AudioQueueStop, "AudioQueueStop");
                    WAVPLAY_ASYM(AudioQueueDispose, "AudioQueueDispose");
#undef WAVPLAY_ASYM
                    loaded = true;
                }
                return a;
            }

            /**
             * @brief 播放完毕的回调。
             * @param user 停止标志。
             * @param queue 队列句柄。
             * @param buffer 缓冲区。
             */
            inline void on_done(void *user, void *queue, void *buffer) {
                (void)queue;
                (void)buffer;
                *static_cast<bool *>(user) = true;
            }

            /**
             * @class Queue
             * @brief AudioQueue 句柄的 RAII 封装。
             */
            class Queue {
                void *handle_ = nullptr; ///< 队列句柄

            public:
                /**
                 * @brief 创建播放队列。
                 * @param w 波形数据。
                 * @param done 播放完毕标志。
                 * @throws WavError 创建失败。
                 */
                Queue(const Wave &w, bool *done) {
                    StreamFormat fmt;
                    fmt.sampleRate       = static_cast<double>(w.sample_rate);
                    fmt.formatID         = 0x6C70636D;
                    fmt.formatFlags      = 0xC;
                    fmt.bytesPerPacket   = w.channels * 2;
                    fmt.framesPerPacket  = 1;
                    fmt.bytesPerFrame    = w.channels * 2;
                    fmt.channelsPerFrame = w.channels;
                    fmt.bitsPerChannel   = 16;
                    fmt.reserved         = 0;
                    if (atb().AudioQueueNewOutput(
                            &fmt, on_done, done, nullptr, nullptr, 0, &handle_)
                            != 0
                        || !handle_)
                        throw WavError("Cannot Create Audio Queue");
                }

                /**
                 * @brief 销毁队列。
                 */
                ~Queue() {
                    if (handle_) atb().AudioQueueDispose(handle_, 1);
                }

                Queue(const Queue &)            = delete;
                Queue &operator=(const Queue &) = delete;

                /**
                 * @brief 获取队列句柄。
                 * @return 句柄。
                 */
                void *handle() const { return handle_; }
            };

            /**
             * @brief 等待播放结束。
             * @param done 播放完毕标志。
             * @details 驱动 CF 运行循环直到回调置位。
             */
            inline void wait_done(bool *done) {
                using RunFn = int (*)(const void *, double, unsigned char);
                void *cf    = dlopen(
                    "/System/Library/Frameworks/CoreFoundation.framework/"
                       "CoreFoundation",
                    RTLD_NOW | RTLD_GLOBAL);
                if (!cf) throw WavError("Cannot Load CoreFoundation");
                RunFn run
                    = reinterpret_cast<RunFn>(dlsym(cf, "CFRunLoopRunInMode"));
                void **mode = reinterpret_cast<void **>(
                    dlsym(cf, "kCFRunLoopDefaultMode"));
                if (!run || !mode)
                    throw WavError(
                        "CoreFoundation Is Missing Required Symbols");
                while (!*done) run(*mode, 0.05, static_cast<unsigned char>(1));
                dlclose(cf);
            }
        }
        /** @endcond */
#endif

#if defined(WAVPLAY_PLATFORM_LINUX)
        /** @cond INTERNAL */
        namespace detail {
            /**
             * @struct Alsa
             * @brief libasound 的函数表。
             */
            struct Alsa {
                int (*pcm_open)(void **, const char *, int, int);
                int (*pcm_close)(void *);
                int (*pcm_hw_params_malloc)(void **);
                void (*pcm_hw_params_free)(void *);
                int (*pcm_hw_params_any)(void *, void *);
                int (*pcm_hw_params_set_access)(void *, void *, int);
                int (*pcm_hw_params_set_format)(void *, void *, int);
                int (*pcm_hw_params_set_channels)(void *, void *, uint32_t);
                int (*pcm_hw_params_set_rate_near)(
                    void *, void *, uint32_t *, int *);
                int (*pcm_hw_params)(void *, void *);
                int (*pcm_prepare)(void *);
                long (*pcm_writei)(void *, const void *, uint32_t);
                int (*pcm_drain)(void *);
                int (*pcm_recover)(void *, int, int);
                const char *(*strerror)(int);
            };

            /**
             * @brief 加载 libasound.so.2。
             * @return 函数表。
             * @throws WavError 无法加载或缺少符号。
             */
            inline Alsa &alsa() {
                static Alsa a{};
                static bool loaded = false;
                if (!loaded) {
                    void *h = dlopen("libasound.so.2", RTLD_NOW | RTLD_GLOBAL);
                    if (!h)
                        throw WavError(
                            std::string("Cannot Load libasound.so.2: ")
                            + dlerror());
#define WAVPLAY_SYM(field, name)                                               \
    a.field = reinterpret_cast<decltype(a.field)>(dlsym(h, name));             \
    if (!a.field) throw WavError("libasound.so.2 Is Missing Symbol " name)
                    WAVPLAY_SYM(pcm_open, "snd_pcm_open");
                    WAVPLAY_SYM(pcm_close, "snd_pcm_close");
                    WAVPLAY_SYM(
                        pcm_hw_params_malloc, "snd_pcm_hw_params_malloc");
                    WAVPLAY_SYM(pcm_hw_params_free, "snd_pcm_hw_params_free");
                    WAVPLAY_SYM(pcm_hw_params_any, "snd_pcm_hw_params_any");
                    WAVPLAY_SYM(pcm_hw_params_set_access,
                        "snd_pcm_hw_params_set_access");
                    WAVPLAY_SYM(pcm_hw_params_set_format,
                        "snd_pcm_hw_params_set_format");
                    WAVPLAY_SYM(pcm_hw_params_set_channels,
                        "snd_pcm_hw_params_set_channels");
                    WAVPLAY_SYM(pcm_hw_params_set_rate_near,
                        "snd_pcm_hw_params_set_rate_near");
                    WAVPLAY_SYM(pcm_hw_params, "snd_pcm_hw_params");
                    WAVPLAY_SYM(pcm_prepare, "snd_pcm_prepare");
                    WAVPLAY_SYM(pcm_writei, "snd_pcm_writei");
                    WAVPLAY_SYM(pcm_drain, "snd_pcm_drain");
                    WAVPLAY_SYM(pcm_recover, "snd_pcm_recover");
                    WAVPLAY_SYM(strerror, "snd_strerror");
#undef WAVPLAY_SYM
                    loaded = true;
                }
                return a;
            }

            /**
             * @class Pcm
             * @brief PCM 句柄的 RAII 封装。
             */
            class Pcm {
                void *handle_ = nullptr; ///< PCM 句柄
                void *params_ = nullptr; ///< 硬件参数

            public:
                /**
                 * @brief 打开设备。
                 * @param device 设备名。
                 * @throws WavError 打开失败。
                 */
                explicit Pcm(const std::string &device) {
                    Alsa &a = alsa();
                    if (a.pcm_open(&handle_, device.c_str(), 0, 0) < 0)
                        throw WavError("Cannot Open Device \"" + device + "\"");
                    a.pcm_hw_params_malloc(&params_);
                }

                /**
                 * @brief 释放句柄。
                 */
                ~Pcm() {
                    Alsa &a = alsa();
                    if (params_) a.pcm_hw_params_free(params_);
                    if (handle_) a.pcm_close(handle_);
                }

                Pcm(const Pcm &)            = delete;
                Pcm &operator=(const Pcm &) = delete;

                /**
                 * @brief 获取 PCM 句柄。
                 * @return 句柄。
                 */
                void *handle() const { return handle_; }

                /**
                 * @brief 获取硬件参数。
                 * @return 硬件参数。
                 */
                void *params() const { return params_; }
            };
        }
        /** @endcond */
#endif

        /**
         * @brief 默认音频设备名。
         * @return 默认设备名的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::device() = "hw:0,0";`
         */
        inline std::string &device() {
            static std::string instance = "default";
            return instance;
        }

        /**
         * @brief 默认音量倍率。
         * @return 默认音量倍率的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::volume() = 0.5;`
         */
        inline double &volume() {
            static double instance = 1.0;
            return instance;
        }

        /**
         * @brief 默认采样率(Hz)。
         * @return 默认采样率的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::sample_rate() = 48000;`
         */
        inline uint32_t &sample_rate() {
            static uint32_t instance = 44100;
            return instance;
        }

        /**
         * @brief 默认声道数。
         * @return 默认声道数的引用。
         * @details 返回引用，因此可通过赋值修改默认值：
         *          `console::wav::channels() = 2;`
         */
        inline uint32_t &channels() {
            static uint32_t instance = 1;
            return instance;
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
         * @brief 播放一段波形数据。
         * @param w 波形数据。
         * @param device_ 设备名，默认取 `device()`。
         * @param volume_ 线性音量倍率，默认取 `volume()`。
         * @throws WavError 音频设备不可用。
         */
        inline void play(const Wave &w,
            const std::string       &device_ = device(),
            double                   volume_ = volume()) {
#if defined(WAVPLAY_PLATFORM_LINUX)
            detail::Alsa                    &a = detail::alsa();
            detail::Pcm                      pcm(device_);
            const std::vector<unsigned char> data = volume_of(w, volume_);
            a.pcm_hw_params_any(pcm.handle(), pcm.params());
            a.pcm_hw_params_set_access(pcm.handle(), pcm.params(), 3);
            a.pcm_hw_params_set_format(pcm.handle(), pcm.params(), 2);
            a.pcm_hw_params_set_channels(
                pcm.handle(), pcm.params(), w.channels);
            uint32_t rate = w.sample_rate;
            int      dir  = 0;
            a.pcm_hw_params_set_rate_near(
                pcm.handle(), pcm.params(), &rate, &dir);
            if (a.pcm_hw_params(pcm.handle(), pcm.params()) < 0)
                throw WavError("Cannot Set Hardware Parameters");
            a.pcm_prepare(pcm.handle());
            const size_t frame = w.channels * 2;
            if (frame == 0 || data.empty()) return;
            const unsigned char *p    = data.data();
            size_t               left = data.size() / frame;
            while (left > 0) {
                long n = a.pcm_writei(pcm.handle(), p, left);
                if (n < 0) {
                    n = a.pcm_recover(pcm.handle(), static_cast<int>(n), 0);
                    if (n < 0) throw WavError("Cannot Write PCM Data");
                    continue;
                }
                if (n == 0) continue;
                p += static_cast<size_t>(n) * frame;
                left -= static_cast<size_t>(n);
            }
            a.pcm_drain(pcm.handle());
#elif defined(WAVPLAY_PLATFORM_MACOS)
            (void)device_;
            const std::vector<unsigned char> data = volume_of(w, volume_);
            if (data.empty()) return;
            bool          done = false;
            detail::Queue queue(w, &done);
            void         *buf = nullptr;
            if (detail::atb().AudioQueueAllocateBuffer(
                    queue.handle(), static_cast<uint32_t>(data.size()), &buf)
                    != 0
                || !buf)
                throw WavError("Cannot Allocate Audio Buffer");
            detail::QueueBuffer *qb = static_cast<detail::QueueBuffer *>(buf);
            std::memcpy(qb->mAudioData, data.data(), data.size());
            qb->mAudioDataByteSize = static_cast<uint32_t>(data.size());
            if (detail::atb().AudioQueueEnqueueBuffer(
                    queue.handle(), buf, 0, nullptr)
                != 0)
                throw WavError("Cannot Enqueue Audio Buffer");
            if (detail::atb().AudioQueueStart(queue.handle(), nullptr) != 0)
                throw WavError("Cannot Start Audio Queue");
            detail::wait_done(&done);
            detail::atb().AudioQueueStop(queue.handle(), 1);
#elif defined(WAVPLAY_PLATFORM_WINDOWS)
            const std::vector<unsigned char> data = volume_of(w, volume_);
            if (data.empty()) return;
            detail::WaveOut    out(device_, w);
            detail::WaveHeader hdr;
            std::memset(&hdr, 0, sizeof(hdr));
            hdr.lpData = reinterpret_cast<char *>(
                const_cast<unsigned char *>(data.data()));
            hdr.dwBufferLength = static_cast<uint32_t>(data.size());
            if (detail::winmm().waveOutPrepareHeader(
                    out.handle(), &hdr, sizeof(hdr))
                != 0)
                throw WavError("Cannot Prepare Audio Header");
            if (detail::winmm().waveOutWrite(out.handle(), &hdr, sizeof(hdr))
                != 0) {
                detail::winmm().waveOutUnprepareHeader(
                    out.handle(), &hdr, sizeof(hdr));
                throw WavError("Cannot Write Audio Data");
            }
            while (!(hdr.dwFlags & 0x00000001)) Sleep(1);
            detail::winmm().waveOutUnprepareHeader(
                out.handle(), &hdr, sizeof(hdr));
#else
            (void)w;
            (void)device_;
            (void)volume_;
            throw WavError("Unsupported Platform");
#endif
        }

        /**
         * @brief 播放一个 WAV 文件。
         * @param path 文件路径。
         * @param device_  设备名，默认为 "default"。
         * @param volume_  线性音量倍率，默认为 1.0。
         * @throws FileError 文件无法读取。
         * @throws ValueError 文件不是合法的 16-bit PCM WAV。
         * @throws WavError 音频设备不可用。
         */
        inline void play(const Path &path,
            const std::string       &device_ = device(),
            double                   volume_ = volume()) {
            play(load(path), device_, volume_);
        }
    }
}
