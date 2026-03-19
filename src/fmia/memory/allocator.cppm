// Copyright (C) 2026 atyxeut
//
// This library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this library.  If not, see <https://www.gnu.org/licenses/>.

export module fmia.memory.allocator;

import std;

import fmia.meta;
import fmia.util.enum_flag;

export namespace fmia {

// represent std::allocator<T>
struct std_allocator_tag
{
};

// represent std::pmr::polymorphic_allocator<T>
struct std_pmr_allocator_tag
{
};

} // namespace fmia

export namespace fmia::meta {

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
  : concat<
      CurAllocatorList,
      std::conditional_t<(length_v<CurAllocatorList>) < DimCnt, type_list<std_allocator_tag>, empty_type_list>
    >
{
};

template <typename CurAllocatorList, std::size_t DimCnt>
using adjust_allocator_type_list_t = adjust_allocator_type_list<CurAllocatorList, DimCnt>::type;

} // namespace fmia::meta

namespace fmia {

enum class construction_category : enum_underlying_type { move, copy };

template <typename Allocator, typename InputIt, typename ForwardIt>
concept uninitialized_construct_function_invocable =
  std::same_as<
    typename std::allocator_traits<Allocator>::value_type, typename std::iterator_traits<ForwardIt>::value_type
  >
  && std::constructible_from<typename std::allocator_traits<Allocator>::value_type, std::iter_reference_t<InputIt>>;

template <construction_category Category, typename Allocator, typename InputIt, typename ForwardIt>
constexpr auto uninitialized_construct_n(Allocator& alloc, InputIt first, std::ptrdiff_t count, ForwardIt dest)
{
  if (count <= 0) {
    if constexpr (Category == construction_category::move)
      return std::pair {first, dest};
    else
      return dest;
  }

  using allocator_traits = std::allocator_traits<Allocator>;
  using value_type = allocator_traits::value_type;

  if !consteval {
    if constexpr (
      std::same_as<Allocator, std::allocator<value_type>> && std::is_trivially_copyable_v<value_type>
      && std::contiguous_iterator<InputIt> && std::contiguous_iterator<ForwardIt>
    ) {
      std::memcpy(std::to_address(dest), std::to_address(first), count * sizeof(value_type));
      if constexpr (Category == construction_category::move)
        return std::pair {first + count, dest + count};
      else
        return dest + count;
    }
  }

  auto cur = dest;
  try {
    for (; count-- > 0; ++first, (void)++cur) {
      if constexpr (Category == construction_category::move)
        allocator_traits::construct(alloc, std::addressof(*cur), std::move(*first));
      else
        allocator_traits::construct(alloc, std::addressof(*cur), *first);
    }
  } catch (...) {
    for (; dest != cur; ++dest)
      allocator_traits::destroy(alloc, std::addressof(*dest));
    throw;
  }
  if constexpr (Category == construction_category::move)
    return std::pair {first, cur};
  else
    return cur;
}

} // namespace fmia

export namespace fmia {

template <typename Allocator, std::input_iterator InputIt, std::forward_iterator ForwardIt>
  requires uninitialized_construct_function_invocable<Allocator, InputIt, ForwardIt>
constexpr auto uninitialized_move_n(Allocator& alloc, InputIt first, std::ptrdiff_t count, ForwardIt dest)
{
  return uninitialized_construct_n<construction_category::move>(alloc, first, count, dest);
}

template <typename Allocator, std::input_iterator InputIt, std::forward_iterator ForwardIt>
  requires uninitialized_construct_function_invocable<Allocator, InputIt, ForwardIt>
constexpr auto uninitialized_copy_n(Allocator& alloc, InputIt first, std::ptrdiff_t count, ForwardIt dest)
{
  return uninitialized_construct_n<construction_category::copy>(alloc, first, count, dest);
}

} // export namespace fmia
