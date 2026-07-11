// SPDX-FileCopyrightText: 2026 atyxeut <atyxeut@outlook.com>
// SPDX-License-Identifier: LGPL-3.0-or-later

export module fmia.data_structure.array.multidimensional_std_vector;

import std;

import fmia.memory.allocator;
import fmia.meta;

namespace fmia {

template <typename, std::size_t DimCnt, typename>
  requires (DimCnt != 0)
class vector_impl;

template <typename T, std::size_t DimCnt, typename FirstDimAllocator, typename... Allocators>
class vector_impl<T, DimCnt, meta::type_list<FirstDimAllocator, Allocators...>> {
private:
  using allocator_list_ = meta::type_list<FirstDimAllocator, Allocators...>;
  using adjusted_allocator_list_ = meta::snoc_if_t<(1 + sizeof...(Allocators) < DimCnt), allocator_list_, std_allocator_tag>;
  using element_type_ = vector_impl<T, DimCnt - 1, meta::init_t<adjusted_allocator_list_>>::type;

public:
  using type = std::vector<element_type_, eval_allocator_t<meta::last_t<adjusted_allocator_list_>, element_type_>>;
};

template <typename T, typename Allocator>
class vector_impl<T, 1, meta::type_list<Allocator>> {
public:
  using type = std::vector<T, eval_allocator_t<Allocator, T>>;
};

} // namespace fmia

export namespace fmia {

// fmia::vector<int> vec1d;
// same as: std::vector<int> vec1d;
//
// fmia::vector<int, 4> vec4d;
// same as: std::vector<std::vector<std::vector<std::vector<int>>>> vec4d;
template <typename T, std::size_t DimCnt = 1, typename FirstDimAllocator = std_allocator_tag, typename... Allocators>
  requires (sizeof...(Allocators) < DimCnt)
using vector = vector_impl<T, DimCnt, meta::type_list<FirstDimAllocator, Allocators...>>::type;

} // export namespace fmia

namespace fmia {

template <typename Elem, typename AllocatorList, typename Dim, typename... Ts>
[[nodiscard]] constexpr auto make_vector_impl(Dim first_dim_size, Ts&&... args) {
  using adjusted_allocator_list = meta::snoc_if_t<(meta::length_v<AllocatorList> < sizeof...(Ts)), AllocatorList, std_allocator_tag>;
  if constexpr (sizeof...(Ts) == 1) {
    using allocator_type = eval_allocator_t<meta::last_t<adjusted_allocator_list>, Elem>;
    return vector<Elem, 1, allocator_type>(static_cast<std::size_t>(first_dim_size), static_cast<Elem>(args)...);
  } else {
    using inner_allocator_list = meta::init_t<adjusted_allocator_list>;
    using element_type = vector_impl<Elem, sizeof...(Ts) - 1, inner_allocator_list>::type;
    using allocator_type = eval_allocator_t<meta::last_t<adjusted_allocator_list>, element_type>;
    return vector<element_type, 1, allocator_type>(
      static_cast<std::size_t>(first_dim_size), make_vector_impl<Elem, inner_allocator_list>(std::forward<Ts>(args)...)
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
template <typename Elem, typename FirstDimAllocator = std_allocator_tag, typename... Allocators, std::integral Dim, typename... Ts>
  requires (sizeof(Dim) <= sizeof(std::size_t) && sizeof...(Ts) > 0 && sizeof...(Allocators) < sizeof...(Ts))
[[nodiscard]] constexpr auto make_vector(Dim first_dim_size, Ts&&... args) {
  return make_vector_impl<Elem, meta::type_list<FirstDimAllocator, Allocators...>>(first_dim_size, std::forward<Ts>(args)...);
}

} // export namespace fmia
