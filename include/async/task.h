/**
 * @file async/task.h
 * @brief 异步任务框架，提供 Task 和 SharedTask 两类异步任务工具。
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
#include <functional>
#include <future>
#include <memory>
#include <thread>

#include "../sfinae.h"
#include "event.h"

namespace console {
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
            if (event_) event_->set();
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
        std::shared_future<T>           future_; ///< 任务的返回值
        std::shared_ptr<std::thread>    thread_; ///< 任务执行的线程
        std::shared_ptr<Event>          event_;  ///< 事件标志
        std::shared_ptr<std::once_flag> flag_;   ///< 取消标志

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
            flag_(other.flag_) {}

        /**
         * @brief 拷贝赋值运算符。
         * @param other 要拷贝的 SharedTask 对象。
         * @return 拷贝后的 SharedTask 对象的引用。
         */
        SharedTask &operator=(const SharedTask &other) {
            future_ = other.future_;
            thread_ = other.thread_;
            event_  = other.event_;
            flag_   = other.flag_;
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
            flag_(std::move(other.flag_)) {}

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
                flag_   = std::move(other.flag_);
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
            std::call_once(*flag_, [this] {
                if (status() == std::future_status::ready) return;
                if (event_) event_->set();
                std::promise<T> promise;
                future_ = promise.get_future().share();
                promise.set_exception(std::make_exception_ptr(
                    AsyncError("SharedTask has been cancelled.")));
            });
        }
    };
}
