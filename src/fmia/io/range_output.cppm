// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.io.range_output;

import std;

import fmia.data_structure.tuple;

namespace fmia::meta::cpp17 {

// must be put here, otherwise the range print functions cannot print ranges that contains std::pair or std::tuple

template <typename, typename = void>
struct is_std_ostream_interactable_impl : std::false_type {};

template <typename T>
struct is_std_ostream_interactable_impl<T, std::void_t<decltype(std::declval<std::ostream&>() << std::declval<T>())>> : std::true_type {};

} // namespace fmia::meta::cpp17

export namespace fmia::meta::cpp17 {

// check if T has an overload of operator << of std::ostream&
// to make is_std_ostream_interactable<...>::value evaluate to true, the candidate overload must be already defined above it
template <typename T>
using is_std_ostream_interactable = is_std_ostream_interactable_impl<T>;

template <typename T>
inline constexpr bool is_std_ostream_interactable_v = is_std_ostream_interactable<T>::value;

} // export namespace fmia::meta::cpp17

export namespace fmia::meta {

// not usable: infinitely recursive constraint (llvm 22)
// however its fine for gcc and msvc
// template <typename T>
// concept std_ostream_interactable = requires(std::ostream& ostr, T t) { ostr << t; };

template <typename T>
concept printable = cpp17::is_std_ostream_interactable_v<T> || requires(std::ostream& os, T t) { print(os, t); };

} // export namespace fmia::meta

export namespace fmia {

// for a range whose elements can be printed by std::ostream by default
// e.g. std::vector<int>, std::vector<std::string>
template <std::ranges::input_range R, std::convertible_to<std::string> Delim = std::string, typename T = std::ranges::range_value_t<R>>
  requires (meta::printable<T> && !std::is_array_v<T>)
int print(std::ostream& ostr, R&& range, Delim&& delim = std::string(1, ' '), bool new_line = false) {
  for (auto it = std::ranges::begin(range), it_end = std::ranges::end(range); it != it_end; ++it)
    ostr << *it << (std::ranges::next(it) == it_end ? std::string {} : delim);

  if (new_line)
    ostr << '\n';

  return 1;
}

// for a range whose elements can not be printed by std::ostream by default
// e.g. std::vector<std::array<int, 4>>, std::vector<std::pair<int, int>>
template <std::ranges::input_range R, std::convertible_to<std::string> Delim = std::string, typename T = std::ranges::range_value_t<R>>
  requires (!meta::cpp17::is_std_ostream_interactable_v<T> && std::ranges::input_range<T>)
int print(std::ostream& ostr, R&& range, Delim&& delim = std::string(1, ' '), bool new_line = false) {
  int cur_dim = 0;
  for (auto it = std::ranges::begin(range), it_end = std::ranges::end(range); it != it_end; ++it) {
    cur_dim = print(ostr, *it, std::forward<Delim>(delim), false);
    const auto dimension_delim = std::string(static_cast<std::size_t>(cur_dim), '\n');
    ostr << (std::ranges::next(it) == it_end ? std::string {} : dimension_delim);
  }

  if (new_line)
    ostr << '\n';

  return cur_dim + 1;
}

template <typename T, std::convertible_to<std::string> Delim = std::string>
  requires (std::rank_v<std::remove_cvref_t<T>> > 1)
void print(std::ostream& ostr, const T& arr, Delim&& delim = std::string(1, ' '), bool new_line = false) {
  for (auto it = std::begin(arr), it_end = std::end(arr); it != it_end; ++it) {
    print(ostr, *it, std::forward<Delim>(delim), false);
    const auto dimension_delim = std::string(std::rank_v<std::remove_cvref_t<T>> - 1, '\n');
    ostr << (std::next(it) == it_end ? std::string {} : dimension_delim);
  }

  if (new_line)
    ostr << '\n';
}

} // export namespace fmia

export namespace fmia::inline operator_overload::inline io_operator {

// avoid ambiguous overloads when Range is std::string&, int[2][3], ...
template <std::ranges::input_range R, typename = std::enable_if_t<!::fmia::meta::cpp17::is_std_ostream_interactable_v<R>>>
auto& operator <<(std::ostream& ostr, R&& range) {
  ::fmia::print(ostr, std::forward<R>(range));
  return ostr;
}

// C-style arrays can decay and be output directly as a pointer, thus need a specific overload
// this overload covers multidimentional arrays
template <typename T, std::size_t N>
  requires (!std::same_as<std::remove_cv_t<T>, char>)
auto& operator <<(std::ostream& ostr, const T (&arr)[N]) {
  ::fmia::print(ostr, arr);
  return ostr;
}

} // export namespace fmia::inline operator_overload::inline io_operator
