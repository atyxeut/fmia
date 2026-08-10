// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.sort.internal;

import std;

import fmia.random;

export namespace fmia {

template <typename Iter, typename Comparator, typename Projection = std::identity>
concept comparison_sortable = std::sortable<Iter, Comparator, Projection>;

} // export namespace fmia

export namespace fmia {

template <std::bidirectional_iterator I, typename Comparator = std::ranges::less>
  requires comparison_sortable<I, Comparator>
constexpr void insertion_sort(I first, I last, Comparator comp = std::ranges::less {}) pre(std::ranges::distance(first, last) >= 0) {
  if (first == last)
    return;

  for (auto src = std::ranges::next(first); src != last; ++src) {
    auto tmp = std::ranges::iter_move(src);
    for (auto i = src, j = src; j != first; --j) {
      if (!comp(tmp, *--i)) {
        *j = std::move(tmp);
        break;
      }
      *j = std::ranges::iter_move(i);
    }
  }
}

template <std::bidirectional_iterator I, typename Comparator = std::ranges::less>
  requires comparison_sortable<I, Comparator>
constexpr void binary_insertion_sort(I first, I last, Comparator comp = std::ranges::less {}) pre(std::ranges::distance(first, last) >= 0) {
  if (first == last)
    return;

  for (auto src = std::ranges::next(first); src != last; ++src) {
    auto tmp = std::ranges::iter_move(src);
    const auto dst = std::ranges::upper_bound(first, src, tmp, comp);
    std::ranges::move_backward(dst, src, std::ranges::next(src));
    *dst = std::move(tmp);
  }
}

template <typename Iter, typename Comparator = std::ranges::less>
  requires comparison_sortable<Iter, Comparator>
constexpr void selection_sort(Iter first, Iter last, Comparator comp = std::ranges::less {}) pre(std::ranges::distance(first, last) >= 0) {
  if (first == last)
    return;

  for (; first != last; ++first)
    std::ranges::iter_swap(first, std::ranges::min_element(first, last, comp));
}

template <std::bidirectional_iterator I, typename Comparator = std::ranges::less>
  requires comparison_sortable<I, Comparator>
constexpr void bubble_sort(I first, I last, Comparator comp = std::ranges::less {}) pre(std::ranges::distance(first, last) >= 0) {
  if (first == last)
    return;

  for (; first != last; ++first) {
    bool sorted = true;
    for (auto i = std::ranges::prev(last), j = i; j != first; --j) {
      if (comp(*j, *--i)) {
        std::ranges::iter_swap(i, j);
        sorted = false;
      }
    }
    if (sorted)
      break;
  }
}

} // export namespace fmia

namespace fmia {

template <typename Iter, typename Comparator>
constexpr void heap_sort_sift_down(std::iter_difference_t<Iter> size, std::iter_difference_t<Iter> cur, Iter first, Comparator comp) {
  while (cur < size / 2) {
    auto right = cur * 2 + 1;
    auto left = right++;
    if (right < size && comp(first[left], first[right]))
      left = right;
    if (!comp(first[cur], first[left]))
      break;
    std::ranges::iter_swap(first + cur, first + left);
    cur = left;
  }
}

} // namespace fmia

export namespace fmia {

template <std::random_access_iterator I, typename Comparator = std::ranges::less>
  requires comparison_sortable<I, Comparator>
constexpr void heap_sort(I first, I last, Comparator comp = std::ranges::less {}) {
  auto size = last - first;
  for (auto i = size / 2 - 1; i >= 0; --i)
    heap_sort_sift_down(size, i, first, comp);
  while (--size > 0) {
    std::ranges::iter_swap(first, first + size);
    heap_sort_sift_down(size, static_cast<std::iter_difference_t<I>>(0), first, comp);
  }
}

} // export namespace fmia

namespace fmia {

template <typename Iter, typename Comparator>
constexpr Iter quick_sort_pivot_median_of_3(Iter first, Iter last, Comparator comp) pre(last - first > 2) {
  const auto mid = first + (--last - first) / 2;
  if (comp(*mid, *first))
    std::ranges::iter_swap(first, mid);
  if (comp(*last, *mid))
    std::ranges::iter_swap(mid, last);
  if (comp(*mid, *first))
    std::ranges::iter_swap(first, mid);
  return mid;
}

struct quick_sort_lomuto_partition_tag {};

template <typename Iter, typename Comparator>
[[nodiscard]] constexpr std::array<Iter, 2> quick_sort_partition(Iter first, Iter last, Comparator comp, quick_sort_lomuto_partition_tag) {
  const auto mid = quick_sort_pivot_median_of_3(first++, last, comp);
  std::ranges::iter_swap(first, mid);
  const auto pivot = first;
  for (auto i = pivot + 1; i != last; ++i)
    if (!comp(*pivot, *i) && ++first != i)
      std::ranges::iter_swap(first, i);
  std::ranges::iter_swap(pivot, first);
  return {first, first + 1};
}

struct quick_sort_dijkstra_partition_tag {};

template <typename Iter, typename Comparator>
[[nodiscard]] constexpr std::array<Iter, 2> quick_sort_partition(
  Iter first, Iter last, Comparator comp, quick_sort_dijkstra_partition_tag
) {
  // during the core loop, the pivot can be moved, but `*first == pivot` always hold
  std::ranges::iter_swap(first, quick_sort_pivot_median_of_3(first, last, comp));
  for (auto i = first + 1; i != last;) {
    if (comp(*i, *first))
      std::ranges::iter_swap(i++, first++);
    else if (comp(*first, *i))
      std::ranges::iter_swap(i, --last);
    else
      ++i;
  }
  return {first, last};
}

enum class quick_sort_hoare_partition_bound_check { off, on };

template <quick_sort_hoare_partition_bound_check>
struct quick_sort_hoare_partition_tag {};

template <typename Iter, typename Comparator, quick_sort_hoare_partition_bound_check BoundCheckFlag>
[[nodiscard]] constexpr std::array<Iter, 2> quick_sort_partition(
  Iter first, Iter last, Comparator comp, quick_sort_hoare_partition_tag<BoundCheckFlag>
) {
  constexpr bool bound_check_off = BoundCheckFlag == quick_sort_hoare_partition_bound_check::off;
  const auto mid = quick_sort_pivot_median_of_3(first++, last--, comp);
  std::ranges::iter_swap(first, mid);
  const auto& pivot = *first;
  for (;;) {
    do {
      ++first;
    } while ((bound_check_off || first < last) && comp(*first, pivot));
    do {
      --last;
    } while ((bound_check_off || first < last) && comp(pivot, *last));
    if (first >= last)
      return {first, first};
    std::ranges::iter_swap(first, last);
  }
}

struct quick_sort_bentley_mcilroy_partition_tag {};

template <typename Iter, typename Comparator>
[[nodiscard]] constexpr std::array<Iter, 2> quick_sort_partition(
  Iter first, Iter last, Comparator comp, quick_sort_bentley_mcilroy_partition_tag
) {
  return {};
}

template <typename PartitionPolicy, typename Iter, typename Comparator>
constexpr void recursive_quick_sort_impl(Iter first, Iter last, Comparator comp) {
  const auto size = last - first;
  if (size < 2)
    return;
  if (size == 2) {
    const auto tail = first + 1;
    if (comp(*tail, *first))
      std::ranges::iter_swap(first, tail);
    return;
  }
  if (size == 3) {
    quick_sort_pivot_median_of_3(first, last, comp);
    return;
  }

  const auto mid = quick_sort_partition(first, last, comp, PartitionPolicy {});
  recursive_quick_sort_impl<PartitionPolicy>(first, mid[0], comp);
  recursive_quick_sort_impl<PartitionPolicy>(mid[1], last, comp);
}

} // namespace fmia

export namespace fmia {

template <std::random_access_iterator I, typename Comparator = std::ranges::less>
  requires comparison_sortable<I, Comparator>
constexpr void recursive_lomuto_quick_sort(I first, I last, Comparator comp = std::ranges::less {}) {
  recursive_quick_sort_impl<quick_sort_lomuto_partition_tag>(first, last, comp);
}

template <std::random_access_iterator I, typename Comparator = std::ranges::less>
  requires comparison_sortable<I, Comparator>
constexpr void recursive_dijkstra_quick_sort(I first, I last, Comparator comp = std::ranges::less {}) {
  recursive_quick_sort_impl<quick_sort_dijkstra_partition_tag>(first, last, comp);
}

template <std::random_access_iterator I, typename Comparator = std::ranges::less>
  requires comparison_sortable<I, Comparator>
constexpr void recursive_hoare_quick_sort(I first, I last, Comparator comp = std::ranges::less {}) {
  recursive_quick_sort_impl<quick_sort_hoare_partition_tag<quick_sort_hoare_partition_bound_check::off>>(first, last, comp);
}

template <std::random_access_iterator I, typename Comparator = std::ranges::less>
  requires comparison_sortable<I, Comparator>
constexpr void recursive_hoare_quick_sort_less_comparison(I first, I last, Comparator comp = std::ranges::less {}) {
  recursive_quick_sort_impl<quick_sort_hoare_partition_tag<quick_sort_hoare_partition_bound_check::on>>(first, last, comp);
}

template <std::random_access_iterator I, typename Comparator = std::ranges::less>
  requires comparison_sortable<I, Comparator>
constexpr void recursive_bentley_mcilroy_quick_sort(I first, I last, Comparator comp = std::ranges::less {}) {
  recursive_quick_sort_impl<quick_sort_bentley_mcilroy_partition_tag>(first, last, comp);
}

} // export namespace fmia

namespace fmia {

enum class merge_sort_merge_policy { normal, inplace };

template <typename Iter, typename Comparator>
constexpr void merge_sort_normal_merge_impl(std::iter_difference_t<Iter> size, Iter first, Iter mid, Iter last, Comparator comp) {
  std::vector<std::iter_value_t<Iter>> buffer;
  buffer.reserve(static_cast<std::size_t>(size));
  std::merge(first, mid, mid, last, std::back_inserter(buffer), comp);
  std::ranges::move(buffer, first);
}

template <merge_sort_merge_policy Policy, std::random_access_iterator I, typename Comparator>
constexpr void merge_sort_impl_recursive(I first, I last, Comparator comp) {
  const auto size = std::ranges::distance(first, last);
  if (size < 2)
    return;

  const auto mid = first + size / 2;
  merge_sort_impl_recursive<Policy>(first, mid, comp);
  merge_sort_impl_recursive<Policy>(mid, last, comp);

  if constexpr (Policy == merge_sort_merge_policy::normal)
    merge_sort_normal_merge_impl(size, first, mid, last, comp);
  else
    std::inplace_merge(first, mid, last, comp);
}

template <merge_sort_merge_policy Policy, std::forward_iterator I, typename Comparator>
constexpr void merge_sort_impl_recursive(I first, I last, Comparator comp) {
  //
}

} // namespace fmia

export namespace fmia {

template <typename Iter, typename Comparator = std::ranges::less>
  requires comparison_sortable<Iter, Comparator>
constexpr void merge_sort(Iter first, Iter last, Comparator comp = std::ranges::less {}) {
  merge_sort_impl_recursive<merge_sort_merge_policy::normal>(first, last, comp);
}

template <typename Iter, typename Comparator = std::ranges::less>
  requires comparison_sortable<Iter, Comparator>
constexpr void inplace_merge_sort(Iter first, Iter last, Comparator comp = std::ranges::less {}) {
  merge_sort_impl_recursive<merge_sort_merge_policy::inplace>(first, last, comp);
}

} // export namespace fmia
