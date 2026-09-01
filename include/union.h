/**
 * @file union.h
 * @brief 提供类型安全的可判别联合体 (Tagged Union)。
 * @details 包含模板类 Union，支持存储多种类型中的一种，并提供类型安全的访问和访问者模式。
 *          所有类型必须是完整的、可析构的。若构造/拷贝/移动过程中抛出异常，将调用 std::terminate。
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
#include <cstddef>
#include <cstring>
#include <exception>
#include <new>
#include <type_traits>
#include <utility>

namespace console {
    /**
     * @class Union
     * @brief 类型安全的可判别联合体。
     * @details 类似于 std::variant，但实现方式更底层。
     *          在任何时刻，Union 要么为空 (默认构造后)，要么存储 Ts... 中某一类型的值。
     *          所有操作均基于函数指针表实现，以减小代码体积。
     * @tparam Ts 允许存储的类型列表，不能包含重复类型或 void (除非作为唯一类型)。
     */
    template <class... Ts>
    class Union {
        /**
         * @brief 在类型列表中按索引获取类型。
         * @tparam N 索引 (从 0 开始)。
         * @tparam Types 类型列表。
         */
        template <size_t, class...>
        struct type_at;
        template <class T, class... Rest>
        struct type_at<0, T, Rest...> {
            using type = T;
        };
        template <size_t N, class T, class... Rest>
        struct type_at<N, T, Rest...> {
            using type = typename type_at<N - 1, Rest...>::type;
        };
        template <size_t N, class... Types>
        using type_at_t = typename type_at<N, Types...>::type;

        /**
         * @brief 获取类型 T 在类型列表中的索引。
         * @tparam T 要查找的类型。
         * @tparam Types 类型列表。
         */
        template <class, class...>
        struct type_index;
        template <class T>
        struct type_index<T> {
            static constexpr size_t value = 0;
        };
        template <class T, class U, class... Rest>
        struct type_index<T, U, Rest...> {
            static constexpr bool _is_match
                = std::is_same<typename std::decay<T>::type,
                    typename std::decay<U>::type>::value;
            static constexpr size_t _rest_index = type_index<T, Rest...>::value;
            static constexpr size_t value = _is_match ? 0 : _rest_index + 1;
        };

        /**
         * @brief 编译期计算一组 size_t 中的最大值。
         */
        template <size_t...>
        struct max;
        template <size_t N>
        struct max<N> {
            static constexpr size_t value = N;
        };
        template <size_t N, size_t... Ns>
        struct max<N, Ns...> {
            static constexpr size_t value
                = N > max<Ns...>::value ? N : max<Ns...>::value;
        };

        /**
         * @brief 检查类型列表中是否包含 void。
         */
        template <class...>
        struct has_void;
        template <class T>
        struct has_void<T> {
            static const bool value
                = std::is_same<typename std::decay<T>::type, void>::value;
        };
        template <class T, class... Rest>
        struct has_void<T, Rest...> {
            static const bool value
                = std::is_same<typename std::decay<T>::type, void>::value
                  || has_void<Rest...>::value;
        };

        template <class...>
        friend class Union;

        /// @brief 原始存储区，对齐到所有类型中最严格的要求。
        alignas(max<alignof(Ts)...>::value) //
            char data_[max<sizeof(Ts)...>::value];

        /// @brief 当前存储类型的索引，-1 表示空。
        size_t index_{static_cast<size_t>(-1)};

        /// @brief 析构函数指针。
        void (*deleter_)(void *) = [](void *) {};
        /// @brief 拷贝赋值函数指针。
        void (*copier_)(void *, const void *) = [](void *, const void *) {};
        /// @brief 移动赋值函数指针。
        void (*mover_)(void *, void *) = [](void *, void *) {};
        /// @brief 拷贝构造函数指针。
        void (*cctor_)(void *, const void *) = [](void *, const void *) {};
        /// @brief 移动构造函数指针。
        void (*mctor_)(void *, void *) = [](void *, void *) {};

/**
 * @brief 宏：在异常发生时调用 std::terminate。
 * @details 用于包装构造/拷贝/移动操作，保证强异常安全 (一旦抛出异常则终止程序)。
 */
#define TERMINATE_FOR_EXCEPTION(...)                                           \
    try {                                                                      \
        __VA_ARGS__;                                                           \
    } catch (...) {                                                            \
        std::terminate();                                                      \
    }

        /**
         * @brief 类型特定的析构函数。
         * @tparam T 当前存储的类型。
         * @param ptr 指向对象的指针。
         */
        template <class T>
        static inline void deleter(void *ptr) {
            static_cast<T *>(ptr)->~T();
        }

        /**
         * @brief 类型特定的拷贝赋值函数 (就地拷贝)。
         * @tparam T 当前存储的类型。
         * @param dest 目标指针。
         * @param src 源指针。
         */
        template <class T>
        static inline void copier(void *dest, const void *src) {
            if (dest == src) return;
            static_cast<T *>(dest)->~T();
            TERMINATE_FOR_EXCEPTION(new (dest) T(*static_cast<const T *>(src)));
        }

        /**
         * @brief 类型特定的移动赋值函数 (就地移动)。
         * @tparam T 当前存储的类型。
         * @param dest 目标指针。
         * @param src 源指针。
         */
        template <class T>
        static void mover(void *dest, void *src) {
            if (dest == src) return;
            static_cast<T *>(dest)->~T();
            TERMINATE_FOR_EXCEPTION(
                new (dest) T(std::move(*static_cast<T *>(src))));
        }

        /**
         * @brief 类型特定的拷贝构造函数 (在未初始化的内存上构造)。
         * @tparam T 当前存储的类型。
         * @param dest 目标指针 (未初始化)。
         * @param src 源指针 (已初始化)。
         */
        template <class T>
        static inline void cctor(void *dest, const void *src) {
            TERMINATE_FOR_EXCEPTION(new (dest) T(*static_cast<const T *>(src)));
        }

        /**
         * @brief 类型特定的移动构造函数 (在未初始化的内存上构造)。
         * @tparam T 当前存储的类型。
         * @param dest 目标指针 (未初始化)。
         * @param src 源指针 (已初始化)。
         */
        template <class T>
        static inline void mctor(void *dest, void *src) {
            TERMINATE_FOR_EXCEPTION(
                new (dest) T(std::move(*static_cast<T *>(src))));
        }

        /**
         * @brief 访问者模式：单函数访问器 (有返回值版本，非 const)。
         * @tparam N 当前递归索引。
         * @tparam F 函数对象类型。
         * @tparam Ret 返回值类型。
         * @param f 函数对象。
         * @param r 用于存储返回值的引用。
         */
        template <size_t N, class F, class Ret>
        typename std::enable_if<(N < sizeof...(Ts))>::type
        _visit1(F &&f, Ret &r) {
            if (N == index_)
                r = f(*get<type_at_t<N, Ts...>>());
            else
                _visit1<N + 1>(std::forward<F>(f), r);
        }

        template <size_t N, class F, class Ret>
        typename std::enable_if<(N == sizeof...(Ts))>::type
        _visit1(F &&, Ret &) {}

        /**
         * @brief 访问者模式：单函数访问器 (有返回值版本，const)。
         * @tparam N 当前递归索引。
         * @tparam F 函数对象类型。
         * @tparam Ret 返回值类型。
         * @param f 函数对象。
         * @param r 用于存储返回值的引用。
         */
        template <size_t N, class F, class Ret>
        typename std::enable_if<(N < sizeof...(Ts))>::type
        _visit1(F &&f, Ret &r) const {
            if (N == index_)
                r = f(*get<type_at_t<N, Ts...>>());
            else
                _visit1<N + 1>(std::forward<F>(f), r);
        }

        template <size_t N, class F, class Ret>
        typename std::enable_if<(N == sizeof...(Ts))>::type
        _visit1(F &&, Ret &) const {}

        /**
         * @brief 访问者模式：单函数访问器 (无返回值版本，非 const)。
         * @tparam N 当前递归索引。
         * @tparam F 函数对象类型。
         * @param f 函数对象。
         */
        template <size_t N, class F>
        typename std::enable_if<(N < sizeof...(Ts))>::type _visit2(F &&f) {
            if (N == index_)
                f(*get<type_at_t<N, Ts...>>());
            else
                _visit2<N + 1>(std::forward<F>(f));
        }

        template <size_t N, class F>
        typename std::enable_if<(N == sizeof...(Ts))>::type _visit2(F &&) {}

        /**
         * @brief 访问者模式：单函数访问器 (无返回值版本，const)。
         * @tparam N 当前递归索引。
         * @tparam F 函数对象类型。
         * @param f 函数对象。
         */
        template <size_t N, class F>
        typename std::enable_if<(N < sizeof...(Ts))>::type
        _visit2(F &&f) const {
            if (N == index_)
                f(*get<type_at_t<N, Ts...>>());
            else
                _visit2<N + 1>(std::forward<F>(f));
        }

        template <size_t N, class F>
        typename std::enable_if<(N == sizeof...(Ts))>::type
        _visit2(F &&) const {}

        /**
         * @brief 访问者模式：多函数访问器 (有返回值版本，非 const)。
         * @tparam N 当前递归索引。
         * @tparam Ret 返回值类型。
         * @tparam F 第一个函数对象类型。
         * @tparam Rest 其余函数对象类型。
         * @param r 用于存储返回值的引用。
         * @param f 当前索引对应的函数对象。
         * @param rest 其余函数对象。
         */
        template <size_t N, class Ret, class F, class... Rest>
        typename std::enable_if<(N < sizeof...(Ts))>::type
        _visit3(Ret &r, F &&f, Rest &&...rest) {
            if (N == index_)
                r = f(*get<type_at_t<N, Ts...>>());
            else
                _visit3<N + 1>(r, std::forward<Rest>(rest)...);
        }

        template <size_t N, class Ret, class F, class... Rest>
        typename std::enable_if<(N == sizeof...(Ts))>::type
        _visit3(Ret &, F &&, Rest &&...) {}

        /**
         * @brief 访问者模式：多函数访问器 (有返回值版本，const)。
         * @tparam N 当前递归索引。
         * @tparam Ret 返回值类型。
         * @tparam F 第一个函数对象类型。
         * @tparam Rest 其余函数对象类型。
         * @param r 用于存储返回值的引用。
         * @param f 当前索引对应的函数对象。
         * @param rest 其余函数对象。
         */
        template <size_t N, class Ret, class F, class... Rest>
        typename std::enable_if<(N < sizeof...(Ts))>::type
        _visit3(Ret &r, F &&f, Rest &&...rest) const {
            if (N == index_)
                r = f(*get<type_at_t<N, Ts...>>());
            else
                _visit3<N + 1>(r, std::forward<Rest>(rest)...);
        }

        template <size_t N, class Ret, class F, class... Rest>
        typename std::enable_if<(N == sizeof...(Ts))>::type
        _visit3(Ret &, F &&, Rest &&...) const {}

        /**
         * @brief 访问者模式：多函数访问器 (无返回值版本，非 const)。
         * @tparam N 当前递归索引。
         * @tparam F 第一个函数对象类型。
         * @tparam Rest 其余函数对象类型。
         * @param f 当前索引对应的函数对象。
         * @param rest 其余函数对象。
         */
        template <size_t N, class F, class... Rest>
        typename std::enable_if<(N < sizeof...(Ts))>::type
        _visit4(F &&f, Rest &&...rest) {
            if (N == index_)
                f(*get<type_at_t<N, Ts...>>());
            else
                _visit4<N + 1>(std::forward<Rest>(rest)...);
        }

        template <size_t N>
        typename std::enable_if<(N == sizeof...(Ts))>::type _visit4() {}

        /**
         * @brief 访问者模式：多函数访问器 (无返回值版本，const)。
         * @tparam N 当前递归索引。
         * @tparam F 第一个函数对象类型。
         * @tparam Rest 其余函数对象类型。
         * @param f 当前索引对应的函数对象。
         * @param rest 其余函数对象。
         */
        template <size_t N, class F, class... Rest>
        typename std::enable_if<(N < sizeof...(Ts))>::type
        _visit4(F &&f, Rest &&...rest) const {
            if (N == index_)
                f(*get<type_at_t<N, Ts...>>());
            else
                _visit4<N + 1>(std::forward<Rest>(rest)...);
        }

        template <size_t N>
        typename std::enable_if<(N == sizeof...(Ts))>::type _visit4() const {}

    public:
        /// @brief 默认构造函数，构造一个空的 Union (不存储任何值)。
        Union() = default;

        /// @brief 析构函数，销毁当前存储的值 (若有)。
        ~Union() { deleter_(data_); }

        /**
         * @brief 从值构造 Union。
         * @tparam T 值的类型，必须是 Ts... 中的一种。
         * @param value 要存储的值。
         * @warning 若 T 不在 Ts... 中，则参与重载决议失败 (SFINAE)。
         */
        template <class T,
            class = typename std::enable_if<
                type_index<typename std::decay<T>::type, Ts...>::value
                != sizeof...(Ts)>::type>
        Union(T &&value) :
            index_(type_index<T, Ts...>::value),
            deleter_(&deleter<typename std::decay<T>::type>),
            copier_(&copier<typename std::decay<T>::type>),
            mover_(&mover<typename std::decay<T>::type>), //
            cctor_(&cctor<typename std::decay<T>::type>), //
            mctor_(&mctor<typename std::decay<T>::type>) {
            new (data_) typename std::decay<T>::type(std::forward<T>(value));
        }

        /**
         * @brief 赋值运算符，从值赋值。
         * @tparam T 值的类型，必须是 Ts... 中的一种。
         * @param value 要存储的值。
         * @return Union & 自身引用。
         * @warning 若 T 不在 Ts... 中，则参与重载决议失败 (SFINAE)。
         */
        template <class T,
            class = typename std::enable_if<
                type_index<typename std::decay<T>::type, Ts...>::value
                != sizeof...(Ts)>::type>
        Union &operator=(T &&value) {
            if (!empty()) deleter_(data_);
            TERMINATE_FOR_EXCEPTION(new (data_)
                    typename std::decay<T>::type(std::forward<T>(value)));
            index_   = type_index<T, Ts...>::value;
            deleter_ = &deleter<typename std::decay<T>::type>;
            copier_  = &copier<typename std::decay<T>::type>;
            mover_   = &mover<typename std::decay<T>::type>;
            cctor_   = &cctor<typename std::decay<T>::type>;
            mctor_   = &mctor<typename std::decay<T>::type>;
            return *this;
        }

        /**
         * @brief 拷贝构造函数。
         * @param other 要拷贝的 Union。
         */
        Union(const Union &other) :
            index_(other.index_), deleter_(other.deleter_),
            copier_(other.copier_), mover_(other.mover_), //
            cctor_(other.cctor_), mctor_(other.mctor_) {
            cctor_(data_, other.data_);
        }

        /**
         * @brief 移动构造函数。
         * @param other 要移动的 Union。
         */
        Union(Union &&other) :
            index_(other.index_), deleter_(other.deleter_),
            copier_(other.copier_), mover_(other.mover_), //
            cctor_(other.cctor_), mctor_(other.mctor_) {
            mctor_(data_, other.data_);
            other.index_ = static_cast<size_t>(-1);
        }

        /**
         * @brief 拷贝赋值运算符。
         * @param other 要拷贝的 Union。
         * @return Union & 自身引用。
         */
        Union &operator=(const Union &other) {
            if (this == &other) return *this;
            if (empty())
                cctor_(data_, other.data_);
            else
                copier_(data_, other.data_);
            index_   = other.index_;
            deleter_ = other.deleter_;
            copier_  = other.copier_;
            mover_   = other.mover_;
            cctor_   = other.cctor_;
            mctor_   = other.mctor_;
            return *this;
        }

        /**
         * @brief 移动赋值运算符。
         * @param other 要移动的 Union。
         * @return Union & 自身引用。
         */
        Union &operator=(Union &&other) {
            if (this == &other) return *this;
            if (empty())
                mctor_(data_, other.data_);
            else
                mover_(data_, other.data_);
            index_       = other.index_;
            deleter_     = other.deleter_;
            copier_      = other.copier_;
            mover_       = other.mover_;
            cctor_       = other.cctor_;
            mctor_       = other.mctor_;
            other.index_ = static_cast<size_t>(-1);
            return *this;
        }

        /**
         * @brief 按类型获取存储的值的指针。
         * @tparam T 要获取的类型。
         * @return T* 若当前存储的类型匹配 T，则返回指向值的指针；否则返回 nullptr。
         */
        template <class T>
        T *get() {
            return type_index<T, Ts...>::value == index_
                       ? reinterpret_cast<T *>(data_)
                       : nullptr;
        }

        /**
         * @brief 按类型获取存储的值的常量指针。
         * @tparam T 要获取的类型。
         * @return const T* 若当前存储的类型匹配 T，则返回指向值的常量指针；否则返回 nullptr。
         */
        template <class T>
        const T *get() const {
            return type_index<T, Ts...>::value == index_
                       ? reinterpret_cast<const T *>(data_)
                       : nullptr;
        }

        /**
         * @brief 使用单个函数对象访问存储的值 (有返回值版本，非 const)。
         * @tparam F 函数对象类型，必须能接受 Ts... 中的任意类型作为参数。
         * @param f 函数对象。
         * @return 所有重载返回值的公共类型 (std::common_type)。
         * @note 若所有重载的返回值均为 void，则匹配无返回值版本的重载。
         *       若对象为空 (由默认构造函数构造)，则调用 std::terminate。
         */
        template <class F>
        auto visit(F &&f) -> typename std::enable_if<
            !has_void<decltype(f(std::declval<Ts &>()))...>::value,
            typename std::common_type<decltype(f(
                std::declval<Ts &>()))...>::type>::type {
            if (empty()) std::terminate();
            typename std::common_type< //
                decltype(f(std::declval<Ts &>()))...>::type result;
            _visit1<0>(std::forward<F>(f), result);
            return result;
        }

        /**
         * @brief 使用单个函数对象访问存储的值 (有返回值版本，const)。
         * @tparam F 函数对象类型，必须能接受 Ts... 中的任意类型作为参数。
         * @param f 函数对象。
         * @return 所有重载返回值的公共类型 (std::common_type)。
         * @note 若所有重载的返回值均为 void，则匹配无返回值版本的重载。
         *       若对象为空 (由默认构造函数构造)，则调用 std::terminate。
         */
        template <class F>
        auto visit(F &&f) const -> typename std::enable_if<
            !has_void<decltype(f(std::declval<const Ts &>()))...>::value,
            typename std::common_type<decltype(f(
                std::declval<const Ts &>()))...>::type>::type {
            if (empty()) std::terminate();
            typename std::common_type< //
                decltype(f(std::declval<const Ts &>()))...>::type result;
            _visit1<0>(std::forward<F>(f), result);
            return result;
        }

        /**
         * @brief 使用单个函数对象访问存储的值 (无返回值版本，非 const)。
         * @tparam F 函数对象类型，必须能接受 Ts... 中的任意类型作为参数，且任一重载返回 void。
         * @param f 函数对象。
         * @note 若对象为空 (由默认构造函数构造)，则调用 std::terminate。
         */
        template <class F>
        auto visit(F &&f) -> typename std::enable_if<
            has_void<decltype(f(std::declval<Ts &>()))...>::value>::type {
            if (empty()) std::terminate();
            _visit2<0>(std::forward<F>(f));
        }

        /**
         * @brief 使用单个函数对象访问存储的值 (无返回值版本，const)。
         * @tparam F 函数对象类型，必须能接受 Ts... 中的任意类型作为参数，且任一重载返回 void。
         * @param f 函数对象。
         * @note 若对象为空 (由默认构造函数构造)，则调用 std::terminate。
         */
        template <class F>
        auto visit(F &&f) const -> typename std::enable_if<
            has_void<decltype(f(std::declval<const Ts &>()))...>::value>::type {
            if (empty()) std::terminate();
            _visit2<0>(std::forward<F>(f));
        }

        /**
         * @brief 使用多个函数对象访问存储的值 (有返回值版本，非 const)。
         * @details 每个类型对应一个函数对象，按索引匹配。
         * @tparam Fs 函数对象类型列表，数量必须与 Ts... 一致。
         * @param fs 函数对象列表。
         * @return 所有函数对象返回值的公共类型 (std::common_type)。
         * @note 若任一函数对象返回 void，则匹配无返回值版本的重载。
         *       若对象为空 (由默认构造函数构造)，则调用 std::terminate。
         */
        template <class... Fs>
        auto visit(Fs &&...fs) -> typename std::enable_if<
            !has_void<decltype(fs(std::declval<Ts &>()))...>::value,
            typename std::common_type<decltype(fs(
                std::declval<Ts &>()))...>::type>::type {
            static_assert(sizeof...(Fs) == sizeof...(Ts),
                "Number of functors does not match number of types!");
            if (empty()) std::terminate();
            typename std::common_type< //
                decltype(fs(std::declval<Ts &>()))...>::type result;
            _visit3<0>(result, std::forward<Fs>(fs)...);
        }

        /**
         * @brief 使用多个函数对象访问存储的值 (有返回值版本，const)。
         * @details 每个类型对应一个函数对象，按索引匹配。
         * @tparam Fs 函数对象类型列表，数量必须与 Ts... 一致。
         * @param fs 函数对象列表。
         * @return 所有函数对象返回值的公共类型 (std::common_type)。
         * @note 若任一函数对象返回 void，则匹配无返回值版本的重载。
         *       若对象为空 (由默认构造函数构造)，则调用 std::terminate。
         */
        template <class... Fs>
        auto visit(Fs &&...fs) const -> typename std::enable_if<
            !has_void<decltype(fs(std::declval<const Ts &>()))...>::value,
            typename std::common_type<decltype(fs(
                std::declval<const Ts &>()))...>::type>::type {
            static_assert(sizeof...(Fs) == sizeof...(Ts),
                "Number of functors does not match number of types!");
            if (empty()) std::terminate();
            typename std::common_type< //
                decltype(fs(std::declval<const Ts &>()))...>::type result;
            _visit3<0>(result, std::forward<Fs>(fs)...);
        }

        /**
         * @brief 使用多个函数对象访问存储的值 (无返回值版本，非 const)。
         * @details 每个类型对应一个函数对象，按索引匹配。
         * @tparam Fs 函数对象类型列表，数量必须与 Ts... 一致，且任一重载返回 void。
         * @param fs 函数对象列表。
         * @note 若对象为空 (由默认构造函数构造)，则调用 std::terminate。
         */
        template <class... Fs>
        auto visit(Fs &&...fs) -> typename std::enable_if<
            has_void<decltype(fs(std::declval<Ts &>()))...>::value>::type {
            static_assert(sizeof...(Fs) == sizeof...(Ts),
                "Number of functors does not match number of types!");
            if (empty()) std::terminate();
            _visit4<0>(std::forward<Fs>(fs)...);
        }

        /**
         * @brief 使用多个函数对象访问存储的值 (无返回值版本，const)。
         * @details 每个类型对应一个函数对象，按索引匹配。
         * @tparam Fs 函数对象类型列表，数量必须与 Ts... 一致，且任一重载返回 void。
         * @param fs 函数对象列表。
         * @note 若对象为空 (由默认构造函数构造)，则调用 std::terminate。
         */
        template <class... Fs>
        auto
        visit(Fs &&...fs) const -> typename std::enable_if<has_void<decltype(fs(
            std::declval<const Ts &>()))...>::value>::type {
            static_assert(sizeof...(Fs) == sizeof...(Ts),
                "Number of functors does not match number of types!");
            if (empty()) std::terminate();
            _visit4<0>(std::forward<Fs>(fs)...);
        }

        /**
         * @brief 获取当前存储类型的索引。
         * @return size_t 索引值，若 Union 为空则返回 size_t(-1)。
         */
        size_t index() const { return index_; }

        /**
         * @brief 检查 Union 是否为空。
         * @details 如果 Union 当前不存储任何值 (由默认构造函数构造)，则返回 true。
         * @return bool 若 Union 为空则返回 true，否则返回 false。
         */
        bool empty() const { return index_ == static_cast<size_t>(-1); }

#undef TERMINATE_FOR_EXCEPTION
    };

    /**
     * @struct ignore_t
     * @brief 忽略任意参数的函数对象类型。
     * @details 提供一个接受任意数量、任意类型参数的函数调用运算符，该运算符不执行任何操作。
     */
    struct ignore_t {
        /**
         * @brief 函数调用运算符，忽略所有参数。
         * @tparam Ts 参数类型列表 (任意)。
         * @param 参数被完全忽略，不执行任何操作。
         */
        template <class... Ts>
        void operator()(Ts &&...) {}
    };

    /// @brief 全局 ignore 实例，可用于忽略任意参数。
    static constexpr ignore_t ignore;
}
