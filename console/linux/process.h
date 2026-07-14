/**
 * @file linux/process.h
 * @brief Linux 平台进程管理库，支持进程创建、控制、等待和信息查询。
 * @details 封装 fork/waitpid/kill 等系统调用，提供 OOP 风格的进程管理接口。
 * @warning 仅支持 Linux 平台。
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

#ifndef __linux__
#error "Unsupported platform - this library is for Linux only"
#endif

#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

namespace console {
    /**
     * @enum Signal
     * @brief 常用信号枚举。
     */
    enum class Signal : int {
        Interrupt = SIGINT,  ///< 中断信号 (Ctrl+C)
        Terminate = SIGTERM, ///< 终止信号
        Kill      = SIGKILL, ///< 强制杀死信号
        Stop      = SIGSTOP, ///< 停止进程信号
        Continue  = SIGCONT, ///< 继续运行信号
        Hangup    = SIGHUP,  ///< 挂起信号
        User1     = SIGUSR1, ///< 用户自定义信号1
        User2     = SIGUSR2  ///< 用户自定义信号2
    };

    /**
     * @struct ExitStatus
     * @brief 进程退出状态，包含完整退出信息。
     */
    struct ExitStatus {
        int  code;        ///< 退出码（正常退出时有效）
        bool signaled;    ///< 是否由信号终止
        int  signal;      ///< 终止信号的编号
        bool core_dumped; ///< 是否产生 core dump

        /**
         * @brief 构造函数，初始化所有成员为默认值。
         */
        ExitStatus() :
            code(0), signaled(false), signal(0), core_dumped(false) {}

        /**
         * @brief 检查进程是否正常退出。
         * @return true 正常退出，false 被信号终止。
         */
        bool normal() const { return !signaled; }

        /**
         * @brief 获取退出码。
         * @return 进程的退出码。
         */
        int exit_code() const { return code; }

        /**
         * @brief 获取终止信号。
         * @return 信号的编号。
         */
        int term_signal() const { return signal; }
    };

    /**
     * @struct ProcessInfo
     * @brief 进程详细信息。
     */
    struct ProcessInfo {
        pid_t  pid;      ///< 进程 ID
        pid_t  ppid;     ///< 父进程 ID
        long   rss_kb;   ///< 常驻内存大小 (KB)
        double cpu_time; ///< CPU 使用时间 (秒)
        bool   running;  ///< 进程是否在运行

        /**
         * @brief 构造函数，初始化所有成员为默认值。
         */
        ProcessInfo() :
            pid(0), ppid(0), rss_kb(0), cpu_time(0), running(false) {}
    };

    /**
     * @class Process
     * @brief 进程控制器，支持创建、等待、控制、信息查询。
     */
    class Process {
    private:
        pid_t pid_; ///< 被管理的进程 ID

        /**
         * @brief 私有构造函数，通过 pid 创建 Process 对象。
         * @param pid 进程 ID。
         */
        explicit Process(pid_t pid) : pid_(pid) {}

        /**
         * @brief 向指定进程发送信号。
         * @param pid 目标进程 ID。
         * @param sig 信号编号。
         * @return 成功返回 0，失败返回 -1。
         */
        static int sys_kill(pid_t pid, int sig) { return ::kill(pid, sig); }

    public:
        /**
         * @brief 无效进程对象常量。
         */
        static Process invalid() { return Process(-1); }

        /**
         * @brief 默认构造函数，创建无效进程对象。
         */
        Process() : pid_(-1) {}

        /**
         * @brief 析构函数。
         */
        ~Process() = default;

        /**
         * @brief 拷贝构造函数（删除）。
         */
        Process(const Process &) = delete;

        /**
         * @brief 拷贝赋值运算符（删除）。
         */
        Process &operator=(const Process &) = delete;

        /**
         * @brief 移动构造函数。
         * @param other 要移动的 Process 对象。
         */
        Process(Process &&other) noexcept : pid_(other.pid_) {
            other.pid_ = -1;
        }

        /**
         * @brief 移动赋值运算符。
         * @param other 要移动的 Process 对象。
         * @return 当前对象的引用。
         */
        Process &operator=(Process &&other) noexcept {
            if (this != &other) {
                pid_       = other.pid_;
                other.pid_ = -1;
            }
            return *this;
        }

        /**
         * @brief 获取进程 ID。
         * @return 进程 ID。
         */
        pid_t pid() const { return pid_; }

        /**
         * @brief 检查进程对象是否有效。
         * @return true 有效，false 无效。
         */
        bool valid() const { return pid_ > 0; }

        /**
         * @brief 经典 fork - 复制当前进程。
         * @return 父进程返回子进程对象，子进程返回空对象（pid=0）。
         */
        static Process fork() {
            pid_t pid = ::fork();
            if (pid == 0) return Process(0);
            if (pid > 0) return Process(pid);
            return Process(-1);
        }

        /**
         * @brief 创建子进程执行指定函数。
         * @param fn 子进程执行的函数指针。
         * @param arg 传递给函数的参数。
         * @return 父进程返回子进程对象，失败返回 invaild()。
         */
        static Process spawn(int (*fn)(void *), void *arg) {
            pid_t pid = ::fork();
            if (pid == -1) return Process(-1);
            if (pid == 0) {
                int ret = fn(arg);
                std::exit(ret);
            }
            return Process(pid);
        }

        /**
         * @brief 发射后不管模式创建子进程。
         * @param fn 子进程执行的函数指针。
         * @param arg 传递给函数的参数。
         * @return 父进程返回子进程对象，失败返回 invaild()。
         * @note 与 spawn 功能相同，命名区分使用场景。
         */
        static Process spawn_detach(int (*fn)(void *), void *arg) {
            pid_t pid = ::fork();
            if (pid == -1) return Process(-1);
            if (pid == 0) {
                int ret = fn(arg);
                std::exit(ret);
            }
            return Process(pid);
        }

        /**
         * @brief 获取当前进程对象。
         * @return 当前进程的 Process 对象。
         */
        static Process self() { return Process(getpid()); }

        /**
         * @brief 获取父进程对象。
         * @return 父进程的 Process 对象。
         */
        static Process parent() { return Process(getppid()); }

        /**
         * @brief 等待子进程退出（阻塞）。
         * @return 子进程的 ExitStatus 状态信息。
         */
        ExitStatus wait() const {
            ExitStatus status;
            int        raw;
            if (waitpid(pid_, &raw, 0) == -1) return status;
            if (WIFEXITED(raw)) {
                status.code     = WEXITSTATUS(raw);
                status.signaled = false;
            } else if (WIFSIGNALED(raw)) {
                status.signaled    = true;
                status.signal      = WTERMSIG(raw);
                status.core_dumped = WCOREDUMP(raw);
            }
            return status;
        }

        /**
         * @brief 非阻塞查询子进程退出状态。
         * @param status 输出参数，存储退出状态信息。
         * @return 如果进程已退出返回 true，否则返回 false。
         */
        bool poll(ExitStatus *status) const {
            int   raw;
            pid_t ret = waitpid(pid_, &raw, WNOHANG);
            if (ret == pid_) {
                if (WIFEXITED(raw)) {
                    status->code     = WEXITSTATUS(raw);
                    status->signaled = false;
                } else if (WIFSIGNALED(raw)) {
                    status->signaled    = true;
                    status->signal      = WTERMSIG(raw);
                    status->core_dumped = WCOREDUMP(raw);
                }
                return true;
            }
            return false;
        }

        /**
         * @brief 等待任意子进程退出（阻塞）。
         * @param status 输出参数，存储退出状态信息。
         * @return 退出的子进程对象。
         */
        static Process wait_any(ExitStatus *status) {
            int   raw;
            pid_t pid = ::wait(&raw);
            if (pid == -1) return Process(-1);
            if (WIFEXITED(raw)) {
                status->code     = WEXITSTATUS(raw);
                status->signaled = false;
            } else if (WIFSIGNALED(raw)) {
                status->signaled    = true;
                status->signal      = WTERMSIG(raw);
                status->core_dumped = WCOREDUMP(raw);
            }
            return Process(pid);
        }

        /**
         * @brief 检查进程是否存活。
         * @return true 存活，false 已退出或无效。
         */
        bool is_alive() const {
            if (pid_ <= 0) return false;
            return sys_kill(pid_, 0) == 0;
        }

        /**
         * @brief 向进程发送信号。
         * @param sig 要发送的信号。
         * @return 成功返回 true，失败返回 false。
         */
        bool send_signal(Signal sig) const {
            return sys_kill(pid_, static_cast<int>(sig)) == 0;
        }

        /**
         * @brief 向进程发送 SIGTERM 信号请求终止。
         * @return 成功返回 true，失败返回 false。
         */
        bool terminate() const { return sys_kill(pid_, SIGTERM) == 0; }

        /**
         * @brief 向进程发送 SIGKILL 信号强制杀死。
         * @return 成功返回 true，失败返回 false。
         */
        bool kill() const { return sys_kill(pid_, SIGKILL) == 0; }

        /**
         * @brief 向进程发送 SIGSTOP 信号暂停执行。
         * @return 成功返回 true，失败返回 false。
         */
        bool stop() const { return sys_kill(pid_, SIGSTOP) == 0; }

        /**
         * @brief 向进程发送 SIGCONT 信号恢复执行。
         * @return 成功返回 true，失败返回 false。
         */
        bool resume() const { return sys_kill(pid_, SIGCONT) == 0; }

        /**
         * @brief 获取进程详细信息。
         * @return ProcessInfo 结构体包含 PID、PPID、内存、CPU 时间等。
         */
        ProcessInfo get_info() const {
            ProcessInfo info;
            info.pid     = pid_;
            info.running = is_alive();
            char path[64];
            snprintf(path, sizeof(path), "/proc/%d/stat", pid_);
            FILE *fp = fopen(path, "r");
            if (fp) {
                unsigned long utime, stime;
                int           ppid;
                fscanf(fp,
                    "%d %*s %*c %d %*d %*d %*d %*d %*d %*d %*d %*d %*d %lu %lu",
                    &info.pid,
                    &ppid,
                    &utime,
                    &stime);
                info.ppid     = ppid;
                info.cpu_time = (utime + stime) / (double)sysconf(_SC_CLK_TCK);
                fclose(fp);
            }
            snprintf(path, sizeof(path), "/proc/%d/statm", pid_);
            fp = fopen(path, "r");
            if (fp) {
                long resident;
                if (fscanf(fp, "%*d %ld", &resident) == 1)
                    info.rss_kb = resident * 4;
                fclose(fp);
            }
            return info;
        }

        /**
         * @brief 获取进程的父进程 ID。
         * @return 父进程 ID，失败返回 -1。
         */
        pid_t ppid() const {
            char path[64];
            snprintf(path, sizeof(path), "/proc/%d/stat", pid_);
            FILE *fp = fopen(path, "r");
            if (!fp) return -1;
            pid_t ppid;
            fscanf(fp, "%*d %*s %*c %d", &ppid);
            fclose(fp);
            return ppid;
        }

        /**
         * @brief 将当前进程守护化为后台服务进程。
         * @return 成功返回 true，失败返回 false。
         * @note 守护化后标准输入/输出/错误会重定向到 /dev/null。
         */
        static bool daemonize() {
            pid_t pid = ::fork();
            if (pid < 0) return false;
            if (pid > 0) std::exit(0);
            if (setsid() < 0) return false;
            pid = ::fork();
            if (pid < 0) return false;
            if (pid > 0) std::exit(0);
            chdir("/");
            close(STDIN_FILENO);
            close(STDOUT_FILENO);
            close(STDERR_FILENO);
            open("/dev/null", O_RDONLY);
            open("/dev/null", O_WRONLY);
            open("/dev/null", O_WRONLY);
            return true;
        }
    };
}
