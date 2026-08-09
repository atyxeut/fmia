// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.sort.internal;

import std;

import fmia.random;
import fmia.sort;

template <std::meta::info Sort>
void check(std::size_t array_size) {
  const auto comp = std::ranges::less {};

  auto data = fmia::random::rand_number_vector<int>(array_size);
  [:Sort:](data.begin(), data.end(), comp);
  contract_assert(std::ranges::is_sorted(data));
  contract_assert(!comp(data.back(), data.front()));

  auto& sorted_data = data;
  [:Sort:](sorted_data.begin(), sorted_data.end(), comp);
  contract_assert(std::ranges::is_sorted(data));
  contract_assert(!comp(data.back(), data.front()));

  auto& identical_data = data;
  std::ranges::fill(data, fmia::random::rand_number(std::numeric_limits<int>::min(), std::numeric_limits<int>::max()));
  [:Sort:](identical_data.begin(), identical_data.end(), comp);
  contract_assert(std::ranges::is_sorted(data));
  contract_assert(!comp(data.back(), data.front()));
}

constexpr std::size_t small_array_size = 1e3;
constexpr std::size_t large_array_size = 5e5;

export {

void check_insertion_sort() {
  check<^^fmia::insertion_sort>(small_array_size);
}

void check_binary_insertion_sort() {
  check<^^fmia::binary_insertion_sort>(small_array_size);
}

void check_selection_sort() {
  check<^^fmia::selection_sort>(small_array_size);
}

void check_bubble_sort() {
  check<^^fmia::bubble_sort>(small_array_size);
}

void check_heap_sort() {
  check<^^fmia::heap_sort>(large_array_size);
}

void check_recursive_hoare_quick_sort() {
  check<^^fmia::recursive_hoare_quick_sort>(large_array_size);
}

void check_recursive_hoare_quick_sort_less_comparison() {
  check<^^fmia::recursive_hoare_quick_sort_less_comparison>(large_array_size);
}

void check_merge_sort() {
  check<^^fmia::merge_sort>(large_array_size);
}

} // export
