#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

#include "../time.h"

namespace console {
    /**
     * @class Scheduler
     * @brief 基于条件变量的简单调度器，用于延迟执行或周期性的后台任务。
     * @note 通过条件变量和取消标志实现唤醒与取消，依赖 std::mutex 保护共享状态。
     * @warning 请不要做这些事情：
     *          - 在提供给 schedule 或者 interval 的函数中抛出异常 -> terminate
     *          - 提供一个可能长时间运行的函数给 interval -> 调度间隔错误
     *          - 在定时器函数中指涉一个可能被提前销毁的对象 -> 悬挂
     */
    class Scheduler {
        std::mutex              mut; ///< 保护共享状态的互斥锁
        std::condition_variable cv;  ///< 用于唤醒/取消等待线程的条件变量
        std::atomic<bool>       canceled{
            false}; ///< 取消标志，置位后所有等待线程立即唤醒
        std::vector<std::thread> threads; ///< interval() 创建的持续运行线程集合

    public:
        /**
         * @brief 在指定延时后执行一次函数。
         * @tparam Rep 计时的刻度类型。
         * @tparam Period std::ratio 类型，表示时间刻度与秒的比例。
         * @tparam Func 可调用对象类型。
         * @param duration 延时的时间长度，从调用时刻起算。
         * @param func 延时结束后要执行的函数。
         * @note 分离线程，调用方不需要也不能 join。
         * @warning 若函数执行时间超过延时，cancel_all() 无法中断其运行，只能阻止其启动。
         */
        template <class Rep, class Period, class Func>
        void schedule(
            const std::chrono::duration<Rep, Period> &duration, Func &&func) {
            std::thread(
                [this](std::chrono::duration<Rep, Period> duration, Func func) {
                    auto deadline = std::chrono::steady_clock::now() + duration;
                    std::unique_lock<std::mutex> lock(mut);
                    bool interrupted = cv.wait_until(lock, deadline, [this] {
                        return canceled.load(std::memory_order_acquire);
                    });
                    if (!interrupted) {
                        lock.unlock();
                        func();
                    }
                },
                duration,
                std::forward<Func>(func))
                .detach();
        }

        /**
         * @brief 便捷重载：以 console::Time 指定延时后执行一次函数。
         * @param duration 延时长度（以纳秒计，内部转换为 std::chrono::nanoseconds）。
         * @param func 延时结束后要执行的函数。
         */
        template <class Func>
        void schedule(Time duration, Func &&func) {
            schedule(std::chrono::nanoseconds(duration.count()),
                std::forward<Func>(func));
        }

        /**
         * @brief 便捷重载：以秒为单位指定延时后执行一次函数。
         * @param seconds 延时秒数（可含小数）。
         * @param func 延时结束后要执行的函数。
         * @note 内部将秒转换为整数纳秒后转发，精度截断到纳秒。
         */
        template <class Func>
        void schedule(double seconds, Func &&func) {
            schedule(std::chrono::duration_cast<std::chrono::nanoseconds>(
                         std::chrono::duration<double>(seconds)),
                std::forward<Func>(func));
        }

        /**
         * @brief 按照固定间隔周期性地执行函数。
         * @tparam Rep 计时的刻度类型。
         * @tparam Period std::ratio 类型，表示时间刻度与秒的比例。
         * @tparam Func 可调用对象类型。
         * @param duration 两次执行之间的时间间隔。
         * @param func 每个周期要执行的函数。
         * @note 线程被记录在 threads 中并由析构函数统一 join，因此不会泄漏线程。
         *       每个周期等待结束后先解除锁再做 func()，避免长时间持锁。
         */
        template <class Rep, class Period, class Func>
        void interval(
            const std::chrono::duration<Rep, Period> &duration, Func &&func) {
            mut.lock();
            threads.push_back(std::thread(
                [this](std::chrono::duration<Rep, Period> duration, Func func) {
                    std::unique_lock<std::mutex> lock(mut, std::adopt_lock);
                    auto deadline = std::chrono::steady_clock::now();
                    while (true) {
                        deadline += duration;
                        bool interrupted = cv.wait_until(
                            lock, deadline, [this] {
                                return canceled.load(std::memory_order_acquire);
                            });
                        if (interrupted) break;
                        lock.unlock();
                        func();
                        lock.lock();
                    }
                },
                duration,
                std::forward<Func>(func)));
        }

        /**
         * @brief 便捷重载：以 console::Time 指定间隔周期性地执行函数。
         * @param duration 两次执行之间的时间间隔（以纳秒计）。
         * @param func 每个周期要执行的函数。
         */
        template <class Func>
        void interval(Time duration, Func &&func) {
            interval(std::chrono::nanoseconds(duration.count()),
                std::forward<Func>(func));
        }

        /**
         * @brief 便捷重载：以秒为单位指定间隔周期性地执行函数。
         * @param seconds 两次执行之间的间隔秒数（可含小数）。
         * @param func 每个周期要执行的函数。
         * @note 内部将秒转换为整数纳秒后转发，精度截断到纳秒。
         */
        template <class Func>
        void interval(double seconds, Func &&func) {
            interval(std::chrono::duration_cast<std::chrono::nanoseconds>(
                         std::chrono::duration<double>(seconds)),
                std::forward<Func>(func));
        }

        /**
         * @brief 取消所有待执行 / 正在等待的任务，并唤醒所有等待线程。
         * @note 通过 release 语义写 canceled，随后 notify_all 唤醒所有阻塞线程，
         *       使其谓词判断到取消标志并立即返回。
         */
        void cancel_all() {
            canceled.store(true, std::memory_order_release);
            cv.notify_all();
        }

        /// @brief 析构函数，先取消所有任务，再等待所有 interval 线程退出。
        ~Scheduler() {
            cancel_all();
            for (auto &thread : threads)
                if (thread.joinable()) thread.join();
        }
    };
}
