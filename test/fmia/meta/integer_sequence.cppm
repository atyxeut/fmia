// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.meta.integer_sequence;

import std;

import fmia.meta.integer_sequence;

using namespace fmia::meta;

consteval void check_custom_integer_sequence_helpers() {
  static_assert(std::same_as<make_reversed_integer_sequence<int, 8>, std::integer_sequence<int, 7, 6, 5, 4, 3, 2, 1, 0>>);
  static_assert(std::same_as<make_reversed_index_sequence<8>, std::index_sequence<7, 6, 5, 4, 3, 2, 1, 0>>);
  static_assert(std::same_as<reversed_index_sequence_for<int, int, int, int, int>, std::index_sequence<4, 3, 2, 1, 0>>);

  static_assert(std::same_as<make_integer_sequence_of_range<int, 3, 7>, std::integer_sequence<int, 3, 4, 5, 6, 7>>);
  static_assert(std::same_as<make_index_sequence_of_range<3, 10>, std::index_sequence<3, 4, 5, 6, 7, 8, 9, 10>>);
  static_assert(std::same_as<make_reversed_integer_sequence_of_range<int, 3, 7>, std::integer_sequence<int, 7, 6, 5, 4, 3>>);
  static_assert(std::same_as<make_reversed_index_sequence_of_range<3, 10>, std::index_sequence<10, 9, 8, 7, 6, 5, 4, 3>>);

  static_assert(
    std::same_as<shift_integer_sequence_t<int, 10, std::integer_sequence<int, 1, 2, 3, 4>>, std::integer_sequence<int, 11, 12, 13, 14>>
  );
  static_assert(std::same_as<shift_index_sequence_t<50, std::index_sequence<1, 2, 3, 4>>, std::index_sequence<51, 52, 53, 54>>);
}

consteval void check_is_no_duplication_integer_sequence() {
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 1, 3>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2, 3, 4, 4, 6>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2, 3, 4, 5, 6>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2, 1, 2, 1, 2, 1>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::integer_sequence<int, 1, 2, 3, 4, 3, 3, 3>> == false);

  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 3>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 1, 3>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 2, 3, 4, 4, 6>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 2, 3, 4, 5, 6>> == true);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 2, 1, 2, 1, 2, 1>> == false);
  static_assert(is_no_duplication_integer_sequence_v<std::index_sequence<1, 2, 3, 4, 3, 3, 3>> == false);
}
