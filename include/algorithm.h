/**
 * @file algorithm.h
 * @brief 算法头文件，包装 STL 的标准算法为函数对象。
 * @details 解决标准库中算法为模板函数无法被存储于对象中的问题，
 *          同时，函数对象的效率略微优于函数指针，提高了效率。
 * @note 可以接入 Generator 的 algorithm 终端中。
 * @author MrXie1109
 * @date 2026
 * @copyright MIT License
 */

#pragma once
#include <algorithm>
#include <numeric>

namespace console {
#define ALGORITHM(NAME)                                                        \
    struct NAME##_t {                                                          \
        template <class... Args>                                               \
        decltype(std::NAME(std::declval<Args>()...))                           \
        operator()(Args &&...args) const {                                     \
            return std::NAME(std::forward<Args>(args)...);                     \
        }                                                                      \
    };                                                                         \
    static constexpr NAME##_t NAME;

    namespace alg {
        /// @name Nonmodifying Algorithm
        /// @brief 非更易型算法
        /// @{
        ALGORITHM(for_each)
        ALGORITHM(count)
        ALGORITHM(count_if)
        ALGORITHM(min_element)
        ALGORITHM(max_element)
        ALGORITHM(minmax_element)
        ALGORITHM(find)
        ALGORITHM(find_if)
        ALGORITHM(find_if_not)
        ALGORITHM(search_n)
        ALGORITHM(search)
        ALGORITHM(find_end)
        ALGORITHM(find_first_of)
        ALGORITHM(adjacent_find)
        ALGORITHM(equal)
        ALGORITHM(is_permutation)
        ALGORITHM(mismatch)
        ALGORITHM(lexicographical_compare)
        ALGORITHM(is_sorted)
        ALGORITHM(is_sorted_until)
        ALGORITHM(is_partitioned)
        ALGORITHM(partition_point)
        ALGORITHM(all_of)
        ALGORITHM(any_of)
        ALGORITHM(none_of)
#if __cplusplus >= 201703L
        ALGORITHM(for_each_n)
#endif
        /// @}

        /// @name Modifying Algorithm
        /// @brief 更易型算法
        /// @{
        // ALGORITHM(for_each) // exists
        ALGORITHM(copy)
        ALGORITHM(copy_if)
        ALGORITHM(copy_n)
        ALGORITHM(copy_backward)
        ALGORITHM(move)
        ALGORITHM(move_backward)
        ALGORITHM(transform)
        ALGORITHM(merge)
        ALGORITHM(swap_ranges)
        ALGORITHM(fill)
        ALGORITHM(fill_n)
        ALGORITHM(generate)
        ALGORITHM(generate_n)
        ALGORITHM(iota)
        ALGORITHM(replace)
        ALGORITHM(replace_if)
        ALGORITHM(replace_copy)
        ALGORITHM(replace_copy_if)
        // ALGORITHM(for_each_n) // exists
        /// @}

        /// @name Removing Algorithm
        /// @brief 移除型算法
        /// @{
        ALGORITHM(remove)
        ALGORITHM(remove_if)
        ALGORITHM(remove_copy)
        ALGORITHM(remove_copy_if)
        ALGORITHM(unique)
        ALGORITHM(unique_copy)
        /// @}

        /// @name Mutating Algorithm
        /// @brief 变序型算法
        /// @{
        ALGORITHM(reverse)
        ALGORITHM(reverse_copy)
        ALGORITHM(rotate)
        ALGORITHM(rotate_copy)
        ALGORITHM(next_permutation)
        ALGORITHM(prev_permutation)
        ALGORITHM(shuffle)
#if __cplusplus < 201703L
        ALGORITHM(random_shuffle)
#endif
        ALGORITHM(partition)
        ALGORITHM(stable_partition)
        ALGORITHM(partition_copy)
#if __cplusplus >= 201703L
        ALGORITHM(sample)
#endif
#if __cplusplus >= 202002L
        ALGORITHM(shift_left)
        ALGORITHM(shift_right)
#endif
        /// @}

        /// @name Sorting Algorithm
        /// @brief 排序算法
        /// @{
        ALGORITHM(sort)
        ALGORITHM(stable_sort)
        ALGORITHM(partial_sort)
        ALGORITHM(partial_sort_copy)
        ALGORITHM(nth_element)
        // ALGORITHM(partition)        // exists
        // ALGORITHM(stable_partition) // exists
        // ALGORITHM(partition_copy)   // exists
        ALGORITHM(make_heap)
        ALGORITHM(push_heap)
        ALGORITHM(pop_heap)
        ALGORITHM(sort_heap)
        /// @}

        /// @name Sorted-Range Algorithm
        /// @brief 已排序区间算法
        /// @{
        ALGORITHM(binary_search)
        ALGORITHM(includes)
        ALGORITHM(lower_bound)
        ALGORITHM(upper_bound)
        ALGORITHM(equal_range)
        // ALGORITHM(merge) // exists
        ALGORITHM(set_union)
        ALGORITHM(set_intersection)
        ALGORITHM(set_difference)
        ALGORITHM(set_symmetric_difference)
        ALGORITHM(inplace_merge)
        // ALGORITHM(partition_point) // exists
        /// @}

        /// @name Numeric Algorithm
        /// @brief 数值算法
        /// @{
        ALGORITHM(accumulate)
        ALGORITHM(inner_product)
        ALGORITHM(adjacent_difference)
        ALGORITHM(partial_sum)
#if __cplusplus >= 201703L
        ALGORITHM(reduce)
        ALGORITHM(transform_reduce)
        ALGORITHM(inclusive_scan)
        ALGORITHM(exclusive_scan)
        ALGORITHM(transform_inclusive_scan)
        ALGORITHM(transform_exclusive_scan)
#endif
        /// @}
    }

#undef ALGORITHM
}
