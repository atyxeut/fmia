// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.sort.internal;

import std;

import fmia.random;
import fmia.sort;

using element_type = int;

constexpr auto comp = std::ranges::less {};

constexpr auto rand = [] {
  return fmia::random::rand_number(std::numeric_limits<element_type>::min(), std::numeric_limits<element_type>::max());
};

template <std::meta::info Sort>
void check(std::size_t array_size) {
  const auto check_impl = [](std::vector<element_type>& data) {
    [:Sort:](data.begin(), data.end(), comp);
    contract_assert(std::ranges::is_sorted(data));
    contract_assert(!comp(data.back(), data.front()));
  };

  std::print("{}: ", std::meta::identifier_of(Sort));

  // unique data
  auto data = fmia::random::rand_permutation<element_type>(array_size);
  check_impl(data);
  std::print("unique data OK | ");

  // sorted data
  check_impl(data);
  std::print("sorted data OK | ");

  // random data
  std::ranges::generate(data, rand);
  check_impl(data);
  std::print("random data OK | ");

  // identical data
  std::ranges::fill(data, rand());
  check_impl(data);
  std::print("identical data OK\n");
}

constexpr std::size_t small_array_size = 1e3;
constexpr std::size_t large_array_size = 1e5;

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
