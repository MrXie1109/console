/**
 * @file async/group.h
 * @brief 异步任务组，用于管理一组异步任务的执行，提供添加任务、等待完成及回调通知功能。
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
#include <atomic>
#include <condition_variable>
#include <exception>
#include <functional>
#include <mutex>

namespace console {
    /**
     * @class Group
     * @brief 异步任务组，用于管理一组异步任务的执行。
     * @details 提供了添加任务、等待任务完成和获取任务数量等功能。
     * @note 我们相信用户，所以我们不提供过多的防护，同时让错误尽早暴露。
     */
    class Group {
        int                             count_{0};    ///< 任务计数器
        std::function<void()>           callback_;    ///< 任务完成回调
        bool                            done_{false}; ///< 任务完成标志
        mutable std::mutex              mutex_;       ///< 互斥锁
        mutable std::condition_variable cv_;          ///< 条件变量

        /// @brief 空操作函数，用于默认回调。
        static void no_op() {}

    public:
        /**
         * @brief 构造函数，初始化任务计数器和回调函数。
         * @param count 任务数量。
         * @param callback 任务完成回调函数。
         */
        Group(size_t count = 0, std::function<void()> callback = no_op) :
            count_(count), callback_(callback) {}

        /**
         * @brief 析构函数，确保所有任务完成。
         * @note 若任务未完成或未设置完成标志，则终止程序。
         */
        ~Group() {
            std::lock_guard<std::mutex> lock(mutex_);
            if (count_ != 0 || !done_) std::terminate();
        }

        /**
         * @brief 添加任务数量。
         * @param count 要添加的任务数量。
         * @note 若已完成，则终止程序。
         */
        void add(size_t count) {
            std::lock_guard<std::mutex> lock(mutex_);
            if (done_) std::terminate();
            count_ += count;
        }

        /**
         * @brief 任务完成。
         * @note 若任务数量减少为0，则调用回调函数并通知所有等待线程。
         *       若此前已经减少为0，则终止程序。
         */
        void done() {
            std::lock_guard<std::mutex> lock(mutex_);
            if (count_ <= 0) std::terminate();
            if (--count_ == 0) {
                done_ = true;
                callback_();
                cv_.notify_all();
            }
        }

        /**
         * @brief 等待所有任务完成。
         */
        void wait() const {
            std::unique_lock<std::mutex> lock(mutex_);
            cv_.wait(lock, [this] { return count_ == 0 && done_; });
        }

        /**
         * @brief 获取回调函数。
         * @return 回调函数。
         */
        std::function<void()> callback() const {
            std::lock_guard<std::mutex> lock(mutex_);
            return callback_;
        }

        /**
         * @brief 连接回调函数。
         * @param callback 要连接的回调函数。
         */
        void connect(std::function<void()> callback) {
            std::lock_guard<std::mutex> lock(mutex_);
            callback_ = callback;
        }

        /// @brief 删除复制构造函数。
        Group(const Group &) = delete;
        /// @brief 删除复制赋值函数。
        Group &operator=(const Group &) = delete;
        /// @brief 删除移动构造函数。
        Group(Group &&) = delete;
        /// @brief 删除移动赋值函数。
        Group &operator=(Group &&) = delete;
    };
}
