/**
 * @file pool.h
 * @brief 简单的线程池实现，模仿了 Python 的
 * concurrent.futures.ThreadPoolExecutor 接口。
 * @details 该模块提供了一个高效的线程池实现，支持任务提交、批量映射执行、
 *          等待所有任务完成、优雅关闭等功能。适用于需要并发执行多个独立任务的场景。
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
#include <functional>
#include <future>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <type_traits>
#include <utility>
#include <vector>

#include "csexc.h"

namespace console {
    /**
     * @class ThreadPool
     * @brief 线程池执行器类，用于管理和执行并发任务。
     * @details
     * 该类提供了一个固定大小的线程池，支持提交可调用对象（函数、lambda、绑定表达式等），
     *          并返回 std::future 对象以获取任务的执行结果。模仿了 Python 的
     *          concurrent.futures.ThreadPoolExecutor 接口。
     */
    class ThreadPool {
    public:
        enum class launch : bool {
            abort, ///< 退出时调用 abort
            close  ///< 退出时调用 close
        };

    private:
        /**
         * @struct TaskBase
         * @brief 任务基类，提供多态接口。
         * @details 用于在队列中存储不同类型的任务，通过虚函数 execute()
         * 实现多态调用。
         */
        struct TaskBase {
            virtual ~TaskBase()    = default;
            virtual void execute() = 0;
        };

        /**
         * @struct Task
         * @brief 具体任务类模板，封装 std::packaged_task。
         * @tparam T 任务的返回值类型。
         * @details 继承自 TaskBase，实现 execute() 方法以执行包装的任务。
         */
        template <class T>
        struct Task : public TaskBase {
            std::packaged_task<T()> task;
            Task(std::packaged_task<T()> &&t) : task(std::move(t)) {}
            void execute() override { task(); }
        };

        std::vector<std::thread>              workers; ///< 工作线程容器
        std::queue<std::unique_ptr<TaskBase>> tasks; ///< 待执行任务队列
        mutable std::mutex mutex; ///< 保护任务队列的互斥锁
        std::condition_variable cv; ///< 用于线程等待和唤醒的条件变量
        std::atomic<bool>   shutdown;     ///< 线程池关闭标志
        std::atomic<size_t> active_tasks; ///< 当前正在执行的任务数
        launch              exit_launch_; ///< 退出策略

    public:
        /**
         * @brief 构造函数，创建指定数量的工作线程。
         * @param num_threads 线程池中的线程数量。
         * @details 创建 num_threads
         * 个工作线程，每个线程不断从任务队列中取出任务并执行，
         *          当线程池关闭且任务队列为空时，线程会退出。
         */
        ThreadPool(launch exit_launch = launch::close,
            size_t        num_threads = std::thread::hardware_concurrency()) :
            shutdown(false), active_tasks(0), exit_launch_(exit_launch) {
            if (num_threads == 0) num_threads = 2;
            for (size_t i = 0; i < num_threads; ++i) {
                workers.emplace_back([this] {
                    while (true) {
                        std::unique_ptr<TaskBase> task;
                        {
                            std::unique_lock<std::mutex> lock(mutex);
                            cv.wait(lock, [this] {
                                return shutdown.load(std::memory_order_acquire)
                                       || !tasks.empty();
                            });
                            if (shutdown.load(std::memory_order_acquire)
                                && tasks.empty())
                                return;
                            task = std::move(tasks.front());
                            tasks.pop();
                        }
                        active_tasks.fetch_add(1, std::memory_order_release);
                        try {
                            task->execute();
                            active_tasks.fetch_sub(
                                1, std::memory_order_release);
                            if (active_tasks.load(std::memory_order_acquire)
                                    == 0
                                && tasks.empty())
                                cv.notify_all();
                        } catch (...) {
                            active_tasks.fetch_sub(
                                1, std::memory_order_release);
                            if (active_tasks.load(std::memory_order_acquire)
                                    == 0
                                && tasks.empty())
                                cv.notify_all();
                            throw;
                        }
                    }
                });
            }
        }

        /**
         * @brief 构造函数，创建指定数量的工作线程。
         * @param num_threads 线程池中的线程数量。
         * @details 创建 num_threads
         * 个工作线程，每个线程不断从任务队列中取出任务并执行，
         *          当线程池关闭且任务队列为空时，线程会退出。
         */
        ThreadPool(size_t num_threads = std::thread::hardware_concurrency(),
            launch        exit_launch = launch::close) :
            ThreadPool(exit_launch, num_threads) {}

        /**
         * @brief 析构函数，根据退出策略决定关闭方式。
         * @brief abort: 调用 abort()
         *        close: 调用 close()
         */
        ~ThreadPool() {
            if (exit_launch_ == launch::close)
                close();
            else
                abort();
        }

        /**
         * @brief 提交一个可调用对象到线程池执行。
         * @tparam F 可调用对象的类型。
         * @tparam Args 参数包的类型。
         * @param f 要执行的可调用对象（函数、lambda 等）。
         * @param args 传递给可调用对象的参数。
         * @return
         * std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
         *         与任务关联的 future 对象，用于获取返回值。
         * @throw ThreadPoolError 如果线程池正在关闭，则抛出异常。
         * @details 将参数绑定到可调用对象后添加到任务队列，由工作线程异步执行。
         *          返回的 future 对象可用于等待任务完成并获取返回值。
         */
        template <class F, class... Args>
        auto
        submit(F &&f, Args &&...args) -> std::future<decltype(std::forward<F>(
                                          f)(std::forward<Args>(args)...))> {
            using return_type
                = decltype(std::forward<F>(f)(std::forward<Args>(args)...));
            auto bound_task
                = std::bind(std::forward<F>(f), std::forward<Args>(args)...);
            std::packaged_task<return_type()> task(bound_task);
            std::future<return_type>          result = task.get_future();
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (shutdown.load(std::memory_order_acquire))
                    throw ThreadPoolError("ThreadPool is shutting down");
                tasks.emplace(std::unique_ptr<TaskBase>(
                    new Task<return_type>(std::move(task))));
            }
            cv.notify_one();
            return result;
        }

        /**
         * @brief 批量提交任务，对容器中的每个元素应用函数。
         * @tparam F 可调用对象的类型。
         * @tparam Container 容器类型，必须包含 value_type 成员类型。
         * @param func 要应用于每个元素的函数。
         * @param items 包含输入元素的容器。
         * @return std::vector<std::future<decltype(func(std::declval<typename
         * Container::value_type>()))>> 包含所有任务 future 对象的
         * vector，顺序与输入容器一致。
         * @throw ThreadPoolError 如果线程池正在关闭，则抛出异常。
         * @details 对容器中的每个元素调用 submit(func, item)，将所有返回的
         * future 对象 收集到 vector 中返回。可用于并行处理集合中的元素。
         */
        template <class F, class Container>
        auto map(F &&func, const Container &items)
            -> std::vector<std::future<decltype(func(
                std::declval<typename Container::value_type>()))>> {
            using return_type = decltype(func(
                std::declval<typename Container::value_type>()));
            std::vector<std::future<return_type>> futures;
            futures.reserve(items.size());
            for (const auto &item : items)
                futures.push_back(submit(func, item));
            return futures;
        }

        /**
         * @brief 等待所有已提交的任务完成。
         * @details 阻塞当前线程，直到任务队列为空且所有正在执行的任务都已完成。
         *          可用于需要等待所有后台任务完成后再继续执行的场景。
         */
        void wait() {
            std::unique_lock<std::mutex> lock(mutex);
            cv.wait(lock, [this] {
                return tasks.empty()
                       && active_tasks.load(std::memory_order_acquire) == 0;
            });
        }

        /**
         * @brief 获取当前任务队列中的任务数量。
         * @return size_t 等待执行的任务数量（不包括正在执行的任务）。
         * @details
         * 返回尚未被工作线程取走的任务数量。注意：正在执行的任务不计入队列大小。
         */
        size_t size() const {
            std::lock_guard<std::mutex> lock(mutex);
            return tasks.size();
        }

        /**
         * @brief 获取当前正在执行的任务数量。
         * @return size_t 正在执行的任务数量。
         * @details 返回已经被工作线程取走但尚未执行完成的任务数量。
         */
        size_t count() const {
            return active_tasks.load(std::memory_order_acquire);
        }

        /**
         * @brief 优雅关闭线程池，等待所有任务完成。
         * @details
         * 设置关闭标志，等待所有已提交的任务执行完毕，然后等待所有工作线程退出。
         *          该方法会等待任务队列中的所有任务完成后再关闭线程池，调用后线程池将无法再提交新任务。
         */
        void close() {
            shutdown.store(true, std::memory_order_release);
            cv.notify_all();
            wait();
            for (auto &worker : workers)
                if (worker.joinable()) worker.join();
        }

        /**
         * @brief 粗暴地关闭线程池，用于需要立刻结束一切。
         * @details
         * 设置关闭标志，不等待已提交的任务，然后分离所有工作线程并退出。
         *          最好不要直接调用，除非必须，调用后线程池将无法再提交新任务。
         */
        void abort() {
            shutdown.store(true, std::memory_order_release);
            {
                std::lock_guard<std::mutex> lock(mutex);
                std::queue<std::unique_ptr<TaskBase>>().swap(tasks);
            }
            cv.notify_all();
            for (auto &worker : workers)
                if (worker.joinable()) worker.detach();
        }

        /// @brief 禁止拷贝构造。
        ThreadPool(const ThreadPool &) = delete;
        /// @brief 禁止拷贝赋值。
        ThreadPool &operator=(const ThreadPool &) = delete;
        /// @brief 禁止移动构造。
        ThreadPool(ThreadPool &&) = delete;
        /// @brief 禁止移动赋值。
        ThreadPool &operator=(ThreadPool &&) = delete;
    };

    /**
     * @brief 全局线程池。
     * @details 内部线程池采用 abort 退出策略，程序退出时不会等待任务完成。
     */
    inline ThreadPool &global_thread_pool() {
        static ThreadPool exe(ThreadPool::launch::abort);
        return exe;
    }

    /**
     * @brief 提交一个可调用对象到全局线程池执行。
     * @tparam F 可调用对象的类型。
     * @tparam Args 参数包的类型。
     * @param f 要执行的可调用对象。
     * @param args 传递给可调用对象的参数。
     * @return
     * std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
     *         与任务关联的 future 对象，用于获取返回值。
     * @throw ThreadPoolError 如果线程池正在关闭，则抛出异常。
     */
    template <class F, class... Args>
    inline auto
    gtp_submit(F &&f, Args &&...args) -> std::future<decltype(std::forward<F>(
                                          f)(std::forward<Args>(args)...))> {
        return global_thread_pool().submit(
            std::forward<F>(f), std::forward<Args>(args)...);
    }

    /**
     * @brief 批量提交任务到全局线程池，对容器中的每个元素应用函数。
     * @tparam F 可调用对象的类型。
     * @tparam Container 容器类型，必须包含 value_type 成员类型。
     * @param func 要应用于每个元素的函数。
     * @param items 包含输入元素的容器。
     * @return std::vector<std::future<decltype(func(std::declval<typename
     * Container::value_type>()))>> 包含所有任务 future 对象的
     * vector，顺序与输入容器一致。
     * @throw ThreadPoolError 如果线程池正在关闭，则抛出异常。
     * @details 对容器中的每个元素调用 submit(func, item)，将所有返回的 future
     * 对象 收集到 vector 中返回。可用于并行处理集合中的元素。
     */
    template <class F, class Container>
    inline auto gtp_map(F &&func, const Container &items)
        -> std::vector<std::future<decltype(func(
            std::declval<typename Container::value_type>()))>> {
        return global_thread_pool().map(func, items);
    }

    /**
     * @brief 等待全局进程池所有已提交的任务完成。
     * @details 阻塞当前线程，直到任务队列为空且所有正在执行的任务都已完成。
     *          可用于需要等待所有后台任务完成后再继续执行的场景。
     */
    inline void gtp_wait() {
        global_thread_pool().wait();
    }

    /**
     * @brief 获取全局对象池当前任务队列中的任务数量。
     * @return size_t 等待执行的任务数量（不包括正在执行的任务）。
     * @details
     * 返回尚未被工作线程取走的任务数量。注意：正在执行的任务不计入队列大小。
     */
    inline size_t gtp_size() {
        return global_thread_pool().size();
    }

    /**
     * @brief 获取全局对象池当前正在执行的任务数量。
     * @return size_t 正在执行的任务数量。
     * @details 返回已经被工作线程取走但尚未执行完成的任务数量。
     */
    inline size_t gtp_count() {
        return global_thread_pool().count();
    }

    /**
     * @brief 提交一个可调用对象到全局线程池执行。
     * @tparam F 可调用对象的类型。
     * @tparam Args 参数包的类型。
     * @param f 要执行的可调用对象。
     * @param args 传递给可调用对象的参数。
     * @return
     * std::future<decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
     *         与任务关联的 future 对象，用于获取返回值。
     * @throw ThreadPoolError 如果线程池正在关闭，则抛出异常。
     * @note 等价于 gtp_submit，模拟 std::async 风格。
     * @see std::async
     */
    template <class F, class... Args>
    inline auto
    async(F &&f, Args &&...args) -> std::future<decltype(std::forward<F>(f)(
                                     std::forward<Args>(args)...))> {
        return gtp_submit(std::forward<F>(f), std::forward<Args>(args)...);
    }
}
