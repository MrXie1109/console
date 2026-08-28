/**
 * @file async/channel.h
 * @brief 基于环形缓冲区的有锁线程安全通信通道。
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
#include <cstddef>
#include <mutex>

#include "../csexc.h"

namespace console {
    template <class T, size_t N>
    class Channel;

    /**
     * @class ChannelIterator
     * @brief 通道迭代器。
     * @note 不要怀疑，输入迭代器里就是这样的，我确保这就是正确的语义。
     */
    template <class T, size_t N>
    class ChannelIterator {
        Channel<T, N> &channel_; ///< 通道引用
        T              cache_;   ///< 缓存值

    public:
        /// @brief 迭代器类别。
        using iterator_category = std::input_iterator_tag;
        /// @brief 值类型。
        using value_type = T;
        /// @brief 差异类型。
        using difference_type = std::ptrdiff_t;
        /// @brief 指针类型。
        using pointer = T *;
        /// @brief 引用类型。
        using reference = T &;

        explicit ChannelIterator(Channel<T, N> &channel) : channel_(channel) {}

        /**
         * @brief 解引用查询。
         * @return 缓存值。
         */
        T &operator*() {
            channel_ >> cache_;
            return cache_;
        }

        /**
         * @brief 成员访问查询。
         * @return 缓存值指针。
         */
        T *operator->() { return &cache_; }

        /**
         * @brief 前置递增。
         * @return 迭代器自身。
         * @note No-OP，return *this。
         */
        ChannelIterator &operator++() { return *this; }

        /**
         * @brief 后置递增。
         * @return 迭代器自身。
         * @note No-OP，return *this。
         */
        ChannelIterator operator++(int) { return *this; }

        /**
         * @brief 比较相等。
         * @return 是否相等。
         * @note 当通道关闭时，迭代器被视为相等。
         */
        bool operator==(const ChannelIterator &) const {
            return !static_cast<bool>(channel_);
        }

        /**
         * @brief 比较不等。
         * @return 是否不等。
         * @note 当通道关闭时，迭代器被视为不等。
         */
        bool operator!=(const ChannelIterator &) const {
            return static_cast<bool>(channel_);
        }
    };

    /**
     * @class Channel
     * @brief 环形缓冲区通道，用于线程间通信。
     * @tparam T 数据类型。
     * @tparam N 缓冲区大小。
     */
    template <class T, size_t N = 0>
    class Channel {
        T                       buffer_[N];      ///< 缓冲区
        size_t                  read_index_{0};  ///< 读索引
        size_t                  write_index_{0}; ///< 写索引
        mutable std::mutex      mutex_;          ///< 互斥锁
        std::condition_variable r_cv_;           ///< 读 条件变量
        std::condition_variable w_cv_;           ///< 写 条件变量
        bool                    closed_{false};  ///< 关闭标志

    public:
        /// @brief 默认构造函数。
        Channel() = default;

        /**
         * @brief 向通道写入数据。
         * @details 若通道已满，则阻塞等待直到有空间可写。
         * @param value 要写入的值。
         * @return 若写入成功。
         * @note 若通道已关闭，则 No-OP。
         */
        bool operator<<(const T &value) {
            std::unique_lock<std::mutex> lock(mutex_);
            w_cv_.wait(lock,
                [this] { return write_index_ - read_index_ < N || closed_; });
            if (closed_) return false;
            buffer_[write_index_ % N] = value;
            ++write_index_;
            r_cv_.notify_one();
            return true;
        }

        /**
         * @brief 向通道写入数据。
         * @details 若通道已满，则阻塞等待直到有空间可写。
         * @param value 要写入的值。
         * @return 若写入成功。
         * @note 若通道已关闭，则 No-OP。
         */
        bool operator<<(T &&value) {
            std::unique_lock<std::mutex> lock(mutex_);
            w_cv_.wait(lock,
                [this] { return write_index_ - read_index_ < N || closed_; });
            if (closed_) return false;
            buffer_[write_index_ % N] = std::move(value);
            ++write_index_;
            r_cv_.notify_one();
            return true;
        }

        /**
         * @brief 尝试向通道写入数据。
         * @details 非阻塞；若通道已满或已关闭，则立即返回失败。
         * @param value 要写入的值。
         * @return 若写入成功。
         */
        bool operator<<=(const T &value) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (closed_ || write_index_ - read_index_ >= N) return false;
            buffer_[write_index_ % N] = value;
            ++write_index_;
            r_cv_.notify_one();
            return true;
        }

        /**
         * @brief 尝试向通道写入数据。
         * @details 非阻塞；若通道已满或已关闭，则立即返回失败。
         * @param value 要写入的值。
         * @return 若写入成功。
         */
        bool operator<<=(T &&value) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (closed_ || write_index_ - read_index_ >= N) return false;
            buffer_[write_index_ % N] = std::move(value);
            ++write_index_;
            r_cv_.notify_one();
            return true;
        }

        /**
         * @brief 从通道读取数据。
         * @details 若通道为空，则阻塞等待直到有数据可读。
         * @param value 读取的值。
         * @return 若读取成功。
         * @note 若通道已关闭且没有剩余数据，则 No-OP。
         */
        bool operator>>(T &value) {
            std::unique_lock<std::mutex> lock(mutex_);
            r_cv_.wait(lock,
                [this] { return write_index_ - read_index_ > 0 || closed_; });
            if (closed_ && write_index_ - read_index_ == 0) return false;
            value = std::move(buffer_[read_index_ % N]);
            ++read_index_;
            w_cv_.notify_one();
            return true;
        }

        /**
         * @brief 从通道尝试读取数据（非阻塞）。
         * @details 若通道为空，则立即返回 false；否则读取数据并返回 true。
         * @param value 读取的值。
         * @return 若读取成功。
         * @note 若通道已关闭且没有剩余数据，返回 false。
         */
        bool operator>>=(T &value) {
            std::unique_lock<std::mutex> lock(mutex_);
            if (write_index_ - read_index_ == 0) return false;
            value = std::move(buffer_[read_index_ % N]);
            ++read_index_;
            w_cv_.notify_one();
            return true;
        }

        /**
         * @brief 从通道读取数据。
         * @details 若通道为空，则阻塞等待直到有数据可读。
         * @return 读取的值。
         * @throw AsyncError 若通道已关闭。
         * @note 若通道已关闭且没有剩余数据，则抛出 AsyncError。
         */
        T operator*() {
            std::unique_lock<std::mutex> lock(mutex_);
            r_cv_.wait(lock,
                [this] { return write_index_ - read_index_ > 0 || closed_; });
            if (closed_ && write_index_ - read_index_ == 0)
                throw AsyncError("Channel has been closed.");
            T value = std::move(buffer_[read_index_ % N]);
            ++read_index_;
            w_cv_.notify_one();
            return value;
        }

        /**
         * @brief 从通道读取数据。
         * @details 若通道为空，则阻塞等待直到有数据可读。
         * @return 读取的值。
         * @throw AsyncError 若通道已关闭。
         * @note 若通道已关闭且没有剩余数据，则抛出 AsyncError。
         */
        std::unique_ptr<T> operator->() {
            std::unique_lock<std::mutex> lock(mutex_);
            r_cv_.wait(lock,
                [this] { return write_index_ - read_index_ > 0 || closed_; });
            if (closed_ && write_index_ - read_index_ == 0)
                throw AsyncError("Channel has been closed.");
            std::unique_ptr<T> ptr(new T(std::move(buffer_[read_index_ % N])));
            ++read_index_;
            w_cv_.notify_one();
            return ptr;
        }

        /**
         * @brief 显式转换为布尔值。
         * @return 若通道已关闭且没有剩余数据，返回 false；否则返回 true。
         * @note 注意，验明通道为 true 后仍然可能出现问题，因为验明和读值不是原子的。
         *       得到 true 只能表明验明前管道有效，不代表读取时有效。
         */
        explicit operator bool() const {
            std::unique_lock<std::mutex> lock(mutex_);
            return !closed_ || write_index_ - read_index_ > 0;
        }

        /**
         * @brief 关闭通道。
         * @note 设置关闭标志，唤醒所有线程。
         */
        friend void close(Channel &channel) {
            std::unique_lock<std::mutex> lock(channel.mutex_);
            channel.closed_ = true;
            channel.r_cv_.notify_all();
            channel.w_cv_.notify_all();
        }

        /// @brief 迭代器类型。
        using iterator = ChannelIterator<T, N>;

        /// @brief 迭代器的 begin 函数。
        iterator begin() { return iterator(*this); }
        /// @brief 迭代器的 end 函数。
        iterator end() { return iterator(*this); }

        /// @brief 删除复制构造函数。
        Channel(const Channel &) = delete;
        /// @brief 删除复制赋值函数。
        Channel &operator=(const Channel &) = delete;
        /// @brief 删除移动构造函数。
        Channel(Channel &&) = delete;
        /// @brief 删除移动赋值函数。
        Channel &operator=(Channel &&) = delete;
    };

    /**
     * @class Channel
     * @brief 无缓冲通道，用于线程间通信。
     * @tparam T 数据类型。
     */
    template <class T>
    class Channel<T, 0> {
        T                       buffer_;         ///< 缓冲区
        mutable std::mutex      mutex_;          ///< 互斥锁
        std::condition_variable r_cv_;           ///< 读 条件变量
        std::condition_variable w_cv_;           ///< 写 条件变量
        bool                    empty_  = true;  ///< 缓冲区是否为空
        bool                    closed_ = false; ///< 关闭标志

    public:
        /// @brief 默认构造函数。
        Channel() = default;

        /**
         * @brief 向通道写入数据。
         * @details 若通道为空，则阻塞等待直到有空间可写。
         * @param value 要写入的值。
         * @return 若写入成功。
         * @note 若通道已关闭，则 No-OP。
         */
        bool operator<<(const T &value) {
            std::unique_lock<std::mutex> lock(mutex_);
            w_cv_.wait(lock, [this] { return empty_ || closed_; });
            if (closed_) return false;
            buffer_ = value;
            empty_  = false;
            r_cv_.notify_one();
            return true;
        }

        /**
         * @brief 向通道写入数据。
         * @details 若通道为空，则阻塞等待直到有空间可写。
         * @param value 要写入的值。
         * @return 若写入成功。
         * @note 若通道已关闭，则 No-OP。
         */
        bool operator<<(T &&value) {
            std::unique_lock<std::mutex> lock(mutex_);
            w_cv_.wait(lock, [this] { return empty_ || closed_; });
            if (closed_) return false;
            buffer_ = std::move(value);
            empty_  = false;
            r_cv_.notify_one();
            return true;
        }

        /**
         * @brief 从通道读取数据。
         * @details 若通道为空，则阻塞等待直到有数据可读。
         * @param value 读取的值。
         * @return 若读取成功。
         * @note 若通道已关闭且没有剩余数据，则 No-OP。
         */
        bool operator>>(T &value) {
            std::unique_lock<std::mutex> lock(mutex_);
            r_cv_.wait(lock, [this] { return !empty_ || closed_; });
            if (closed_ && empty_) return false;
            value  = std::move(buffer_);
            empty_ = true;
            w_cv_.notify_one();
            return true;
        }

        /**
         * @brief 从通道读取数据。
         * @details 若通道为空，则阻塞等待直到有数据可读。
         * @return 读取的值。
         * @throw AsyncError 若通道已关闭。
         * @note 若通道已关闭且没有剩余数据，则抛出 AsyncError。
         */
        T operator*() {
            std::unique_lock<std::mutex> lock(mutex_);
            r_cv_.wait(lock, [this] { return !empty_ || closed_; });
            if (closed_ && empty_) throw AsyncError("Channel has been closed.");
            T value = std::move(buffer_);
            empty_  = true;
            w_cv_.notify_one();
            return value;
        }

        /**
         * @brief 从通道读取数据。
         * @details 若通道为空，则阻塞等待直到有数据可读。
         * @return 读取的值。
         * @throw AsyncError 若通道已关闭。
         * @note 若通道已关闭且没有剩余数据，则抛出 AsyncError。
         */
        std::unique_ptr<T> operator->() {
            std::unique_lock<std::mutex> lock(mutex_);
            r_cv_.wait(lock, [this] { return !empty_ || closed_; });
            if (closed_ && empty_) throw AsyncError("Channel has been closed.");
            std::unique_ptr<T> ptr(new T(std::move(buffer_)));
            empty_ = true;
            w_cv_.notify_one();
            return ptr;
        }

        /**
         * @brief 显式转换为布尔值。
         * @return 若通道已关闭且没有剩余数据，返回 false；否则返回 true。
         * @note 注意，验明通道为 true 后仍然可能出现问题，因为验明和读值不是原子的。
         *       得到 true 只能表明验明前管道有效，不代表读取时有效。
         */
        explicit operator bool() const {
            std::unique_lock<std::mutex> lock(mutex_);
            return !closed_ || !empty_;
        }

        /**
         * @brief 关闭通道。
         * @note 设置关闭标志，唤醒所有线程。
         */
        friend void close(Channel &channel) {
            std::unique_lock<std::mutex> lock(channel.mutex_);
            channel.closed_ = true;
            channel.r_cv_.notify_all();
            channel.w_cv_.notify_all();
        }

        /// @brief 迭代器类型。
        using iterator = ChannelIterator<T, 0>;

        /// @brief 迭代器的 begin 函数。
        iterator begin() { return iterator(*this); }
        /// @brief 迭代器的 end 函数。
        iterator end() { return iterator(*this); }

        /// @brief 删除复制构造函数。
        Channel(const Channel &) = delete;
        /// @brief 删除复制赋值函数。
        Channel &operator=(const Channel &) = delete;
        /// @brief 删除移动构造函数。
        Channel(Channel &&) = delete;
        /// @brief 删除移动赋值函数。
        Channel &operator=(Channel &&) = delete;
    };

    /**
     * @brief 遍历通道中的所有元素。
     * @param channel 要遍历的通道。
     * @param func 遍历函数。
     * @return 操作后的遍历函数。
     * @note 相对于 range-based for，这个函数可以做到确保不抛出异常，
     *       Channel 的 range-based for 对于会关闭的函数若在检查与取值之间 close，
     *       则会抛出一个 AsyncError，而这个函数不会被钻空子。
     */
    template <class T, size_t N, class F>
    inline F for_each(Channel<T, N> &channel, F &&func) {
        T value;
        while (channel >> value) func(value);
        return std::forward<F>(func);
    }
}
