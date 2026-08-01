// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module benchmark.fmia.sort.comparison;

import std;

import fmia.debug.timer;
import fmia.io.format_flag;
import fmia.random;
import fmia.sort;

using namespace fmia;

constexpr std::size_t array_size = 2e6;

template <std::random_access_iterator I, typename Cmp = std::ranges::less>
  requires comparison_sortable<I, Cmp>
void hoare_quick_sort_modified(I first, I last, Cmp cmp = std::ranges::less {}) {
  if (last - first < 2)
    return;

  auto pivot = median_of_3(first, last, cmp);
  if (last - first < 4)
    return;

  auto i = first + 1;
  std::ranges::iter_swap(i, pivot);
  pivot = i;
  for (auto j = last - 1;;) {
    do
      ++i;
    while (i < j && cmp(*i, *pivot));
    do
      --j;
    while (i < j && cmp(*pivot, *j));
    if (i >= j)
      break;
    std::ranges::iter_swap(i, j);
  }

  hoare_quick_sort_modified(first, i, cmp);
  hoare_quick_sort_modified(i, last, cmp);
}

export {

void check_modified_version_correctness() {
  auto arr = random::rand_number_vector(array_size);
  hoare_quick_sort_modified(arr.begin(), arr.end());
  contract_assert(std::ranges::is_sorted(arr) && arr.front() <= arr.back());
}

void compare_hoare_quick_sort_variations() {
  check_modified_version_correctness();

  stopwatch timer;

  auto int_arr_0 = random::rand_number_vector(array_size);
  auto int_arr_1 = int_arr_0;
  auto string_arr_of_len_1000_0 = random::rand_vector<std::string>(array_size / 10, [] { return random::rand_positive_integer(1000); });
  auto string_arr_of_len_1000_1 = string_arr_of_len_1000_0;

  constexpr const char* test_type[] {"random", "sorted", "duplicate"};
  constexpr const char* data_type[] {"int array", "string array"};
  constexpr const char* algorithm_type[] {"standard", "modified"};
  constexpr std::size_t n[] {std::ranges::size(data_type), std::ranges::size(algorithm_type)};

  const auto run = [&](std::size_t i) {
    timer.lap();
    hoare_quick_sort(int_arr_0.begin(), int_arr_0.end());
    timer.lap();
    hoare_quick_sort_modified(int_arr_1.begin(), int_arr_1.end());
    timer.lap();
    hoare_quick_sort(string_arr_of_len_1000_0.begin(), string_arr_of_len_1000_0.end());
    timer.lap();
    hoare_quick_sort_modified(string_arr_of_len_1000_1.begin(), string_arr_of_len_1000_1.end());
    timer.lap();

    for (auto j = 0uz; j < n[0]; ++j) {
      for (auto k = 0uz; k < n[1]; ++k) {
        std::print("{} version on {} {}: ", algorithm_type[k], test_type[i], data_type[j]);
        timer.print_lap(timer.lap_count() - 4 + (j * n[1] + k + 1), io::fmt::endl);
      }
    }
    std::println();
  };

  run(0);
  run(1);
  std::fill(int_arr_0.begin() + 1, int_arr_0.end(), int_arr_0[0]);
  std::fill(string_arr_of_len_1000_0.begin() + 1, string_arr_of_len_1000_0.end(), string_arr_of_len_1000_0[0]);
  run(2);
}

} // export
