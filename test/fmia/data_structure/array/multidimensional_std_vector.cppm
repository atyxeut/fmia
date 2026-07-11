// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

module;

#include <cassert>

export module test.fmia.data_structure.array.multidimensional_std_vector;

import std;

import fmia.data_structure.array.multidimensional_std_vector;
import fmia.memory.allocator;

using namespace fmia;

consteval void fmia_vector_should_be_nested_std_vector() noexcept {
  using T1 = std::vector<double>;
  using T1_ = vector<double>;
  static_assert(std::same_as<T1, T1_>);

  using T2 = std::pmr::vector<double>;
  using T2_ = vector<double, 1, std_pmr_allocator_tag>;
  static_assert(std::same_as<T2, T2_>);

  using T3 = std::pmr::vector<std::pmr::vector<std::pmr::vector<std::pmr::vector<std::pmr::vector<double>>>>>;
  using T3_ =
    vector<double, 5, std_pmr_allocator_tag, std_pmr_allocator_tag, std_pmr_allocator_tag, std_pmr_allocator_tag, std_pmr_allocator_tag>;
  static_assert(std::same_as<T3, T3_>);

  using T4 = std::vector<std::vector<std::vector<std::vector<std::vector<double>>>>>;
  using T4_ = vector<double, 5>;
  static_assert(std::same_as<T4, T4_>);
}

consteval void every_empty_allocator_template_parameter_of_fmia_vector_defaults_to_std_allocator() noexcept {
  using T1 = std::pmr::vector<double>;
  using T1_ = vector<double, 1, std::pmr::polymorphic_allocator<double>>;
  static_assert(std::same_as<T1, T1_>);

  using T2 = std::vector<std::vector<std::pmr::vector<int>>>;
  using T2_ = vector<int, 3, std::pmr::polymorphic_allocator<int>>;
  using T2__ = vector<int, 3, std::pmr::polymorphic_allocator<int>, std::allocator<std::vector<int, std::pmr::polymorphic_allocator<int>>>>;
  static_assert(std::same_as<T2, T2_> && std::same_as<T2_, T2__>);
}

export {

void does_make_vector_work() {
  auto get_v1 = [] consteval noexcept {
    return std::vector<double>(3, 3.14);
  };
  auto get_v1_ = [] consteval noexcept {
    return make_vector<double>(3, 3.14);
  };
  static_assert(get_v1() == get_v1_());

  // std::pmr::polymorphic_allocator cannot be used in constexpr context
  auto v2 = std::vector<std::pmr::vector<std::vector<double>>>(3, std::pmr::vector<std::vector<double>>(4, std::vector<double>(5, 3.14)));
  auto v2_ = make_vector<double, std::allocator<double>, std::pmr::polymorphic_allocator<std::vector<double>>>(3, 4, 5, 3.14);
  auto v2__ = make_vector<double, std_allocator_tag, std_pmr_allocator_tag>(3, 4, 5, 3.14);
  assert(v2 == v2_ && v2_ == v2__);
}

} // export
