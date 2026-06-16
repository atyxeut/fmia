// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.array.multidimensional_std_array;

import std;

import fmia.memory.allocator;
import fmia.meta;

namespace fmia {

template <typename, std::size_t...>
struct array_impl;

template <typename T, std::size_t Dim>
struct array_impl<T, Dim>
{
  using type = std::array<T, Dim>;
};

template <typename T, std::size_t Dim, std::size_t... Dims>
struct array_impl<T, Dim, Dims...> : array_impl<typename array_impl<T, Dims...>::type, Dim>
{
};

} // namespace fmia

export namespace fmia {

// fmia::array<int, 3, 5, 2> arr3d {};
// same as: std::array<std::array<std::array<int, 2>, 5>, 3> arr3d {};
template <typename T, std::size_t... Dims>
using array = array_impl<T, Dims...>::type;

} // export namespace fmia

export namespace fmia::meta {

template <typename>
struct is_no_cv_std_array : std::false_type
{
};

template <typename T, std::size_t N>
struct is_no_cv_std_array<std::array<T, N>> : std::true_type
{
};

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
constexpr void fill_array(std::array<Elem, Dim>& arr, const T& val)
{
  if constexpr (meta::is_no_cv_std_array_v<Elem>)
    for (auto& inner_arr : arr)
      fill_array(inner_arr, val);
  else
    arr.fill(static_cast<Elem>(val));
}

// same as above, for C-style arrays
template <typename Elem, std::size_t N, typename T>
constexpr void fill_array(Elem (&arr)[N], const T& val)
{
  if constexpr (std::is_array_v<Elem>)
    for (auto& inner_arr : arr)
      fill_array(inner_arr, val);
  else
    std::fill(arr, arr + N, static_cast<Elem>(val));
}

// auto arr4d = fmia::make_array<int, 5, 8, 3, 2>(val);
// combines `fmia::array<int, 5, 8, 3, 2> arr4d` and `fmia::fill_array(arr4d, val)`
template <typename Elem, std::size_t... Dims, typename T>
[[nodiscard]] constexpr auto make_array(const T& val) -> array<Elem, Dims...>
{
  array<Elem, Dims...> arr;
  fill_array(arr, static_cast<Elem>(val));
  return arr;
}

} // export namespace fmia

namespace fmia::meta {

// allocator type order: dim_n, dim_(n - 1), ..., dim_1
template <typename Elem, nonempty_list_of_types AllocatorList, typename LastAllocator = last_t<AllocatorList>>
struct cur_dim_allocator
{
  using type = LastAllocator;
};

template <typename Elem, typename AllocatorList>
struct cur_dim_allocator<Elem, AllocatorList, std_allocator_tag>
{
  using type = std::allocator<Elem>;
};

template <typename Elem, typename AllocatorList>
struct cur_dim_allocator<Elem, AllocatorList, std_pmr_allocator_tag>
{
  using type = std::pmr::polymorphic_allocator<Elem>;
};

template <typename Elem, typename AllocatorList>
using cur_dim_allocator_t = cur_dim_allocator<Elem, AllocatorList>::type;

// used in recursion, add a default std::allocator, if the length of the allocator type list < dim count
template <list_of_types CurAllocatorList, std::size_t DimCnt>
struct adjust_allocator_type_list
  : concat<CurAllocatorList, std::conditional_t<(length_v<CurAllocatorList>) < DimCnt, type_list<std_allocator_tag>, empty_type_list>>
{
};

template <typename CurAllocatorList, std::size_t DimCnt>
using adjust_allocator_type_list_t = adjust_allocator_type_list<CurAllocatorList, DimCnt>::type;

} // namespace fmia::meta

namespace fmia {

template <typename, std::size_t DimCnt, typename>
  requires (DimCnt != 0)
class vector_impl;

template <typename T, std::size_t DimCnt, typename InnermostDimAllocator, typename... Allocators>
class vector_impl<T, DimCnt, meta::type_list<InnermostDimAllocator, Allocators...>>
{
private:
  using adjusted_allocator_type_list_ = meta::adjust_allocator_type_list_t<meta::type_list<InnermostDimAllocator, Allocators...>, DimCnt>;

  using element_type_ = vector_impl<T, DimCnt - 1, meta::init_t<adjusted_allocator_type_list_>>::type;

public:
  using type = std::vector<element_type_, meta::cur_dim_allocator_t<element_type_, adjusted_allocator_type_list_>>;
};

template <typename T, typename Allocator>
class vector_impl<T, 1, meta::type_list<Allocator>>
{
public:
  using type = std::vector<T, meta::cur_dim_allocator_t<T, meta::type_list<Allocator>>>;
};

} // namespace fmia

export namespace fmia {

// fmia::vector<int> vec1d;
// same as: std::vector<int> vec1d;
//
// fmia::vector<int, 4> vec4d;
// same as: std::vector<std::vector<std::vector<std::vector<int>>>> vec4d;
template <typename T, std::size_t DimCnt = 1, typename InnermostDimAllocator = std_allocator_tag, typename... Allocators>
  requires (sizeof...(Allocators) < DimCnt)
using vector = vector_impl<T, DimCnt, meta::type_list<InnermostDimAllocator, Allocators...>>::type;

} // export namespace fmia

namespace fmia {

template <typename Elem, typename AllocatorList, typename Dim, typename... Ts>
[[nodiscard]] constexpr auto make_vector_impl(Dim first_dim_size, Ts&&... args)
{
  using adjusted_allocator_type_list = meta::adjust_allocator_type_list_t<AllocatorList, sizeof...(Ts)>;
  if constexpr (sizeof...(Ts) == 1) {
    using cur_dim_allocator_type = meta::cur_dim_allocator_t<Elem, adjusted_allocator_type_list>;
    return vector<Elem, 1, cur_dim_allocator_type>(static_cast<std::size_t>(first_dim_size), static_cast<Elem>(args)...);
  } else {
    using inner_allocator_type_list = meta::init_t<adjusted_allocator_type_list>;
    using inner_element_type = vector_impl<Elem, sizeof...(Ts) - 1, inner_allocator_type_list>::type;
    using cur_dim_allocator_type = meta::cur_dim_allocator_t<inner_element_type, adjusted_allocator_type_list>;
    return vector<inner_element_type, 1, cur_dim_allocator_type>(
      static_cast<std::size_t>(first_dim_size), make_vector_impl<Elem, inner_allocator_type_list>(std::forward<Ts>(args)...)
    );
  }
}

} // namespace fmia

export namespace fmia {

// auto vec3d = fmia::make_vector<int>(x, y, z, 1);
// same as: auto vec3d = std::vector<std::vector<std::vector<int>>>(
//                         x,
//                         std::vector<std::vector<int>>(y, std::vector<int>(z, 1))
//                       );
//
// constexpr usage example (error if fmia::make_vector is not constexpr):
// std::cout << [](const std::vector<int>& vec, int sum = 0) consteval {
//   std::ranges::for_each(vec, [&sum](int elem) { return sum += elem; });
//   return sum;
// }(fmia::make_vector<int>(10, -1)) << "\n";
template <typename Elem, typename InnermostDimAllocator = std_allocator_tag, typename... Allocators, std::integral Dim, typename... Ts>
  requires (sizeof(Dim) <= sizeof(std::size_t) && sizeof...(Ts) > 0 && sizeof...(Allocators) < sizeof...(Ts))
[[nodiscard]] constexpr auto make_vector(Dim first_dim_size, Ts&&... args)
{
  return make_vector_impl<Elem, meta::type_list<InnermostDimAllocator, Allocators...>>(first_dim_size, std::forward<Ts>(args)...);
}

} // export namespace fmia
