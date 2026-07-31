// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.sort;

import std;

import fmia.random;
import fmia.sort;

using namespace fmia;

constexpr std::size_t array_size_small = 1e3;
constexpr std::size_t array_size_large = 5e5;

export {

void insertion_sort_math_correctness() {
  auto arr = random::rand_number_vector(array_size_small);

  insertion_sort(arr.begin(), arr.end());
  contract_assert(std::ranges::is_sorted(arr) && arr.front() <= arr.back());

  std::ranges::shuffle(arr, random::mt19937_engine);

  binary_insertion_sort(arr.begin(), arr.end());
  contract_assert(std::ranges::is_sorted(arr) && arr.front() <= arr.back());
}

void selection_sort_math_correctness() {
  auto arr = random::rand_number_vector(array_size_small);

  selection_sort(arr.begin(), arr.end());
  contract_assert(std::ranges::is_sorted(arr) && arr.front() <= arr.back());
}

void bubble_sort_math_correctness() {
  auto arr = random::rand_number_vector(array_size_small);

  bubble_sort(arr.begin(), arr.end());
  contract_assert(std::ranges::is_sorted(arr) && arr.front() <= arr.back());
}

void heap_sort_math_correctness() {
  auto arr = random::rand_number_vector(array_size_large);

  heap_sort(arr.begin(), arr.end());
  contract_assert(std::ranges::is_sorted(arr) && arr.front() <= arr.back());
}

void hoare_quick_sort_math_correctness() {
  auto arr = random::rand_number_vector(array_size_large);
  hoare_quick_sort(arr.begin(), arr.end());
  contract_assert(std::ranges::is_sorted(arr) && arr.front() <= arr.back());
}

void bentley_mcilroy_quick_sort_math_correctness() {
  auto arr = random::rand_number_vector(array_size_large);
  bentley_mcilroy_quick_sort(arr.begin(), arr.end());
  contract_assert(std::ranges::is_sorted(arr) && arr.front() <= arr.back());
}

void merge_sort_math_correctness() {
  auto arr = random::rand_number_vector(array_size_large);

  merge_sort(arr.begin(), arr.end());
  contract_assert(std::ranges::is_sorted(arr) && arr.front() <= arr.back());

  std::ranges::shuffle(arr, random::mt19937_engine);

  inplace_merge_sort(arr.begin(), arr.end());
  contract_assert(std::ranges::is_sorted(arr) && arr.front() <= arr.back());
}

} // export
