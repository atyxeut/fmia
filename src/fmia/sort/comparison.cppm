// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.sort.comparison;

import std;

import fmia.random;

export namespace fmia {

template <typename Iter, typename Cmp, typename Proj = std::identity>
concept comparison_sortable = std::sortable<Iter, Cmp, Proj>;

} // export namespace fmia

export namespace fmia {

template <std::bidirectional_iterator I, typename Cmp = std::ranges::less>
  requires comparison_sortable<I, Cmp>
constexpr void insertion_sort(I first, I last, Cmp cmp = std::ranges::less {}) pre(std::ranges::distance(first, last) >= 0) {
  if (first == last)
    return;

  for (auto src = std::ranges::next(first); src != last; ++src) {
    auto tmp = std::ranges::iter_move(src);
    for (auto i = src, j = src; j != first; --j) {
      if (!cmp(tmp, *--i)) {
        *j = std::move(tmp);
        break;
      }
      *j = std::ranges::iter_move(i);
    }
  }
}

template <std::bidirectional_iterator I, typename Cmp = std::ranges::less>
  requires comparison_sortable<I, Cmp>
constexpr void binary_insertion_sort(I first, I last, Cmp cmp = std::ranges::less {}) pre(std::ranges::distance(first, last) >= 0) {
  if (first == last)
    return;

  for (auto src = std::ranges::next(first); src != last; ++src) {
    auto tmp = std::ranges::iter_move(src);
    const auto dst = std::ranges::upper_bound(first, src, tmp, cmp);
    std::ranges::move_backward(dst, src, std::ranges::next(src));
    *dst = std::move(tmp);
  }
}

template <typename Iter, typename Cmp = std::ranges::less>
  requires comparison_sortable<Iter, Cmp>
constexpr void selection_sort(Iter first, Iter last, Cmp cmp = std::ranges::less {}) pre(std::ranges::distance(first, last) >= 0) {
  if (first == last)
    return;

  for (; first != last; ++first)
    std::ranges::iter_swap(first, std::ranges::min_element(first, last, cmp));
}

template <std::bidirectional_iterator I, typename Cmp = std::ranges::less>
  requires comparison_sortable<I, Cmp>
constexpr void bubble_sort(I first, I last, Cmp cmp = std::ranges::less {}) pre(std::ranges::distance(first, last) >= 0) {
  if (first == last)
    return;

  for (; first != last; ++first) {
    bool sorted = true;
    for (auto i = std::ranges::prev(last), j = i; j != first; --j) {
      if (cmp(*j, *--i)) {
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

template <typename RandomAccessIter, typename Cmp>
constexpr void heap_sort_sift_down(
  std::iter_difference_t<RandomAccessIter> size, std::iter_difference_t<RandomAccessIter> cur, RandomAccessIter first, Cmp cmp
) {
  while (cur < size / 2) {
    auto right = cur * 2 + 1;
    auto left = right++;
    if (right < size && cmp(first[left], first[right]))
      left = right;
    if (!cmp(first[cur], first[left]))
      break;
    std::ranges::iter_swap(first + cur, first + left);
    cur = left;
  }
}

} // namespace fmia

export namespace fmia {

template <std::random_access_iterator I, typename Cmp = std::ranges::less>
  requires comparison_sortable<I, Cmp>
constexpr void heap_sort(I first, I last, Cmp cmp = std::ranges::less {}) {
  auto size = last - first;
  for (auto i = size / 2 - 1; i >= 0; --i)
    heap_sort_sift_down(size, i, first, cmp);
  while (--size > 0) {
    std::ranges::iter_swap(first, first + size);
    heap_sort_sift_down(size, static_cast<std::iter_difference_t<I>>(0), first, cmp);
  }
}

} // export namespace fmia

namespace fmia {

template <typename RandomAccessIter, typename Cmp>
[[nodiscard]] constexpr RandomAccessIter median_of_3(RandomAccessIter first, RandomAccessIter last, Cmp cmp) noexcept(
  noexcept(++first) && noexcept(--first) && noexcept(first + (first - first) / 2) && noexcept(cmp(*first, *first))
  && noexcept(std::ranges::iter_swap(first, first))
) pre(last - first > 2) {
  --last;
  const auto mid = first + (last - first) / 2;

  if (cmp(*mid, *first))
    std::ranges::iter_swap(first, mid);
  if (cmp(*last, *mid))
    std::ranges::iter_swap(mid, last);
  if (cmp(*mid, *first))
    std::ranges::iter_swap(first, mid);

  ++first;
  std::ranges::iter_swap(first, mid);
  return first;
}

enum class quick_sort_hoare_partition_bound_check { off, on };

template <quick_sort_hoare_partition_bound_check>
struct quick_sort_hoare_partition_tag {};

template <typename RandomAccessIter, typename Cmp, quick_sort_hoare_partition_bound_check BoundCheckFlag>
[[nodiscard]] constexpr std::array<RandomAccessIter, 2> quick_sort_partition(
  RandomAccessIter first, RandomAccessIter last, Cmp cmp, quick_sort_hoare_partition_tag<BoundCheckFlag>
) noexcept(noexcept(median_of_3(first, first, cmp))) {
  const auto size = last - first;
  if (size == 2) {
    if (const auto tail = first + 1; cmp(*tail, *first))
      std::ranges::iter_swap(first, tail);
    return {first, last};
  }

  const auto& pivot = *median_of_3(first, last, cmp);
  if (size == 3)
    return {first, last};

  constexpr bool bound_check_off = BoundCheckFlag == quick_sort_hoare_partition_bound_check::off;
  ++first;
  --last;
  for (;;) {
    do {
      ++first;
    } while ((bound_check_off || first < last) && cmp(*first, pivot));
    do {
      --last;
    } while ((bound_check_off || first < last) && cmp(pivot, *last));
    if (first >= last)
      return {first, first};
    std::ranges::iter_swap(first, last);
  }
}

struct quick_sort_bentley_mcilroy_partition_tag {};

template <typename RandomAccessIter, typename Cmp>
constexpr std::array<RandomAccessIter, 2> quick_sort_partition(
  RandomAccessIter first, RandomAccessIter last, Cmp cmp, quick_sort_bentley_mcilroy_partition_tag
) {
  return {};
}

template <typename PartitionPolicy, typename RandomAccessIter, typename Cmp>
constexpr void recursive_quick_sort_impl(RandomAccessIter first, RandomAccessIter last, Cmp cmp) {
  if (last - first < 2)
    return;

  const auto mid = quick_sort_partition(first, last, cmp, PartitionPolicy {});
  recursive_quick_sort_impl<PartitionPolicy>(first, mid[0], cmp);
  recursive_quick_sort_impl<PartitionPolicy>(mid[1], last, cmp);
}

} // namespace fmia

export namespace fmia {

template <std::random_access_iterator I, typename Cmp = std::ranges::less>
  requires comparison_sortable<I, Cmp>
constexpr void recursive_hoare_quick_sort(I first, I last, Cmp cmp = std::ranges::less {}) {
  recursive_quick_sort_impl<quick_sort_hoare_partition_tag<quick_sort_hoare_partition_bound_check::off>>(first, last, cmp);
}

template <std::random_access_iterator I, typename Cmp = std::ranges::less>
  requires comparison_sortable<I, Cmp>
constexpr void recursive_hoare_quick_sort_less_comparison(I first, I last, Cmp cmp = std::ranges::less {}) {
  recursive_quick_sort_impl<quick_sort_hoare_partition_tag<quick_sort_hoare_partition_bound_check::on>>(first, last, cmp);
}

template <std::random_access_iterator I, typename Cmp = std::ranges::less>
  requires comparison_sortable<I, Cmp>
constexpr void recursive_bentley_mcilroy_quick_sort(I first, I last, Cmp cmp = std::ranges::less {}) {
  recursive_quick_sort_impl<quick_sort_bentley_mcilroy_partition_tag>(first, last, cmp);
}

} // export namespace fmia

namespace fmia {

enum class merge_sort_merge_policy { normal, inplace };

template <typename ForwardIter, typename Cmp>
constexpr void merge_sort_normal_merge_impl(
  std::iter_difference_t<ForwardIter> size, ForwardIter first, ForwardIter mid, ForwardIter last, Cmp cmp
) {
  std::vector<std::iter_value_t<ForwardIter>> buffer;
  buffer.reserve(static_cast<std::size_t>(size));
  std::merge(first, mid, mid, last, std::back_inserter(buffer), cmp);
  std::ranges::move(buffer, first);
}

template <merge_sort_merge_policy Policy, std::random_access_iterator I, typename Cmp>
constexpr void merge_sort_impl_recursive(I first, I last, Cmp cmp) {
  const auto size = std::ranges::distance(first, last);
  if (size < 2)
    return;

  const auto mid = first + size / 2;
  merge_sort_impl_recursive<Policy>(first, mid, cmp);
  merge_sort_impl_recursive<Policy>(mid, last, cmp);

  if constexpr (Policy == merge_sort_merge_policy::normal)
    merge_sort_normal_merge_impl(size, first, mid, last, cmp);
  else
    std::inplace_merge(first, mid, last, cmp);
}

template <merge_sort_merge_policy Policy, std::forward_iterator I, typename Cmp>
constexpr void merge_sort_impl_recursive(I first, I last, Cmp cmp) {
  //
}

} // namespace fmia

export namespace fmia {

template <typename Iter, typename Cmp = std::ranges::less>
  requires comparison_sortable<Iter, Cmp>
constexpr void merge_sort(Iter first, Iter last, Cmp cmp = std::ranges::less {}) {
  merge_sort_impl_recursive<merge_sort_merge_policy::normal>(first, last, cmp);
}

template <typename Iter, typename Cmp = std::ranges::less>
  requires comparison_sortable<Iter, Cmp>
constexpr void inplace_merge_sort(Iter first, Iter last, Cmp cmp = std::ranges::less {}) {
  merge_sort_impl_recursive<merge_sort_merge_policy::inplace>(first, last, cmp);
}

} // export namespace fmia
