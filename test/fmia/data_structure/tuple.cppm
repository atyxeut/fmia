// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.data_structure.tuple;

import std;

import fmia.data_structure.tuple;

using namespace fmia;

export {

void std_pair_output_with_custom_print_and_std_ostream() noexcept {
  constexpr auto p = std::make_pair("printing a pair", 3.14159);

  // print using ` | ` as the delimiter
  print(std::cout, p, " | ", true);

  // operator << uses ` ` as the delimiter
  std::cout << p << '\n';

  std::cout << '\n';
}

void std_tuple_output_with_custom_print_and_std_ostream() noexcept {
  constexpr auto t = std::make_tuple("printing a tuple", 3.14159f, 3.1415926535, 123456, 1'000'000'000'000'000ll, 'A');

  // print using ` | ` as the delimiter
  print(std::cout, t, " | ", true);

  // operator << uses ` ` as the delimiter
  std::cout << t << '\n';

  std::cout << '\n';
}

} // export
