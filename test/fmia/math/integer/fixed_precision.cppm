// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: GPL-3.0-or-later

export module test.fmia.math.integer.fixed_precision;

import std;

import fmia.math.integer.fixed_precision;

using namespace fmia;
using namespace fmia::meta;

consteval void check_is_boolean() {
  static_assert(is_boolean_v<int> == false);
  static_assert(is_boolean_v<bool> == true);
  static_assert(is_boolean_v<const bool> == true);
  static_assert(is_boolean_v<volatile bool> == true);
  static_assert(is_boolean_v<const volatile bool> == true);

  static_assert(boolean<int> == false);
  static_assert(boolean<bool> == true);
  static_assert(boolean<const bool> == true);
  static_assert(boolean<volatile bool> == true);
  static_assert(boolean<const volatile bool> == true);
}

consteval void check_make_signed() {
  static_assert(std::same_as<make_signed_t<int>, int> == true);
  static_assert(std::same_as<make_signed_t<const int>, const int> == true);
  static_assert(std::same_as<make_signed_t<const volatile unsigned int>, const volatile int> == true);
  static_assert(std::same_as<make_signed_t<const u64>, const i64> == true);
  static_assert(std::same_as<make_signed_t<i128>, i128> == true);
  static_assert(std::same_as<make_signed_t<u128>, i128> == true);
  static_assert(std::same_as<make_signed_t<integer::u<1024>>, integer::i<1024>> == true);
}

consteval void check_make_unsigned() {
  static_assert(std::same_as<make_unsigned_t<int>, unsigned int> == true);
  static_assert(std::same_as<make_unsigned_t<const int>, const unsigned int> == true);
  static_assert(std::same_as<make_unsigned_t<const volatile unsigned int>, const volatile unsigned int> == true);
  static_assert(std::same_as<make_unsigned_t<const u64>, const u64> == true);
  static_assert(std::same_as<make_unsigned_t<i128>, u128> == true);
  static_assert(std::same_as<make_unsigned_t<u128>, u128> == true);
  static_assert(std::same_as<make_unsigned_t<integer::i<1024>>, integer::u<1024>> == true);
}

export {

void check_int128_input_with_std_istream() {
  i128 a, b;
  u128 c, d;

  std::cin >> a >> b >> c >> d;
  std::println("i128: {}\ni128: {}\nu128: {}\nu128: {}\n", a, b, c, d);
}

void check_int128_output_with_std_ostream_and_std_print() {
  i128 a = -30, b = -40;
  u128 c = 30, d = 40;

  std::cout << a + b << ' ' << c + d << '\n';
  std::println("{:05} {:05}\n", a + b, c + d);
}

} // export
