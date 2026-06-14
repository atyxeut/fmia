// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.array.multidimensional_std_array;

import std;

namespace fmia {

template <typename, std::size_t...>
struct array_impl;

template <typename T, std::size_t Dim>
struct array_impl<T, Dim> {
  using type = std::array<T, Dim>;
};

template <typename T, std::size_t Dim, std::size_t... Dims>
struct array_impl<T, Dim, Dims...> : array_impl<typename array_impl<T, Dims...>::type, Dim> {};

} // namespace fmia

export namespace fmia {

// fmia::array<int, 3, 5, 2> arr3d {};
// same as: std::array<std::array<std::array<int, 2>, 5>, 3> arr3d {};
template <typename T, std::size_t... Dims>
using array = array_impl<T, Dims...>::type;

} // export namespace fmia

export namespace fmia::meta {

template <typename>
struct is_no_cv_std_array : std::false_type {};

template <typename T, std::size_t N>
struct is_no_cv_std_array<std::array<T, N>> : std::true_type {};

template <typename T>
inline constexpr bool is_no_cv_std_array_v = is_no_cv_std_array<T>::value;

template <typename T>
using is_std_array = is_no_cv_std_array<std::remove_cv_t<T>>;

template <typename T>
inline constexpr bool is_std_array_v = is_std_array<T>::value;

} // export namespace fmia::meta

export namespace fmia {

// sets every element of a fmia::array to val
// fmia::array<int, 3, 5, 2, 10> arr4d;
// int val = -1;
// fmia::fill_array(arr4d, val);
template <typename Elem, std::size_t Dim, typename T>
constexpr void fill_array(std::array<Elem, Dim>& arr, const T& val) {
  if constexpr (meta::is_no_cv_std_array_v<Elem>)
    for (auto& inner_arr : arr)
      fill_array(inner_arr, val);
  else
    arr.fill(static_cast<Elem>(val));
}

// same as above, for C-style arrays
template <typename Elem, std::size_t N, typename T>
constexpr void fill_array(Elem (&arr)[N], const T& val) {
  if constexpr (std::is_array_v<Elem>)
    for (auto& inner_arr : arr)
      fill_array(inner_arr, val);
  else
    std::fill(arr, arr + N, static_cast<Elem>(val));
}

// auto arr4d = fmia::make_array<int, 5, 8, 3, 2>(val);
// combines `fmia::array<int, 5, 8, 3, 2> arr4d` and `fmia::fill_array(arr4d, val)`
template <typename Elem, std::size_t... Dims, typename T>
[[nodiscard]] constexpr array<Elem, Dims...> make_array(const T& val) {
  array<Elem, Dims...> arr;
  fill_array(arr, static_cast<Elem>(val));
  return arr;
}

} // export namespace fmia
