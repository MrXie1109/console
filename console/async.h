/**
 * @file thread.h
 * @brief 提供现代化的Thread类，封装了std::thread并内置Event以便协作式地请求线程终止。
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
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

#include "sfinae.h"
#include "time.h"

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

    /**
     * @class Thread
     * @brief 线程类，封装了 std::thread 和 Event 类。
     * @details 提供了线程的启动、停止和等待操作，以及事件标志的设置和检查。
     */
    class Thread {
        std::unique_ptr<Event> event_;  ///< 指向事件标志的指针
        std::thread            thread_; ///< 线程对象

        /**
         * @brief true_type分支，用于处理带有const Event &参数的可调用对象。
         * @tparam F 可调用对象类型。
         * @tparam Args 可调用对象的参数类型。
         * @param f 可调用对象。
         * @param args 可调用对象的参数。
         * @return 新创建的线程对象。
         */
        template <class F, class... Args>
        std::thread make_thread(std::true_type, F &&f, Args &&...args) {
            return std::thread(std::forward<F>(f),
                std::cref(*event_),
                std::forward<Args>(args)...);
        }

        /**
         * @brief false_type分支，用于处理不带const Event &参数的可调用对象。
         * @tparam F 可调用对象类型。
         * @tparam Args 可调用对象的参数类型。
         * @param f 可调用对象。
         * @param args 可调用对象的参数。
         * @return 新创建的线程对象。
         */
        template <class F, class... Args>
        std::thread make_thread(std::false_type, F &&f, Args &&...args) {
            return std::thread(std::forward<F>(f), std::forward<Args>(args)...);
        }

    public:
        /**
         * @brief 默认构造函数，创建线程对象但不启动线程。
         * @note 稍后可以通过赋值启动任务。
         */
        Thread() : thread_(), event_(nullptr) {}

        /**
         * @brief 构造函数，创建线程对象并启动线程。
         * @tparam F 可调用对象类型。
         * @tparam Args 可调用对象的参数类型。
         * @param f 可调用对象。
         * @param args 可调用对象的参数。
         * @note 通过标签分发，如果首个参数为const Event &，
         *       分发至true_type分支，否则分发至false_type分支。
         */
        template <class F, class... Args>
        Thread(F &&f, Args &&...args) :
            event_(is_callable<F, const Event &, Args...>::value ? new Event()
                                                                 : nullptr),
            thread_(make_thread(is_callable<F, const Event &, Args...>{},
                std::forward<F>(f),
                std::forward<Args>(args)...)) {}

        /// @brief 析构函数，设置事件标志并等待线程结束。
        ~Thread() {
            stop();
            if (thread_.joinable()) thread_.join();
        }

        /// @brief 等待线程结束。
        void join() {
            if (thread_.joinable()) thread_.join();
        }

        /// @brief 分离线程。
        void detach() {
            if (thread_.joinable()) thread_.detach();
        }

        /// @brief 停止线程。
        void stop() {
            if (event_) event_->set();
        }

        /// @brief 判断线程是否可连接。
        bool is_joinable() const { return thread_.joinable(); }

        /// @brief 获取线程ID。
        std::thread::id get_id() const { return thread_.get_id(); }

        /// @brief 获取线程的原生句柄。
        auto native_handle() -> decltype(thread_.native_handle()) {
            return thread_.native_handle();
        }

        /// @brief 禁止拷贝构造。
        Thread(const Thread &) = delete;
        /// @brief 禁止拷贝赋值。
        Thread &operator=(const Thread &) = delete;

        /**
         * @brief 移动构造。
         * @param t 要移动的线程对象。
         */
        Thread(Thread &&t) :
            event_(std::move(t.event_)), thread_(std::move(t.thread_)) {}

        /**
         * @brief 移动赋值。
         * @param t 要移动的线程对象。
         * @return 移动后的线程对象。
         */
        Thread &operator=(Thread &&t) noexcept {
            if (this == &t) return *this;
            stop();
            if (thread_.joinable()) thread_.join();
            event_  = std::move(t.event_);
            thread_ = std::move(t.thread_);
            return *this;
        }

        /**
         * @brief 交换线程对象。
         * @param t 要交换的线程对象。
         */
        void swap(Thread &t) noexcept {
            std::swap(thread_, t.thread_);
            std::swap(event_, t.event_);
        }

        /**
         * @brief 交换两个线程对象。
         * @param lhs 要交换的线程对象。
         * @param rhs 要交换的线程对象。
         */
        friend void swap(Thread &lhs, Thread &rhs) noexcept { lhs.swap(rhs); }
    };
}
