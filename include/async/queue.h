/**
 * @file async/queue.h
 * @brief 基于自旋的高性能 FIFO 队列，支持多生产者/多消费者并发场景。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 * @note 相信我，这真的很快 :-)
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
#include <memory>
#include <thread>
#include <utility>
#include <vector>

namespace console {
    /**
     * @class LFQueue
     * @brief 基于自旋的高性能 FIFO 队列，支持多生产者/多消费者并发。
     * @tparam T 队列中元素的类型。
     * @tparam Alloc 分配器类型，默认为 std::allocator<T>。
     * @note 通过原子 exchange 实现轻量级互斥，临界区极短 (~3-10ns)。
     *       提供 Lock-Free 进度保证 (至少一个线程能前进)，但不是 Wait-Free。
     */
    template <class T, class Alloc = std::allocator<T>>
    class LFQueue : private Alloc {
        struct Node {
            T                   data_; ///< 节点数据
            std::atomic<Node *> next_; ///< 指向下一个节点的原子指针

            /// @brief 构造函数，使用数据初始化节点。
            Node(const T &data) : data_(data), next_(nullptr) {}
            /// @brief 构造函数，使用移动数据初始化节点。
            Node(T &&data) : data_(std::move(data)), next_(nullptr) {}
        };

        std::atomic<Node *> head_; ///< 指向队列头部的原子指针
        std::atomic<Node *> tail_; ///< 指向队列尾部的原子指针

        /// @brief 节点分配器类型。
        using NodeAlloc =
            typename std::allocator_traits<Alloc>::template rebind_alloc<Node>;
        /// @brief 节点分配器的特征类型。
        using NodeTraits = std::allocator_traits<NodeAlloc>;

        /// @brief 获取分配器的引用。
        Alloc &get_alloc() { return *this; }

        /// @brief 创建一个新的节点。
        Node *create_node(const T &data) {
            NodeAlloc node_alloc(get_alloc());
            Node     *node = NodeTraits::allocate(node_alloc, 1);
            NodeTraits::construct(node_alloc, node, data);
            return node;
        }

        /// @brief 销毁节点。
        void destroy_node(Node *node) {
            NodeAlloc node_alloc(get_alloc());
            NodeTraits::destroy(node_alloc, node);
            NodeTraits::deallocate(node_alloc, node, 1);
        }

    public:
        /// @brief 默认构造函数，使用默认分配器初始化队列。
        LFQueue() :
            Alloc(Alloc{}), head_(create_node(T())), tail_(head_.load()) {}

        /**
         * @brief 使用指定分配器初始化队列。
         * @param alloc 分配器对象。
         */
        LFQueue(const Alloc &alloc) :
            Alloc(alloc), head_(create_node(T())), tail_(head_.load()) {}

        /**
         * @brief 使用移动分配器初始化队列。
         * @param alloc 分配器对象。
         */
        LFQueue(Alloc &&alloc) :
            Alloc(std::move(alloc)), //
            head_(create_node(T())), tail_(head_.load()) {}

        /**
         * @brief 析构函数，销毁队列中的所有节点。
         * @note 首先独占 head_ 和 tail_ 资源，然后销毁所有节点。
         * @warning 析构后的任何操作都会永久阻塞，直到队列被重新初始化，
         *          这是合理的，因为用户有确保队列析构后不在被使用的义务。
         */
        ~LFQueue() {
            Node *head;
            Node *tail;
            while (!(head = head_.exchange(nullptr, std::memory_order_acq_rel)))
                std::this_thread::yield();
            while (!(tail = tail_.exchange(nullptr, std::memory_order_acq_rel)))
                std::this_thread::yield();
            while (head) {
                Node *next = head->next_.load(std::memory_order_relaxed);
                destroy_node(head);
                head = next;
            }
            (void)tail;
        }

        /**
         * @brief 将数据推送到队列中。
         * @param data 要推送的数据。
         * @note 首先独占 tail_ 资源，然后将新节点链接到队列尾部。
         *       然后释放 tail_ 资源，允许其他线程访问队列。
         */
        void push(const T &data) {
            Node *new_node = create_node(data);
            Node *tail;
            while (!(tail = tail_.exchange(nullptr, std::memory_order_acq_rel)))
                std::this_thread::yield();
            tail->next_.store(new_node, std::memory_order_relaxed);
            tail_.store(new_node, std::memory_order_release);
        }

        /**
         * @brief 将数据推送到队列中。
         * @param data 要推送的数据。
         * @note 首先独占 tail_ 资源，然后将新节点链接到队列尾部。
         *       然后释放 tail_ 资源，允许其他线程访问队列。
         */
        void push(T &&data) {
            Node *new_node = create_node(std::move(data));
            Node *tail;
            while (!(tail = tail_.exchange(nullptr, std::memory_order_acq_rel)))
                std::this_thread::yield();
            tail->next_.store(new_node, std::memory_order_relaxed);
            tail_.store(new_node, std::memory_order_release);
        }

        /**
         * @brief 从队列中弹出数据。
         * @param output 弹出的数据将存储在此处。
         * @return 如果成功弹出数据，则返回 true；否则返回 false。
         * @note 首先独占 head_ 资源，然后获取队列头部的下一个节点。
         *       如果没有下一个节点，则释放 head_ 资源，返回 false。
         *       否则，将数据移动到输出参数中，并释放 head_ 资源，返回 true。
         */
        bool pop(T &output) {
            Node *head;
            while (!(head = head_.exchange(nullptr, std::memory_order_acq_rel)))
                std::this_thread::yield();
            if (!head->next_) {
                head_.store(head, std::memory_order_release);
                return false;
            }
            output
                = std::move(head->next_.load(std::memory_order_relaxed)->data_);
            Node *new_head = head->next_.load(std::memory_order_relaxed);
            head_.store(new_head, std::memory_order_release);
            destroy_node(head);
            return true;
        }

        /**
         * @brief 从队列中弹出数据，并返回一个独占所有权的 std::unique_ptr。
         * @return 如果成功弹出数据，则返回一个包含数据的 std::unique_ptr；否则返回 nullptr。
         * @note 将实际工作委托至 pop(T &)。
         */
        std::unique_ptr<T> pop() {
            std::unique_ptr<T> result(new T{});
            if (pop(*result)) return result;
            return nullptr;
        }
    };

    /**
     * @class MultiLFQueue
     * @brief 多队列版本的 LFQueue，将数据分布到多个独立的 LFQueue 中以提高并发性能。
     * @tparam T 队列中元素的类型。
     * @tparam Alloc 分配器类型，默认为 std::allocator<T>。
     * @note 通过多个子队列减少竞争者数量，降低自旋等待的概率。
     *       推送时使用线程本地索引进行负载均衡，弹出时使用轮询策略遍历所有子队列。
     */
    template <class T, class Alloc = std::allocator<T>>
    class MultiLFQueue {
        /// @brief 子队列集合，每个子队列用于存储不同线程的队列数据。
        std::vector<std::unique_ptr<console::LFQueue<T, Alloc>>> queues_;
        /// @brief 轮询计数器，用于弹出时的负载均衡。
        std::atomic<size_t> round_robin_{0};
        /// @brief 序号计数器，用于推送时的线程索引分配。
        std::atomic<size_t> nth_{0};

    public:
        /**
         * @brief 构造函数，初始化指定数量的子队列。
         * @param num_queues 子队列的数量。
         */
        MultiLFQueue(size_t num_queues) {
            for (size_t i = 0; i < num_queues; ++i)
                queues_.emplace_back(new console::LFQueue<T, Alloc>());
        }

        /**
         * @brief 将数据推送到队列中。
         * @param data 要推送的数据。
         * @note 使用线程本地存储 (thread_local) 为每个线程分配一个固定的子队列索引，
         *       从而减少多个生产者争抢同一队列的自旋开销。
         */
        void push(T &&data) {
            thread_local size_t idx = nth_.fetch_add(1) % queues_.size();
            queues_[idx]->push(std::move(data));
        }

        /**
         * @brief 将数据推送到队列中。
         * @param data 要推送的数据。
         * @note 将实际工作委托至 push(T &&)。
         */
        void push(const T &data) { push(T(data)); }

        /**
         * @brief 从队列中弹出数据。
         * @param output 弹出的数据将存储在此处。
         * @return 如果成功弹出数据，则返回 true；否则返回 false。
         * @note 使用轮询计数器 round_robin_ 选择起始子队列，然后依次遍历所有子队列，
         *       直到找到一个非空的子队列并成功弹出数据。如果所有子队列均为空，则返回 false。
         *       可能佯败，如果在询问过一个子队列之后改子队列才被 push，则发生佯败。
         */
        bool pop(T &output) {
            size_t start = round_robin_.fetch_add(1, std::memory_order_relaxed);
            for (size_t i = 0; i < queues_.size(); ++i) {
                size_t idx = (start + i) % queues_.size();
                if (queues_[idx]->pop(output)) return true;
            }
            return false;
        }

        /**
         * @brief 从队列中弹出数据，并返回一个独占所有权的 std::unique_ptr。
         * @return 如果成功弹出数据，则返回一个包含数据的 std::unique_ptr；否则返回 nullptr。
         * @note 将实际工作委托至 pop(T &)。
         */
        std::unique_ptr<T> pop() {
            std::unique_ptr<T> result(new T{});
            if (pop(*result)) return result;
            return nullptr;
        }
    };
}
