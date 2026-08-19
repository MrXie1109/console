/**
 * @file async/event.h
 * @brief 提供线程同步的 Event 类，支持设置、清除、等待和超时等待等操作。
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
#include <condition_variable>
#include <mutex>

#include "../time.h"

namespace console {
    /**
     * @class Event
     * @brief 线程同步事件类，用于线程间通信。
     */
    class Event {
        bool                            flag_;  ///< 事件标志，用于线程间通信
        mutable std::mutex              mutex_; ///< 互斥锁，保护事件标志
        mutable std::condition_variable cv_;    ///< 条件变量，用于线程间同步

    public:
        /// @brief 默认构造函数，初始化事件标志为 false。
        Event() : flag_(false) {}

        /// @brief 设置事件标志为 true，并通知所有等待的线程。
        void set() {
            std::unique_lock<std::mutex> lock(mutex_);
            flag_ = true;
            cv_.notify_all();
        }

        /// @brief 清除事件标志为 false。
        void clear() {
            std::unique_lock<std::mutex> lock(mutex_);
            flag_ = false;
        }

        /// @brief 等待事件标志变为 true。
        void wait() const {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this]() { return flag_; });
        }

        /**
         * @brief 等待事件标志变为 true，带有超时机制。
         * @param duration 等待的时间。
         * @return 如果在指定时间内事件标志变为 true，则返回 true；否则返回 false。
         */
        template <class Rep, class Period>
        bool wait(const std::chrono::duration<Rep, Period> &duration) const {
            std::unique_lock<std::mutex> lock(mutex_);
            return cv_.wait_for(lock, duration, [this]() { return flag_; });
        }

        /**
         * @brief 等待事件标志变为 true，带有超时机制。
         * @param timeout 等待的时间。
         * @return 如果在指定时间内事件标志变为 true，则返回 true；否则返回 false。
         */
        bool wait(Time timeout) const {
            return wait(std::chrono::nanoseconds(timeout.count()));
        }

        /**
         * @brief 等待事件标志变为 true，带有超时机制。
         * @param seconds 等待的时间(秒)。
         * @return 如果在指定时间内事件标志变为 true，则返回 true；否则返回 false。
         */
        bool wait(double seconds) const {
            return wait(std::chrono::duration<double>(seconds));
        }

        /**
         * @brief 检查事件标志是否为 true。
         * @return 如果事件标志为 true，则返回 true；否则返回 false。
         */
        bool is_set() const {
            std::unique_lock<std::mutex> lock(mutex_);
            return flag_;
        }

        /// @brief 禁止拷贝构造。
        Event(const Event &) = delete;
        /// @brief 禁止拷贝赋值。
        Event &operator=(const Event &) = delete;
        /// @brief 禁止移动构造。
        Event(Event &&other) = delete;
        /// @brief 禁止移动赋值。
        Event &operator=(Event &&other) = delete;
    };
}
