// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module benchmark.fmia.sort.comparison;

import std;

import fmia.debug.timer;
import fmia.io.format_flag;
import fmia.random;
import fmia.sort;

constexpr std::size_t large_array_size = 2e6;
constexpr std::size_t small_array_size = 2e5;

export {

void compare_hoare_quick_sort_variations() {
  const auto comp = std::ranges::less {};

  fmia::stopwatch timer;

  auto int_arr_0 = fmia::random::rand_number_vector(large_array_size);
  auto int_arr_1 = int_arr_0;
  auto string_arr_of_len_1000_0 = fmia::random::rand_vector<std::string>(small_array_size, [] {
    return fmia::random::rand_positive_big_integer(1000);
  });
  auto string_arr_of_len_1000_1 = string_arr_of_len_1000_0;

  constexpr const char* test_type[] {"random", "sorted", "duplicate"};
  constexpr const char* data_type[] {"int array", "string array"};
  constexpr const char* algorithm_type[] {"standard", "less comparison"};
  constexpr std::size_t n[] {std::ranges::size(data_type), std::ranges::size(algorithm_type)};

  const auto run = [&](std::size_t i) {
    timer.lap();
    fmia::recursive_hoare_quick_sort(int_arr_0.begin(), int_arr_0.end(), comp);
    timer.lap();
    fmia::recursive_hoare_quick_sort_less_comparison(int_arr_1.begin(), int_arr_1.end(), comp);
    timer.lap();
    fmia::recursive_hoare_quick_sort(string_arr_of_len_1000_0.begin(), string_arr_of_len_1000_0.end(), comp);
    timer.lap();
    fmia::recursive_hoare_quick_sort_less_comparison(string_arr_of_len_1000_1.begin(), string_arr_of_len_1000_1.end(), comp);
    timer.lap();

    for (auto j = 0uz; j < n[0]; ++j) {
      for (auto k = 0uz; k < n[1]; ++k) {
        std::print("{} version on {} {}: ", algorithm_type[k], test_type[i], data_type[j]);
        timer.print_lap(timer.lap_count() - 4 + (j * n[1] + k + 1), fmia::io::fmt::endl);
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
