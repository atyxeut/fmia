// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.data_structure.array.multidimensional_std_array;

import std;

import fmia.data_structure.array.multidimensional_std_array;

using namespace fmia;

consteval void is_fmia_array_nested_std_array() {
  using T1 = std::array<double, 10>;
  using T1_ = array<double, 10>;
  static_assert(std::same_as<T1, T1_>);

  using T2 = std::array<std::array<double, 2>, 3>;
  using T2_ = array<double, 3, 2>;
  static_assert(std::same_as<T2, T2_>);

  using T3 = std::array<std::array<std::array<std::array<int, 2>, 3>, 4>, 5>;
  using T3_ = array<int, 5, 4, 3, 2>;
  static_assert(std::same_as<T3, T3_>);

  using T4 = std::array<std::array<std::array<std::array<std::vector<int>, 0>, 2>, 4>, 7>;
  using T4_ = array<std::vector<int>, 7, 4, 2, 0>;
  static_assert(std::same_as<T4, T4_>);
}
