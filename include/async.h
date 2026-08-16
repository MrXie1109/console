/**
 * @file async.h
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
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>

#include "csexc.h"
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
        Thread() = default;

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

        /**
         * @brief 获取内部的 event 指针。
         * @return 指向 Event 的指针。
         * @note 可能为空。
         */
        Event *get_event() const { return event_.get(); }
    };

    /**
     * @class Task
     * @brief 异步任务类，用于在后台线程中执行任务。
     * @tparam T 任务返回值的类型。
     * @note Task 的限制与 std::future 几乎一致，如：
     *       - 线程不安全，需手动同步。
     *       - 仅一次获取结果，再次调用将抛出异常。
     *       - 不可拷贝，仅可移动。
     */
    template <class T>
    class Task {
        std::future<T>         future_; ///< 任务的返回值
        std::thread            thread_; ///< 任务执行的线程
        std::unique_ptr<Event> event_;  ///< 事件标志

    public:
        /// @brief 默认构造函数。
        Task() = default;

        /**
         * @brief 构造函数，接受一个可调用对象和参数，启动异步任务。
         * @tparam F 可调用对象的类型。
         * @tparam Args 参数的类型。
         * @param f 可调用对象。
         * @param args 参数。
         * @note 若需传递引用，需使用 std::ref/cref 包装。
         */
        template <class F, class... Args>
        explicit Task(F &&f, Args &&...args) {
            start(std::forward<F>(f), std::forward<Args>(args)...);
        }

        /// @brief 删除拷贝构造函数。
        Task(const Task &) = delete;
        /// @brief 删除拷贝赋值运算符。
        Task &operator=(const Task &) = delete;

        /**
         * @brief 移动构造函数。
         * @param other 要移动的 Task 对象。
         */
        Task(Task &&other) noexcept :
            future_(std::move(other.future_)),
            thread_(std::move(other.thread_)), //
            event_(std::move(other.event_)) {}

        /**
         * @brief 移动赋值运算符。
         * @param other 要移动的 Task 对象。
         * @return 移动后的 Task 对象的引用。
         */
        Task &operator=(Task &&other) noexcept {
            if (this != &other) {
                if (event_) event_->set();
                if (thread_.joinable()) thread_.join();
                future_ = std::move(other.future_);
                thread_ = std::move(other.thread_);
                event_  = std::move(other.event_);
            }
            return *this;
        }

        /**
         * @brief 析构函数。
         */
        ~Task() {
            if (event_) event_->set();
            if (thread_.joinable()) thread_.join();
        }

        /**
         * @brief 启动异步任务。
         * @tparam F 要执行的函数类型。
         * @tparam Args 要传递给函数的参数类型。
         * @param f 要执行的函数。
         * @param args 要传递给函数的参数。
         * @note 若需传递引用，需使用 std::ref/cref 包装。
         */
        template <class F, class... Args>
        typename std::enable_if<
            !is_callable<F, const Event &, Args...>::value>::type
        start(F &&f, Args &&...args) {
            auto bound_task
                = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
            std::packaged_task<T()> task(std::move(bound_task));
            future_ = task.get_future();
            thread_ = std::thread(std::move(task));
        }

        template <class F, class... Args>
        typename std::enable_if<
            is_callable<F, const Event &, Args...>::value>::type
        start(F &&f, Args &&...args) {
            if (!event_) event_.reset(new Event());
            start(std::forward<F>(f),
                std::cref(*event_),
                std::forward<Args>(args)...);
        }

        /**
         * @brief 获取异步任务的返回值。
         * @return 异步任务的返回值。
         */
        T get() { return future_.get(); }

        /**
         * @brief 检查异步任务是否有效。
         * @return 若任务有效则返回 true，否则返回 false。
         */
        bool valid() { return future_.valid(); }

        /// @brief 等待异步任务完成。
        void wait() const { future_.wait(); }

        /**
         * @brief 等待异步任务完成，最多等待指定时间。
         * @tparam Rep 时间单位的类型。
         * @tparam Period 时间单位的精度。
         * @param timeout 等待的时间。
         * @return 异步任务的状态。
         */
        template <class Rep, class Period>
        std::future_status
        wait_for(const std::chrono::duration<Rep, Period> &timeout) const {
            return future_.wait_for(timeout);
        }

        /**
         * @brief 等待异步任务完成，最多等待指定时间。
         * @param timeout 等待的时间。
         * @return 异步任务的状态。
         */
        std::future_status wait_for(Time timeout) const {
            return future_.wait_for(std::chrono::nanoseconds(timeout.count()));
        }

        /**
         * @brief 等待异步任务完成，最多等待指定时间。
         * @param seconds 等待的时间 (秒)。
         * @return 异步任务的状态。
         */
        std::future_status wait_for(double seconds) const {
            return future_.wait_for(std::chrono::duration<double>(seconds));
        }

        /**
         * @brief 等待异步任务完成，最多等待指定时间。
         * @param time_point 等待的时间点。
         * @return 异步任务的状态。
         */
        template <class Clock, class Duration>
        std::future_status wait_until(
            const std::chrono::time_point<Clock, Duration> &time_point) const {
            return future_.wait_until(time_point);
        }

        /**
         * @brief 获取异步任务的状态。
         * @return 异步任务的状态。
         */
        std::future_status status() const {
            return future_.wait_for(std::chrono::nanoseconds(0));
        }

        /**
         * @brief 取消异步任务。
         * @details 停止线程并设置异常，以表示任务已被取消。
         */
        void cancel() {
            if (status() == std::future_status::ready) return;
            event_->set();
            std::promise<T> promise;
            future_ = promise.get_future();
            promise.set_exception(std::make_exception_ptr(
                AsyncError("Task has been cancelled.")));
        }
    };

    /**
     * @class SharedTask
     * @brief 异步任务类，用于在后台线程中执行任务 (共享版)。
     * @tparam T 任务返回值的类型。
     * @note SharedTask 的限制相对于 Task 更少，如：
     *       - 线程安全，无需手动同步。
     *       - 可多次获取结果，不会抛出异常。
     *       - 可拷贝，也可移动。
     *       但可能略慢。
     */
    template <class T>
    class SharedTask {
        std::shared_future<T>        future_; ///< 任务的返回值
        std::shared_ptr<std::thread> thread_; ///< 任务执行的线程
        std::shared_ptr<Event>       event_;  ///< 事件标志
        std::shared_ptr<std::mutex>  mutex_{new std::mutex()}; ///< 互斥锁

        /**
         * @brief 线程删除器，用于在 std::shared_ptr 销毁时自动调用。
         * @param t 线程指针。
         */
        static void thread_deleter(std::thread *t) {
            if (t && t->joinable()) t->join();
        }

        /**
         * @brief 事件删除器，用于在 std::shared_ptr 销毁时自动调用。
         * @param e 事件指针。
         */
        static void event_deleter(Event *e) { e->set(); }

    public:
        /// @brief 默认构造函数。
        SharedTask() = default;

        /**
         * @brief 构造函数，接受一个可调用对象和参数，启动异步任务。
         * @tparam F 可调用对象的类型。
         * @tparam Args 参数的类型。
         * @param f 可调用对象。
         * @param args 参数。
         * @note 若需传递引用，需使用 std::ref/cref 包装。
         */
        template <class F,
            class... Args,
            class = typename std::enable_if<
                is_callable<F, Args...>::value
                || is_callable<F, const Event &, Args...>::value>::type>
        explicit SharedTask(F &&f, Args &&...args) {
            start(std::forward<F>(f), std::forward<Args>(args)...);
        }

        /**
         * @brief 拷贝构造函数。
         * @param other 要拷贝的 SharedTask 对象。
         */
        SharedTask(const SharedTask &other) :
            future_(other.future_), //
            thread_(other.thread_), //
            event_(other.event_),   //
            mutex_(other.mutex_) {}

        /**
         * @brief 拷贝赋值运算符。
         * @param other 要拷贝的 SharedTask 对象。
         * @return 拷贝后的 SharedTask 对象的引用。
         */
        SharedTask &operator=(const SharedTask &other) {
            future_ = other.future_;
            thread_ = other.thread_;
            event_  = other.event_;
            mutex_  = other.mutex_;
            return *this;
        }

        /**
         * @brief 移动构造函数。
         * @param other 要移动的 Task 对象。
         */
        SharedTask(SharedTask &&other) noexcept :
            future_(std::move(other.future_)),
            thread_(std::move(other.thread_)), //
            event_(std::move(other.event_)),   //
            mutex_(std::move(other.mutex_)) {}

        /**
         * @brief 移动赋值运算符。
         * @param other 要移动的 Task 对象。
         * @return 移动后的 Task 对象的引用。
         */
        SharedTask &operator=(SharedTask &&other) noexcept {
            if (this != &other) {
                future_ = std::move(other.future_);
                thread_ = std::move(other.thread_);
                event_  = std::move(other.event_);
                mutex_  = std::move(other.mutex_);
            }
            return *this;
        }

        /**
         * @brief 析构函数。
         */
        ~SharedTask() = default;

        /**
         * @brief 启动异步任务。
         * @tparam F 要执行的函数类型。
         * @tparam Args 要传递给函数的参数类型。
         * @param f 要执行的函数。
         * @param args 要传递给函数的参数。
         * @note 若需传递引用，需使用 std::ref/cref 包装。
         */
        template <class F, class... Args>
        typename std::enable_if<
            !is_callable<F, const Event &, Args...>::value>::type
        start(F &&f, Args &&...args) {
            auto bound_task
                = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
            std::packaged_task<T()> task(std::move(bound_task));
            future_ = task.get_future().share();
            thread_ = std::shared_ptr<std::thread>(
                new std::thread(std::move(task)), thread_deleter);
        }

        template <class F, class... Args>
        typename std::enable_if<
            is_callable<F, const Event &, Args...>::value>::type
        start(F &&f, Args &&...args) {
            if (!event_)
                event_ = std::shared_ptr<Event>(new Event(), event_deleter);
            start(std::forward<F>(f),
                std::cref(*event_),
                std::forward<Args>(args)...);
        }

        /**
         * @brief 获取异步任务的返回值。
         * @return 异步任务的返回值。
         */
        T get() { return future_.get(); }

        /**
         * @brief 检查异步任务是否有效。
         * @return 若任务有效则返回 true，否则返回 false。
         */
        bool valid() { return future_.valid(); }

        /// @brief 等待异步任务完成。
        void wait() const { future_.wait(); }

        /**
         * @brief 等待异步任务完成，最多等待指定时间。
         * @tparam Rep 时间单位的类型。
         * @tparam Period 时间单位的精度。
         * @param timeout 等待的时间。
         * @return 异步任务的状态。
         */
        template <class Rep, class Period>
        std::future_status
        wait_for(const std::chrono::duration<Rep, Period> &timeout) const {
            return future_.wait_for(timeout);
        }

        /**
         * @brief 等待异步任务完成，最多等待指定时间。
         * @param timeout 等待的时间。
         * @return 异步任务的状态。
         */
        std::future_status wait_for(Time timeout) const {
            return future_.wait_for(std::chrono::nanoseconds(timeout.count()));
        }

        /**
         * @brief 等待异步任务完成，最多等待指定时间。
         * @param seconds 等待的时间 (秒)。
         * @return 异步任务的状态。
         */
        std::future_status wait_for(double seconds) const {
            return future_.wait_for(std::chrono::duration<double>(seconds));
        }

        /**
         * @brief 等待异步任务完成，最多等待指定时间。
         * @param time_point 等待的时间点。
         * @return 异步任务的状态。
         */
        template <class Clock, class Duration>
        std::future_status wait_until(
            const std::chrono::time_point<Clock, Duration> &time_point) const {
            return future_.wait_until(time_point);
        }

        /**
         * @brief 获取异步任务的状态。
         * @return 异步任务的状态。
         */
        std::future_status status() const {
            return future_.wait_for(std::chrono::nanoseconds(0));
        }

        /**
         * @brief 取消异步任务。
         * @details 停止线程并设置异常，以表示任务已被取消。
         * @warning 在调用 cancel 前，应确保已经 start，否则将触发未定义行为。
         */
        void cancel() {
            if (status() == std::future_status::ready) return;
            if (!mutex_->try_lock()) return;
            std::lock_guard<std::mutex> lock(*mutex_, std::adopt_lock);
            event_->set();
            std::promise<T> promise;
            future_ = promise.get_future().share();
            promise.set_exception(std::make_exception_ptr(
                AsyncError("Task has been cancelled.")));
        }
    };
}
